// ShowDataView.cpp : 实现文件
//

#include "stdafx.h"
#include "Commate.h"
#include "ChildFrm.h"
#include "ShowDataView.h"
#include "SourceView.h"
#include "SignalListView.h"
#include "RuntimeView.h"
#include "RecvDataDlg.h"
#include "FilterDlg.h"
#include "..\..\TmnLibs\SocketApi\CommonFacility.h"

// CShowDataView

IMPLEMENT_DYNCREATE(CShowDataView, CSizingControlBarG)

#define	COLOR_0		RGB(218,0,218)	//101	pink
#define	COLOR_1		RGB(0,218,218)	//011	cyan
#define	COLOR_2		RGB(0,218,0)	//010	green
#define	COLOR_3		RGB(218,0,0)	//100	red
#define	COLOR_4		RGB(218,218,0)	//110	grass
#define	COLOR_5		RGB(96,96,218)	//..1	purple
#define	COLOR_6		RGB(0,0,218)	//001	blue
#define	COLOR_7		RGB(192,192,192)//111	grey

#define	COLOUR_FOREGROUND		RGB(255,255,255)
#define	COLOUR_BACKGROUND		((CProbeApp*) AfxGetApp())->m_colorShowData
#define	MAX_CHARS_IN_DATAVIEW	1024*1024

#define	FONT_0		240
//#define	FONT_1		160
//#define	FONT_2		150

static COLORREF	staticCOLOUR[8]	= {	COLOR_0,COLOR_1,COLOR_2,COLOR_3,COLOR_4,COLOR_5,COLOR_6,COLOR_7 };
////////////////////////////////////////////////////////////////////////
CRecvBuff::CRecvBuff()
{
	m_bSendRecv = false;
	m_dwAddress = 0;
	memset( &m_oTimeB,0x00,sizeof(struct __timeb64) );
	m_nSerial = 0;
	m_nBuffer = 0;
	m_sBuffer = NULL;
}

CRecvBuff::~CRecvBuff()
{
	if( m_sBuffer ) delete [] m_sBuffer;
	m_sBuffer = NULL;
}

void CRecvBuff::m_Append( bool bSendRecv,DWORD dwAddress,char* sBuffer,long nLength,long nSerial )
{
	if( nLength <= 0 ) return;
	long	zeros = (nLength%2) ? 3 : 2;
	errno_t	error = _ftime64_s( &m_oTimeB );
	m_nSerial = nSerial;
	m_bSendRecv = bSendRecv;
	m_dwAddress = dwAddress;
	char*	sOldBuf = m_sBuffer;
	long	nOldBuf = m_nBuffer;
	m_nBuffer += nLength;
	m_sBuffer = new char[m_nBuffer+zeros];
	memset( m_sBuffer,0x00,m_nBuffer+zeros );
	if( sOldBuf && nOldBuf > 0 ) memcpy( m_sBuffer,sOldBuf,nOldBuf );
	if( sBuffer && nLength > 0 ) memcpy( m_sBuffer+nOldBuf,sBuffer,nLength );
	if( sOldBuf ) delete [] sOldBuf;
}

void CRecvBuff::Serialize(CArchive &ar)
{
	if( ar.IsStoring() )
	{
		ar << m_bSendRecv;
		ar << m_dwAddress;
		ar << m_oTimeB.time;
		ar << m_oTimeB.millitm;
		ar << m_nSerial;
		ar << m_nBuffer;
		for( int i = 0; i < m_nBuffer; i ++ )
			ar << m_sBuffer[i];
	}
	else
	{
		ar >> m_bSendRecv;
		ar >> m_dwAddress;
		ar >> m_oTimeB.time;
		ar >> m_oTimeB.millitm;
		ar >> m_nSerial;
		ar >> m_nBuffer;
		if( m_nBuffer > 0 )
		{
			if( m_sBuffer ) delete m_sBuffer;
			m_sBuffer = new char[m_nBuffer+3];
			memset( m_sBuffer,0x00,m_nBuffer+3 );
			for( int i = 0; i < m_nBuffer; i ++ )
				ar >> m_sBuffer[i];
		}
	}
}
////////////////////////////////////////////////////////////////////////
CShowDataView::CShowDataView()
	: m_Brush( COLOUR_BACKGROUND )
{
	m_pDoc				= NULL;
	m_counter			= 0;
	m_maxline			= 0;
	m_current			= 0;
	m_bCaretPosChanged	= false;
	m_cColor			= 0;
	m_nTimer			= 0;
	m_bEffect			= DISPLAY_ASCII;
	m_pFont				= new CFont();
	m_nBuff				= 0;
	memset( m_buffer,0x00,RECV_BUFF_SIZE );
}

