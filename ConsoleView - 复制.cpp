// ConsoleView.cpp : 实现文件
//

#include "stdafx.h"
#include "Commate.h"
#include "ConsoleView.h"


// CConsoleView
#define		FONT_0					200
#define		CONSOLE_MAX_ROWS		80
#define		CONSOLE_MAX_COLS		80
#define		MAX_FILTER				10

static COLORREF	COLOR_TEXT_BACK[] = { RGB(0,0,0),RGB(128,0,0),RGB(0,255,0),RGB(255,255,0),RGB(0,0,255),RGB(255,0,255),RGB(0,128,0),RGB(255,255,255) };
static COLORREF	COLOR_TEXT_FORE[] = { RGB(0,0,0),RGB(255,0,0),RGB(0,255,0),RGB(255,255,0),RGB(0,0,255),RGB(255,0,255),RGB(0,255,255),RGB(255,255,255) };
static char sFILTER[MAX_FILTER] = { 'm','A','B','C','D','H','J','K','s','u' };

IMPLEMENT_DYNCREATE(CConsoleView, CRichEditView)

CConsoleView::CConsoleView(COLORREF defaultBackColor,COLORREF defaultForeColor)
{
	m_positionEOF	= 0;
	m_currentBackColor = m_defaultBackColor = defaultBackColor;
	m_currentForeColor = m_defaultForeColor = defaultForeColor;
	m_cMode = WORKING_MODE_DEFAULT;
	m_dwEffect = 0;
	//m_bOnCommand = false;
	m_pFont	= new CFont();
}

CConsoleView::~CConsoleView()
{
	delete m_pFont;
	m_szSource.RemoveAll();
}

BEGIN_MESSAGE_MAP(CConsoleView, CRichEditView)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()


// CConsoleView 诊断

#ifdef _DEBUG
void CConsoleView::AssertValid() const
{
	CRichEditView::AssertValid();
}

#ifndef _WIN32_WCE
void CConsoleView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}
#endif
#endif //_DEBUG


// CConsoleView 消息处理程序
BYTE CConsoleView::m_GetMode()
{
	return m_cMode;
}

void CConsoleView::m_SetMode(BYTE mode)
{
	m_cMode = mode;
}

void CConsoleView::m_SetEffect(COLORREF back,COLORREF text,DWORD dwEffect)
{
	if( back != 0xFFFFFFFF ) m_currentBackColor = back;
	if( text != 0xFFFFFFFF ) m_currentForeColor = text;
	if( back == text ) text = 0x00ff0000&(~back);
	if( dwEffect != 0 ) m_dwEffect = dwEffect;

	CHARFORMAT2		cf;
	GetRichEditCtrl().GetDefaultCharFormat(cf);

	cf.dwMask		= CFM_SIZE | CFM_WEIGHT | CFM_COLOR | CFM_BACKCOLOR | CFM_BOLD | CFM_UNDERLINE | CFM_HIDDEN | CFM_REVISED | CFM_STRIKEOUT;
	cf.yHeight		= FONT_0;
	cf.wWeight		= 80;
	cf.crBackColor	= m_currentBackColor;
	cf.crTextColor	= m_currentForeColor;
	cf.dwEffects	= dwEffect;
	SetCharFormat( cf );
}

CString CConsoleView::m_SearchFirstOne( CString& szText,char* sFilter,int nFilter,char& cFirst,int& nFirst)
{
	CString	szFirst;
	nFirst = m_SearchFirstOne( szText,sFilter,nFilter,cFirst );
	if( nFirst >= 0 && nFirst < szText.GetLength() )
	{
		szFirst = szText.Left( nFirst );
		szText = szText.Right( szText.GetLength() - nFirst - 1 );
	}
	return szFirst;
}

int CConsoleView::m_SearchFirstOne( CString szText,char* sFilter,int nFilter,char& cFirst)
{
	int		nFirst = szText.GetLength();
	for( int i = 0; i < nFilter; i ++ )
	{
		int	nIndex = szText.Find( sFilter[i] );
		if( nIndex == -1 || nIndex >= 10 ) continue;	//假设VT100命令长度不会超过10
		if( nIndex < nFirst )
		{
			nFirst = nIndex;
			cFirst = sFilter[i];
		}
	}
	return nFirst;
}

