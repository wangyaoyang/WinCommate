// SignalListCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "Commate.h"
#include "SignalListCtrl.h"
#include "CommateDoc.h"


// CSignalListCtrl

//IMPLEMENT_DYNAMIC(CSignalListCtrl, CXListCtrl)

CSignalListCtrl::CSignalListCtrl()
{
	m_nItem = -1;
	m_nSubItem = -1;
	m_pDoc = NULL;
	m_szCommMode.Add( _T("串行端口") );
	m_szCommMode.Add( _T("TCP/IP") );
	m_szCommMode.Add( _T("UDP/IP") );
	for( int com = 1; com <= 40; com ++ )
	{
		CString	szPara;
		szPara.Format( _T("%d"),com );
		m_szCommPort.Add( szPara );
	}
	m_szCommData.Add( _T("5") );
	m_szCommData.Add( _T("6") );
	m_szCommData.Add( _T("7") );
	m_szCommData.Add( _T("8") );
	m_szCommStop.Add( _T("1") );
	m_szCommStop.Add( _T("1.5") );
	m_szCommStop.Add( _T("2") );
	m_szCommParity.Add( _T("无校验") );
	m_szCommParity.Add( _T("奇校验") );
	m_szCommParity.Add( _T("偶校验") );
	m_szCommBaud.Add( _T("1200") );
	m_szCommBaud.Add( _T("2400") );
	m_szCommBaud.Add( _T("4800") );
	m_szCommBaud.Add( _T("9600") );
	m_szCommBaud.Add( _T("14400") );
	m_szCommBaud.Add( _T("19200") );
	m_szCommBaud.Add( _T("28800") );
	m_szCommBaud.Add( _T("33600") );
	m_szCommBaud.Add( _T("57600") );
	m_szCommBaud.Add( _T("115200") );
	m_saTransMode.Add( _T("不启用") );
	m_saTransMode.Add( _T("→串口") );
	m_saTransMode.Add( _T("→TCPIP") );
	m_saTransMode.Add( _T("→UDPIP") );
}

CSignalListCtrl::~CSignalListCtrl()
{
	m_szCommMode.RemoveAll();
}


BEGIN_MESSAGE_MAP(CSignalListCtrl, CXListCtrl)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &CSignalListCtrl::OnLvnItemchanged)
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CSignalListCtrl 消息处理程序
int CSignalListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	ModifyStyle(0,LVS_REPORT|LVS_SHOWSELALWAYS|LVS_SINGLESEL);
	SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,0,
						LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP);//|LVS_EX_GRIDLINES
	EnableToolTips(TRUE);
	m_cImageList.Create(IDB_BITMAP_LED,16,0,RGB(0,128,128));
	m_cImageList.SetBkColor(GetSysColor(COLOR_WINDOW));
	SetImageList(&m_cImageList,LVSIL_SMALL);
	//SetEllipsis(TRUE);
	SetBkColor( (COLORREF)RGB(0,0,0) );
	SetTextBkColor( (COLORREF)RGB(0,0,0) );
	SetTextColor( (COLORREF)RGB(0,255,0) );
	InsertColumn( 0,_T("通信设置"),LVCFMT_LEFT,100 );
	InsertColumn( 1,_T("参数"),LVCFMT_LEFT,120 );
	InsertColumn( 2,_T("状态"),LVCFMT_LEFT,200 );

	return 0;
}

void CSignalListCtrl::m_InitCtrl(CProbeDoc& doc)
{
	m_pDoc = &doc;

}

int	CSignalListCtrl::m_SearchInArray( CStringArray& arr,int nValue )
{
	CString	szValue;
	szValue.Format( _T("%d"),nValue );
	return m_SearchInArray( arr,szValue );
}

int	CSignalListCtrl::m_SearchInArray( CStringArray& arr,CString szValue )
{
	INT_PTR	nCount = arr.GetCount();
	for( INT_PTR n = 0; n < nCount; n ++ )
	{
		if( arr[n] == szValue ) return (int) n;
	}
	return 0;
}

