// ShowDataView.cpp : 实现文件
//

#include "stdafx.h"
#include "Commate.h"
#include "ShowDataView.h"
#include "SignalListView.h"
#include "RuntimeView.h"
#include ".\showdataview.h"
#include "RecvDataDlg.h"

// CShowDataView

#define	COLOR_0		RGB(255,255,255)
#define	COLOR_1		RGB(192,192,192)
#define	COLOR_2		RGB(255,0,0)
#define	COLOR_3		RGB(0,255,0)
#define	COLOR_4		RGB(0,0,255)
#define	COLOR_5		RGB(255,255,0)
#define	COLOR_6		RGB(255,0,255)
#define	COLOR_7		RGB(0,255,255)
#define	COLOR_BACK	RGB(0,96,0)
#define	FONT_0		180
#define	FONT_1		160
#define	FONT_2		150

IMPLEMENT_DYNCREATE(CShowDataView, CRichEditView)

CShowDataView::CShowDataView()
	: m_Brush( RGB(96,96,0) )
{
	m_pDoc				= NULL;
	m_dispMode			= DISPLAY_HEX;
	m_nIdelCounter		= 128;
	m_counter			= 0;
	m_maxline			= 0;
	m_current			= 0;
	m_bCaretPosChanged	= false;
	m_cColorSend		= 0;
	m_cColorRecv		= 0;
	m_nEffectHex		= 0;
	memset( m_buffer,0x00,RECV_BUFF_SIZE );
}

CShowDataView::~CShowDataView()
{
	m_ClearHistory();
}

BEGIN_MESSAGE_MAP(CShowDataView, CRichEditView)
	ON_WM_CONTEXTMENU()
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_COMMAND(ID_RECV_LIST_VIEW_HEX, OnRecvListViewHex)
	ON_COMMAND(ID_RECV_LIST_VIEW_ASCII, OnRecvListViewAscii)
	ON_COMMAND(ID_RECV_LIST_VIEW_CLEAR, OnRecvListViewClear)
	ON_COMMAND(ID_RECV_LIST_VIEW_REVIEW, OnRecvListViewReview)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_RECV_LIST_VIEW_SAVELOG, OnRecvListViewSavelog)
	ON_WM_CREATE()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
END_MESSAGE_MAP()


// CShowDataView 诊断

#ifdef _DEBUG
void CShowDataView::AssertValid() const
{
	CRichEditView::AssertValid();
}

void CShowDataView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}
#endif //_DEBUG


// CShowDataView 消息处理程序
CProbeDoc* CShowDataView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CProbeDoc)));
	return (CProbeDoc*)m_pDocument;
}

void CShowDataView::OnInitialUpdate()
{
	CRichEditView::OnInitialUpdate();

	CFont		newFont;

	newFont.CreateFont(	20,0,0,0,FW_BOLD,FALSE,FALSE,0,
						ANSI_CHARSET,OUT_DEFAULT_PRECIS,
						CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
						DEFAULT_PITCH | FF_DONTCARE, "宋体 & 新宋体" );

	CRichEditCtrl&	ctrlEdit = GetRichEditCtrl();
	ctrlEdit.SetBackgroundColor( FALSE,RGB(0,96,0) );
	ctrlEdit.SetReadOnly(TRUE);
	ctrlEdit.SetFont( &newFont );
	m_pDoc = (CProbeDoc*) GetDocument();
	SetTimer( 0x99050313,100,NULL );
}

