// MainFrm.h : CMainFrame ��Ľӿ�
//


#pragma once
class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// ʵ��
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
friend class CWinThreadExec;
protected:  // �ؼ���Ƕ���Ա
	CStatusBar  m_wndStatusBar;
	CReBar      m_wndReBar;
	CToolBar    m_wndToolBar;
	CToolBar    m_wndToolBar2;

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
};


