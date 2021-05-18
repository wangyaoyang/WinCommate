#pragma once


// CEthIfSelDlg 对话框

class CEthIfSelDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEthIfSelDlg)

public:
	CEthIfSelDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CEthIfSelDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_ETH_SELECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
