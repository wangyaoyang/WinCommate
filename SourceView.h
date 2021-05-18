
/////////////////////////////////////////////////////////////////////////////
// CSourceView view
#pragma once


// CSourceView йсм╪
#include "ConsoleView.h"

#define	SEND_ASCII_LINE			0
#define	SEND_ASCII_LINE_LF		1
#define	SEND_ASCII_PARAGRAPH	2

class CSourceView : public CConsoleView
{
protected:
	CSourceView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CSourceView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSourceView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSourceView();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	CProbeDoc*	GetDocument(); // non-debug version is inline
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CSourceView)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSendViewCancel();
	afx_msg void OnSendViewAscii();
	afx_msg void OnSendViewHex();
	afx_msg void OnViewColour();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSendViewCyclesend();
	afx_msg void OnSendViewChecksum();
	afx_msg void OnSendViewCheckcrc();
	afx_msg void OnSendviewClear();
	afx_msg void OnSendviewSelAll();
	afx_msg void OnSendViewChecksum2();
	afx_msg void OnSendViewTransHex();
	afx_msg void OnSendViewTransAscii();
	afx_msg void OnSendViewAsciiLine();
	afx_msg void OnSendViewTxtLf();
	afx_msg void OnSendViewEcho();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnUpdateSendViewEcho(CCmdUI *pCmdUI);
	afx_msg void OnSendViewEdit();
	afx_msg void OnUpdateSendViewEdit(CCmdUI *pCmdUI);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
private:
	int			m_nCycleTimer;
	bool		m_bLastSendHex;
private:
	BOOL Trans( CString& source,int& result );
	DWORD SendHexString(CString szText);
	DWORD SendHexLine(int index);
	DWORD SendHex();
	DWORD SendAscii(char cMode);
	DWORD	m_RealSend(LPSTR sData,DWORD nData);
	DWORD	m_RealSend(CString szData);
public:
	void	m_LoadRuntime(CListCtrl& listCtrl);
public:
};

/////////////////////////////////////////////////////////////////////////////
