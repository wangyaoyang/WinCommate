// SignalListView.cpp : 实现文件
//

#include "stdafx.h"
#include "Commate.h"
#include "CommateDoc.h"
#include "SignalListView.h"
#include "RuntimeView.h"
#include ".\signallistview.h"
#include "232SettingDlg.h"


// CSignalListView

IMPLEMENT_DYNCREATE(CSignalListView, CSizingControlBarG)

CSignalListView::CSignalListView()
{
	m_occupied		= FALSE;
	m_oldParm		= 0;
	m_oldStatus		= 0;
	m_newStatus		= 0;
	m_pDoc			= NULL;
}

CSignalListView::~CSignalListView()
{
}

BEGIN_MESSAGE_MAP(CSignalListView, CSizingControlBarG)
//	ON_NOTIFY_REFLECT(NM_DBLCLK, OnNMDblclk)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SIGNAL_VIEW, OnNMDblclk)
	ON_WM_CHAR()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(WM_XLISTCTRL_COMBO_SELECTION, OnComboSelection)
	ON_REGISTERED_MESSAGE(WM_XLISTCTRL_EDIT_END, OnEditEnd)
	ON_REGISTERED_MESSAGE(WM_XLISTCTRL_CHECKBOX_CLICKED, OnCheckbox)
END_MESSAGE_MAP()


// CSignalListView 诊断

#ifdef _DEBUG
void CSignalListView::AssertValid() const
{
	CSizingControlBarG::AssertValid();
}

void CSignalListView::Dump(CDumpContext& dc) const
{
	CSizingControlBarG::Dump(dc);
}
#endif //_DEBUG


// CSignalListView 消息处理程序
CSignalListCtrl& CSignalListView::GetListCtrl()
{
	return m_ctrlList;
}

CProbeDoc* CSignalListView::GetDocument()
{
	return m_pDoc;
}

int CSignalListView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CSizingControlBarG::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_ctrlList.CreateEx(LVS_EX_INFOTIP|WS_EX_STATICEDGE,//,LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|WS_EX_CLIENTEDGE
		WS_CHILD|WS_VISIBLE|LVS_REPORT|LVS_SHOWSELALWAYS|LVS_SINGLESEL,
		CRect(0,0,0,0), this, IDC_LIST_SIGNAL_VIEW))
		return -1;

	return 0;
}

void CSignalListView::OnInitialUpdate(CProbeDoc& doc)
{
	CSizingControlBarG::OnInitialUpdate();

	m_pDoc = &doc;
	GetListCtrl().m_InitCtrl(doc);
	GetListCtrl().m_ShowConfig();
	GetListCtrl().m_ModifyConfig();
	SetTimer( 0x05040117,500,NULL );
}

void CSignalListView::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	//C232SettingDlg	dlg(this);
	//CProbeDoc*		pDoc = (CProbeDoc*) GetDocument();

	//if( pDoc->m_GetThread() == NULL ) return;
	//CCommunication&	comm = pDoc->m_GetThread()->m_GetComm();

	//dlg.m_nMode = pDoc->m_nCommunicateMode-1;
	//dlg.m_parameter = pDoc->m_dwSerialParameter;
	//dlg.m_dwIpAddress = pDoc->m_dwIPAddress;
	//dlg.m_nLocalPort = pDoc->m_dwServerPort;
	//dlg.m_nTcpipPort = pDoc->m_dwClientPort;
	//dlg.m_bIpTransfer = pDoc->m_bIpTransfer;
	//dlg.m_dwIpAddress1 = pDoc->m_dwIpAddress1;
	//dlg.m_dwIpAddress2 = pDoc->m_dwIpAddress2;
	//dlg.m_nIpPort1 = pDoc->m_nIpPort1;
	//dlg.m_nIpPort2 = pDoc->m_nIpPort2;
	//if( dlg.DoModal() == IDOK )
	//{
	//	int		nRetCode = 0;
	//	pDoc->m_nCommunicateMode = dlg.m_nMode+1;
	//	pDoc->m_dwSerialParameter = dlg.m_parameter;
	//	pDoc->m_dwIPAddress = dlg.m_dwIpAddress;
	//	pDoc->m_dwServerPort = dlg.m_nLocalPort;
	//	pDoc->m_dwClientPort = dlg.m_nTcpipPort;
	//	pDoc->m_bIpTransfer = dlg.m_bIpTransfer;
	//	pDoc->m_dwIpAddress1 = dlg.m_dwIpAddress1;
	//	pDoc->m_dwIpAddress2 = dlg.m_dwIpAddress2;
	//	pDoc->m_nIpPort1 = dlg.m_nIpPort1;
	//	pDoc->m_nIpPort2 = dlg.m_nIpPort2;
	//	nRetCode = comm.m_Open(	m_pDoc->m_nCommunicateMode,
	//							m_pDoc->m_dwSerialParameter,
	//							m_pDoc->m_dwIPAddress,
	//							m_pDoc->m_dwClientPort,
	//							m_pDoc->m_dwServerPort);
	//	if( nRetCode ) m_occupied = TRUE;
	//	else
	//	{
	//		this->MessageBox( "Failed !","Setting Serial Port:",MB_ICONSTOP );
	//		m_occupied = FALSE;
	//	}
	//}
	//else
	//{
	//	comm.m_Close();
	//	m_occupied = FALSE;
	//}
	//GetListCtrl().m_ShowConfig();
	*pResult = 0;
}

