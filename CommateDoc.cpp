// CommateDoc.cpp :  CProbeDoc 类的实现
//

#include "stdafx.h"
#include "Commate.h"

#include "CommateDoc.h"
#include "SourceView.h"
#include "ShowDataView.h"
#include "RuntimeView.h"
#include "SignalListView.h"
#include "CalculaterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define		PROBE_CONFIG_FILENAME		"Commate.ini"

// CProbeDoc

IMPLEMENT_DYNCREATE(CProbeDoc, CDocument)

BEGIN_MESSAGE_MAP(CProbeDoc, CDocument)
	ON_COMMAND(ID_SEND_VIEW_CALCULATER, OnSendViewCalculater)
	ON_COMMAND(ID_POPUP_EXEC_LOAD, &CProbeDoc::OnPopupExecLoad)
	ON_COMMAND(ID_POPUP_EXEC_EXECUTE, &CProbeDoc::OnPopupExecExecute)
	ON_COMMAND(ID_POPUP_EXEC_STOP, &CProbeDoc::OnPopupExecStop)
	ON_COMMAND(ID_POPUP_EXEC_SEL_ALL, &CProbeDoc::OnPopupExecSelAll)
	ON_COMMAND(ID_POPUP_EXEC_CLEAR, &CProbeDoc::OnPopupExecClear)
	ON_COMMAND(ID_RECV_LIST_VIEW_HEX, &CProbeDoc::OnRecvListViewHex)
	ON_UPDATE_COMMAND_UI(ID_RECV_LIST_VIEW_HEX, &CProbeDoc::OnUpdateRecvListViewHex)
	ON_COMMAND(ID_POPUP_EXEC_DEBUG, &CProbeDoc::OnPopupExecDebug)
	ON_UPDATE_COMMAND_UI(ID_POPUP_EXEC_DEBUG, &CProbeDoc::OnUpdatePopupExecDebug)
	ON_UPDATE_COMMAND_UI(ID_POPUP_EXEC_SEL_ALL, &CProbeDoc::OnUpdatePopupExecSelAll)
	ON_UPDATE_COMMAND_UI(ID_POPUP_EXEC_CLEAR, &CProbeDoc::OnUpdatePopupExecClear)
	ON_COMMAND(ID_POPUP_EXEC_CLEAR_TRACE, &CProbeDoc::OnPopupExecClearTrace)
	ON_UPDATE_COMMAND_UI(ID_POPUP_EXEC_CLEAR_TRACE, &CProbeDoc::OnUpdatePopupExecClearTrace)
END_MESSAGE_MAP()


// CProbeDoc 构造/析构

CProbeDoc::CProbeDoc()
{
	m_nCommunicateMode = 0;
	m_dwSerialParameter = 0;
	m_dwIPAddress = 0;
	m_dwServerPort = 0;
	m_dwClientPort = 0;
	m_bIpTransfer = 0;
	m_dwIpAddress1 = 0;
	m_dwIpAddress2 = 0;
	m_nIpPort1 = 0;
	m_nIpPort2 = 0;
	m_pViewSignal = NULL;
	m_pViewRuntime = NULL;
	m_pViewShowData = NULL;
	m_threadExec = NULL;
	m_bRunning = false;
	m_bEcho	= false;
	m_bDisp = DISPLAY_ASCII;
}

CProbeDoc::~CProbeDoc()
{
	m_ClearThread();
	while( !m_aFilters.IsEmpty() ) {
		delete (CFilter*) m_aFilters.RemoveHead();
	}
	m_ClearEthList();
}

BOOL CProbeDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	m_ClearThread();
	if( m_CreateThread() ) return TRUE;
	return FALSE;
}

HANDLE CProbeDoc::m_CreateThread()
{
	m_threadExec = new CWinThreadExec();
	m_threadExec->CreateThread( CREATE_SUSPENDED );
	m_bRunning = false;
	return m_threadExec->m_hThread;
}

void CProbeDoc::m_ClearThread()
{
	if( m_threadExec && m_threadExec->m_hThread )
	{
		m_threadExec->SuspendThread();
		TerminateThread( m_threadExec->m_hThread,0 );
	}
	if( m_threadExec )
	{
		delete m_threadExec;
		m_threadExec = NULL;
	}
}

CWinThreadExec* CProbeDoc::m_GetThread()
{
	return m_threadExec;
}

// CProbeDoc 序列化

CSourceView* CProbeDoc::m_GetSourceView()
{
	for( POSITION pos = m_viewList.GetHeadPosition(); pos; m_viewList.GetNext(pos) )
	{
		CView*	pView = (CView*) m_viewList.GetAt(pos);
		if( pView->IsKindOf(RUNTIME_CLASS(CSourceView)) )
			return (CSourceView*)pView;
	}
	return NULL;
}