CShowDataView::~CShowDataView()
{
	delete m_pFont;
	m_ClearHistory();
}

BEGIN_MESSAGE_MAP(CShowDataView, CSizingControlBarG)
	ON_WM_CONTEXTMENU()
	ON_WM_CTLCOLOR()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CShowDataView 诊断

#ifdef _DEBUG
void CShowDataView::AssertValid() const
{
	CSizingControlBarG::AssertValid();
}

void CShowDataView::Dump(CDumpContext& dc) const
{
	CSizingControlBarG::Dump(dc);
}
#endif //_DEBUG


// CShowDataView 消息处理程序
CProbeDoc* CShowDataView::GetDocument() // non-debug version is inline
{
	//ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CProbeDoc)));
	//return (CProbeDoc*)m_pDocument;
	return m_pDoc;
}

void CShowDataView::OnInitialUpdate(CProbeDoc& doc)
{
	m_pDoc = &doc;
	doc.m_pViewShowData = this;

	CSizingControlBarG::OnInitialUpdate();

	//CFont	newFont;
	//newFont.CreateFont(	16,0,0,0,FW_BOLD,TRUE,FALSE,0,
	//					ANSI_CHARSET,OUT_DEFAULT_PRECIS,
	//					CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
	//					DEFAULT_PITCH | FF_DONTCARE, _T("宋体 & 新宋体") );
	m_pFont->CreatePointFont( FONT_0,(LPCTSTR)_T("宋体") ); 
	m_pFont->GetLogFont(&m_lFont);
	m_lFont.lfWeight = FW_THIN;//FW_NORMAL;//FW_BOLD;
	m_pFont->DeleteObject(); 
	m_pFont->CreateFontIndirect(&m_lFont);
	m_ctrlEdit.SetFont(m_pFont);
	m_ctrlEdit.SetBackgroundColor(false,COLOUR_BACKGROUND);
	long	nMask = m_ctrlEdit.GetEventMask();
	m_ctrlEdit.SetEventMask(nMask|ENM_MOUSEEVENTS);
	SetTimer( 0x99050313,50,NULL );

	if( !m_pDoc->GetPathName().IsEmpty() )
	{
		CSignalListView*	pViewSignal = m_pDoc->m_GetSignalView();
		CString				szFileName = m_pDoc->GetPathName() + _T(".log");
		CFile				flog;
		if( flog.Open( szFileName,CFile::modeCreate|CFile::modeWrite ) )
		{
			try {
				CArchive			ar(&flog,CArchive::store);
				CString				szParameter = _T("端口参数:\r\n");
				int					nItem = pViewSignal->GetListCtrl().GetItemCount();
				for( int n = 0; n < nItem; n ++ )
					szParameter +=	pViewSignal->GetListCtrl().GetItemText( n,1 ) + _T("=") +
									pViewSignal->GetListCtrl().GetItemText( n,2 ) + _T("\r\n");
				ar << szParameter;
				ar << (int)m_oHistory.GetCount();
			} catch( CException* e ) {
				e->ReportError();
				flog.Close();	return;
			}
			flog.Close();	return;
		}
	}
}

