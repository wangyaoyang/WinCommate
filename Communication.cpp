#include "StdAfx.h"
#include "Commate.h"
#include "CommateDoc.h"
#include "EthIfSelDlg.h"
#include ".\communication.h"

#define	STATUS_BAR_MESSAGE(msg)		( (CStatusBar*) AfxGetMainWnd()->GetDescendantWindow(AFX_IDW_STATUS_BAR) )->SetPaneText(0,msg)

CCommunication::CCommunication()
	: m_tcpct(m_buf_cli)
{
	m_nMode				= 0;
	m_dwComParameter	= 0;
	m_nRemotePort		= 0;
	m_tcpsv				= NULL;
	m_udpip				= NULL;
	m_dwSrcAddress		= 0;
	memset( m_sIpAddress,0x00,4 );
	m_bIpTransfer		= 0;
	m_dwIpTermA			= 0;
	m_dwIpTermB			= 0;
	m_nPortTermA		= 0;
	m_nPortTermB		= 0;
}

CCommunication::~CCommunication(void)
{
	if( m_tcpsv ) delete m_tcpsv;
	if( m_udpip ) delete m_udpip;
}

int	CCommunication::m_Send(char* pData,DWORD& nCounter)
{
	int		nCode = 0;
	switch( m_nMode )
	{
	case PORT_SERIAL:	m_serial.m_PortWritting( pData,nCounter );	return 1;
	case PORT_TCPIP:	nCode = m_nRemotePort > 0 ?
							m_tcpct.Send( pData,nCounter ) :
							m_tcpsv->Broadcast(pData,nCounter);
						return nCode;
	case PORT_UDPIP:	if( m_udpip )
						{
							char	sIpAddress[16];
							sprintf_s( sIpAddress,sizeof(sIpAddress),	"%u.%u.%u.%u",
										0x00ff&m_sIpAddress[0],0x00ff&m_sIpAddress[1],
										0x00ff&m_sIpAddress[2],0x00ff&m_sIpAddress[3] );
							nCounter = m_udpip->Send( sIpAddress,m_nRemotePort,pData,nCounter );
							return 1;
						}
						else { nCounter = 0; return 0; }
	}
	return 1;
}

int	CCommunication::m_RelayA(char* pData,DWORD& nCounter)
{
	switch( m_nMode )
	{
	case PORT_SERIAL:	return m_serial.m_PortWritting( pData,nCounter );
	case PORT_TCPIP:
		if( m_nRemotePort > 0 ) return m_tcpct.Send( pData,nCounter );
		else if( m_tcpsv ) return m_tcpsv->Broadcast( pData,nCounter );
	case PORT_UDPIP:
		if( m_udpip )
		{	//如果需要转发，立刻执行
			int		b1 = 0,b2 = 0,b3 = 0,b4 = 0;
			char	sIpAddress[32];
			memset(	sIpAddress,0x00,32 );
			b1	= 0x000000ff & (m_dwIpTermA>>24);
			b2	= 0x000000ff & (m_dwIpTermA>>16);
			b3	= 0x000000ff & (m_dwIpTermA>>8);
			b4	= 0x000000ff & (m_dwIpTermA>>0);
			sprintf_s( sIpAddress,sizeof(sIpAddress),"%d.%d.%d.%d",b1,b2,b3,b4 );
			CString	szRelayMessage;

			szRelayMessage.Format( _T("Relay %d BYTEs to %s:%d"),nCounter,sIpAddress,m_nPortTermB );
			STATUS_BAR_MESSAGE( szRelayMessage );
			return m_udpip->Send( sIpAddress,m_nPortTermA,pData,nCounter );
		}
		break;
	default:;
	}
	return 0;
}

int	CCommunication::m_RelayB(char* pData,DWORD& nCounter)
{
	switch( m_bIpTransfer )
	{
	case RELAY_SERIAL:	return m_serial.m_PortWritting( pData,nCounter );
	case RELAY_TCPIP:	return m_tcpct.Send( pData,nCounter );
	case RELAY_UDPIP:
		if( m_udpip )
		{	//如果需要转发，立刻执行
			int		b1 = 0,b2 = 0,b3 = 0,b4 = 0;
			char	sIpAddress[32];
			memset(	sIpAddress,0x00,32 );
			b1	= 0x000000ff & (m_dwIpTermB>>24);
			b2	= 0x000000ff & (m_dwIpTermB>>16);
			b3	= 0x000000ff & (m_dwIpTermB>>8);
			b4	= 0x000000ff & (m_dwIpTermB>>0);
			sprintf_s( sIpAddress,sizeof(sIpAddress),"%d.%d.%d.%d",b1,b2,b3,b4 );
			CString	szRelayMessage;

			szRelayMessage.Format( _T("Relay %d BYTEs to %s:%d"),nCounter,sIpAddress,m_nPortTermB );
			STATUS_BAR_MESSAGE( szRelayMessage );
			return m_udpip->Send( sIpAddress,m_nPortTermB,pData,nCounter );
		}
		break;
	case RELAY_DISABLE:
	default:;
	}
	return 0;
}

