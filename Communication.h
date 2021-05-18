#pragma once

#include "E:\Development\TmnDev\TmnLibs\SerialCom\SerialCom.h"
#include "E:\Development\TmnDev\TmnLibs\SocketApi\SOCKET.H"

#define	PORT_SERIAL		1
#define	PORT_TCPIP		2
#define	PORT_UDPIP		3

#define	RELAY_DISABLE	0
#define	RELAY_SERIAL	1
#define	RELAY_TCPIP		2
#define	RELAY_UDPIP		3

class CProbeDoc;
class CCommunication
{
private:
	int					m_nMode;
	DWORD				m_dwComParameter;
	DWORD				m_dwSrcAddress;
	DWORD				m_dwSourcePort;
	char				m_sIpAddress[4];
	int					m_nRemotePort;
	char				m_buf_cli[MAX_BUFF_SIZE];
	char				m_buf_svr[MAX_BUFF_SIZE];
private:
	CSerialCom			m_serial;
	CSocketServer*		m_tcpsv;
	CSocketClient		m_tcpct;
	CSocketDatagram*	m_udpip;
private:
	BOOL				m_bIpTransfer;
	DWORD				m_dwIpTermA;
	DWORD				m_dwIpTermB;
	int					m_nPortTermA;
	int					m_nPortTermB;
private:
	int	m_OpenCOM(DWORD dwSerialPara);									//打开串行通信口
	int	m_OpenTCP(CProbeDoc* pDoc, DWORD dwIpAddress,int nRemotePort,int nLocalPort);	//打开TCPIP端口
	int	m_OpenUDP(CProbeDoc* pDoc, DWORD dwIpAddress,int nRemotePort,int nLocalPort);	//打开UDPIP端口
	int	m_Receive(char* pData,DWORD& nCounter);
	int	m_Response(char* pData,DWORD& nCounter);
	int	m_RelayA(char* pData,DWORD& nCounter);
	int	m_RelayB(char* pData,DWORD& nCounter);
public:
	CCommunication();
	~CCommunication(void);
	int	m_GetCommMode()	{ return m_nMode; }
	DWORD				m_GetRemoteAdd()	{ return m_dwSrcAddress; }
	CSerialCom&			m_GetSerial()	{ return m_serial; }
	CSocketDatagram*	m_GetUdpIP()	{ return m_udpip; }
	int		m_Open(CProbeDoc* pDoc);
	int		m_Close();
	int		m_Send(char* pData,DWORD& nCounter);
	bool	m_Recv(char* pData,DWORD& nData,bool& bMajor);
	void	m_SetIpTransfer(BOOL bIpTransfer,DWORD dwIp1,int nPort1,DWORD dwIp2,int nPort2);
};