void CConsoleView::Serialize(CArchive &ar)
{
	try
	{
		CRichEditCtrl&	ctrl = this->GetRichEditCtrl();
		CString			szSource;
		if( ar.IsStoring() )
		{
			wchar_t		sLineBuff[MAX_LENGTH];
			for( int n = 0; n < ctrl.GetLineCount(); n ++ )
			{
				memset( sLineBuff,0x00,MAX_LENGTH );
				if( ctrl.GetLine( n,sLineBuff,MAX_LENGTH-1 ) > 0 )
				{
					szSource = CString( sLineBuff );
					szSource.Replace( _T("\r"),_T("\r\n") );
					ar.WriteString( szSource );
				}
			}
		}
		else
		{
			m_szSource.RemoveAll();
				while( ar.ReadString( szSource ) )
					m_szSource.AddTail( szSource );
		}
	}
	catch (CArchiveException* e)
	{
		e->ReportError();
	}
}

long CConsoleView::m_SetCaretEOF(long nPositionEOF)
{
	if( nPositionEOF < 0 )
	{
		SetWindowText( _T("") );
		m_positionEOF = 0;
	}
	m_positionEOF = nPositionEOF;
	return m_positionEOF;
}

long CConsoleView::m_GetCaretEOF()
{
	return m_positionEOF;
}

long CConsoleView::m_GetCaretPos()
{
	long	nStart = 0,nEnd = 0;
	GetRichEditCtrl().GetSel( nStart,nEnd );
	return nStart;
}

long CConsoleView::m_SetCaretPos(long nIndex)
{
	GetRichEditCtrl().SetSel( nIndex,nIndex );
	return nIndex;
}

long CConsoleView::m_SetCaretPos(int x,int y)
{	//x,y 是行号、列号, 1 基索引
	CRichEditCtrl&	ctrl = GetRichEditCtrl();
	int				nLineCount = ctrl.GetLineCount();
	int				n = 0;
	if( nLineCount < y )
	{	//行数不够，补齐
		CString		szInsert;
		int			nInsert = y - nLineCount;
		for( n = 0; n < nInsert; n ++ ) szInsert += _T("\r\n");
		if( !szInsert.IsEmpty() )
		{
			this->m_AppendText( szInsert );
		}
	}
	int	nMaxColumn = ctrl.LineLength( ctrl.LineIndex(y-1) );
	if( nMaxColumn < x )
	{
		CString		szInsert;
		int			nInsert = x - nMaxColumn;
		int			nPosition = ctrl.LineIndex( y-1 )+nMaxColumn;
		for( n = 0,szInsert = _T(""); n < nInsert; n ++ ) szInsert += _T(" ");
		if( !szInsert.IsEmpty() )
		{
			this->m_SetCaretPos( nPosition );
			this->m_ModifyText( szInsert );
		}
	}
	int	nPosition = ctrl.LineIndex(y-1) + (x-1);
	if( nPosition < 0 ) nPosition = 0;
	m_SetCaretPos( nPosition );
	if( this->m_cMode & WORKING_MODE_VT_EDIT ) this->m_SetCaretEOF( nPosition );
	return m_GetCaretPos();
}

long CConsoleView::m_SetCaretBOL()
{	//x,y 是行号、列号, 1 基索引
	CRichEditCtrl&	ctrl = GetRichEditCtrl();
	int	nPosition = ctrl.LineIndex(-1);
	if( nPosition < 0 ) nPosition = 0;
	m_SetCaretPos( nPosition );
	if( this->m_cMode & WORKING_MODE_VT_EDIT ) this->m_SetCaretEOF( nPosition );
	return m_GetCaretPos();
}

long CConsoleView::m_GetCaretEnd()
{
	long	nCurrent = m_GetCaretPos();
	long	nLastPos = m_SetCaretEnd();
	m_SetCaretPos( nCurrent );
	return nLastPos;
}

long CConsoleView::m_SetCaretEnd()
{
	CRichEditCtrl&	ctrl = GetRichEditCtrl();
	long			nLength = ctrl.GetTextLength();
	long			nStart = 0, nEnd = 0;

	ctrl.SetSel( nLength,nLength );
	ctrl.GetSel( nStart,nEnd );
	return nEnd;
}

void CConsoleView::m_GetCaretLineRange(long& nLineHead,long& nLineTail)
{
	nLineHead = GetRichEditCtrl().LineIndex(-1);
	nLineTail = nLineHead + GetRichEditCtrl().LineLength(nLineHead);
}

void CConsoleView::m_AppendText(CString szText)
{
	this->m_SetCaretEnd();
	this->m_ModifyText(szText);
}