void CSignalListCtrl::m_ShowConfig()
{
	DeleteAllItems();
	m_saTransActive.RemoveAll();
	m_saTransActive.Copy( m_saTransMode );
	if( m_pDoc->m_nCommunicateMode < PORT_SERIAL ) m_pDoc->m_nCommunicateMode = PORT_SERIAL;
	if( m_pDoc->m_nCommunicateMode > PORT_UDPIP ) m_pDoc->m_nCommunicateMode = PORT_UDPIP;

	InsertItem( INDEX_ITEM_BASE,_T("连接/中断") );
	SetCheckbox( INDEX_ITEM_BASE,0,0 );
	InsertItem( INDEX_ITEM_COMM_MODE,_T("通信方式") );
	SetComboBox( INDEX_ITEM_COMM_MODE,1,TRUE,&m_szCommMode,10,m_pDoc->m_nCommunicateMode-1 );
	
	switch( m_pDoc->m_nCommunicateMode )
	{
	case PORT_SERIAL:	//COM
		{
			int	port_no	= (( 0xfc000000 & m_pDoc->m_dwSerialParameter ) >> 26) - 1;
			int	byte_size	= (( 0x03c00000 & m_pDoc->m_dwSerialParameter ) >> 22);
			int	stop_bit	= (( 0x00300000 & m_pDoc->m_dwSerialParameter ) >> 20) - 1;
			int	parity	= (( 0x000c0000 & m_pDoc->m_dwSerialParameter ) >> 18);
			int	baud_rate	= (( 0x0003ffff & m_pDoc->m_dwSerialParameter ));
			if( port_no < 0 ) port_no = 0;
			if( byte_size <= 0 ) byte_size = 8;
			if( stop_bit < 0 ) stop_bit = 0;
			if( parity < 0 ) parity = 0;
			if( baud_rate < 0 ) baud_rate = 0;
			InsertItem( INDEX_ITEM_COMM_PORT,_T("├端口号") );
			InsertItem( INDEX_ITEM_COMM_DATA,_T("├数据位") );		SetItemText( INDEX_ITEM_COMM_DATA,2,_T("DSR") );
			InsertItem( INDEX_ITEM_COMM_STOP,_T("├停止位") );		SetItemText( INDEX_ITEM_COMM_STOP,2,_T("CTS") );
			InsertItem( INDEX_ITEM_COMM_PARITY,_T("├校验位") );	SetItemText( INDEX_ITEM_COMM_PARITY,2,_T("CD") );
			InsertItem( INDEX_ITEM_COMM_BAUD,_T("└波特率") );		SetItemText( INDEX_ITEM_COMM_BAUD,2,_T("RING") );
			SetComboBox( INDEX_ITEM_COMM_PORT,1,TRUE,&m_szCommPort,10,port_no );
			SetComboBox( INDEX_ITEM_COMM_DATA,1,TRUE,&m_szCommData,10,m_SearchInArray(m_szCommData,byte_size) );
			SetComboBox( INDEX_ITEM_COMM_STOP,1,TRUE,&m_szCommStop,10,stop_bit );
			SetComboBox( INDEX_ITEM_COMM_PARITY,1,TRUE,&m_szCommParity,10,parity );
			SetComboBox( INDEX_ITEM_COMM_BAUD,1,TRUE,&m_szCommBaud,10,m_SearchInArray(m_szCommBaud,baud_rate) );
		}
		break;
	case PORT_TCPIP:	//TCPIP
	case PORT_UDPIP:	//UDPIP
		{
			CString	szAddress;
			CString	szLocalPort;
			CString	szRemotePort;
			szAddress.Format( _T("%d.%d.%d.%d"),0x000000ff & (m_pDoc->m_dwIPAddress >> 24),
												0x000000ff & (m_pDoc->m_dwIPAddress >> 16),
												0x000000ff & (m_pDoc->m_dwIPAddress >>  8),
												0x000000ff & m_pDoc->m_dwIPAddress );
			szLocalPort.Format( _T("%d"),m_pDoc->m_dwServerPort );
			szRemotePort.Format( _T("%d"),m_pDoc->m_dwClientPort );
			InsertItem( INDEX_ITEM_COMM_IP_ADDR,_T("├目的地址") );		SetItemText( INDEX_ITEM_COMM_IP_ADDR,1,szAddress );
			InsertItem( INDEX_ITEM_COMM_SVR_PORT,_T("├本地端口") );	SetItemText( INDEX_ITEM_COMM_SVR_PORT,1,szLocalPort );
			InsertItem( INDEX_ITEM_COMM_CLT_PORT,_T("└目的端口") );	SetItemText( INDEX_ITEM_COMM_CLT_PORT,1,szRemotePort );
			SetEdit( INDEX_ITEM_COMM_IP_ADDR,1 );
			SetEdit( INDEX_ITEM_COMM_SVR_PORT,1 );
			SetEdit( INDEX_ITEM_COMM_CLT_PORT,1 );
		}
		break;
	default:;
	}
	int	nITEM_RELAY = GetItemCount();
	InsertItem( nITEM_RELAY,_T("数据转发") );
	SetComboBox( nITEM_RELAY,1,TRUE,&m_saTransActive,10,m_pDoc->m_bIpTransfer );
	switch( m_pDoc->m_bIpTransfer )
	{
	case RELAY_SERIAL:
		if( m_pDoc->m_nCommunicateMode != PORT_SERIAL )
		{
			int	port_no	= (( 0xfc000000 & m_pDoc->m_dwSerialParameter ) >> 26) - 1;
			int	byte_size	= (( 0x03c00000 & m_pDoc->m_dwSerialParameter ) >> 22);
			int	stop_bit	= (( 0x00300000 & m_pDoc->m_dwSerialParameter ) >> 20) - 1;
			int	parity	= (( 0x000c0000 & m_pDoc->m_dwSerialParameter ) >> 18);
			int	baud_rate	= (( 0x0003ffff & m_pDoc->m_dwSerialParameter ));
			if( port_no < 0 ) port_no = 0;
			if( byte_size <= 0 ) byte_size = 8;
			if( stop_bit < 0 ) stop_bit = 0;
			if( parity < 0 ) parity = 0;
			if( baud_rate < 0 ) baud_rate = 0;
			InsertItem( nITEM_RELAY+INDEX_ITEM_COMM_PORT,_T("├端口号") );
			InsertItem( nITEM_RELAY+INDEX_ITEM_COMM_DATA,_T("├数据位") );
			InsertItem( nITEM_RELAY+INDEX_ITEM_COMM_STOP,_T("├停止位") );
			InsertItem( nITEM_RELAY+INDEX_ITEM_COMM_PARITY,_T("├校验位") );
			InsertItem( nITEM_RELAY+INDEX_ITEM_COMM_BAUD,_T("└波特率") );
			SetComboBox( nITEM_RELAY+INDEX_ITEM_COMM_PORT,1,TRUE,&m_szCommPort,10,port_no );
			SetComboBox( nITEM_RELAY+INDEX_ITEM_COMM_DATA,1,TRUE,&m_szCommData,10,m_SearchInArray(m_szCommData,byte_size) );
			SetComboBox( nITEM_RELAY+INDEX_ITEM_COMM_STOP,1,TRUE,&m_szCommStop,10,stop_bit );
			SetComboBox( nITEM_RELAY+INDEX_ITEM_COMM_PARITY,1,TRUE,&m_szCommParity,10,parity );
			SetComboBox( nITEM_RELAY+INDEX_ITEM_COMM_BAUD,1,TRUE,&m_szCommBaud,10,m_SearchInArray(m_szCommBaud,baud_rate) );
		}
		break;
	case RELAY_TCPIP:
		{
			CString	szRemoteAddr;
			CString	szRemotePort;
			szRemoteAddr.Format( _T("%d.%d.%d.%d"),	0x000000ff & (m_pDoc->m_dwIpAddress2 >> 24),
													0x000000ff & (m_pDoc->m_dwIpAddress2 >> 16),
													0x000000ff & (m_pDoc->m_dwIpAddress2 >>  8),
													0x000000ff & m_pDoc->m_dwIpAddress2 );
			szRemotePort.Format( _T("%d"),m_pDoc->m_nIpPort2 );
			InsertItem( nITEM_RELAY+1,_T("├目的IP地址") );	SetItemText( nITEM_RELAY+1,1,szRemoteAddr );	SetEdit( nITEM_RELAY+1,1 );
			InsertItem( nITEM_RELAY+2,_T("└目的IP端口") );	SetItemText( nITEM_RELAY+2,1,szRemotePort );	SetEdit( nITEM_RELAY+2,1 );
		}
		break;
	case RELAY_UDPIP:
		{
			CString	szSourceAddr,szTargetAddr;
			CString	szSourcePort,szTargetPort;
			szSourceAddr.Format( _T("%d.%d.%d.%d"),	0x000000ff & (m_pDoc->m_dwIpAddress1 >> 24),
													0x000000ff & (m_pDoc->m_dwIpAddress1 >> 16),
													0x000000ff & (m_pDoc->m_dwIpAddress1 >>  8),
													0x000000ff & m_pDoc->m_dwIpAddress1 );
			szTargetAddr.Format( _T("%d.%d.%d.%d"),	0x000000ff & (m_pDoc->m_dwIpAddress2 >> 24),
													0x000000ff & (m_pDoc->m_dwIpAddress2 >> 16),
													0x000000ff & (m_pDoc->m_dwIpAddress2 >>  8),
													0x000000ff & m_pDoc->m_dwIpAddress2 );
			szSourcePort.Format( _T("%d"),m_pDoc->m_nIpPort1 );
			szTargetPort.Format( _T("%d"),m_pDoc->m_nIpPort2 );
			InsertItem( nITEM_RELAY+1,_T("├源IP地址") );	SetItemText( nITEM_RELAY+1,1,szSourceAddr );	SetEdit( nITEM_RELAY+1,1 );
			InsertItem( nITEM_RELAY+2,_T("├源IP端口") );	SetItemText( nITEM_RELAY+2,1,szSourcePort );	SetEdit( nITEM_RELAY+2,1 );
			InsertItem( nITEM_RELAY+3,_T("├目的IP地址") );	SetItemText( nITEM_RELAY+3,1,szTargetAddr );	SetEdit( nITEM_RELAY+3,1 );
			InsertItem( nITEM_RELAY+4,_T("└目的IP端口") );	SetItemText( nITEM_RELAY+4,1,szTargetPort );	SetEdit( nITEM_RELAY+4,1 );
		}
	case RELAY_DISABLE:
	default:;
	}
	m_Invalidate();
}

