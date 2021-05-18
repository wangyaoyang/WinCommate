// 232SettingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Commate.h"
#include "232SettingDlg.h"
#include ".\232settingdlg.h"


// C232SettingDlg 对话框

IMPLEMENT_DYNAMIC(C232SettingDlg, CDialog)
C232SettingDlg::C232SettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(C232SettingDlg::IDD, pParent)
	, m_nMode(FALSE)
	, m_nLocalPort(0)
	, m_nTcpipPort(0)
	, m_dwIpAddress(0)
	, m_dwIpAddress1(0)
	, m_dwIpAddress2(0)
	, m_bIpTransfer(FALSE)
	, m_nIpPort1(0)
	, m_nIpPort2(0)
{
	//{{AFX_DATA_INIT(C232SettingDlg)
	m_parity = -1;
	m_stop_bit = -1;
	m_port_no = -1;
	m_parameter = 0;
	//}}AFX_DATA_INIT
}

C232SettingDlg::~C232SettingDlg()
{
}

void C232SettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(C232SettingDlg)
	DDX_Control(pDX, IDC_BYTE_SIZE, m_bytesize);
	DDX_Control(pDX, IDC_BOUD_RATE, m_baudrate);
	DDX_Radio(pDX, IDC_PARITY_NO, m_parity);
	DDX_Radio(pDX, IDC_STOP1, m_stop_bit);
	DDX_CBIndex(pDX, IDC_CURRENT_PORT, m_port_no);
	DDX_Radio(pDX, IDC_RADIO_SERIAL, m_nMode);
	DDX_Text(pDX, IDC_EDIT_LOCAL_PORT, m_nLocalPort);
	DDX_Text(pDX, IDC_EDIT_TCPIP_PORT, m_nTcpipPort);
	DDX_IPAddress(pDX, IDC_SETTING_IPADDRESS, m_dwIpAddress);
	//}}AFX_DATA_MAP
	DDX_IPAddress(pDX, IDC_IPADDRESS_TERM1, m_dwIpAddress1);
	DDX_IPAddress(pDX, IDC_IPADDRESS_TERM2, m_dwIpAddress2);
	DDX_Check(pDX, IDC_CHECK_IP_TRANSFER, m_bIpTransfer);
	DDX_Text(pDX, IDC_EDIT_IP_PORT1, m_nIpPort1);
	DDX_Text(pDX, IDC_EDIT_IP_PORT2, m_nIpPort2);
}


BEGIN_MESSAGE_MAP(C232SettingDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// C232SettingDlg 消息处理程序

BOOL C232SettingDlg::OnInitDialog()
{
	int	byte_size = 0;
	int	baud_rate = 0;

	m_port_no	= (( 0xf0000000 & m_parameter ) >> 28) -1;
	byte_size	= (( 0x0f000000 & m_parameter ) >> 24);
	m_stop_bit	= (( 0x00c00000 & m_parameter ) >> 22) -1;
	m_parity	= (( 0x00300000 & m_parameter ) >> 20);
	baud_rate	= (( 0x000fffff & m_parameter ));

	CDialog::OnInitDialog();
	
	char	buffer[64];
	errno_t	error = 0;

	error = _itoa_s( byte_size,buffer,sizeof(buffer),10);	m_bytesize.SelectString( -1,buffer  );
	error = _itoa_s( baud_rate,buffer,sizeof(buffer),10);	m_baudrate.SelectString( -1,buffer  );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void C232SettingDlg::OnBnClickedOk()
{
	int	byte_size = 0;
	int	baud_rate = 0;

	byte_size = GetDlgItemInt( IDC_BYTE_SIZE );
	baud_rate = GetDlgItemInt( IDC_BOUD_RATE );

	CDialog::OnOK();

	m_parameter =	((m_port_no+1)	& 0x0000000f)<<28	|
					(byte_size		& 0x0000000f)<<24	|
					((m_stop_bit+1)	& 0x00000003)<<22	|
					(m_parity		& 0x00000003)<<20	|
					(baud_rate		& 0x000fffff);
}
