// EthIfSelDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Commate.h"
#include "EthIfSelDlg.h"
#include "afxdialogex.h"


// CEthIfSelDlg 对话框

IMPLEMENT_DYNAMIC(CEthIfSelDlg, CDialogEx)

CEthIfSelDlg::CEthIfSelDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEthIfSelDlg::IDD, pParent)
{

}

CEthIfSelDlg::~CEthIfSelDlg()
{
}

void CEthIfSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEthIfSelDlg, CDialogEx)
END_MESSAGE_MAP()


// CEthIfSelDlg 消息处理程序