CString CConsoleView::m_PickupCommand()
{
	CString	szCommand;
	long	nLineHead = 0, nLineTail = 0;
	long	nPosition = m_GetCaretEOF();

	this->m_SetCaretEnd();								//光标设置在命令行结束的位置
	this->m_GetCaretLineRange( nLineHead,nLineTail );	//取当前行的行首、行尾索引
	if( nLineHead < nPosition && nPosition < nLineTail )
	{
		if( GetRichEditCtrl().GetTextRange( nPosition,nLineTail,szCommand ) > 0 )
			return szCommand;
	}
	else m_SetCaretEOF( nLineTail );
	return szCommand;
}

void CConsoleView::m_ModifyCommand(CString szCommand)
{
	long	nLineHead = 0, nLineTail = 0;
	long	nPosition = m_GetCaretEOF();
	
	m_SetCaretEnd();
	m_GetCaretLineRange( nLineHead,nLineTail );
	if( nLineHead < nPosition && nPosition <= nLineTail )
	{
		GetRichEditCtrl().SetSel( nPosition,nLineTail );
		m_SetEffect();
		GetRichEditCtrl().ReplaceSel( szCommand );
	}
}

CString CConsoleView::m_GetSelectedText(long& nStartChar,long& nTextLen)
{
	CRichEditCtrl&	ctrl = GetRichEditCtrl();
	CString		szText;
	long		nEndChar = 0;
	wchar_t*	sLineBuff = NULL;

	ctrl.GetSel( nStartChar, nEndChar );
	nTextLen = nEndChar - nStartChar;
	sLineBuff = new wchar_t[nTextLen+1];
	wmemset( sLineBuff,0x00,nTextLen+1 );
	ctrl.GetSelText((LPSTR)sLineBuff);
	szText = sLineBuff;
	delete [] sLineBuff;
	return szText;
}

void CConsoleView::m_ModifyText(CString szText,int x,int y)
{
	CRichEditCtrl&	ctrl = GetRichEditCtrl();

	long	nLineHead = 0, nLineTail = 0;
	//long	nInsert = (this->m_cMode & WORKING_MODE_VT_EDIT ) ? m_GetCaretEOF() : m_GetCaretPos();
	long	nPosition = ( x > 0 && y > 0 ) ? m_SetCaretPos(x,y) : m_GetCaretPos();//nInsert;

	m_GetCaretLineRange( nLineHead,nLineTail );

	long	nLineRest = nLineTail - nPosition;
	long	nLengText = szText.GetLength();
	long	nSelected = (nLengText < nLineRest) ? nLengText : nLineRest;
	//long	nSelected = nLengText;

	ctrl.SetSel( nPosition,nPosition + nSelected );
	m_SetEffect();
	ctrl.ReplaceSel( szText );

	if( m_cMode & WORKING_MODE_VT_EDIT )
	{	//
		if( ctrl.GetLineCount() > CONSOLE_MAX_ROWS )
		{
			ctrl.SetSel( m_GetCaretPos(),-1 );
			ctrl.ReplaceSel( _T("") );
		}
		//m_SetCaretEOF( nPosition );
		m_SetEffect();
	}
	else if( m_cMode & WORKING_MODE_CONSOLE )
	{
		m_SetCaretEOF( m_GetCaretPos() );
	}
}