void CShowDataView::OnTimer(UINT nIDEvent)
{
	CProbeDoc*			pDoc = m_pDoc;
	CSignalListView*	pViewSignal = (CSignalListView*)pDoc->GetViewAt(VIEW_POSITION_SIGNAL);
	CRuntimeView*		pViewRuntime = (CRuntimeView*)pDoc->GetViewAt(VIEW_POSITION_RUNTIME);
	HANDLE				idlEvent = OpenEvent(EVENT_ALL_ACCESS,FALSE,TRACER_EXEREADING_EVENT);
	HANDLE				newEvent = OpenEvent(EVENT_ALL_ACCESS,FALSE,TRACER_EXECUTABLE_EVENT);
	//	newEvent 复位后，本线程有权访问CWinThreadExec成员m_buffer及m_counter
	//	newEvent 置位后，CWinThreadExec线程有权访问其成员m_buffer及m_counter
	if( idlEvent && WaitForSingleObject( idlEvent,0 ) != WAIT_OBJECT_0 )
	{
		if( newEvent ) CloseHandle( newEvent );
		if( idlEvent ) CloseHandle( idlEvent );

		CRichEditView::OnTimer(nIDEvent);
		return;
	}
	
	pViewSignal->SendMessage( WM_CHAR,0,0 );
	CCommunication&	comm = pViewRuntime->m_GetThread().m_GetComm();
	BOOL	result = comm.m_Recv(m_buffer,m_counter=RECV_BUFF_SIZE);
	if( result && m_counter > 0 )
	{
		m_nIdelCounter = 1;
		if( pViewRuntime->m_threadExec.m_counter + m_counter > RECV_BUFF_SIZE )
			pViewRuntime->m_threadExec.m_counter = 0;
		memcpy( pViewRuntime->m_threadExec.m_buffer+
				pViewRuntime->m_threadExec.m_counter, m_buffer,m_counter );
		pViewRuntime->m_threadExec.m_counter += m_counter;
		m_Append( false,m_buffer,m_counter );	//false 表示收数据
	}
	else
	{
		if( m_nIdelCounter ) m_nIdelCounter ++;
		if( m_nIdelCounter > 1 && newEvent )
		{	
			SetEvent( newEvent );		//通知CWinThreadExec线程有新数据
			m_nIdelCounter = 0;
			Sleep(100);
		}
	}
	if( newEvent ) CloseHandle( newEvent );
	if( idlEvent ) CloseHandle( idlEvent );

	CRichEditView::OnTimer(nIDEvent);
}

void CShowDataView::m_SetColor( COLORREF cText,COLORREF cBackground )
{
	CRichEditCtrl&	ctrlEdit = GetRichEditCtrl();
	CHARFORMAT2		cf;
	ctrlEdit.GetDefaultCharFormat(cf);
	cf.dwMask		= CFM_SIZE | CFM_BOLD | CFM_COLOR | CFM_BACKCOLOR;
	cf.dwEffects	= CFE_BOLD;
	cf.crBackColor	= cBackground;
	cf.crTextColor	= cText;
	cf.yHeight		= FONT_0;
	SetCharFormat( cf );
}

void CShowDataView::m_SetEffect( char cEffect )
{
	CRichEditCtrl&	ctrlEdit = GetRichEditCtrl();
	CHARFORMAT2		cf;
	ctrlEdit.GetDefaultCharFormat(cf);
	cf.dwMask		= CFM_UNDERLINE;
	cf.dwEffects	= cEffect ? CFE_UNDERLINE : 0;
	SetCharFormat( cf );
}

void CShowDataView::m_AppendText( CString szText )
{
	if( szText == "" ) return;
	CRichEditCtrl&	ctrlEdit = GetRichEditCtrl();
	
	ctrlEdit.SetReadOnly(FALSE);
	m_ctrlEditTemp.SetWindowText(szText);
	m_ctrlEditTemp.SetSel( 0,-1 );
	m_ctrlEditTemp.Copy();
	ctrlEdit.Paste();
	ctrlEdit.SetReadOnly(TRUE);
	m_ctrlEditTemp.Clear();
}

