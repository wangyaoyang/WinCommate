#pragma once
#include "afxwin.h"


// CDataConvertionDlg �Ի���

class CDataConvertionDlg : public CDialog
{
	DECLARE_DYNAMIC(CDataConvertionDlg)

public:
	CDataConvertionDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDataConvertionDlg();

// �Ի�������
	enum { IDD = IDD_CONVERTION_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ctrlEdit;
	CString m_szConverted;
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
