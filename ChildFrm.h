// ChildFrm.h : CChildFrame ��Ľӿ�
//


#pragma once

#include "SignalListView.h"
#include "RuntimeView.h"
#include "ShowDataView.h"

class CSourceView;
class CShowDataView;
class CChildFrame : public CMDIChildWnd
{
friend class CSourceView;
friend class CShowDataView;
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();

// ����
protected:
	CSplitterWnd	m_wndSplitter;
	CSignalListView	m_wndSignal;
	CRuntimeView	m_wndRuntime;
	CShowDataView	m_wndShowData;
public:

// ����
public:

// ��д
	public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
private:
	BOOL	VerifyBarState(LPCTSTR lpszProfileName);
	virtual BOOL DestroyWindow();
	afx_msg void OnViewRuntime();
	afx_msg void OnUpdateViewRuntime(CCmdUI *pCmdUI);
	afx_msg void OnViewSignal();
	afx_msg void OnUpdateViewSignal(CCmdUI *pCmdUI);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

// ʵ��
public:
	virtual ~CChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRecvListViewClear();
	afx_msg void OnRecvListViewSavelog();
	afx_msg void OnRecvListViewOpenlog();
	afx_msg void OnRecvListViewReview();
	afx_msg void OnRecvListViewColor();
	afx_msg void OnRecvListViewSelectAll();
	afx_msg void OnRecvListViewCopy();
public:
	afx_msg void OnViewRefresh();
public:
	afx_msg void OnViewShowdata();
public:
	afx_msg void OnUpdateViewShowdata(CCmdUI *pCmdUI);
public:
	afx_msg void OnRecvListViewFilter();
};