bool CCommunication::m_Recv(char* pData,DWORD& nData,bool& bMajor)
{	//bMajor = true means received data from major comm port
	DWORD	dwToRead = nData;
	if( m_Receive(pData,nData=dwToRead) && nData > 0 )
	{
		m_RelayB(pData,nData);
		bMajor = true;
		return true;
	}
	else if( m_Response(pData,nData=dwToRead) && nData > 0 )
	{
		m_RelayA(pData,nData);
		bMajor = false;
		return true;
	}
	return false;
}

int	CCommunication::m_Receive(char* pData,DWORD& nCounter)
{
	m_dwSrcAddress = 0;
	switch( m_nMode )
	{
	case PORT_SERIAL:	return m_serial.m_PortReading( pData,nCounter );
	case PORT_TCPIP:
		{
			int	nIndex = 0;
			int nRecv = 0;
			if( m_nRemotePort > 0 ) nRecv = m_tcpct.Recv();
			else if( m_tcpsv ) nRecv = m_tcpsv->Browse(nIndex);
			if( nRecv > 0 ) {
				nCounter = nRecv;
				if (m_nRemotePort) {
					int		b0	= m_sIpAddress[0];
					int		b1	= m_sIpAddress[1];
					int		b2	= m_sIpAddress[2];
					int		b3	= m_sIpAddress[3];
					m_dwSrcAddress =	((b0<<24)&0xff000000) | ((b1<<16)&0x00ff0000) |
										((b2<<8 )&0x0000ff00) | (b3&0x000000ff);
					memcpy( pData,m_tcpct.GetBuffer(),nCounter );
				} else {
					m_dwSrcAddress = m_tcpsv->ClientAdd(nIndex);
					memcpy( pData,m_tcpsv->GetBuffer(),nCounter );
				}
				return nCounter;
			}
			else nCounter = 0;
		}
		break;
	case PORT_UDPIP:
		if( m_udpip ) {
			DWORD	dwIP = 0;
			WORD	nPort = m_nRemotePort;
			int nRecv = m_udpip->Recv(dwIP,nPort);

			if( nRecv > 0 ) {
				int		b1 = 0x00ff & (dwIP >> 24);
				int		b2 = 0x00ff & (dwIP >> 16);
				int		b3 = 0x00ff & (dwIP >> 8);
				int		b4 = 0x00ff & dwIP;
				nCounter = nRecv;
				memcpy( pData,m_udpip->GetBuffer(),nCounter );
				//将目的地址设置为源地址
				m_sIpAddress[0] = b1;
				m_sIpAddress[1] = b2;
				m_sIpAddress[2] = b3;
				m_sIpAddress[3] = b4;
				m_dwSrcAddress =	((b1<<24)&0xff000000) | ((b2<<16)&0x00ff0000) |
									((b3<<8 )&0x0000ff00) | (b4&0x000000ff);
				m_dwSourcePort = nPort;
				return nCounter;
			}
			else nCounter = 0;
		}
		break;
	default:			nCounter = 0;
	}
	return 0;
}


