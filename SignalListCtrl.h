#pragma once


// CSignalListCtrl
#define		INDEX_ITEM_BASE				0
#define		INDEX_ITEM_COMM_MODE		INDEX_ITEM_BASE+1

#define		INDEX_ITEM_COMM_PORT		INDEX_ITEM_BASE+2
#define		INDEX_ITEM_COMM_DATA		INDEX_ITEM_BASE+3
#define		INDEX_ITEM_COMM_STOP		INDEX_ITEM_BASE+4
#define		INDEX_ITEM_COMM_PARITY		INDEX_ITEM_BASE+5
#define		INDEX_ITEM_COMM_BAUD		INDEX_ITEM_BASE+6

#define		INDEX_ITEM_COMM_IP_ADDR		INDEX_ITEM_COMM_MODE+1
#define		INDEX_ITEM_COMM_SVR_PORT	INDEX_ITEM_COMM_MODE+2
#define		INDEX_ITEM_COMM_CLT_PORT	INDEX_ITEM_COMM_MODE+3

class CProbeDoc;
class CSignalListCtrl : public CXListCtrl
{
	//DECLARE_DYNAMIC(CSignalListCtrl)
private:
	int				m_nItem;
	int				m_nSubItem;
	CStringArray	m_szCommMode;
	CStringArray	m_szCommPort;
	CStringArray	m_szCommData;
	CStringArray	m_szCommStop;
	CStringArray	m_szCommParity;
	CStringArray	m_szCommBaud;
	CStringArray	m_saTransMode;
	CStringArray	m_saTransActive;
	CProbeDoc*		m_pDoc;
public:
	CSignalListCtrl();
	virtual ~CSignalListCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
private:
	int		m_SearchInArray( CStringArray& arr,int nValue );
	int		m_SearchInArray( CStringArray& arr,CString szValue );
	void	m_ModifyConfig(int nItem,int nSubItem);
public:
	int		m_CurrentItem();
	int		m_CurrentSubItem();
public:
	void	m_InitCtrl(CProbeDoc& doc);
	void	m_ShowConfig();
	void	m_ModifyConfig();
	void	m_Invalidate();
public:
};


