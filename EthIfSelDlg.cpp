// EthIfSelDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Commate.h"
#include "CommateDoc.h"
#include "EthIfSelDlg.h"
#include "afxdialogex.h"


// CEthIfSelDlg 对话框

IMPLEMENT_DYNAMIC(CEthIfSelDlg, CDialogEx)

CEthIfSelDlg::CEthIfSelDlg(CProbeDoc* pDoc, CWnd* pParent /*=NULL*/)
	: CDialogEx(CEthIfSelDlg::IDD, pParent)
{
	this->m_pDoc = pDoc;
}

CEthIfSelDlg::~CEthIfSelDlg()
{
}

void CEthIfSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ETH_IFS, m_combo_ethIfs);
	DDX_Control(pDX, IDC_EDIT_IF_DETAILS, m_edit_if_details);
}


BEGIN_MESSAGE_MAP(CEthIfSelDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CEthIfSelDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO_ETH_IFS, &CEthIfSelDlg::OnCbnSelchangeComboEthIfs)
END_MESSAGE_MAP()

// CEthIfSelDlg 消息处理程序
BOOL CEthIfSelDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (this->m_pDoc) {
		this->m_pDoc->m_EthListToCombBox(this->m_combo_ethIfs);
		this->m_combo_ethIfs.SetCurSel(0);
		this->m_UpdateIfDetails();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CEthIfSelDlg::OnBnClickedOk()
{
	CString	echo, entry = this->m_szMcastIP.Trim(), cmd = _T("arp -d ") + entry;
	CStringList	echolist;

	CDialogEx::OnOK();

	if (entry.GetLength() > 7) {
		ShellExec(cmd.GetBuffer(), echolist, echo);
	}
}

void CEthIfSelDlg::m_UpdateIfDetails(void) {
	this->m_combo_ethIfs.GetWindowText(this->m_szEthIfName);
	this->m_szEthIfName.Trim('\n');
	this->m_szEthIfName.Trim('\r');

	if (this->m_pDoc) {
		CString	echo, cmd = _T("netsh interface ipv4 show addresses ");
		CStringList	echolist;

		cmd += this->m_szEthIfName;
		if (ShellExec(cmd.GetBuffer(), echolist, echo)) {
			const CString	szKeyword = _T("IP Address:");
			const int		nKeyword = szKeyword.GetLength();
			this->m_edit_if_details.SetWindowText(echo);
			while (!echolist.IsEmpty()) {
				CString	szEcho = echolist.RemoveHead();
				int		nPos = szEcho.Find(szKeyword);
				if (nPos >= 0) {
					this->m_szEthIfAddr = szEcho.Right(szEcho.GetLength() - nPos - nKeyword);
					this->m_szEthIfAddr.Trim();
					break;
				}
			}
		}
	}
}

void CEthIfSelDlg::OnCbnSelchangeComboEthIfs()
{
	this->m_UpdateIfDetails();
}