void CShowDataView::m_Display( DWORD nPosition )
{
	POSITION	position = NULL;
	char		cEffectHex[2]	= {	1,2 };
	COLORREF	oColorText[6]	= {	COLOR_0,COLOR_2,COLOR_3,COLOR_5,COLOR_6,COLOR_7 };
	COLORREF	oColorBack[2]	= { RGB(128,128,128),RGB(192,192,192) };
	switch( nPosition )
	{
	case 0:	position = m_oHistory.GetHeadPosition();	break;
	case 1:	position = m_oHistory.GetTailPosition();	break;
	default:	return;
	}
	for( ; position != NULL; m_oHistory.GetNext(position) )
	{
		CString		szAppend;
		CRecvBuff*	pRecvBuff = (CRecvBuff*)m_oHistory.GetAt(position);
		bool		bSend = pRecvBuff->m_IsSend();
		DWORD		nCounter = pRecvBuff->m_GetLength();
		char*		sBuffer = pRecvBuff->m_GetBuffer();

		if( bSend )	{ m_cColorSend ++;	m_cColorSend %= 6; szAppend = "|"; }
		
		m_SetColor( oColorText[m_cColorSend],COLOR_BACK );
		m_SetEffect( !bSend );
		for( DWORD i=0;i<nCounter;i++ )
		{
			CString		szOneChar;
			short		cPrev	= 0x00ff&sBuffer[i-1];
			short		cData	= 0x00ff&sBuffer[i];
			short		cNext	= 0x00ff&sBuffer[i+1];
			
			switch( m_dispMode )
			{
			case DISPLAY_HEX:
				szOneChar.Format( "%02x ",cData );
				szAppend += szOneChar;
				break;
			case DISPLAY_ASCII:
				if( (cData == 0x0D && cNext == 0x0A) ||
					cData >= 0x20 || cData == 0x09 )
				{	//可见ASCII字符
					szOneChar.Format( "%c",cData );
					szAppend += szOneChar;
				}
				else if( cPrev == 0x0D && cData == 0x0A )
				{
					szOneChar.Format( "%c↓",cData );
					szAppend += szOneChar;
					m_AppendText( szAppend );	szAppend = "";
				}
				else
				{	//特殊字符处理
					m_AppendText( szAppend );	szAppend = "";
					m_nEffectHex ++;
					m_nEffectHex %= 2;
					szOneChar.Format( "%02x",cData );
					m_SetColor( oColorText[m_cColorSend],oColorBack[m_nEffectHex] );
					m_AppendText( szOneChar );
					m_SetColor( oColorText[m_cColorSend],COLOR_BACK );
				}
				break;
			default:;
			}
		}
		m_AppendText( szAppend );
	}
}

void CShowDataView::m_ClearHistory()
{
	while( !m_oHistory.IsEmpty() )
	{
		CRecvBuff*	p = (CRecvBuff*)m_oHistory.RemoveHead();
		delete p;	p = NULL;
	}
}

int CShowDataView::m_Append( bool bSendRecv,LPSTR pData,DWORD nCounter )
{
//	if( nCounter > 10240 ) return 0;
	CRecvBuff*	pRecvBuff = new CRecvBuff;
	long		nSerial = m_oHistory.GetCount() + 1;

	pRecvBuff->m_Append( bSendRecv,pData,nCounter,nSerial );
	m_oHistory.AddTail( pRecvBuff );
	if( m_bCaretPosChanged )	m_Refresh( m_dispMode );
	else						m_Display( 1 );
	return m_oHistory.GetCount();
}

void CShowDataView::m_Refresh(BOOL bHexAscii)
{
	CRichEditCtrl&	ctrlEdit = GetRichEditCtrl();
	m_dispMode = bHexAscii;
	ctrlEdit.SetWindowText( "" );
	m_bCaretPosChanged = false;
	m_Display( 0 );
}

void CShowDataView::OnRecvListViewHex()
{
	m_Refresh( DISPLAY_HEX );
}

void CShowDataView::OnRecvListViewAscii()
{
	m_Refresh( DISPLAY_ASCII );
}

void CShowDataView::OnRecvListViewClear()
{
	CRichEditCtrl&	ctrlEdit = GetRichEditCtrl();
	ctrlEdit.SetWindowText( "" );
	m_bCaretPosChanged = false;
	m_ClearHistory();
}

