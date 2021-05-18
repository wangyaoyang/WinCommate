// 232SettingDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// C232SettingDlg dialog

//##ModelId=39C098EE019F
class C232SettingDlg : public CDialog
{
// Construction
public:
	//##ModelId=39C098EE0332
	C232SettingDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
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
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(C232SettingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(C232SettingDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};