void CShowDataView::OnTimer(UINT nIDEvent)
{
	if( 0x99050313 == nIDEvent )
	{
		CProbeDoc*			pDoc = (CProbeDoc*) GetDocument();
		BOOL				result = FALSE;
		DWORD				nBlocks = 0;		//本次调用读取到的数据块总数
		//	newEvent 复位后，本线程有权访问CWinThreadExec成员m_buffer及m_counter
		//	newEvent 置位后，CWinThreadExec线程有权访问其成员m_buffer及m_counter
		if( NULL == pDoc->m_pViewRuntime || NULL == pDoc->m_GetThread() ) {
			CSizingControlBarG::OnTimer(nIDEvent);	return;
		}
		HANDLE&	eventIdel = pDoc->m_GetThread()->m_buffer.m_eventIdel;
		HANDLE&	eventUpdt = pDoc->m_GetThread()->m_buffer.m_eventUpdated;
		if( eventIdel && WaitForSingleObject( eventIdel,0 ) != WAIT_OBJECT_0 ) {
			CSizingControlBarG::OnTimer(nIDEvent);	return;
		}
		//if( newEvent && WaitForSingleObject( newEvent,0 ) == WAIT_OBJECT_0 )
		//{
		//	if( newEvent ) CloseHandle( newEvent );
		//	if( idlEvent ) CloseHandle( idlEvent );
		//	CSizingControlBarG::OnTimer(nIDEvent);	return;
		//}
		if( pDoc->m_pViewSignal )
			pDoc->m_pViewSignal->SendMessage( WM_CHAR,0,0 );
		
		CCommunication&	comm = pDoc->m_GetThread()->m_GetComm();
		if( m_counter > 0 )	{	//还有未处理的数据
			//将数据复制到CWinThreadExec线程m_buffer及m_counter,并通知该线程
			pDoc->m_GetThread()->m_buffer.m_Update(m_counter,m_buffer);
			nBlocks ++;		//通知CWinThreadExec线程有新数据
		} else for( DWORD nChance = 1; nChance > 0; nChance -- ) {
			//否则认为所有数据都已经处理了
			bool	bRecvMajor = false;
			m_counter = sizeof(m_buffer);	//期望读取的最大长度
			if (0 < this->m_nBuff && this->m_nBuff < m_counter)
				memset( m_buffer,0x00,this->m_nBuff);
			result = comm.m_Recv(m_buffer,m_counter,bRecvMajor);
			if( result && m_counter > 0 ) {	//现在m_counter为实际读取的长度
				this->m_nBuff = m_counter;
				m_Append( !bRecvMajor,nBlocks,comm.m_GetRemoteAdd(),m_buffer,m_counter );	//false 表示收数据
				if( pDoc->m_GetThread()->m_buffer.m_len + m_counter >= RECV_BUFF_SIZE ) {
					//如果加上本次数据，CWinThreadExec线程缓冲区将溢出，则本次数据暂不处理
					if( eventUpdt ) SetEvent( eventUpdt );	//通知CWinThreadExec线程处理其已有的数据
					CSizingControlBarG::OnTimer(nIDEvent);	return;
				} else pDoc->m_GetThread()->m_buffer.m_Append(m_counter,m_buffer);
				nBlocks ++;
				nChance = 3;	//连续 30 毫秒未收到数据就断句
				Sleep(10);
			}
		}

		if( nBlocks > 0 ) {
			m_cColor ++;
			m_cColor %= 8;
			m_counter = 0;
			if( eventUpdt ) SetEvent( eventUpdt );		//通知CWinThreadExec线程有新数据
		}
		m_nTimer ++;
	}
	CSizingControlBarG::OnTimer(nIDEvent);
}

bool CShowDataView::m_ChangeEffect(bool bEffect)
{
	CProbeApp*	pApp = (CProbeApp*) AfxGetApp();
	//if( !pApp->m_bEffectReadOnly && m_bEffect != bEffect )
	if( m_bEffect != bEffect )
	{
		m_bEffect = bEffect;
		return true;
	}
	return false;
}

