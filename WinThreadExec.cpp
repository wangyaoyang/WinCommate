// WinThreadExec.cpp : implementation file
//

#include "stdafx.h"
#include "Commate.h"
#include "CommateDoc.h"
#include "RuntimeView.h"
#include "ShowDataView.h"
#include "MainFrm.h"
#include "WinThreadExec.h"
#include "..\..\TmnLibs\SocketApi\CommonFacility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
int WideStr2MultiStr(CString wStr,char* mStr) {
	int		wLen = wStr.GetLength();
	int		mLen = wLen*2+1;
	if (NULL == mStr) mStr = new char[mLen];
	memset( mStr,0x00,mLen );
	return WideCharToMultiByte( 936,WC_COMPOSITECHECK,wStr,wLen,mStr,mLen,NULL,NULL);
}

/////////////////////////////////////////////////////////////////////////////
int Txt2Hex( CString szInput,char* output )
{
	CString	szHex = _T("0x");
	int		length = (int)szInput.GetLength();
	int		i = 0,counter = 0;

	if (NULL == output) return 0;
	for( i=0; i<=length && counter<SEND_BUFF_SIZE-1; i++ ) {
		wchar_t	c = szInput[i];
		if( (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') ||
			(c >= '0' && c <= '9') || c == ' ' || c == '\n' || c == '\0' ) {
			if( c == ' ' || c == '\n' || c == '\0' ) {	// c is a separator
				int		hex = 0;
				swscanf_s( szHex,_T("%x"),&hex );
				output[counter++] = (char)(0x0000ffff&hex);
				if( c != '\0' ) szHex = _T("0x");
				else {
					output[counter] = 0;
					return counter;
				}
			} else {	// c is a digital
				szHex += c;
			}
		}
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
CLineSyntax::CLineSyntax(CProgramLine* pEntry) {
	m_pEntry = pEntry;
	m_len = 0;
	m_param = -1;
	memset( m_ptr,0x00,SEND_BUFF_SIZE );
}

int CLineSyntax::m_SetSyntax(CString sz) {
	m_len = Txt2Hex( sz,m_ptr);
	this->m_Set(m_ptr,m_len);
	return m_len;
}

int CLineSyntax::m_GetLineIndex() {
	if (this->m_pEntry) return this->m_pEntry->m_lineIdx;
	else return -1;
}

bool CLineSyntax::operator == (CHashPerson& target) {
	if (this->m_Equal(target)) {
		CLineSyntax&	tag = (CLineSyntax&) target;
		return (this->m_param == tag.m_param);
	} else return false;
}

/////////////////////////////////////////////////////////////////////////////
CProgramLine::CProgramLine() : m_syntax(this) {
	m_lineIdx = -1;
	m_keyword = KEYWORD_NOTHING;
}

/////////////////////////////////////////////////////////////////////////////
CCommBuffer::CCommBuffer() : CLineSyntax(NULL) {
	m_eventUpdated	= NULL;
	m_eventIdel		= NULL;
}

void CCommBuffer::m_Clear() {
	this->m_len = 0;
}

void CCommBuffer::operator = (CCommBuffer& buf) {
	if (buf.m_len > 0) {
		memcpy_s( this->m_ptr,RECV_BUFF_SIZE,buf.m_ptr,buf.m_len );
		this->m_len = buf.m_len;
		this->m_Set(this->m_ptr,this->m_len);
	}
}

bool CCommBuffer::m_Update(int nBuf,char* sBuf) {
	if (0 < nBuf && nBuf < RECV_BUFF_SIZE && sBuf != NULL) {
		memcpy_s( this->m_ptr,RECV_BUFF_SIZE,sBuf,nBuf);
		this->m_len = nBuf;
		this->m_Set(this->m_ptr,this->m_len);
		return true;
	} else return false;
}

int CCommBuffer::m_Append(int nBuf,char* sBuf) {
	if (0 < nBuf && this->m_len + nBuf < RECV_BUFF_SIZE && sBuf != NULL) {
		memcpy_s( this->m_ptr + this->m_len,RECV_BUFF_SIZE,sBuf,nBuf);
		this->m_len += nBuf;
		this->m_Set(this->m_ptr,this->m_len);
	}
	return this->m_len;
}

bool CCommBuffer::m_IsContain(int nBuf,char* sBuf) {
	if (sBuf && 0 < nBuf && nBuf <= m_len && memcmp(m_ptr,sBuf,nBuf) == 0) return true;
	else return false;
}

CString CCommBuffer::m_AsString() {
	if (0 < this->m_len && this->m_len < RECV_BUFF_SIZE-1) {
		this->m_ptr[this->m_len] = 0;
		return CString(this->m_ptr);
	} else return _T("");
}
/////////////////////////////////////////////////////////////////////////////
// CWinThreadExec

IMPLEMENT_DYNCREATE(CWinThreadExec, CWinThread)

CWinThreadExec::CWinThreadExec()
{
	m_bDebug = false;
	m_isRun = NULL;
	m_isBreak = NULL;
	m_nMaxline = 0;
	m_pDoc			= NULL;
	m_pViewRuntime	= NULL;
	m_pViewInput	= NULL;
	m_pViewSignal	= NULL;
}

CWinThreadExec::~CWinThreadExec()
{
}

void CWinThreadExec::m_Attach(CProbeDoc* pDoc)
{
	m_pDoc = pDoc;
	m_pViewRuntime = pDoc->m_GetRuntimeView();
	m_pViewSignal = pDoc->m_GetSignalView();
	m_pViewInput = pDoc->m_GetShowDataView();
	m_com.m_SetIpTransfer(m_pDoc->m_bIpTransfer,m_pDoc->m_dwIpAddress1,m_pDoc->m_nIpPort1,
												m_pDoc->m_dwIpAddress2,m_pDoc->m_nIpPort2);
}

BOOL CWinThreadExec::InitInstance()
{
	m_buffer.m_eventUpdated = CreateEvent( NULL,TRUE,FALSE,TRACER_EXECUTABLE_EVENT );
	m_buffer.m_eventIdel = CreateEvent(NULL,TRUE,TRUE,TRACER_EXEREADING_EVENT);
	m_isRun = CreateEvent( NULL,TRUE,TRUE,TRACER_EXECFINISH_EVENT );
	m_isBreak = CreateEvent( NULL,TRUE,TRUE,TRACER_EXEC_BREAK_EVENT );
	return TRUE;
}

int CWinThreadExec::ExitInstance()
{
	if( m_buffer.m_eventUpdated ) CloseHandle( m_buffer.m_eventUpdated );	
	if( m_buffer.m_eventIdel ) CloseHandle( m_buffer.m_eventIdel );	
	if( m_isRun ) CloseHandle( m_isRun );	
	if( m_isBreak ) CloseHandle( m_isBreak );	
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CWinThreadExec, CWinThread)
	//{{AFX_MSG_MAP(CWinThreadExec)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinThreadExec message handlers

int CWinThreadExec::Run() 
{
	bool	run = true;
	for( ;!IsOver();Sleep(50) )
	{
		TRACE0( _T("\nExec") );
		m_buffer.m_Clear();
		Execute();
	}
	return CWinThread::Run();
}

BOOL CWinThreadExec::IsOver()
{
	DWORD	result = WaitForSingleObject( m_isRun,0 );
	switch( result )
	{
	case WAIT_OBJECT_0:		return FALSE;	//未结束
	case WAIT_ABANDONED:
	case WAIT_TIMEOUT:
	default:				return TRUE;	//任务结束
	}
	return FALSE;
}

//int GetLeableIndex( CProgramLine program[],int nMaxLine,int hashcode)
//{
//	for( int i = 0; i <= nMaxLine; i ++ ) {
//		if( program[i].m_keyword == KEYWORD_LABEL &&
//			program[i].m_syntax.m_para == hashcode ) return i;
//	}
//	return -1;
//}
//
void CWinThreadExec::DispCurrentLine( CProgramLine& prgLine,int nNext,UINT uState )
{
	if (m_bDebug) {
		CString	szIndex;
		if (m_pViewRuntime == 0 || prgLine.m_lineIdx == -1) return;
		if (0 == uState) {	//Current line executed over
			szIndex.Format(_T("%d→№ %d"),prgLine.m_lineIdx+1,nNext+1);	Sleep(200);
		} else {
			szIndex.Format(_T("%d■■■■"),prgLine.m_lineIdx+1);			Sleep(200);
		}
		m_pViewRuntime->GetListCtrl().SetItemText(prgLine.m_lineIdx,0,szIndex);
		m_pViewRuntime->GetListCtrl().EnsureVisible(prgLine.m_lineIdx,TRUE);
		if( m_pViewRuntime->GetListCtrl().GetItemState( prgLine.m_lineIdx,LVIS_SELECTED ) == LVIS_SELECTED ) {	//选中中断
			WaitForSingleObject( m_isBreak,INFINITE );
		}
	}
	//if( uState == 0 ) {
	//	CString		szSteps;
	//	szSteps.Format( _T("[%03d]→%03d"),m_nStep,prgLine.m_lineIdx,m_nStep );
	//	m_pViewRuntime->GetListCtrl().SetItemText( prgLine.m_lineIdx,0,szSteps );
	//	m_nStep = prgLine.m_lineIdx;
	//}
}

bool CWinThreadExec::Execute()
{
	CProgramLine*	prg = m_prg;
	bool			bIncomming = false;
	bool			bWhenMatch = false;
	for( int i = 0; !IsOver() && i <= m_nMaxline; i ++ ) {
		if (i < 0 || i >= MAX_PROGRAM_LINE) return false;
		DispCurrentLine( prg[i],i,DISP_LINE_BEGIN );
		switch (prg[i].m_keyword) {
		case KEYWORD_WAIT:
			if (WAIT_OBJECT_0 == WaitForSingleObject(m_buffer.m_eventUpdated,prg[i].m_syntax.m_param)) {
				ResetEvent( m_buffer.m_eventIdel );	//	有新数据更新
				m_recentMessage = m_buffer;
				m_recentMessage.m_param = i;
				m_buffer.m_Clear();
				bIncomming = true;
				ResetEvent( m_buffer.m_eventUpdated );
				SetEvent( m_buffer.m_eventIdel );
			}	//	无新数据
			DispCurrentLine( prg[i],i+1,DISP_LINE_OVER );
			break;
		case KEYWORD_WHEN:
		case KEYWORD_WHENTXT:
			if (bIncomming) {
				bIncomming = false;
				CLineSyntax*	syntax = (CLineSyntax*)this->m_map.m_Find((CHashPerson&)this->m_recentMessage);
				int				nFound = -1;
				if (syntax) nFound = syntax->m_GetLineIndex();
				if (nFound >= 0) {
					DispCurrentLine( prg[i],nFound,DISP_LINE_OVER );
					i = nFound-1;
					bWhenMatch = true;
				}
			} else DispCurrentLine( prg[i],i+1,DISP_LINE_OVER );
			break;
		case KEYWORD_THEN:
		case KEYWORD_THENTXT:
			if (bWhenMatch) {
				bWhenMatch = false;
				DispCurrentLine( prg[i],prg[i].m_syntax.m_param,DISP_LINE_OVER );
				i = (prg[i].m_syntax.m_param >= 0) ? (prg[i].m_syntax.m_param-1) : (i+1);
			} else DispCurrentLine( prg[i],i+1,DISP_LINE_OVER );
			break;
		case KEYWORD_GOTO:
			DispCurrentLine( prg[i],prg[i].m_syntax.m_param,DISP_LINE_OVER );
			i = (prg[i].m_syntax.m_param >= 0) ? (prg[i].m_syntax.m_param-1) : (i+1);
			break;
		case KEYWORD_SLEEP:
			Sleep( prg[i].m_syntax.m_param );
			DispCurrentLine( prg[i],i+1,DISP_LINE_OVER );
			break;
		case KEYWORD_SEND:
			ResetEvent( m_buffer.m_eventIdel );
			m_SendHex( prg[i].m_syntax.m_len,prg[i].m_syntax.m_ptr );
			SetEvent( m_buffer.m_eventIdel );
			DispCurrentLine( prg[i],i+1,DISP_LINE_OVER );
			break;
		case KEYWORD_SENDTXT:
			ResetEvent( m_buffer.m_eventIdel );
			m_SendTxt( prg[i].m_string + _T("\r\n"));
			SetEvent( m_buffer.m_eventIdel );
			DispCurrentLine( prg[i],i+1,DISP_LINE_OVER );
			break;
		case KEYWORD_LABEL:
			DispCurrentLine( prg[i],i+1,DISP_LINE_OVER );
			break;
		case KEYWORD_END:
			return true;
		default:;
		}
	}
	return true;
}

DWORD CWinThreadExec::m_SendHex(int nData,char* sData)
{
	if( m_pDoc && m_pDoc->m_GetThread() )
	{
		DWORD	add = m_pDoc->m_GetThread()->m_GetComm().m_GetRemoteAdd();
		if( sData != NULL && nData > 0 )
			m_pDoc->m_GetThread()->m_GetComm().m_Send(sData,(DWORD&)nData);
		if( m_pViewInput && sData != NULL && nData > 0 )
			m_pViewInput->m_Append( true,0,add,sData,(DWORD&)nData );
	}
	return nData;
}

DWORD CWinThreadExec::m_SendTxt(CString szData)
{
	DWORD	nData = szData.GetLength()*2;
	char*	sData = new char[ nData + 1 ];
	nData = WideStr_MultiByteStr( szData,sData,nData );
	if( m_pDoc && m_pDoc->m_GetThread() )
	{
		DWORD	add = m_pDoc->m_GetThread()->m_GetComm().m_GetRemoteAdd();
		m_pDoc->m_GetThread()->m_GetComm().m_Send(sData,nData);
		if( m_pViewInput)
			m_pViewInput->m_Append( true,0,add,sData,nData );
	}
	delete [] sData;
	return nData;
}

bool CProgramLine::m_Init(int index,CString szKeyword,CString szValue,CHashMapping& map) {
	m_lineIdx = index;
	m_string = szValue;
	if( szKeyword == _T("WAIT") ) {	//para = waiting time
		m_keyword = KEYWORD_WAIT;
		if (m_string.Trim().IsEmpty()) m_syntax.m_param = INFINITE;
		else m_syntax.m_param = _wtoi(m_string);
	} else if( szKeyword == _T("WHEN") )	{	//para = hash code of the hex string
		m_keyword = KEYWORD_WHEN;
		m_syntax.m_SetSyntax(m_string);
	} else if( szKeyword == _T("WHENTXT") ) {	//para = hash code of the text string
		m_keyword = KEYWORD_WHENTXT;
		m_syntax.m_len = WideStr2MultiStr(m_string,m_syntax.m_ptr);
		m_syntax.m_Set(m_syntax.m_ptr,m_syntax.m_len);
	} else if( szKeyword == _T("THEN") ) {		//para = line index of the label text(search by hash code)
		m_keyword = KEYWORD_THEN;
		m_syntax.m_len = WideStr2MultiStr(m_string,m_syntax.m_ptr);
	} else if( szKeyword == _T("THENTXT") ) {	//para = line index of the label text(search by hash code)
		m_keyword = KEYWORD_THENTXT;
		m_syntax.m_len = WideStr2MultiStr(m_string,m_syntax.m_ptr);
	} else if( szKeyword=="GOTO" ) {			//para = line index of the label text(search by hash code)
		m_keyword = KEYWORD_GOTO;
		m_syntax.m_len = WideStr2MultiStr(m_string,m_syntax.m_ptr);
	} else if( szKeyword=="SLEEP" ) {			//para = sleep time (int ms),default(not specified) is 1000 ms
		m_keyword = KEYWORD_SLEEP;
		if (m_string.Trim().IsEmpty()) m_syntax.m_param = 1000;
		else m_syntax.m_param = _wtoi(m_string);
	} else if( szKeyword=="SEND" ) {
		m_keyword = KEYWORD_SEND;
		m_syntax.m_SetSyntax(m_string);
	} else if( szKeyword=="SENDTXT" ) {
		m_keyword = KEYWORD_SENDTXT;
	} else if( szKeyword=="END" ) {
		m_keyword = KEYWORD_END;
	} else {									//para = hash code of the label text
		m_keyword = KEYWORD_LABEL;
		m_string = szKeyword;
		m_syntax.m_len = WideStr2MultiStr(m_string,m_syntax.m_ptr);
		m_syntax.m_Set(m_syntax.m_ptr,m_syntax.m_len);
		map.m_Add(&m_syntax);
	}
	return true;
}

bool CProgramLine::m_Finalize(CHashMapping& map,int& waitIndex) {
	CLineSyntax*	syntax = NULL;
	switch (m_keyword) {
	case KEYWORD_WAIT:
		waitIndex = m_syntax.m_GetLineIndex();
		break;
	case KEYWORD_WHEN:		//All WHEN/WHENTEXT syntax are mapped into a 
	case KEYWORD_WHENTXT:	//WAIT syntax that specified by the line index
		m_syntax.m_param = waitIndex;
		map.m_Add(&m_syntax);
		break;
	case KEYWORD_GOTO:
	case KEYWORD_THEN:
	case KEYWORD_THENTXT:
		m_syntax.m_Set(m_syntax.m_ptr,m_syntax.m_len);
		syntax = (CLineSyntax*) map.m_Find(m_syntax);
		m_syntax.m_param = syntax ? syntax->m_GetLineIndex() : -1;
		break;
	default:;
	}
	return true;
}

BOOL CWinThreadExec::LoadProgram()
{
	CProbeApp*			pApp = (CProbeApp*)AfxGetApp();
	CMainFrame*			pWnd = (CMainFrame*)pApp->GetMainWnd();
	pWnd->m_wndStatusBar.UpdateWindow();

	Beep( 1000,	100 );
	CWaitCursor		wait;
	if( !m_pDoc ) return FALSE;
	if( !m_pViewRuntime ) return FALSE;
	CProgramLine*	program = m_prg;
	CListCtrl&		ctrl = m_pViewRuntime->GetListCtrl();

	m_nMaxline = ctrl.GetItemCount();
	for( int curline = 0; curline < m_nMaxline; curline ++ ) {
		program[curline].m_Init(curline,
			ctrl.GetItemText( curline,1 ),
			ctrl.GetItemText( curline,2 ),this->m_map);
	}
	int	waitIndex = -1;
	for( int curline = 0; curline < m_nMaxline; curline ++ ) {
		program[curline].m_Finalize(this->m_map,waitIndex);
	}
	
	Beep( 500,	100 );
	return TRUE;
}