int	CCommunication::m_Response(char* pData,DWORD& nCounter)
{
	switch( m_bIpTransfer )
	{
	case RELAY_SERIAL:
		return m_serial.m_PortReading( pData,nCounter );
	case RELAY_TCPIP:
		{
			int	nIndex = 0;
			int nRecv = 0;
			if( m_nRemotePort > 0 ) nRecv = m_tcpct.Recv();
			else if( m_tcpsv ) nRecv = m_tcpsv->Browse(nIndex);
			if( nRecv > 0 ) {
				nCounter = nRecv;
				if( m_nRemotePort ) {
					int		b0	= m_sIpAddress[0];
					int		b1	= m_sIpAddress[1];
					int		b2	= m_sIpAddress[2];
					int		b3	= m_sIpAddress[3];
					m_dwSrcAddress =	((b0<<24)&0xff000000) | ((b1<<16)&0x00ff0000) |
										((b2<<8 )&0x0000ff00) | (b3&0x000000ff);
					memcpy( pData,m_tcpct.GetBuffer(),nCounter );
				} else {
					m_dwSrcAddress = m_tcpsv->ClientAdd(nIndex);
					memcpy( pData,m_tcpsv->GetBuffer(),nCounter );
				}
				return nCounter;
			}
			else nCounter = 0;
		}
		break;
	case RELAY_UDPIP:
		if( m_udpip )
		{
			DWORD	dwIP = 0;
			WORD	nPort = m_nRemotePort;
			int nRecv = m_udpip->Recv(dwIP,nPort);

			if (nRecv > 0) {
				int		b1 = 0x00ff & (dwIP >> 24);
				int		b2 = 0x00ff & (dwIP >> 16);
				int		b3 = 0x00ff & (dwIP >> 8);
				int		b4 = 0x00ff & dwIP;

				nCounter = nRecv;
				memcpy( pData,m_udpip->GetBuffer(),nCounter );
				//将目的地址设置为源地址
				m_sIpAddress[0] = b1;
				m_sIpAddress[1] = b2;
				m_sIpAddress[2] = b3;
				m_sIpAddress[3] = b4;
				m_dwSrcAddress =	((b1<<24)&0xff000000) | ((b2<<16)&0x00ff0000) |
									((b3<<8 )&0x0000ff00) | (b4&0x000000ff);
				m_dwSourcePort = nPort;
				return nCounter;
			}
			else nCounter = 0;
		}
		break;
	default:			nCounter = 0;
	}
	return 0;
}

int	CCommunication::m_OpenTCP(CProbeDoc* pDoc, DWORD dwTargetIP,int nRemotePort,int nLocalPort)	//打开TCPIP端口
{
	m_nMode = PORT_TCPIP;
	char	b1 = (char)(0x00ff & (dwTargetIP>>24));
	char	b2 = (char)(0x00ff & (dwTargetIP>>16));
	char	b3 = (char)(0x00ff & (dwTargetIP>>8));
	char	b4 = (char)(0x00ff & (dwTargetIP>>0));
	if( m_tcpsv ) delete m_tcpsv;
	if( m_udpip ) delete m_udpip;
	m_udpip = NULL;
	m_tcpsv = new CSocketServer(m_buf_svr, nLocalPort);
	if( nLocalPort > 0 && nRemotePort == 0 )
	{
		m_tcpsv->StartListenThread();	return 1;
	}
	if( m_tcpct.Connect(b1,b2,b3,b4,nRemotePort) )
	{
		m_sIpAddress[0] = b1;
		m_sIpAddress[1] = b2;
		m_sIpAddress[2] = b3;
		m_sIpAddress[3] = b4;
		STATUS_BAR_MESSAGE(_T("TCP/IP 连接:Successed !"));
		m_nRemotePort = nRemotePort;
		return 1;
	}
	else return 0;
}

int	CCommunication::m_OpenUDP(CProbeDoc* pDoc, DWORD dwTargetIP,int nRemotePort,int nLocalPort)	//打开UDPIP端口
{
	DWORD	dwLocalIP = 0;
	unsigned char b1 = m_sIpAddress[0] = (char)(0x00ff & (dwTargetIP>>24));
	unsigned char b2 = m_sIpAddress[1] = (char)(0x00ff & (dwTargetIP>>16));
	unsigned char b3 = m_sIpAddress[2] = (char)(0x00ff & (dwTargetIP>>8));
	unsigned char b4 = m_sIpAddress[3] = (char)(0x00ff & (dwTargetIP>>0));
	bool isMulticast = !!(224 <= b1 && b1 <= 239);

	if (pDoc && pDoc->m_EthListIfs() > 1) {
		CEthIfSelDlg	dlg(pDoc);

		if (isMulticast) dlg.m_szMcastIP.Format(_T("%d.%d.%d.%d"), b1, b2, b3, b4);
		if (IDOK == dlg.DoModal()) {
			DWORD	a1 = 0, a2 = 0, a3 = 0, a4 = 0;
			UNI_SCANF(dlg.m_GetEthIP(), _T("%d.%d.%d.%d"), &a1, &a2, &a3, &a4);
			dwLocalIP = (a1 << 24) | (a2 << 16) | (a3 << 8) | a4;
		}
	}
	if( m_udpip ) delete m_udpip;
	if( m_tcpsv ) { m_tcpsv->StopListenThread();	delete m_tcpsv; m_tcpsv = NULL; }
	m_udpip = new CSocketDatagram(m_buf_svr, nLocalPort);
	m_nMode = PORT_UDPIP;
	m_nRemotePort = nRemotePort;
	if( nLocalPort > 0 ) {
		if (isMulticast) {
			char	mcast_ip[16];
			memset (mcast_ip, 0x00, 16);
			sprintf(mcast_ip, "%u.%u.%u.%u", b1, b2, b3, b4);
			m_udpip->Bind(mcast_ip, nLocalPort, dwLocalIP);
		} else m_udpip->Bind(NULL, nLocalPort, dwLocalIP);		//do not join multicast group
	}
	STATUS_BAR_MESSAGE(_T("UDP/IP 连接:Successed !"));
	return 1;
}

