// 232ProgramView.cpp : implementation file
//

#include "stdafx.h"
#include "Commate.h"
#include "CommateDoc.h"
#include "ChildFrm.h"
#include "SourceView.h"
#include "RuntimeView.h"
#include "ShowDataView.h"
#include "SignalListView.h"
#include "CycleSettingDlg.h"
#include "resource.h"

#include "..\..\TmnLibs\SocketApi\CommonFacility.h"
#include "..\..\TmnCpps\calculat.h"
#include "..\..\TmnCpps\CRC.h"
#include ".\sourceview.h"
#include "DataConvertionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSourceView
#define		APP_EDIT_BACK_GROUND_COLOR	(((CProbeApp*) AfxGetApp())->m_colorSource)
#define		APP_EDIT_FORE_GROUND_COLOR	(0x00ffffff&(~APP_EDIT_BACK_GROUND_COLOR))
//#define		EDIT_FORE_GROUND_COLOR	RGB(0,0,192)

IMPLEMENT_DYNCREATE(CSourceView, CConsoleView)

CSourceView::CSourceView()
	:CConsoleView(APP_EDIT_BACK_GROUND_COLOR,APP_EDIT_FORE_GROUND_COLOR)
{
	m_nCycleTimer	= 1000;
	m_bLastSendHex	= false;	//true = Hex; false = Ascii
}

CSourceView::~CSourceView()
{
	m_szCommands.RemoveAll();
}


BEGIN_MESSAGE_MAP(CSourceView, CRichEditView)
	//{{AFX_MSG_MAP(CSourceView)
	ON_WM_CONTEXTMENU()
	ON_WM_TIMER()
	ON_COMMAND(ID_SEND_VIEW_CANCEL, OnSendViewCancel)
	ON_COMMAND(ID_SEND_VIEW_ASCII, OnSendViewAscii)
	ON_COMMAND(ID_SEND_VIEW_HEX, OnSendViewHex)
	ON_COMMAND(ID_SEND_VIEW_CYCLESEND, OnSendViewCyclesend)
	ON_COMMAND(ID_SEND_VIEW_CHECKSUM, OnSendViewChecksum)
	ON_COMMAND(ID_SEND_VIEW_CHECKCRC, OnSendViewCheckcrc)
	ON_COMMAND(ID_SENDVIEW_CLEAR, OnSendviewClear)
	ON_COMMAND(ID_SENDVIEW_SEL_ALL, OnSendviewSelAll)
	ON_COMMAND(ID_SEND_VIEW_CHECKSUM2, OnSendViewChecksum2)
	ON_COMMAND(ID_SEND_VIEW_TRANS_HEX, OnSendViewTransHex)
	ON_COMMAND(ID_SEND_VIEW_TRANS_ASCII, OnSendViewTransAscii)
	ON_COMMAND(ID_SEND_VIEW_ASCII_LINE, OnSendViewAsciiLine)
	ON_COMMAND(ID_SEND_VIEW_TXT_LF, &CSourceView::OnSendViewTxtLf)
	ON_COMMAND(ID_SEND_VIEW_ECHO, &CSourceView::OnSendViewEcho)
	ON_COMMAND(ID_VIEW_COLOUR, &CSourceView::OnViewColour)
	ON_UPDATE_COMMAND_UI(ID_SEND_VIEW_ECHO, &CSourceView::OnUpdateSendViewEcho)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_SEND_VIEW_EDIT, &CSourceView::OnSendViewEdit)
	ON_UPDATE_COMMAND_UI(ID_SEND_VIEW_EDIT, &CSourceView::OnUpdateSendViewEdit)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSourceView drawing

void CSourceView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
}

/////////////////////////////////////////////////////////////////////////////
// CSourceView diagnostics

#ifdef _DEBUG
void CSourceView::AssertValid() const
{
	CRichEditView::AssertValid();
}

void CSourceView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}
#endif //_DEBUG
CProbeDoc* CSourceView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CProbeDoc)));
	return (CProbeDoc*)m_pDocument;
}


