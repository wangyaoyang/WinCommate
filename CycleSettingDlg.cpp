// CycleSettingDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Commate.h"
#include "CycleSettingDlg.h"


// CCycleSettingDlg �Ի���

IMPLEMENT_DYNAMIC(CCycleSettingDlg, CDialog)
CCycleSettingDlg::CCycleSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCycleSettingDlg::IDD, pParent)
	, m_bSendMode(FALSE)
	, m_nCycleTimer(1000)
	, m_bRun(true)
{
}

CCycleSettingDlg::~CCycleSettingDlg()
{
}

void CCycleSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_ASCII, m_bSendMode);
	DDX_Text(pDX, IDC_EDIT_CYCLE_TIMER, m_nCycleTimer);
}


BEGIN_MESSAGE_MAP(CCycleSettingDlg, CDialog)
	ON_BN_CLICKED(IDNO, &CCycleSettingDlg::OnBnClickedNo)
END_MESSAGE_MAP()


// CCycleSettingDlg ��Ϣ�������


void CCycleSettingDlg::OnBnClickedNo()
{
	m_bRun = 0;
	CDialog::OnOK();
}
