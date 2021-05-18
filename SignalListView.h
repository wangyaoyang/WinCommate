#pragma once

#include "SignalListCtrl.h"

// CSignalListView 视图
class CProbeDoc;
class CChildFrame;
class CSignalListView : public CSizingControlBarG
{
friend class CChildFrame;
	DECLARE_DYNCREATE(CSignalListView)

protected:
	CSignalListView();           // 动态创建所使用的受保护的构造函数
	virtual ~CSignalListView();

private:
	CProbeDoc*		m_pDoc;
	CSignalListCtrl	m_ctrlList;
	DWORD			m_oldStatus;
	DWORD			m_newStatus;
	DWORD			m_oldParm;
	BOOL			m_occupied;
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnComboSelection(WPARAM, LPARAM);
	afx_msg LRESULT OnCheckbox(WPARAM, LPARAM);
	afx_msg LRESULT OnEditEnd(WPARAM, LPARAM);
private:
	CProbeDoc*	CSignalListView::GetDocument();
	void		ConfigModified(int nItem, int nSubItem);
public:
	void		OnInitialUpdate(CProbeDoc& doc);
	CSignalListCtrl&	GetListCtrl();
};


