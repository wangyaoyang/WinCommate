#pragma once


// CEthIfSelDlg �Ի���

class CEthIfSelDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEthIfSelDlg)

public:
	CEthIfSelDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CEthIfSelDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_ETH_SELECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
