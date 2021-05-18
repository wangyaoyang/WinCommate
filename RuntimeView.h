#pragma once


// CRuntimeView 视图

class CChildFrame;
class CRuntimeView : public CSizingControlBarG
{
friend class CChildFrame;
	DECLARE_DYNCREATE(CRuntimeView)

protected:
	CRuntimeView();           // 动态创建所使用的受保护的构造函数
	virtual ~CRuntimeView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	// Generated message map functions
	//{{AFX_MSG(C232SettingDlg)
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
private:
	int				m_nSentence;
	CImageList		m_ImageList;
	CListCtrl		m_ctrlList;
	CProbeDoc*		m_pDoc;
public:
	int				GetCurrItem();
	CListCtrl&		GetListCtrl();
	CProbeDoc*		GetDocument();
	void			OnInitialUpdate(CProbeDoc& doc);
};