/////////////////////////////////////////////////////////////////////////////
// CSourceView message handlers
BOOL CSourceView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此添加专用代码和/或调用基类
	BOOL bResult = CRichEditView::PreCreateWindow(cs);
	
	cs.style &= ~( ES_AUTOHSCROLL | WS_HSCROLL );

	return bResult;
}

void CSourceView::OnInitialUpdate() 
{
	CChildFrame*	pFrame = (CChildFrame*)this->GetParentFrame();
	CProbeDoc*		pDoc = (CProbeDoc*) GetDocument();
	pDoc->m_pViewSignal = &pFrame->m_wndSignal;
	pDoc->m_pViewRuntime = &pFrame->m_wndRuntime;
	pDoc->m_pViewShowData = &pFrame->m_wndShowData;
	pDoc->m_pViewSignal->OnInitialUpdate( *pDoc );
	pDoc->m_pViewRuntime->OnInitialUpdate( *pDoc );
	pDoc->m_pViewShowData->OnInitialUpdate( *pDoc );
	m_SetMode( pDoc->m_bEcho );
	CConsoleView::OnInitialUpdate();
	OnSendViewEcho();
	OnSendViewEcho();
}

DWORD CSourceView::SendHexLine(int index)
{
	CRichEditCtrl&		ctrl = GetRichEditCtrl();
	CString		szBuffer;
	wchar_t		sLineBuff[MAX_LENGTH];

	wmemset( sLineBuff,0x00,MAX_LENGTH );

	if( ctrl.GetLine( index, sLineBuff,MAX_LENGTH-1 ) > 0 )
	{
		return SendHexString( CString(sLineBuff) );
	}
	return 0;
}

