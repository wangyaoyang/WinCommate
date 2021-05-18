#pragma once
#include "afxwin.h"


// CEthIfSelDlg �Ի���
class CProbeDoc;
class CEthIfSelDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEthIfSelDlg)

private:
	CProbeDoc*	m_pDoc;
	CComboBox	m_combo_ethIfs;
	CEdit		m_edit_if_details;
	CString		m_szEthIfName;
	CString		m_szEthIfAddr;
public:
	CEthIfSelDlg(CProbeDoc* pDoc, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CEthIfSelDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_ETH_SELECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString	m_szMcastIP;
private:
	void	m_UpdateIfDetails(void);
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelchangeComboEthIfs();
public:
	CString	m_GetEthIf() { return this->m_szEthIfName; }
	CString	m_GetEthIP() { return this->m_szEthIfAddr; }
};