bool CConsoleView::m_VT100_Head(CString& szText)
{
	char	cKey = 0;
	int		nKey = 0;
	int		nX = 0;
	int		nY = 0;
	int		nSeperator = 0;

	CString	szKey = m_SearchFirstOne( szText,sFILTER,MAX_FILTER,cKey,nKey );
	switch( cKey )
	{
	case 'A':
	case 'B':
	case 'C':
	case 'D':	break;
	case 'H':
		if( (nSeperator = szKey.Find( ';' )) > 0 )
		{
			CString	szY = szKey.Left( nSeperator );
			CString	szX = szKey.Mid( nSeperator+1,nKey-nSeperator-1 );
			swscanf_s( szX,_T("%d"),&nX );
			swscanf_s( szY,_T("%d"),&nY );
			m_SetCaretPos( nX,nY );
		}
		break;
	case 'K':
		{
			long	nPosition = m_GetCaretPos();
			int		nLineHead = GetRichEditCtrl().LineIndex();
			int		nLineLeng = GetRichEditCtrl().LineLength();
			int		nLineTail = nLineHead + nLineLeng;
			int		nParameter = 0;
			swscanf_s( szKey,_T("%d"),&nParameter );
			switch( nParameter )
			{
			case 0:	GetRichEditCtrl().SetSel( nPosition,nLineTail );	break;
			case 1:	GetRichEditCtrl().SetSel( nLineHead,nLineTail );	break;
			case 2:	GetRichEditCtrl().SetSel( nLineHead,nPosition );	break;
			default:;
			}
			GetRichEditCtrl().ReplaceSel( _T("") );
		}
		break;
	case 'J':
		{
			m_SetCaretEOF(-1);
		}
		break;
	case 'm':
		{
			COLORREF	colorBack = m_defaultBackColor;
			COLORREF	colorFore = m_defaultForeColor;
			int			nBackColor = 0;
			int			nTextColor = 0;
			if( (nSeperator = szKey.Find( ';' )) > 0 )
			{
				CString	szBackColor = szKey.Left( nSeperator );
				CString	szTextColor = szKey.Mid( nSeperator+1,nKey-nSeperator-1 );
				swscanf_s( szBackColor,_T("%d"),&nBackColor );
				swscanf_s( szTextColor,_T("%d"),&nTextColor );
				if( nBackColor >= 40 )
				{
					nBackColor -= 40;	nBackColor = nBackColor > 7 ? 7 : nBackColor;
					colorBack = COLOR_TEXT_BACK[nBackColor];
				}
				if( nTextColor >= 30 )
				{
					nTextColor -= 30;	nTextColor = nTextColor > 7 ? 7 : nTextColor;
					colorFore = COLOR_TEXT_FORE[nTextColor];
				}
				m_SetEffect(colorBack,colorFore,CFE_BOLD);
			}
			else
			{
				CString	szTextColor = szKey.Left( nKey );
				swscanf_s( szTextColor,_T("%d"),&nTextColor );
				if( nTextColor >= 30 )
				{
					nTextColor -= 30;	nTextColor = nTextColor > 7 ? 7 : nTextColor;
					m_currentForeColor = COLOR_TEXT_FORE[nTextColor];
					m_SetEffect(0xFFFFFFFF,COLOR_TEXT_FORE[nTextColor],0);
				}
				else if( nTextColor == 0 ) m_SetEffect(m_defaultBackColor,m_defaultForeColor,CFE_BOLD);
				else if( nTextColor == 1 ) m_SetEffect(m_defaultBackColor,m_defaultForeColor+RGB(32,32,32),CFE_BOLD);//CFE_STRIKEOUT
				else if( nTextColor == 4 ) m_SetEffect(m_defaultBackColor,m_defaultForeColor,CFE_BOLD);//CFE_UNDERLINE
				else if( nTextColor == 5 ) m_SetEffect(m_defaultBackColor,m_defaultForeColor,CFE_BOLD);//CFM_REVISED
				else if( nTextColor == 7 ) m_SetEffect(m_defaultForeColor,m_defaultBackColor,CFE_BOLD);
				else if( nTextColor == 8 ) m_SetEffect(m_defaultBackColor,m_defaultForeColor,CFE_ITALIC);//CFE_HIDDEN
			}
		}
		break;
	default:	return false;
	}
	return true;
}

bool CConsoleView::m_VT100_DecodingWithout0x0D(CString& szText)
{
	while( !szText.IsEmpty() )
	{
		int	nVT100 = szText.Find( VT100_HEAD );
		if( nVT100 == 0 )
		{	//VT100标志在头部
			szText = szText.Right( szText.GetLength() - VT100_HEAD.GetLength() );
			bool bHead = m_VT100_Head( szText );
			if( !bHead )
			{
				if( szText.Find( VT100_HEAD ) < 0 && szText.GetLength() < 10 )
				{	//没有其他的VT100命令且剩余长度不足10，则认为报文还没收全
					szText = VT100_HEAD + szText;	return false;
				}
			}
		}
		else if( nVT100 > 0 )
		{	//VT100标志在中间
			CString	szContent = szText.Left( nVT100 );
			szText = szText.Right( szText.GetLength() - nVT100 );
			this->m_ModifyText(szContent);
		}
		else if( nVT100 < 0 )
		{	//没找到VT100标志
			if( (nVT100 = szText.Find( VT100_FLAG0 )) >= 0 )
			{	//找到前半个VT100标志
				return false;
			}
			else
			{
				this->m_ModifyText(szText);	szText = _T("");
			}
		}
	}
	return true;
}