int	CCommunication::m_OpenCOM(DWORD dwSerialPara)	//打开串行通信口
{
	m_nMode = PORT_SERIAL;
	if( m_udpip ) { delete m_udpip;	m_udpip = NULL;	}
	if( m_tcpsv ) { m_tcpsv->StopListenThread();	delete m_tcpsv; m_tcpsv = NULL; }
	if( m_serial.m_PortSetting( dwSerialPara ) ) {
		STATUS_BAR_MESSAGE(_T("打开串行口::Successed !"));
		return m_serial.m_PortOpen( USE_CURR_PORT );
	}
	else return 0;
}

int	CCommunication::m_Open(CProbeDoc* pDoc) {
	int		nRetCode = 0;
	int		nRelay = 0;
	int		nMode = pDoc->m_nCommunicateMode;
	DWORD	dwSerialPara = pDoc->m_dwSerialParameter;
	DWORD	dwIpAddress = pDoc->m_dwIPAddress;
	int		nRemotePort = pDoc->m_dwClientPort;
	int		nLocalPort = pDoc->m_dwServerPort;

	switch( nMode ) {
	case PORT_SERIAL:	switch( m_bIpTransfer ) {
						case PORT_TCPIP:	if( nRemotePort > 0 )
												m_OpenTCP(pDoc, dwIpAddress,nRemotePort,0);	break;
						case RELAY_UDPIP:	if( nRemotePort > 0 )
												m_OpenUDP(pDoc, dwIpAddress,nRemotePort,nLocalPort); break;
						default:;
						}
						nRetCode = m_OpenCOM(	dwSerialPara );
		break;
	case PORT_TCPIP:	switch( m_bIpTransfer ) {
						case RELAY_SERIAL:	if( m_serial.m_PortSetting( dwSerialPara ) ) {
												STATUS_BAR_MESSAGE(_T("打开串行口用于转发::Successed !"));
												m_serial.m_PortOpen( USE_CURR_PORT );
											}
							break;
						case RELAY_UDPIP:	if( nRemotePort > 0 ) m_OpenUDP(pDoc, dwIpAddress,nRemotePort,nLocalPort);
							break;
							break;
						default:;
						}
						nRetCode = m_OpenTCP(pDoc, dwIpAddress,nRemotePort,nLocalPort);
		break;
	case PORT_UDPIP:	switch( m_bIpTransfer ) {
						case PORT_TCPIP:	if( nRemotePort > 0 ) m_OpenTCP(pDoc, dwIpAddress,nRemotePort,0);
							break;
						case RELAY_SERIAL:	if( m_serial.m_PortSetting( dwSerialPara ) ) {
												STATUS_BAR_MESSAGE(_T("打开串行口用于转发::Successed !"));
												m_serial.m_PortOpen( USE_CURR_PORT );
											}
							break;
						default:;
						}
						nRetCode = m_OpenUDP(pDoc, dwIpAddress,nRemotePort,nLocalPort);
		break;
	default:;
	}
	return nRetCode;
}

int	CCommunication::m_Close()
{
	m_serial.m_PortClose();
	m_tcpct.DisConnect();
	if( m_tcpsv )
	{
		m_tcpsv->StopListenThread();
		delete m_tcpsv;
		m_tcpsv = NULL;
	}
	if( m_udpip )
	{
		delete m_udpip;
		m_udpip = NULL;
	}
	m_nMode = 0;
	return 1;
}

void CCommunication::m_SetIpTransfer(BOOL bIpTransfer,DWORD dwIp1,int nPort1,DWORD dwIp2,int nPort2)
{
	m_bIpTransfer		= bIpTransfer;
	m_dwIpTermA			= dwIp1;
	m_dwIpTermB			= dwIp2;
	m_nPortTermA		= nPort1;
	m_nPortTermB		= nPort2;
}