void CShowDataView::m_SetEffect( long nHeight,COLORREF cText,COLORREF cBackground )
{
	CRichEditCtrl&	ctrlEdit = m_ctrlEdit;
	CHARFORMAT2		cf;
	ctrlEdit.GetDefaultCharFormat(cf);

	cf.dwMask		= CFM_SIZE | CFM_WEIGHT | CFM_BOLD | CFM_COLOR | CFM_BACKCOLOR | CFM_LINK;
	cf.yHeight		= nHeight;
	cf.wWeight		= 40;
	cf.crBackColor	= cBackground;
	cf.crTextColor	= cText;
	cf.dwEffects	= m_bEffect ? CFE_ITALIC : CFE_BOLD;//CFE_LINK | CFE_BOLD : CFE_BOLD;
	ctrlEdit.SetWordCharFormat( cf );
}

void CShowDataView::m_AppendEcho( CString szText)
{
	CProbeDoc*		pDoc = (CProbeDoc*) GetDocument();
	CSourceView*	pSrc = pDoc->m_GetSourceView();
	pSrc->m_AppendEcho(szText);
}

void CShowDataView::m_AppendText( CString& szText )
{
	if( szText.IsEmpty() ) return;
	//if( m_szTemp.GetLength() < 10 && m_nTimer < 10 ) return;

	CRichEditCtrl&	ctrlEdit = m_ctrlEdit;
	int				nLength = ctrlEdit.GetTextLength();	
	int				nLineOld = ctrlEdit.GetLineCount();

	ctrlEdit.SetReadOnly(FALSE);
	if( nLength > MAX_CHARS_IN_DATAVIEW ) ctrlEdit.SetWindowText(_T(""));
	ctrlEdit.SetSel( nLength,nLength );
	ctrlEdit.ReplaceSel( szText );
	ctrlEdit.SetReadOnly(TRUE);

	int				nLineNew = ctrlEdit.GetLineCount();
	ctrlEdit.LineScroll(nLineNew-nLineOld, 0);
	szText = _T("");
}

bool CShowDataView::m_IsPermissible(CObList& aFilters,CString szIP)
{
	DWORD	b0 = 0,b1 = 0,b2 = 0,b3 = 0,dwIP = 0;
	swscanf_s( szIP,_T("%d.%d.%d.%d"),&b0,&b1,&b2,&b3 );
	dwIP = (b0<<24) | (b1<<16) | (b2<<8) | b3;
	return m_IsPermissible(aFilters,dwIP);
}

bool CShowDataView::m_IsPermissible(CObList& aFilters,DWORD dwIP)
{
	if( 0 == dwIP ) return true;
	if( 0 == aFilters.GetCount() ) return true;
	for( POSITION pos = aFilters.GetHeadPosition();pos;aFilters.GetNext(pos) )
	{
		CFilter*	filter = (CFilter*) aFilters.GetAt(pos);
		if( filter->m_dwIP == dwIP ) return true;
		if( (filter->m_dwIP & 0x000000ff) == 0x000000ff &&
			(filter->m_dwIP & 0xffffff00) == (dwIP & 0xffffff00 ) ) return true;
		if( (filter->m_dwIP & 0x0000ffff) == 0x0000ffff &&
			(filter->m_dwIP & 0xffff0000) == (dwIP & 0xffff0000 ) ) return true;
		if( (filter->m_dwIP & 0x00ffffff) == 0x00ffffff &&
			(filter->m_dwIP & 0xff000000) == (dwIP & 0xff000000 ) ) return true;
	}
	return false;
}

