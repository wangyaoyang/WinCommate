#pragma once


// CCycleSettingDlg �Ի���

class CCycleSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CCycleSettingDlg)

public:
	CCycleSettingDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCycleSettingDlg();

// �Ի�������
	enum { IDD = IDD_SEND_RECYCYLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	bool	m_bRun;
	BOOL	m_bSendMode;
	int		m_nCycleTimer;
	afx_msg void OnBnClickedNo();
};