void CConsoleView::m_VT100_Decoding(CString& szVT100)
{
	int	nMaxSel = 0;
	while( !szVT100.IsEmpty() )
	{
		int	nVT100 = szVT100.Find( '\r' );
		if( nVT100 >= 0 )
		{
			if( szVT100[nVT100+1] != '\n' )
			{	//将0x0d解释为：光标移动到行首
				CString	szText = szVT100.Left(nVT100);
				if( !this->m_VT100_DecodingWithout0x0D( szText ) )
				{
					this->m_ModifyText( szText.Left(nVT100) );
				}
				this->m_SetCaretBOL();
				szVT100 = szVT100.Right( szVT100.GetLength()-nVT100-1 );
			}
			else
			{
				//this->m_bOnCommand = false;
				this->m_VT100_DecodingWithout0x0D( szVT100.Left(nVT100+1) );
				szVT100 = szVT100.Right( szVT100.GetLength()-nVT100-2 );
			}
		}
		else
		{
			if( !this->m_VT100_DecodingWithout0x0D( szVT100 ) ) return;
		}
	}
}

void CConsoleView::m_AppendEcho(CString szText)
{
	if( m_cMode & (WORKING_MODE_CONSOLE|WORKING_MODE_VT_EDIT)  )
	{
		m_szBuffer += szText;
		//m_szBuffer.Replace( "\r","" );
		m_VT100_Decoding( m_szBuffer );
	}
}

void CConsoleView::OnInitialUpdate()
{
	CFont			newFont;
	CRichEditCtrl&	ctrl = GetRichEditCtrl();

	newFont.CreateFont(	12,0,0,0,FW_BOLD,TRUE,FALSE,0,
						ANSI_CHARSET,OUT_DEFAULT_PRECIS,
						CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
						DEFAULT_PITCH | FF_DONTCARE, _T("宋体 & 新宋体") );
	m_pFont->CreatePointFont( 90,(LPCTSTR)_T("宋体") ); 
	m_pFont->GetLogFont(&m_lFont);
	m_lFont.lfWeight = FW_BOLD;
	m_pFont->DeleteObject(); 
	m_pFont->CreateFontIndirect(&m_lFont);
	ctrl.SetFont(m_pFont);
	ctrl.SetBackgroundColor( FALSE,m_defaultBackColor );
	m_SetEffect(m_defaultBackColor,m_defaultForeColor);

	CRichEditView::OnInitialUpdate();
}

void CConsoleView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	m_SetEffect( m_defaultBackColor,m_defaultForeColor,m_dwEffect );
	for( bool bFirstLine = true; !m_szSource.IsEmpty(); bFirstLine = false )
	{
		CString	szSource = m_szSource.RemoveHead();
		if( !bFirstLine ) szSource = _T("\r\n") + szSource;
		if( !szSource.IsEmpty() ) this->m_AppendText(szSource);
	}
}

void CConsoleView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( this->m_cMode & WORKING_MODE_VT_EDIT )
	{
		//if( !m_bOnCommand ) this->m_cMode &= (~WORKING_MODE_VT_EDIT);
		//else 
			//this->m_SetCaretPos( this->m_GetCaretEOF() );
		if( nChar == VK_BACK || nChar == VK_CLEAR )
			return;
	}
	else if( this->m_cMode & WORKING_MODE_CONSOLE )
	{
		//if( nChar == VK_RETURN ) m_bOnCommand = true;
		//else if( m_bOnCommand ) this->m_cMode |= WORKING_MODE_VT_EDIT;
		//if( nChar == VK_RETURN ) return;
	}
	CRichEditView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CConsoleView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( this->m_cMode & WORKING_MODE_VT_EDIT ) return;
	else if( this->m_cMode & WORKING_MODE_CONSOLE )
	{
		//if( nChar == VK_RETURN )
		//	return;
	}
	CRichEditView::OnChar(nChar, nRepCnt, nFlags);
}

void CConsoleView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( this->m_cMode & WORKING_MODE_VT_EDIT ) return;

	CRichEditView::OnLButtonDown(nFlags, point);
}

void CConsoleView::OnRButtonDown(UINT nFlags, CPoint point)
{
	if( this->m_cMode & WORKING_MODE_VT_EDIT ) return;

	CRichEditView::OnRButtonDown(nFlags, point);
}