void CSignalListCtrl::m_ModifyConfig()
{
	int	nItemCount = GetItemCount()-1;
	for( int nItem = 0; nItem < nItemCount; nItem ++ )
	{
		m_ModifyConfig( nItem,1 );
	}
	m_ShowConfig();
}

void CSignalListCtrl::m_ModifyConfig(int nItem,int nSubItem)
{
	CString	szName = GetItemText( nItem,0 );
	CString	szValue = GetItemText( nItem,nSubItem );
	DWORD	b0 = 0,b1 = 0,b2 = 0,b3 = 0;
	int		nValue = 0;
	if( nItem == INDEX_ITEM_COMM_MODE )
	{
		nValue = m_SearchInArray( m_szCommMode,szValue );
		m_pDoc->m_nCommunicateMode = nValue + 1;
	}
	if( szName == _T("├端口号") ) {
		nValue = m_SearchInArray( m_szCommPort,szValue ) + 1;
		m_pDoc->m_dwSerialParameter &= 0x03ffffff;
		m_pDoc->m_dwSerialParameter |= 0xfc000000 & ( nValue << 26 );
	} else if( szName == _T("├数据位") ) {
		swscanf_s( szValue,_T("%d"),&nValue );
		m_pDoc->m_dwSerialParameter &= 0xfc3fffff;
		m_pDoc->m_dwSerialParameter |= 0x03c00000 & ( nValue << 22 );
	} else if( szName == _T("├停止位") ) {
		nValue = m_SearchInArray( m_szCommStop,szValue ) + 1;
		m_pDoc->m_dwSerialParameter &= 0xffcfffff;
		m_pDoc->m_dwSerialParameter |= 0x00300000 & ( nValue << 20 );
	} else if( szName == _T("├校验位") ) {
		nValue = m_SearchInArray( m_szCommParity,szValue );
		m_pDoc->m_dwSerialParameter &= 0xfff3ffff;
		m_pDoc->m_dwSerialParameter |= 0x000c0000 & ( nValue << 18 );
	} else if( szName == _T("└波特率") ) {
		swscanf_s( szValue,_T("%d"),&nValue );
		m_pDoc->m_dwSerialParameter &= 0xfffc0000;
		m_pDoc->m_dwSerialParameter |= 0x0003ffff & nValue;
	} else if( szName == _T("├目的地址") ) {
		swscanf_s( szValue,_T("%d.%d.%d.%d"),&b0,&b1,&b2,&b3 );
		m_pDoc->m_dwIPAddress = ( 0xff000000 & (b0 << 24) ) | ( 0x00ff0000 & (b1 << 16) ) |
								( 0x0000ff00 & (b2 <<  8) ) | ( 0x000000ff & b3 );
	} else if( szName == _T("├本地端口") ) {
		swscanf_s( szValue,_T("%d"),&m_pDoc->m_dwServerPort );
	} else if( szName == _T("└目的端口") ) {
		swscanf_s( szValue,_T("%d"),&m_pDoc->m_dwClientPort );
	} else if( szName == _T("数据转发") ) {
		m_pDoc->m_bIpTransfer = m_SearchInArray( m_saTransMode,szValue );
	} else if( szName == _T("├源IP地址") ) {
		swscanf_s( szValue,_T("%d.%d.%d.%d"),&b0,&b1,&b2,&b3 );
		m_pDoc->m_dwIpAddress1 = ( 0xff000000 & (b0 << 24) ) | ( 0x00ff0000 & (b1 << 16) ) |
								( 0x0000ff00 & (b2 <<  8) ) | ( 0x000000ff & b3 );
	} else if( szName == _T("├目的IP地址") ) {
		swscanf_s( szValue,_T("%d.%d.%d.%d"),&b0,&b1,&b2,&b3 );
		m_pDoc->m_dwIpAddress2 = ( 0xff000000 & (b0 << 24) ) | ( 0x00ff0000 & (b1 << 16) ) |
								( 0x0000ff00 & (b2 <<  8) ) | ( 0x000000ff & b3 );
	} else if( szName == _T("├源IP端口") ) {
		swscanf_s( szValue,_T("%d"),&m_pDoc->m_nIpPort1 );
	} else if( szName == _T("└目的IP端口") ) {
		swscanf_s( szValue,_T("%d"),&m_pDoc->m_nIpPort2 );
	}
}

void CSignalListCtrl::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	m_nItem = pNMLV->iItem;
	m_nSubItem = pNMLV->iSubItem;
	*pResult = 0;
}

int CSignalListCtrl::m_CurrentItem()	{ return m_nItem; }
int CSignalListCtrl::m_CurrentSubItem() { return m_nSubItem; }

void CSignalListCtrl::m_Invalidate()
{
	CRect	clientRect;
	this->GetClientRect(clientRect);
	this->InvalidateRect(clientRect);
}

