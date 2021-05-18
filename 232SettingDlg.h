#pragma once


// C232SettingDlg 对话框

class C232SettingDlg : public CDialog
{
	DECLARE_DYNAMIC(C232SettingDlg)

public:
	C232SettingDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~C232SettingDlg();

// 对话框数据
	//{{AFX_DATA(C232SettingDlg)
	enum { IDD = IDD_PORT_SETTING };
	CComboBox	m_bytesize;
	CComboBox	m_baudrate;
	int		m_parity;
	int		m_stop_bit;
	int		m_port_no;
	BOOL	m_nMode;
	DWORD	m_parameter;
	DWORD	m_dwIpAddress;
	int		m_nTcpipPort;
	BOOL	m_bIpTransfer;
	DWORD	m_dwIpAddress1;
	DWORD	m_dwIpAddress2;
	int		m_nIpPort1;
	int		m_nIpPort2;
	//}}AFX_DATA

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	//{{AFX_MSG(C232SettingDlg)
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	int m_nLocalPort;
};