void CShowDataView::m_DisplayData( DWORD nData,LPSTR sData,bool bSend )
{
	class CPacketItem : public CObject
	{
	public:
		BYTE	m_nType;
		CString	m_szValue;
	public:
		CPacketItem(BYTE nType,CString szValue)
		{
			m_nType = nType;
			m_szValue = szValue;
		}
	};
	CProbeDoc*	pDoc = (CProbeDoc*) GetDocument();
	COLORREF	oTextColor;
	COLORREF	oBackColor;
	CString		szBuffer = MultiByteStr_WideStr(sData);
	CString		szEcho = szBuffer;
	CObList		oItems;
	CString		szResult;
	CString		szAppend;
	DWORD		nBuffer = 0;
	LPWSTR		sBuffer = NULL;
	
	if( bSend ) {
		//oBackColor = 0x00ffffff & (~COLOUR_BACKGROUND);
		//oTextColor = staticCOLOUR[m_cColor];
		oTextColor = ~COLOUR_FOREGROUND;
		oBackColor =  staticCOLOUR[m_cColor];
	} else {
		//oBackColor = staticCOLOUR[m_cColor];
		//oTextColor = COLOUR_FOREGROUND;
		oBackColor = COLOUR_BACKGROUND;	
		oTextColor = staticCOLOUR[m_cColor];
	}
	m_ChangeEffect( bSend );
	m_SetEffect( FONT_0,oTextColor,oBackColor );

	switch( pDoc->m_bDisp ) {
	case DISPLAY_ASCII:
		while( !szBuffer.IsEmpty() )
		{	//
			int	nBegin = 0,nEnd = 0;
			if( (nBegin = szBuffer.Find( _T("【" ) )) > 0 &&
				(nEnd = szBuffer.Find( _T("】") )) > 0 && nBegin < nEnd ) {
				CPacketItem*	item = new CPacketItem( 0,szBuffer.Left( nBegin ) );
				CPacketItem*	flag = new CPacketItem( 1,szBuffer.Mid( nBegin+1,nEnd-nBegin-1 ) );
				oItems.AddTail( item );
				oItems.AddTail( flag );
				szBuffer = szBuffer.Right( szBuffer.GetLength() - nEnd - 1 );
			} else {
				CPacketItem*	item = new CPacketItem( 0,szBuffer );
				oItems.AddTail( item );
				szBuffer = _T("");
			}
		}
		for( bool bLeft = true; !oItems.IsEmpty(); )
		{
			CPacketItem*	item = (CPacketItem*) oItems.RemoveHead();
			switch( item->m_nType ) {
			case 0:	if( bLeft ) szResult += item->m_szValue;	break;
			case 1:	bLeft = m_IsPermissible( pDoc->m_aFilters,item->m_szValue );
					if( bLeft ) szResult += _T("【" ) + item->m_szValue + _T("】");
					break;
			default:;
			}
			delete (CPacketItem*) item;
		}
		sBuffer = szResult.GetBuffer();
		nBuffer = szResult.GetLength();
		break;
	case DISPLAY_HEX:	sBuffer = (LPWSTR) sData;
						nBuffer = nData;
						break;
	}
	for( DWORD i=0;i<nBuffer;i++ )
	{
		CString		szOneChar;			
		switch( pDoc->m_bDisp )
		{
		case DISPLAY_HEX:
			szOneChar.Format( _T("%02x "),0x00ff&sData[i] );
			szAppend += szOneChar;
			break;
		case DISPLAY_ASCII:
			{
				wchar_t		cPrev	= (i > 0) ? sBuffer[i-1] : 0;
				wchar_t		cData	= sBuffer[i];
				wchar_t		cNext	= (i+1 < nBuffer) ? sBuffer[i+1] : 0;
				if( cData == (short) 0x000d )
				{
					if( cNext != (short) 0x000a ) 
					{
						m_AppendText( szAppend );
						m_ChangeEffect( true );
						m_SetEffect( FONT_0,oTextColor,oBackColor );
						szAppend += _T("』");	//不能将本行直接作为m_AggendText的参数，因为参数要求的是引用类型
						szAppend.AppendChar( (char)cData );
						m_AppendText( szAppend );
						m_ChangeEffect( bSend );
						m_SetEffect( FONT_0,oTextColor,oBackColor );
					}
				}
				else if( cData == (short) 0x000a )
				{
					if( cPrev != (short) 0x000d ) 
					{
						m_AppendText( szAppend );
						m_ChangeEffect( true );
						m_SetEffect( FONT_0,oTextColor,oBackColor );
						szAppend += _T("『");	//不能将本行直接作为m_AggendText的参数，因为参数要求的是引用类型
						szAppend.AppendChar( (char)cData );
						m_AppendText( szAppend );
						m_ChangeEffect( bSend );
						m_SetEffect( FONT_0,oTextColor,oBackColor );
					}
					else
					{
						szAppend += _T("↓\r\n");	//不能将本行直接作为m_AggendText的参数，因为参数要求的是引用类型
						m_AppendText( szAppend );
						m_ChangeEffect( bSend );
						m_SetEffect( FONT_0,oTextColor,oBackColor );
					}
				}
				else szAppend.AppendChar( (wchar_t)cData );
			}
			break;
		}
	}
	m_AppendText( szAppend );
//	if( nPosition && !bSend )
	if( !bSend )
		m_AppendEcho( szEcho );
}

