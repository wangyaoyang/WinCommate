#pragma once
#include "afxwin.h"


// CDataConvertionDlg 对话框

class CDataConvertionDlg : public CDialog
{
	DECLARE_DYNAMIC(CDataConvertionDlg)

public:
	CDataConvertionDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDataConvertionDlg();

// 对话框数据
	enum { IDD = IDD_CONVERTION_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ctrlEdit;
	CString m_szConverted;
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