DWORD CSourceView::SendHexString(CString szText)
{
	int					nTextLen = szText.GetLength();
	if( nTextLen <= 0 ) return 0;
	CProbeDoc*			pDoc = (CProbeDoc*)GetDocument();
	CShowDataView*		pInView = pDoc->m_GetShowDataView();
	LPWSTR				buffer = szText.GetBuffer( nTextLen );
	DWORD				i=0,counter=0;
	CString				szHex;
	char				output[MAX_LENGTH];

	memset( output,0x00,MAX_LENGTH );
	for( i=0,counter=0; counter<MAX_LENGTH; i++ )
	{
		wchar_t	c = buffer[i];

		if( (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || c == '\r' || c == '\n' ||
			(c >= '0' && c <= '9') || c == ' '  || c == ':' || c == '\0' )
		{
			if( c == ' ' || c == '\r' || c == '\n' || c == '\0' )
			{	// c is a separator
				int		hex = 0;
				swscanf_s( szHex,_T("%x"),&hex );
				output[counter++] = (char)(0x000000ff&hex);
				if( '\0' == c || '\r' == c || '\n' == c )	break;	//读数完毕，退出循环
				else szHex = _T("");	//继续读下一个数
					
			}
			else if( c == ':' ) szHex = _T("");
			else szHex += c;	// c is a digital
		}
		else szHex = _T("");
	}
	if( counter > 0 )
	{
		pInView->m_Append( true,0,0x00000000,output,counter );
	}
	if( pDoc->m_GetThread() )
		pDoc->m_GetThread()->m_GetComm().m_Send(output,counter);

	return counter;
}

DWORD CSourceView::SendHex()
{
	CRichEditCtrl&	ctrl = GetRichEditCtrl();
	long			nStart = 0,nTextLen= 0;
	CString			szText = m_GetSelectedText( nStart,nTextLen );

	m_bLastSendHex	= true;
	if( nTextLen > 0 )
	{
		return SendHexString(szText);
	}
	else
	{
		//this->MessageBox( _T("请选择要发送的内容"),_T("未发送任何数据"),MB_ICONWARNING );
		//return 0;
		long lineNo = ctrl.LineFromChar( nStart );
		return SendHexLine(lineNo);
	}
}

void CSourceView::OnTimer(UINT nIDEvent) 
{
	if( nIDEvent == 0X20000511 )
	{
		CProbeDoc*	pDoc = (CProbeDoc*)GetDocument();
		DWORD		nSend = 0;
		if( pDoc && !pDoc->m_bEcho )
			nSend = ( m_bLastSendHex ) ? SendHex() : SendAscii(SEND_ASCII_PARAGRAPH);
	}
	CRichEditView::OnTimer(nIDEvent);
}

void CSourceView::OnSendViewCyclesend() 
{
	CProbeDoc*			pDoc = (CProbeDoc*)GetDocument();
	CSignalListView*	pSingalView = pDoc->m_GetSignalView();
	CCycleSettingDlg	dlg(this);
	INT_PTR				ret = IDCANCEL;

	dlg.m_bSendMode = m_bLastSendHex;
	dlg.m_nCycleTimer = m_nCycleTimer;
	if( pSingalView && (ret = dlg.DoModal()) == IDOK ) {
		if (dlg.m_bRun) {
			m_bLastSendHex	= dlg.m_bSendMode ? true : false;
			m_nCycleTimer	= dlg.m_nCycleTimer;
			CString		szCycleMode = m_bLastSendHex ? _T("十六进制") : _T("ASCII码");
			CString		szCycleTimer;
			szCycleTimer.Format( _T("发送周期：%d ms"), m_nCycleTimer );
			pSingalView->GetListCtrl().DeleteItem( 5 );
			pSingalView->GetListCtrl().InsertItem( 5,_T("循环发送") );
			pSingalView->GetListCtrl().SetItemText( 5,1,szCycleMode );
			pSingalView->GetListCtrl().SetItemText( 5,2,szCycleTimer );
			SetTimer( 0X20000511,m_nCycleTimer,NULL );
		} else OnSendViewCancel();
	}
}

void CSourceView::OnSendViewCancel() 
{
	CProbeDoc*			pDoc = (CProbeDoc*)GetDocument();
	CSignalListView*	pSingalView = pDoc->m_GetSignalView();
	pSingalView->GetListCtrl().DeleteItem( 5 );
	KillTimer( 0X20000511 );
}


DWORD CSourceView::SendAscii(char cMode)
{
	CString				szCommand;
	CRichEditCtrl&		ctrl = GetRichEditCtrl();
	CProbeDoc*			pDoc = (CProbeDoc*)GetDocument();
	long				nStartChar = 0,nTextLen = 0;
	CString				szText = m_GetSelectedText( nStartChar,nTextLen );
	long				lineNo = ctrl.LineFromChar( nStartChar );

	m_bLastSendHex	= false;
	if( nTextLen > 0 )
	{
		if( cMode == SEND_ASCII_LINE_LF ) szText += "\r\n";
		else if (cMode == SEND_ASCII_LINE) szText += "\r";
	}
	else
	{
		wchar_t		sLineBuff[MAX_LENGTH];
		long		nPosition = this->m_GetCaretEOF();

		wmemset( sLineBuff,0x00,MAX_LENGTH );
		switch( cMode )
		{
		case SEND_ASCII_LINE:
			if( ctrl.GetLine( lineNo, sLineBuff,MAX_LENGTH-1 ) > 0 )
				szText = CString(sLineBuff).Trim(0x0d);
			break;
		case SEND_ASCII_LINE_LF:
			szCommand = m_PickupCommand();
			if( !szCommand.IsEmpty() ) m_szCommands.AddTail( szCommand );
			szText = szCommand + _T("\r\n");
			break;
		case SEND_ASCII_PARAGRAPH:
			for( int nLine = lineNo; nLine >= 0; nLine -- )
			{
				if( ctrl.GetLine( nLine, sLineBuff,MAX_LENGTH-1 ) > 0 )
					szText = CString( sLineBuff ) + szText; else break;
			}
			break;
		}
	}
	return m_RealSend( szText );
}

void CSourceView::OnSendViewHex() 
{
	SendHex();
}

void CSourceView::OnSendViewAscii() 
{
	SendAscii(SEND_ASCII_PARAGRAPH);
}

void CSourceView::OnSendViewTxtLf()
{
	SendAscii(SEND_ASCII_LINE_LF);
}

void CSourceView::OnSendViewAsciiLine()
{
	SendAscii(SEND_ASCII_LINE);
}

BOOL CSourceView::Trans( CString& source,int& result )
{
	int		length = source.GetLength();
	int		index = source.Find( _T(" ") );

	if( index == -1 )
	{
		swscanf_s( source,_T("%x"),&result );
		return 0;
	}
	CString		temp = source.Left( index );
	source = source.Right( length-index-1 );
	swscanf_s( temp,_T("%x"),&result );
	return 1;
}

void CSourceView::OnSendViewChecksum2()
{
	int			i = 0;
	CString		szText;
	long		nStartChar = 0,nTextLen = 0;

	szText = m_GetSelectedText( nStartChar, nTextLen );

	int				length = szText.GetLength();
	CString			szFormula;
	CCalculater		cal;

	while( 1 )
	{
		int		result = 0;
		if( Trans( szText,result ) )
		{
			szFormula.Format( _T("%d"),result );
			cal.m_SourceString += szFormula + _T("+");
		}
		else
		{
			szFormula.Format( _T("%d"),result );
			cal.m_SourceString += szFormula + _T("=");
			break;
		}
	}

	double			result;
	int				errorNo = cal.Calculate( &result );
	if( errorNo != 0 )
	{
		MessageBox( cal.ErrorMessage(errorNo),_T("无法计算"),MB_ICONSTOP  );
	}
	else
	{
		DWORD		checksum = (DWORD) result;
		CString		szCheckSum;

		checksum &= 0X0000FFFF;
		checksum = ~checksum;
		checksum += 1;
		checksum &= 0X0000FFFF;
		szCheckSum.Format( _T("Checksum = %04X(High Low)"),checksum );
		MessageBox( szCheckSum );
	}
}

void CSourceView::OnSendViewChecksum() 
{
	int			i = 0;
	CString		szText;
	long		nStartChar = 0,nTextLen = 0;

	szText = m_GetSelectedText( nStartChar, nTextLen );

	int				length = szText.GetLength();
	CString			szFormula;
	CCalculater		cal;

	while( true )
	{
		int		result = 0;
		if( Trans( szText,result ) )
		{
			szFormula.Format( _T("%d"),result );
			cal.m_SourceString += szFormula + _T("+");
		}
		else
		{
			szFormula.Format( _T("%d"),result );
			cal.m_SourceString += szFormula + _T("=");
			break;
		}
	}

	double		result;
	if( cal.Calculate( &result )!=0 )
		MessageBox( _T("Invalidate string,can not calculate the checksum.") );
	else
	{
		__int16		checksum = (__int16) result;
		CString		szCheckSum;

		szCheckSum.Format( _T("Checksum = %04X(High Low)"),checksum );
		MessageBox( szCheckSum );
	}
}

void CSourceView::OnSendViewCheckcrc() 
{
	int			i = 0;
	long		nStartChar = 0,nTextLen = 0;
	CString		szText = m_GetSelectedText( nStartChar, nTextLen );

	WORD	crc = CRC(szText);
	CString	szCRC;
	szCRC.Format( _T("CRC = %04X(High Low)"),crc );
	MessageBox( szCRC );
}

void CSourceView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if( m_bLastSendHex ) SendHex(); else SendAscii(false);
	CRichEditView::OnLButtonDblClk(nFlags, point);
	//GetRichEditCtrl().SetSel(-1,0);
}

