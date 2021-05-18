// DataConvertionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Commate.h"
#include "DataConvertionDlg.h"
#include ".\dataconvertiondlg.h"


// CDataConvertionDlg 对话框

IMPLEMENT_DYNAMIC(CDataConvertionDlg, CDialog)
CDataConvertionDlg::CDataConvertionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDataConvertionDlg::IDD, pParent)
	, m_szConverted(_T(""))
{
}

CDataConvertionDlg::~CDataConvertionDlg()
{
}

void CDataConvertionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CONVERTION, m_ctrlEdit);
	DDX_Text(pDX, IDC_EDIT_CONVERTION, m_szConverted);
}


BEGIN_MESSAGE_MAP(CDataConvertionDlg, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDataConvertionDlg 消息处理程序

void CDataConvertionDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( m_ctrlEdit.GetSafeHwnd() && m_ctrlEdit.IsWindowVisible() )
		m_ctrlEdit.MoveWindow( 0,0,cx,cy );
}
