#pragma once
#include "afxwin.h"


// CCalculaterDlg �Ի���

class CCalculaterDlg : public CDialog
{
	DECLARE_DYNAMIC(CCalculaterDlg)

public:
	CCalculaterDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCalculaterDlg();

// �Ի�������
	enum { IDD = IDD_CALCULATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonExecute();
private:
	CEdit m_ctrlEditFormula;
	CEdit m_ctrlEditResult;
};
