#pragma once


// CConsoleView 视图
#define		MAX_LENGTH		4096
#define		VT100_FLAG0				_T("\x1b")
#define		VT100_FLAG1				_T("\x5b")
#define		VT100_FLAG				_T("\x1b\x5b")
#define		VT100_HEAD				CString(VT100_FLAG)
#define		VT100_UP				CString(VT100_FLAG) + _T("A")
#define		VT100_DOWN				CString(VT100_FLAG) + _T("B")
#define		VT100_RIGHT				CString(VT100_FLAG) + _T("C")
#define		VT100_LEFT				CString(VT100_FLAG) + _T("D")

#define		VT100_FLAG_LEN			2
#define		WORKING_MODE_DEFAULT	0x00
#define		WORKING_MODE_CONSOLE	0x01
#define		WORKING_MODE_VT_EDIT	0x02

class CConsoleView : public CRichEditView
{
	DECLARE_DYNCREATE(CConsoleView)

protected:
	CConsoleView(COLORREF defaultBackColor=RGB(0,0,0),COLORREF defaultForeColor=RGB(255,255,255));           // 动态创建所使用的受保护的构造函数
	virtual ~CConsoleView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnInitialUpdate();
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
protected:
	DECLARE_MESSAGE_MAP()
private:
	int			m_positionEOF;
	long		m_nSavedCaretPos;
	BYTE		m_cMode;
	DWORD		m_dwEffect;
	COLORREF	m_defaultBackColor;
	COLORREF	m_defaultForeColor;
	COLORREF	m_currentBackColor;
	COLORREF	m_currentForeColor;
protected:
	CFont*		m_pFont; 
	LOGFONT		m_lFont;
	CString		m_szBuffer;
	CStringList	m_szSource;
	CStringList	m_szCommands;
private:
	int		m_SearchFirstOne( CString szText,char* sFilter,int nFilter,char& cFirst);
	CString m_SearchFirstOne( CString& szText,char* sFilter,int nFilter,char& cFirst,int& nFirst);
	void	m_SetEffect(COLORREF back=0xFFFFFFFF,COLORREF text=0xFFFFFFFF,DWORD dwEffect = 0);
	void	m_AppendText(CString szText);
	bool	m_VT100_Head(CString& szText);
	bool	m_VT100_DecodingWithout0x0D(CString& szText);
	void	m_VT100_Decoding(CString& szText);
protected:
	long	m_GetCaretEnd();
	long	m_GetCaretPos();
	long	m_GetCaretEOF();
	void	m_GetCaretLineRange(long& nLineHead,long& nLineTail);
	long	m_SetCaretEnd();
	long	m_SetCaretPos(long nPosition);
	long	m_SetCaretPos(int x,int y);
	long	m_SetCaretBOL();
	long	m_SetCaretEOF(long nPositionEOF);
	CString m_PickupCommand();
public:
	void	Serialize(CArchive &ar);
	void	m_AppendEcho(CString szText);
	void	m_InsertEcho(int x,int y,CString szText);
	CString m_GetSelectedText(long& nStartChar,long& nTextLen);
	void	m_ModifyText(CString szText,int x=0,int y=0);	//在x,y位置修改文本，如果x=0或y=0则在当前光标位置修改文本
	void	m_ModifyCommand(CString szCommand);
	BYTE	m_GetMode();
	void	m_SetMode(BYTE mode);
public:
};