void CShowDataView::m_Display( DWORD nData,LPSTR sData,bool bSendRecv )
{
	if( 0 == nData || NULL == sData )
	{
		for( POSITION	position = m_oHistory.GetHeadPosition(); position; m_oHistory.GetNext(position) )
		{
			CRecvBuff*	pRecvBuff = (CRecvBuff*)m_oHistory.GetAt(position);
			bool		bSend = pRecvBuff->m_IsSend();
			LPSTR		sBuff = pRecvBuff->m_GetBuffer();
			DWORD		nBuff = pRecvBuff->m_GetLength();
			m_cColor ++;
			m_cColor %= 8;
			m_DisplayData( nBuff,sBuff,bSend );
		}
	}
	else
	{
		m_cColor ++;
		m_cColor %= 8;
		m_DisplayData( nData,sData,bSendRecv );
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

int CShowDataView::m_Append( bool bSendRecv,DWORD nBlocks,DWORD dwAddress,LPSTR pData,DWORD nCounter )
{
//	if( nCounter > 10240 ) return 0;
	CProbeApp*	pApp = (CProbeApp*) AfxGetApp();
	CRecvBuff*	pRecvBuff = NULL;
	bool		bNewRecord = ( 0 == nBlocks ) ? true : false;
	long		nSerial = 0;
	
	if( m_oHistory.GetCount() > 0 ) {
		pRecvBuff = (CRecvBuff*)m_oHistory.GetTail();
		nSerial = pRecvBuff->m_GetSerial();
		//如果收发标志与最后一条记录不一致，则创建新记录
		nBlocks = (pRecvBuff->m_IsSend() == bSendRecv) ? nBlocks : 0;
	}
	if( 0 == m_oHistory.GetCount() || 0 == nBlocks ) {
		//如果历史列表为空,或nBlocks为0（第一个数据块）则创建一条新的记录
		pRecvBuff = new CRecvBuff();
		m_oHistory.AddTail( pRecvBuff );
		nSerial ++;
	}

	pRecvBuff->m_Append( bSendRecv,dwAddress,pData,nCounter,nSerial );
	m_Display( nCounter,pData,bSendRecv );
	if( m_oHistory.GetCount() > pApp->m_nMaxRecord && pApp->m_nMaxRecord > 0 )
	{
		CRecvBuff*	pHead = (CRecvBuff*)m_oHistory.RemoveHead();
		delete pHead;
	}
	else if( -1 == pApp->m_nMaxRecord && !m_pDoc->GetPathName().IsEmpty() )
	{
		CString		szFileName = m_pDoc->GetPathName() + _T(".log");
		CFile		flog;
		if( flog.Open( szFileName,CFile::modeWrite|CFile::modeNoTruncate ) )
		{
			try {
				flog.SeekToEnd();
				CArchive	ar(&flog,CArchive::store);
				CRecvBuff*	pLine = (CRecvBuff*)m_oHistory.GetTail();
				CRecvBuff&	rLine = *pLine;
				rLine.Serialize( ar );
			} catch( CException* e ) {
				e->ReportError();
				flog.Close();
				return (int)m_oHistory.GetCount();
			}
			flog.Close();
		}
	}
	return (int)m_oHistory.GetCount();
}

void CShowDataView::m_Refresh(BOOL bHexAscii)
{
	CRichEditCtrl&	ctrlEdit = m_ctrlEdit;
	ctrlEdit.SetWindowText( _T("") );
	m_bCaretPosChanged = false;
	m_Display();
}

void CShowDataView::m_OnRecvListViewSelectAll()
{
	m_ctrlEdit.SetSel(0,-1);
}

void CShowDataView::m_OnRecvListViewCopy()
{
	m_ctrlEdit.Copy();
}

void CShowDataView::m_OnRecvListViewClear()
{
	CProbeDoc*		pDoc = (CProbeDoc*) GetDocument();

	if( pDoc && pDoc->m_bRunning )
	{
		MessageBox( _T("模拟程序正在运行，请先中断模拟程序"),
					_T("无法清除数据"),MB_ICONSTOP );	return;
	}
	CRichEditCtrl&	ctrlEdit = m_ctrlEdit;
	ctrlEdit.SetWindowText( _T("") );
	m_bCaretPosChanged = false;
	m_ClearHistory();
}

void CShowDataView::m_OnRecvListViewReview()
{
	CRecvDataDlg	dlg(m_oHistory,this);
	dlg.DoModal();
}

void CShowDataView::m_OnRecvListViewSavelog()
{
	CProbeDoc*			pDoc = (CProbeDoc*) GetDocument();
	CSignalListView*	pViewSignal = pDoc->m_GetSignalView();

	if( pDoc && pDoc->m_bRunning )
	{
		MessageBox( _T("模拟程序正在运行，请先中断模拟程序"),
					_T("无法保存数据"),MB_ICONSTOP );	return;
	}
	static wchar_t BASED_CODE szFilter[] = _T("协议数据记录文件 (*.log)|*.log||");
	CFileDialog	fdlg(FALSE,_T("*.log"),_T("Wyy232"),OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);

	if( pViewSignal && fdlg.DoModal() == IDOK )
	{
		CFile		flog;
		if( flog.Open( fdlg.GetPathName(),CFile::modeCreate|CFile::modeWrite ) )
		{
			CArchive			ar(&flog,CArchive::store);
			CString				szParameter = _T("端口参数:\r\n");
			int					nItem = pViewSignal->GetListCtrl().GetItemCount();
			for( int n = 0; n < nItem; n ++ )
				szParameter +=	pViewSignal->GetListCtrl().GetItemText( n,1 ) + _T("=") +
								pViewSignal->GetListCtrl().GetItemText( n,2 ) + _T("\r\n");
			ar << szParameter;
			ar << (int)m_oHistory.GetCount();
			while( !m_oHistory.IsEmpty() )
			{
				CRecvBuff*	pLine = (CRecvBuff*)m_oHistory.RemoveHead();
				CRecvBuff&	rLine = *pLine;
				rLine.Serialize( ar );
				delete pLine;
			}
		}
		flog.Close();
		m_Display();
	}
}

void CShowDataView::m_OnRecvListViewOpenlog()
{
	CProbeDoc*			pDoc = (CProbeDoc*) GetDocument();
	CSignalListView*	pViewSignal = (CSignalListView*)pDoc->m_GetSignalView();

	if( pDoc->m_bRunning )
	{
		MessageBox( _T("模拟程序正在运行，请先中断模拟程序"),
					_T("无法保存数据"),MB_ICONSTOP );	return;
	}
	static wchar_t BASED_CODE szFilter[] = _T("协议数据记录文件 (*.log)|*.log||");
	CFileDialog	fdlg(TRUE,_T("*.log"),_T("Wyy232"),OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);

	if( fdlg.DoModal() == IDOK )
	{
		CFile		flog;
		if( MessageBox( _T("当前窗口内的数据将被清除"),
						_T("请确认"),MB_YESNO ) != IDYES ) return;
		else m_ClearHistory();
		if( flog.Open( fdlg.GetPathName(),CFile::modeRead ) )
		{
			CWaitCursor			wait;
			CArchive			ar(&flog,CArchive::load);
			CString				szParameter;
			int					nCount = 0;
			ar >> szParameter;
			ar >> nCount;
			//for( int i = 0; i < nCount; i ++ )
			//{
			//	CRecvBuff*	pLine = new CRecvBuff;
			//	CRecvBuff&	rLine = *pLine;
			//	rLine.Serialize( ar );
			//	m_oHistory.AddTail( pLine );
			//}
			try {
				while( true )
				{
					CRecvBuff*	pLine = new CRecvBuff;
					CRecvBuff&	rLine = *pLine;
					rLine.Serialize( ar );
					m_oHistory.AddTail( pLine );
				}
			} catch( CException* e ) {
				e->ReportError();
				flog.Close();
				m_Display();
				return;
			}
		}
		flog.Close();
		m_Display();
	}
}

int CShowDataView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CSizingControlBarG::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect	clientRect;

	GetClientRect( clientRect );
	if (!m_ctrlEdit.Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_MULTILINE|WS_VSCROLL|ES_AUTOVSCROLL,//|ES_AUTOHSCROLL
		CRect(0,0,0,0), this, IDC_EDIT_SHOWDATA_VIEW))
		return -1;
	m_ctrlEdit.ModifyStyle(0,ES_READONLY|ES_LEFT|ES_AUTOVSCROLL);//|LVS_NOCOLUMNHEADER);
	//if (!m_ctrlEdit.CreateEx(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP|WS_EX_CLIENTEDGE,
	//	WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL|ES_AUTOVSCROLL|ES_MULTILINE|ES_READONLY,
	//	CRect(0,0,0,0), this, IDC_LIST_RUNTIME_VIEW))
	//	return -1;
	//m_ctrlEditTemp.Create( 0,clientRect,this,0 );
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

		//point.Offset(CSize(-rect.left,-rect.top));
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

	CSizingControlBarG::OnRButtonDown(nFlags, point);
}

