#pragma once


// CCycleSettingDlg 对话框

class CCycleSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CCycleSettingDlg)

public:
	CCycleSettingDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCycleSettingDlg();

// 对话框数据
	enum { IDD = IDD_SEND_RECYCYLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	bool	m_bRun;
	BOOL	m_bSendMode;
	int		m_nCycleTimer;
	afx_msg void OnBnClickedNo();
};