void CSignalListView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CProbeDoc*		pDoc = (CProbeDoc*) GetDocument();
	if( pDoc->m_GetThread() == NULL ) return;
	CCommunication&	comm = pDoc->m_GetThread()->m_GetComm();

	int		portReady	= m_occupied ? 1 : 0;
	LVITEM	lvItem;
	lvItem.iItem = 0;
	lvItem.iSubItem = 2;
	lvItem.mask = LVIF_IMAGE;
	lvItem.pszText = _T("");
	lvItem.iImage = portReady*2;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	GetListCtrl().SetItem( &lvItem );

	if( comm.m_GetCommMode() == PORT_TCPIP || comm.m_GetCommMode() == PORT_UDPIP )
	{
		CString	szNewAddress;
		unsigned char	b1 = (unsigned char)(0x00ff & (pDoc->m_dwIPAddress>>24));
		unsigned char	b2 = (unsigned char)(0x00ff & (pDoc->m_dwIPAddress>>16));
		unsigned char	b3 = (unsigned char)(0x00ff & (pDoc->m_dwIPAddress>>8));
		unsigned char	b4 = (unsigned char)(0x00ff & (pDoc->m_dwIPAddress>>0));
		unsigned int	nServerPort = pDoc->m_dwServerPort;
		unsigned int	nClientPort = pDoc->m_dwClientPort;

		szNewAddress.Format( _T("%d->%u.%u.%u.%u:%d"),nServerPort,b1,b2,b3,b4,nClientPort );
		GetListCtrl().SetItemText( INDEX_ITEM_COMM_MODE,2,szNewAddress,RGB(255,0,0),RGB(0,192,0) );
	}
	if( comm.m_GetCommMode() == PORT_SERIAL )
	{
		if( pDoc->m_dwSerialParameter != m_oldParm )
		{
			m_oldParm = pDoc->m_dwSerialParameter;
			CString	szComPara;
			int		port_no	= ( 0xfc000000 & m_oldParm ) >> 26;
			int		byte_size	= ( 0x03c00000 & m_oldParm ) >> 22;
			int		stop_bit	= ( 0x00300000 & m_oldParm ) >> 20;
			int		parity	= ( 0x000c0000 & m_oldParm ) >> 18;
			int		baud_rate	= ( 0x0003ffff & m_oldParm );

			szComPara.Format( _T("COM:%d,速率:%d,数据位:%d,停止位:%d,校验位:%d"),
								port_no,baud_rate,byte_size,stop_bit,parity );
			GetListCtrl().SetItemText( INDEX_ITEM_COMM_MODE,2,szComPara,RGB(255,0,0),RGB(0,192,0) );
		}
		m_newStatus = comm.m_GetSerial().m_ModemGetStatus();
		if( m_newStatus != m_oldStatus )
		{
			m_oldStatus = m_newStatus;
			int		portDSR		= m_oldStatus & MS_DSR_ON ? 1 : 0;
			int		portCTS		= m_oldStatus & MS_CTS_ON ? 1 : 0;
			int		portRLSD	= m_oldStatus & MS_RLSD_ON ? 1 : 0;
			int		portRING	= m_oldStatus & MS_RING_ON ? 1 : 0;
			COLORREF	cColor[2] = { RGB(0,0,0),RGB(255,0,0) };
			GetListCtrl().SetItemColors( INDEX_ITEM_COMM_DATA,2,cColor[!portDSR],cColor[portDSR] );
			GetListCtrl().SetItemColors( INDEX_ITEM_COMM_STOP,2,cColor[!portCTS],cColor[portCTS] );
			GetListCtrl().SetItemColors( INDEX_ITEM_COMM_PARITY,2,cColor[!portRLSD],cColor[portRLSD] );
			GetListCtrl().SetItemColors( INDEX_ITEM_COMM_BAUD,2,cColor[!portRING],cColor[portRING] );
			GetListCtrl().UpdateSubItem(INDEX_ITEM_COMM_DATA,2);
			GetListCtrl().UpdateSubItem(INDEX_ITEM_COMM_STOP,2);
			GetListCtrl().UpdateSubItem(INDEX_ITEM_COMM_PARITY,2);
			GetListCtrl().UpdateSubItem(INDEX_ITEM_COMM_BAUD,2);
		}
	}

	CSizingControlBarG::OnChar(nChar, nRepCnt, nFlags);
}