void CShowDataView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bCaretPosChanged = true;	//光标位置发生了变化
}

void CShowDataView::m_OnViewColour()
{
	CColorDialog	dlg( COLOUR_BACKGROUND,0,this );

	if( dlg.DoModal() == IDOK )
		COLOUR_BACKGROUND = dlg.GetColor();
}

void CShowDataView::m_OnViewFilter()
{
	CProbeDoc*	pDoc = (CProbeDoc*) GetDocument();

	if( pDoc )
	{
		CFilterDlg	dlg( pDoc->m_aFilters,this );
		dlg.DoModal();
	}
}

void CShowDataView::OnSize(UINT nType, int cx, int cy)
{
	CSizingControlBarG::OnSize(nType, cx, cy);
	m_ctrlEdit.MoveWindow( 0,0,cx,cy );
}


BOOL CShowDataView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	MSGFILTER*	msgFilter = (MSGFILTER*) lParam;

	if( wParam == IDC_EDIT_SHOWDATA_VIEW )
	{
		if( msgFilter->nmhdr.code == EN_MSGFILTER )
		{
			if( msgFilter->msg == WM_RBUTTONDOWN )
			{
				POINT	point;
				GetCursorPos(&point);
				OnRButtonDown(MK_RBUTTON,point);
			}
			else if( msgFilter->msg == WM_LBUTTONDOWN )
			{
				POINT	point;
				GetCursorPos(&point);
				OnLButtonDown(MK_LBUTTON,point);
			}
		}
	}
	return CSizingControlBarG::OnNotify(wParam, lParam, pResult);
}