CShowDataView* CProbeDoc::m_GetShowDataView()
{
	return m_pViewShowData;
}

CSignalListView* CProbeDoc::m_GetSignalView()
{
	return m_pViewSignal;
}

CRuntimeView* CProbeDoc::m_GetRuntimeView()
{
	return m_pViewRuntime;
}

void CProbeDoc::Serialize(CArchive& ar)
{
	CString				szConf = ar.GetFile()->GetFilePath() + _T(".cfg");
	CString				szParameter;
	CFile				config;

	if (ar.IsStoring())
	{
		if( config.Open( szConf,CFile::modeCreate|CFile::modeWrite ) )
		{
			CArchive	arc(&config,CArchive::store );

			int	nSerialPort		= ( 0xfc000000 & m_dwSerialParameter ) >> 26;	//xxxx_xx00 0000_0000 0000_0000 0000_0000
			int	nSerialData		= ( 0x03c00000 & m_dwSerialParameter ) >> 22;	//0000_00xx xx00_0000 0000_0000 0000_0000
			int	nSerialStop		= ( 0x00300000 & m_dwSerialParameter ) >> 20;	//0000_0000 00xx_0000 0000_0000 0000_0000
			int	nSerialParity		= ( 0x000c0000 & m_dwSerialParameter ) >> 18;	//0000_0000 0000_xx00 0000_0000 0000_0000
			int	nSerialBaud		= ( 0x0003ffff & m_dwSerialParameter );		//0000_0000 0000_00xx xxxx_xxxx xxxx_xxxx
			szParameter.Format( _T("%d\t : Communication mode\r\n"),m_nCommunicateMode );	arc.WriteString( szParameter );
			szParameter.Format( _T("%d\t : Serail port number\r\n"),nSerialPort );			arc.WriteString( szParameter );
			szParameter.Format( _T("%d\t : Serial port baud\r\n"),nSerialBaud );			arc.WriteString( szParameter );
			szParameter.Format( _T("%d\t : Serial data bits\r\n"),nSerialData );			arc.WriteString( szParameter );
			szParameter.Format( _T("%d\t : Serial stop bits\r\n"),nSerialStop );			arc.WriteString( szParameter );
			szParameter.Format( _T("%d\t : Serial parity bits\r\n"),nSerialParity );		arc.WriteString( szParameter );
			///////////////////////
			szParameter.Format( _T("%d\t : IP Address\r\n"),m_dwIPAddress );				arc.WriteString( szParameter );
			szParameter.Format( _T("%d\t : Local port\r\n"),m_dwServerPort );				arc.WriteString( szParameter );
			szParameter.Format( _T("%d\t : Remote port\r\n"),m_dwClientPort );				arc.WriteString( szParameter );
			szParameter.Format( _T("%d\t : Enable Transfer\r\n"),m_bIpTransfer );			arc.WriteString( szParameter );
			szParameter.Format( _T("%d\t : Trans source IP\r\n"),m_dwIpAddress1 );			arc.WriteString( szParameter );
			szParameter.Format( _T("%d\t : Trans source Port\r\n"),m_nIpPort1 );			arc.WriteString( szParameter );
			szParameter.Format( _T("%d\t : Trans target IP\r\n"),m_dwIpAddress2 );			arc.WriteString( szParameter );
			szParameter.Format( _T("%d\t : Trans target Port\r\n"),m_nIpPort2 );			arc.WriteString( szParameter );
			szParameter.Format( _T("%d\t : Interaction Mode\r\n"),m_bEcho );				arc.WriteString( szParameter );
			szParameter.Format( _T("%d\t : AsciiDisplay Mode\r\n"),m_bDisp );				arc.WriteString( szParameter );
			for( POSITION pos = m_aFilters.GetHeadPosition(); pos; m_aFilters.GetNext(pos) )
			{
				CFilter*	filter = (CFilter*) m_aFilters.GetAt(pos);
				DWORD		b0 = 0x000000ff & (filter->m_dwIP >> 24);
				DWORD		b1 = 0x000000ff & (filter->m_dwIP >> 16);
				DWORD		b2 = 0x000000ff & (filter->m_dwIP >> 8);
				DWORD		b3 = 0x000000ff & (filter->m_dwIP);
				szParameter.Format( _T("%d.%d.%d.%d\t : IP Filter\r\n"),b0,b1,b2,b3 );
				arc.WriteString( szParameter );
			}
		}
	}
	else
	{
		if( config.Open( szConf,CFile::modeRead ) )
		{
			CArchive	arc(&config,CArchive::load );

			int	nSerialPort		= 0;
			int	nSerialBaud		= 0;
			int	nSerialData		= 0;
			int	nSerialStop		= 0;
			int	nSerialParity	= 0;
			int	nValue			= 0;
			arc.ReadString( szParameter );	swscanf_s( szParameter,_T("%d"),&m_nCommunicateMode );
			arc.ReadString( szParameter );	swscanf_s( szParameter,_T("%d"),&nSerialPort );
			arc.ReadString( szParameter );	swscanf_s( szParameter,_T("%d"),&nSerialBaud );
			arc.ReadString( szParameter );	swscanf_s( szParameter,_T("%d"),&nSerialData );
			arc.ReadString( szParameter );	swscanf_s( szParameter,_T("%d"),&nSerialStop );
			arc.ReadString( szParameter );	swscanf_s( szParameter,_T("%d"),&nSerialParity );
			m_dwSerialParameter =		(nSerialPort		& 0x0000003f) << 26	|
									(nSerialData		& 0x0000000f) << 22	|
									(nSerialStop		& 0x00000003) << 20	|
									(nSerialParity	& 0x00000003) << 18	|
									(nSerialBaud		& 0x000fffff);
			////////////////////////////////////////////////////////////////
			arc.ReadString( szParameter );	swscanf_s( szParameter,_T("%d"),&m_dwIPAddress );
			arc.ReadString( szParameter );	swscanf_s( szParameter,_T("%d"),&m_dwServerPort );
			arc.ReadString( szParameter );	swscanf_s( szParameter,_T("%d"),&m_dwClientPort );
			arc.ReadString( szParameter );	swscanf_s( szParameter,_T("%d"),&m_bIpTransfer );
			arc.ReadString( szParameter );	swscanf_s( szParameter,_T("%d"),&m_dwIpAddress1 );
			arc.ReadString( szParameter );	swscanf_s( szParameter,_T("%d"),&m_nIpPort1 );
			arc.ReadString( szParameter );	swscanf_s( szParameter,_T("%d"),&m_dwIpAddress2 );
			arc.ReadString( szParameter );	swscanf_s( szParameter,_T("%d"),&m_nIpPort2 );
			arc.ReadString( szParameter );	swscanf_s( szParameter,_T("%d"),&nValue );	m_bEcho = nValue ? true : false;
			arc.ReadString( szParameter );	swscanf_s( szParameter,_T("%d"),&nValue );	m_bDisp = nValue ? true : false;
			while( arc.ReadString( szParameter ) )
			{
				DWORD	b0 = 0,b1 = 0,b2 = 0,b3 = 0,dwIP = 0;
				swscanf_s( szParameter,_T("%d.%d.%d.%d"),&b0,&b1,&b2,&b3 );
				dwIP = (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;
				CFilter*	filter = new CFilter(dwIP);
				m_aFilters.AddTail( filter );
			}
		}
		m_ClearThread();
		m_CreateThread();
	}
	if( m_GetSourceView() )
		m_GetSourceView()->Serialize(ar);
}


// CProbeDoc 诊断

#ifdef _DEBUG
void CProbeDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CProbeDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CProbeDoc 命令

void CProbeDoc::OnSendViewCalculater()
{
	CCalculaterDlg	dlg;
	dlg.DoModal();
}

void CProbeDoc::OnPopupExecLoad()
{
	if( this->m_GetSourceView() && this->m_pViewRuntime )
	{
		if( m_threadExec )
			m_threadExec->m_Attach( this );
		this->m_GetSourceView()->m_LoadRuntime(
			this->m_pViewRuntime->GetListCtrl() );
	}
}

void CProbeDoc::OnPopupExecExecute()
{
	HANDLE hEventBreak = CreateEvent( NULL,TRUE,TRUE,TRACER_EXEC_BREAK_EVENT );
	SetEvent( hEventBreak );
	CloseHandle( hEventBreak );
	if( m_threadExec && m_threadExec->m_hThread )
	{
		m_threadExec->LoadProgram();
		m_threadExec->ResumeThread();
		m_bRunning = true;
	}
}

void CProbeDoc::OnPopupExecStop()
{
	if( m_threadExec && m_threadExec->m_hThread )
	{
		m_threadExec->SuspendThread();
		m_bRunning = false;
	}
}

void CProbeDoc::OnPopupExecDebug()
{
	if (m_threadExec) {
		m_threadExec->m_bDebug = !m_threadExec->m_bDebug;
		CMenu*	pMenu = AfxGetApp()->m_pMainWnd->GetMenu();
		UINT	uCheck = pMenu->CheckMenuItem(ID_POPUP_EXEC_DEBUG,
			m_threadExec->m_bDebug ? MF_CHECKED:MF_UNCHECKED);
	}
}

void CProbeDoc::OnPopupExecSelAll()
{
	int	nItem = m_pViewRuntime->GetCurrItem();
	if (nItem >= 0) {
		CListCtrl&	ctrl = m_pViewRuntime->GetListCtrl();
	
		ctrl.SetItem( nItem,0,LVIF_IMAGE,_T(""),1,0,0,0 );
		HANDLE hEventBreak = CreateEvent( NULL,TRUE,TRUE,TRACER_EXEC_BREAK_EVENT );
		SetEvent( hEventBreak );
		ResetEvent( hEventBreak );
		CloseHandle( hEventBreak );
	}
}

void CProbeDoc::OnPopupExecClear()
{
	int	nItem = m_pViewRuntime->GetCurrItem();
	if (nItem >= 0) {
		CListCtrl&	ctrl = m_pViewRuntime->GetListCtrl();
	
		ctrl.SetItem( nItem,0,LVIF_IMAGE,_T(""),0,0,0,0 );
		HANDLE hEventBreak = CreateEvent( NULL,TRUE,TRUE,TRACER_EXEC_BREAK_EVENT );
		SetEvent( hEventBreak );
		CloseHandle( hEventBreak );
	}
}

void CProbeDoc::OnPopupExecClearTrace()
{
	CListCtrl&	ctrl = m_pViewRuntime->GetListCtrl();
	int			nItem = ctrl.GetItemCount();
	for (int i = 0; i < nItem; i ++) {
		CString	szIndex = ctrl.GetItemText(i,0);
		if (szIndex.Find(_T("■■■■")) < 0) {
			szIndex.Format(_T("%d"),i+1);
			ctrl.SetItemText(i,0,szIndex);
		}
	}
}

void CProbeDoc::OnRecvListViewHex()
{
	m_bDisp = !m_bDisp;

	CShowDataView*	pView = this->m_GetShowDataView();
	if( pView )
		pView->m_Refresh( m_bDisp );
}

void CProbeDoc::OnUpdateRecvListViewHex(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_bDisp );
}

