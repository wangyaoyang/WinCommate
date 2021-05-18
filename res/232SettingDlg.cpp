// 232SettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Commate.h"
#include "232SettingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// C232SettingDlg dialog


C232SettingDlg::C232SettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(C232SettingDlg::IDD, pParent)
	, m_nMode(FALSE)
	, m_nTcpipPort(0)
	, m_dwIpAddress(0)
{
	//{{AFX_DATA_INIT(C232SettingDlg)
	m_parity = -1;
	m_stop_bit = -1;
	m_port_no = -1;
	//}}AFX_DATA_INIT
	m_parameter = 0;
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
	//}}AFX_DATA_MAP
	DDX_Radio(pDX, IDC_RADIO_SERIAL, m_nMode);
	DDX_Text(pDX, IDC_EDIT_TCPIP_PORT, m_nTcpipPort);
	DDX_IPAddress(pDX, IDC_SETTING_IPADDRESS, m_dwIpAddress);
}


BEGIN_MESSAGE_MAP(C232SettingDlg, CDialog)
	//{{AFX_MSG_MAP(C232SettingDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// C232SettingDlg message handlers

BOOL C232SettingDlg::OnInitDialog() 
{
	int	byte_size = 0;
	int	baud_rate = 0;

	m_port_no	= (( 0xff000000 & m_parameter ) >> 24) -1;
	byte_size	= ( 0x00f00000 & m_parameter ) >> 20;
	m_stop_bit	= (( 0x000c0000 & m_parameter ) >> 18) -1;
	m_parity	= ( 0x00030000 & m_parameter ) >> 16;
	baud_rate	= ( 0x0000ffff & m_parameter );

	CDialog::OnInitDialog();
	
	char	buffer[64];

	m_bytesize.SelectString( -1, itoa(byte_size,buffer,10) );
	m_baudrate.SelectString( -1, itoa(baud_rate,buffer,10) );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void C232SettingDlg::OnOK() 
{
	int	byte_size = 0;
	int	baud_rate = 0;

	byte_size = GetDlgItemInt( IDC_BYTE_SIZE );
	baud_rate = GetDlgItemInt( IDC_BOUD_RATE );

	CDialog::OnOK();

	m_parameter =	((m_port_no+1)	& 0x000000ff)<<24	|
					(byte_size		& 0x0000000f)<<20	|
					((m_stop_bit+1)	& 0x00000003)<<18	|
					(m_parity		& 0x00000003)<<16	|
					(baud_rate		& 0x0000ffff);
}