void CShowDataView::OnRecvListViewReview()
{
	CRichEditCtrl&	ctrlEdit = GetRichEditCtrl();
	ctrlEdit.SetSel(0,-1);
}

void CShowDataView::OnEditCopy()
{
	CRichEditCtrl&	ctrlEdit = GetRichEditCtrl();
	ctrlEdit.Copy();
}

void CShowDataView::OnRecvListViewSavelog()
{
	static char BASED_CODE szFilter[] = "串口数据记录文件 (*.log)|*.log||";
	CFileDialog	fdlg(FALSE,"*.log","Wyy232",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);
	CStdioFile	flog;
	CRichEditCtrl&	ctrlEdit = GetRichEditCtrl();

	if( fdlg.DoModal() != IDOK ) return;
	CString		pathname = fdlg.GetPathName();
	if( flog.Open( pathname,CFile::modeCreate|CFile::modeWrite|CFile::typeText ) )
	{
		CProbeDoc*			pDoc = (CProbeDoc*) GetDocument();
		CSignalListView*	pViewSignal = (CSignalListView*)pDoc->GetViewAt(VIEW_POSITION_SIGNAL);
		CString				szParameter = "端口参数:\r\n";
		CString				szOutput	= "发送数据:\r\n";
		CString				szInput		= "接收数据:\r\n";
		CString				szText;
		int					n = 0,nItem = 0;
		nItem = pViewSignal->GetListCtrl().GetItemCount();
		for( n = 0; n < nItem; n ++ )
			szParameter +=	pViewSignal->GetListCtrl().GetItemText( n,1 ) + "=" +
							pViewSignal->GetListCtrl().GetItemText( n,2 ) + "\r\n";
//		nItem = pViewOutput->GetListCtrl().GetItemCount();
//		for( n = 0; n < nItem; n ++ )
//			szOutput += pViewOutput->GetListCtrl().GetItemText( n,1 ) + "\r\n";
		ctrlEdit.GetWindowText(szText);
		szInput += szText;
		flog.WriteString(szParameter);
		flog.WriteString(szOutput);
		flog.WriteString(szInput);
		flog.Close();
	}
}

int CShowDataView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CRichEditView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect	clientRect;

	GetClientRect( clientRect );
	m_ctrlEditTemp.Create( 0,clientRect,this,0 );
//	m_ctrlTitle.CreateEx( WS_EX_TRANSPARENT, WS_VISIBLE,clientRect,this,0 );
//	m_ctrlTitle.CreateEx(	WS_EX_TOOLWINDOW|WS_EX_TRANSPARENT ,
//							lpCreateStruct->lpszClass,
//							lpCreateStruct->lpszName,
//							WS_VISIBLE | WS_CHILD | WS_BORDER,clientRect,this,0 );
//	m_ctrlTitle.SetWindowText( "接收发送数据" );
	return 0;
}

void CShowDataView::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (point.x == -1 && point.y == -1)
	{
		//keystroke invocation
		CRect rect;
		GetClientRect(rect);
		ClientToScreen(&rect);

		point = rect.TopLeft();
		point.Offset(5, 5);
	}
	else
	{
		//keystroke invocation
		CRect rect;
		GetClientRect(rect);
		ClientToScreen(&rect);

		point.Offset(CSize(rect.left,rect.top));
	}
	CMenu menu;
	VERIFY(menu.LoadMenu(CG_IDR_POPUP_RECV_LIST_VIEW));

	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	CWnd* pWndPopupOwner = this;

	while (pWndPopupOwner->GetStyle() & WS_CHILD)
		pWndPopupOwner = pWndPopupOwner->GetParent();

	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
		pWndPopupOwner);

	CRichEditView::OnRButtonDown(nFlags, point);
}

void CShowDataView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bCaretPosChanged = true;	//光标位置发生了变化
	CRichEditView::OnLButtonDown(nFlags, point);
}

void CShowDataView::OnEditPaste()
{
	CRecvDataDlg	dlg(m_oHistory,this);
	dlg.DoModal();
}