void CSignalListView::OnSize(UINT nType, int cx, int cy)
{
	CSizingControlBarG::OnSize(nType, cx, cy);

	m_ctrlList.MoveWindow(0,0,cx,cy);
}

void CSignalListView::ConfigModified(int nItem, int nSubItem)
{
	if (nItem >= 0 && nSubItem >= 0)
	{
		CString strText = m_ctrlList.GetItemText(nItem, nSubItem);
		COLORREF crText, crBackground;
		m_ctrlList.GetItemColors(nItem, nSubItem, crText, crBackground);
		if (m_ctrlList.GetModified(nItem, nSubItem))
		{
			m_ctrlList.SetItemText(nItem, nSubItem, strText, RGB(255,0,0), crBackground);
			m_ctrlList.m_ModifyConfig();
		}
		else m_ctrlList.SetItemText(nItem, nSubItem, strText, RGB(0,0,0), crBackground);
	}
}

LRESULT CSignalListView::OnEditEnd(WPARAM nItem, LPARAM nSubItem)
{
	ConfigModified( (int) nItem,(int) nSubItem );
	return 0;
}

LRESULT CSignalListView::OnComboSelection(WPARAM nItem, LPARAM nSubItem)
{
	ConfigModified( (int) nItem,(int) nSubItem );
	return 0;
}

LRESULT CSignalListView::OnCheckbox(WPARAM wParam, LPARAM lParam)
{
	int	nItem = (int) wParam;
	int	nSubItem = (int) lParam;
	if (nItem >= 0 && nSubItem >= 0)
	{
		COLORREF crText, crBackground;
		m_ctrlList.GetItemColors(nItem, nSubItem, crText, crBackground);
		if (m_ctrlList.GetModified(nItem, nSubItem))
		{
			// subitem was modified - color it red
			CCommunication&	comm = m_pDoc->m_GetThread()->m_GetComm();
			if( m_ctrlList.GetCheckbox( nItem,nSubItem ) )
			{
				CWaitCursor	wait;
				comm.m_SetIpTransfer(	m_pDoc->m_bIpTransfer,
										m_pDoc->m_dwIpAddress1,
										m_pDoc->m_nIpPort1,
										m_pDoc->m_dwIpAddress2,
										m_pDoc->m_nIpPort2);
				int		nRetCode = comm.m_Open(m_pDoc);
				if( nRetCode ) m_occupied = TRUE; else m_occupied = FALSE;
			}
			else
			{
				comm.m_Close();
				m_occupied = FALSE;
			}
			m_ctrlList.SetCheckbox( nItem,0,m_occupied );
			m_ctrlList.SetItemColors( nItem,1,RGB(0,0,0),m_occupied ? RGB(0,192,0) : RGB(192,0,0) );
			m_ctrlList.SetItemText( nItem,1,m_occupied ? _T("已连接") : _T("已断开") );
		}
	}
	return 0;
}