void CSourceView::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (point.x == -1 && point.y == -1){
		//keystroke invocation
		CRect rect;
		GetClientRect(rect);
		ClientToScreen(rect);

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
	VERIFY(menu.LoadMenu(CG_IDR_POPUP_C232PROGRAM_VIEW));

	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	CWnd* pWndPopupOwner = this;

	while (pWndPopupOwner->GetStyle() & WS_CHILD)
		pWndPopupOwner = pWndPopupOwner->GetParent();

	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
		pWndPopupOwner);

	CConsoleView::OnRButtonDown(nFlags, point);
}

void CSourceView::OnSendviewClear() 
{
	m_SetCaretEOF(-1);
}

void CSourceView::OnSendviewSelAll() 
{
	CRichEditCtrl&	ctrl = GetRichEditCtrl();
	WORD	low = 0;
	WORD	high = -1;
	DWORD	sel = (high<<16) | low;
//	ctrl.SetSel(sel);
	ctrl.SetSel( low,high );
}

void CSourceView::OnSendViewTransHex()
{
	CString		szSource;
	CString		szResult;

	szSource = GetRichEditCtrl().GetSelText();
	//GetSelectedText( szSource );

	int		nLen = szSource.GetLength();
	for( int n = 0; n < nLen; n ++ )
	{
		CString	szHex;
		szHex.Format( _T("%02x "),0x00ff&szSource[n] );
		szResult += szHex;
	}
	CDataConvertionDlg	dlg;

	dlg.m_szConverted = szResult;
	dlg.DoModal();
}

