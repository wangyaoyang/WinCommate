#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CFilterDlg 对话框

class CFilterDlg : public CDialog
{
	DECLARE_DYNAMIC(CFilterDlg)

public:
	CFilterDlg(CObList& aFilters,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFilterDlg();

// 对话框数据
	enum { IDD = IDD_IP_FILTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CIPAddressCtrl m_ctrlIP;
	CListCtrl		m_listIP;
	CObList&		m_aFilters;
	int				m_nItem;
private:
	afx_msg void OnBnClickedFilterInsert();
	afx_msg void OnBnClickedFilterDelete();
	afx_msg void OnBnClickedFilterModify();
	afx_msg void OnLvnItemchangedListFilter(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckSimplifyMode();
	virtual BOOL OnInitDialog();
	void			m_Refresh();
private:
	CButton m_simplifyMode;
public:
	int		m_bSimplifyMode;
};