void CProbeDoc::OnUpdatePopupExecDebug(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_threadExec && m_threadExec->m_bDebug );
}

void CProbeDoc::OnUpdatePopupExecSelAll(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( m_threadExec && m_threadExec->m_bDebug );
}


void CProbeDoc::OnUpdatePopupExecClear(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( m_threadExec && m_threadExec->m_bDebug );
}

void CProbeDoc::OnUpdatePopupExecClearTrace(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( m_threadExec && m_threadExec->m_bDebug );
}

#define	PARSE_STRING(head, src, sep)	do {\
	head = CutStringHead(src, sep);\
	if (head.IsEmpty()) return NULL;\
} while (0)

CNetworkIf* CProbeDoc::m_EthListAddFromString(CString info) {
	wchar_t sep = ' ';
	CString	szIdx, szMet, szMtu, szStat, szName;
	int nIdx = 0, nMet = 0, nMtu = 0, nStat = 0;

	PARSE_STRING(szIdx, info, sep);
	PARSE_STRING(szMet, info, sep);
	PARSE_STRING(szMtu, info, sep);
	PARSE_STRING(szStat, info, sep);
	PARSE_STRING(szName, info, sep);
	swscanf(szIdx, _T("%d"), &nIdx);
	swscanf(szMet, _T("%d"), &nMet);
	swscanf(szMtu, _T("%d"), &nMtu);
	nStat = (szStat == _T("connected")) ? 1 : 0;
	if (nIdx > 0 && nMtu > 0 && nStat) {
		CNetworkIf*	eth = new CNetworkIf(nIdx, nMet, nMtu, nStat);
		eth->m_szName = szName;
		this->m_listEth.AddTail(eth);
		return eth;
	}
	return NULL;
}

int	CProbeDoc::m_EthListIfs(void) {
	CString	cmd = _T("netsh interface ipv4 show interface"), echo;
	CStringList	echolist;
	int nIfs = 0;

	this->m_ClearEthList();
	if (ShellExec(cmd.GetBuffer(), echolist, echo)) {
		while (!echolist.IsEmpty()) {
			CString	szIf = echolist.RemoveHead();
			if (this->m_EthListAddFromString(szIf)) nIfs ++;
		}
	}
	return nIfs;
}

int CProbeDoc::m_EthListToCombBox(CComboBox& combo) {
	if (this->m_listEth.IsEmpty()) return 0;
	for (POSITION pos = this->m_listEth.GetHeadPosition();
		pos != NULL; this->m_listEth.GetNext(pos)) {
			CNetworkIf* eth = (CNetworkIf*) this->m_listEth.GetAt(pos);
			if (eth) combo.AddString(eth->m_szName);
	}
	return this->m_listEth.GetCount();
}

void CProbeDoc::m_ClearEthList(void) {
	while (!this->m_listEth.IsEmpty()) {
		CNetworkIf*	eth = (CNetworkIf*) this->m_listEth.RemoveHead();
		delete eth;
	}
}