void CSourceView::OnSendViewTransAscii()
{
	CString		szSource;
	CString		szResult;

	szSource = GetRichEditCtrl().GetSelText();
	//GetSelectedText( szSource );

	while( !szSource.IsEmpty() )
	{
		int	nValue = 0;
		int	nIndex = szSource.Find(' ');
		if( nIndex >= 0 )
		{
			CString	szHex = szSource.Left(nIndex);
			swscanf_s( szHex,_T("%x"),&nValue );
			szHex.Format( _T("%c"),0x00ff&nValue );
			szResult += szHex;
			szSource = szSource.Right( szSource.GetLength()-nIndex-1 );
		}
		else
		{
			CString	szHex = szSource;
			swscanf_s( szHex,_T("%x"),&nValue );
			szHex.Format( _T("%c"),0x00ff&nValue );
			szResult += szHex;
			szSource = _T("");
		}
	}
	CDataConvertionDlg	dlg;

	dlg.m_szConverted = szResult;
	dlg.DoModal();
}

void CSourceView::m_LoadRuntime(CListCtrl& listCtrl)
{
	CRichEditCtrl&	ctrlEdit = GetRichEditCtrl();
	int		nStartChar = 0,nEndChar = 0;
	int		nStartLine = 0,nEndLine = 0;
	int		nItem = 0;
	CString	szSource;
	CString	szLine;
	CString	szLfLn = _T("\r\n");

	listCtrl.DeleteAllItems();
	ctrlEdit.GetWindowText( szSource );
	while( !szSource.IsEmpty() )
	{
		int	nSource = szSource.GetLength();
		int	nIndexEOL = szSource.Find( szLfLn );
		if( nIndexEOL >= 0 )
		{
			szLine = szSource.Left( nIndexEOL );
			szSource = szSource.Right( nSource - nIndexEOL - 2 );
		}
		else
		{
			szLine = szSource;
			szSource = _T("");
		}
		int			length = szLine.GetLength();
		int			index = szLine.Find(':');
		CString		szIndex;
		CString		szKeyword;
		CString		szSyntex;

		if( index != -1 )
		{
			szIndex.Format( _T("%d"),nItem+1 );
			szKeyword = szLine.Left( index );
			szSyntex = szLine.Right( length-index-1 );
			
			listCtrl.InsertItem(nItem,szIndex);
			listCtrl.SetItemText(nItem,1,szKeyword);
			listCtrl.SetItemText(nItem,2,szSyntex);
			nItem ++;
		}
	}
}

void CSourceView::OnSendViewEcho()
{
	CProbeDoc*	pDoc = (CProbeDoc*)GetDocument();
	BYTE		cMode = 0;
	if( pDoc )
	{
		pDoc->m_bEcho = !pDoc->m_bEcho;
		if( pDoc->m_bEcho )
		{
			this->m_SetMode( WORKING_MODE_CONSOLE );
			this->m_SetCaretEnd();
		}
		else this->m_SetMode( WORKING_MODE_DEFAULT );
	}
}

void CSourceView::OnSendViewEdit()
{
	BYTE		cMode = this->m_GetMode();
	if( cMode & WORKING_MODE_CONSOLE )
	{
		if( cMode & WORKING_MODE_VT_EDIT )
			cMode &= (~WORKING_MODE_VT_EDIT);
		else cMode |= WORKING_MODE_VT_EDIT;
		this->m_SetMode( cMode );
	}
}

void CSourceView::OnUpdateSendViewEcho(CCmdUI *pCmdUI)
{
	CProbeDoc*	pDoc = (CProbeDoc*)GetDocument();
	if( pDoc && pDoc->m_bEcho )
		pCmdUI->SetCheck( pDoc->m_bEcho );
}

