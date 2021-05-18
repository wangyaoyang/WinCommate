#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CRecvDataDlg 对话框
#define	FILTER_ALL		0xFFFFFFFF
#define	FILTER_RECV		0x00000001
#define	FILTER_SEND		0x00000002

class CRecvDataDlg : public CDialog
{
	DECLARE_DYNAMIC(CRecvDataDlg)
private:
	CObList*	m_oHistory;
public:
	CRecvDataDlg(CObList& oHistory,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRecvDataDlg();

// 对话框数据
	enum { IDD = IDD_RECV_BUFF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	int			m_nItem;
	int			m_nDispMode;
	int			m_nFilterMode;
	POSITION	m_currPos;
	CListCtrl	m_ctrlList;
	CEdit		m_ctrlEdit;
	CImageList	m_ImageList;
private:
	void		m_ShowData( char* pBuff,long nBuff,int nDispMode );
	int			m_RefreshTable(int nFilterMode);
	void		m_GetPosition(long nNo,POSITION& pos);
public:
	afx_msg void OnLvnItemchangedListRecvData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRecvBuffDelete();
	afx_msg void OnRecvBuffRefresh();
	afx_msg void OnRecvBuffAsciiHex();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRecvBuffFilterRecv();
	afx_msg void OnRecvBuffFilterSend();
	afx_msg void OnRecvBuffFilterAll();
public:
//	afx_msg void OnUpdateRecvBuffAsciiHex(CCmdUI *pCmdUI);
public:
	afx_msg void OnUpdateRecvBuffAsciiHex(CCmdUI *pCmdUI);
};
