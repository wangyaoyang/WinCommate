#pragma once
#include "afxwin.h"


// CCalculaterDlg 对话框

class CCalculaterDlg : public CDialog
{
	DECLARE_DYNAMIC(CCalculaterDlg)

public:
	CCalculaterDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCalculaterDlg();

// 对话框数据
	enum { IDD = IDD_CALCULATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonExecute();
private:
	CEdit m_ctrlEditFormula;
	CEdit m_ctrlEditResult;
};