void CSourceView::OnUpdateSendViewEdit(CCmdUI *pCmdUI)
{
	if( this->m_GetMode() & WORKING_MODE_VT_EDIT ) pCmdUI->SetCheck();
	else pCmdUI->SetCheck(0);
}

void CSourceView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	BYTE	cMode = m_GetMode();
	if( cMode & WORKING_MODE_VT_EDIT )
	{
		//char	cChar = (char)nChar;
		m_RealSend((LPSTR)(&nChar),1);
	}
	CConsoleView::OnChar(nChar, nRepCnt, nFlags);
}

void CSourceView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	DWORD		len = 0;
	CString		szData = CString( (char)nChar );
	BYTE		cMode = m_GetMode();

	TRACE( _T("%d pressed"),nChar);
	if( cMode & WORKING_MODE_VT_EDIT )
	{	//优先处理VT100编辑模式
		switch( nChar )
		{
		case VK_UP:		szData = VT100_UP;		m_RealSend( szData );	break;
		case VK_DOWN:	szData = VT100_DOWN;	m_RealSend( szData );	break;
		case VK_LEFT:	szData = VT100_LEFT;	m_RealSend( szData );	break;
		case VK_RIGHT:	szData = VT100_RIGHT;	m_RealSend( szData );	break;
		case VK_RETURN:
		default:;
		}		
	}
	else if( cMode & WORKING_MODE_CONSOLE ) 
	{
		switch( nChar )
		{
		case VK_RETURN:
			OnSendViewTxtLf();
			break;
		case VK_UP:
			if( !m_szCommands.IsEmpty() )
			{
				CString	szCommand = m_szCommands.RemoveTail();
				m_szCommands.AddHead( szCommand );
				this->m_ModifyCommand( szCommand );
			}
			return;
		case VK_DOWN:
			if( !m_szCommands.IsEmpty() )
			{
				CString	szCommand = m_szCommands.RemoveHead();
				m_szCommands.AddTail( szCommand );
				this->m_ModifyCommand( szCommand );
			}
			return;
		case VK_BACK:
			{
				int	nRemain = m_GetCaretPos() - m_GetCaretEOF();
				if( nRemain <= 0 ) return;
				if( nRemain < (int)nRepCnt ) return;
			}
			break;
		case VK_CANCEL:
			{
				char	cChar = (char)VK_CANCEL;
				m_RealSend(&cChar,1);
			}
			break;
		case (UINT)('C'):
			if( GetKeyState(VK_CONTROL) < 0 )
			{
				char	cChar = (char)VK_CANCEL;
				m_RealSend(&cChar,1);
			}
			break;
		default: if( this->m_GetCaretPos() < this->m_GetCaretEOF() ) this->m_SetCaretEnd();
		}
	}
	CConsoleView::OnKeyDown(nChar, nRepCnt, nFlags);
}

DWORD CSourceView::m_RealSend(CString szData)
{
	DWORD	nData = szData.GetLength()*2;
	char*	sData = new char[ nData + 1 ];
	nData = WideStr_MultiByteStr( szData,sData,nData );
	nData = m_RealSend( sData,nData );
	delete [] sData;
	return nData;
}

DWORD CSourceView::m_RealSend(LPSTR sData,DWORD nData)
{
	CProbeDoc*	pDoc = (CProbeDoc*)GetDocument();
	if( pDoc && pDoc->m_GetThread() )
	{
		DWORD	add = pDoc->m_GetThread()->m_GetComm().m_GetRemoteAdd();
		pDoc->m_GetShowDataView()->m_Append( true,0,add,sData,nData );
		pDoc->m_GetThread()->m_GetComm().m_Send(sData,nData);
		return nData;
	}
	return 0;
}

void CSourceView::OnViewColour()
{
	CColorDialog	dlg( APP_EDIT_BACK_GROUND_COLOR,0,this );

	if( dlg.DoModal() == IDOK )
		APP_EDIT_BACK_GROUND_COLOR = dlg.GetColor();
}


void CSourceView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CConsoleView::OnLButtonDown(nFlags, point);
}

void CSourceView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	CConsoleView::OnUpdate( pSender,lHint,pHint );
}
