// CommateDoc.h :  CProbeDoc ��Ľӿ�
//


#pragma once

#include "WinThreadExec.h"

class CSourceView;
class CShowDataView;
class CSignalListView;
class CRuntimeView;
class CFilter : public CObject
{
public:
	DWORD	m_dwIP;
public:
	CFilter(DWORD dwIP) {	m_dwIP = dwIP; }
	CFilter(BYTE b0,BYTE b1,BYTE b2,BYTE b3) {
		m_dwIP = (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;
	}
};

class CNetworkIf : public CObject
{
public:
	int		m_nIndex;
	int		m_nMet;
	int		m_nMtu;
	int		m_nStat;
	CString	m_szName;
	CString	m_szIP;
public:
	CNetworkIf(int idx = -1, int met = 0, int mtu = -1, int stat = 0) {
		this->m_nIndex = idx;
		this->m_nMet = met;
		this->m_nMtu = mtu;
		this->m_nStat = stat;
	}
};

class CProbeDoc : public CDocument
{
friend class CSourceView;
friend class CShowDataView;
friend class CRuntimeView;
protected: // �������л�����
	CProbeDoc();
	DECLARE_DYNCREATE(CProbeDoc)

// ����
public:

// ����
public:

// ��д
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~CProbeDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CShowDataView*		m_pViewShowData;
	CSignalListView*	m_pViewSignal;
	CRuntimeView*		m_pViewRuntime;
	CObList				m_aFilters;
// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSendViewCalculater();
	afx_msg void OnPopupExecLoad();
	afx_msg void OnPopupExecExecute();
	afx_msg void OnPopupExecStop();
	afx_msg void OnPopupExecSelAll();
	afx_msg void OnPopupExecClear();
	afx_msg void OnRecvListViewHex();
	afx_msg void OnUpdateRecvListViewHex(CCmdUI *pCmdUI);
	afx_msg void OnPopupExecDebug();
	afx_msg void OnUpdatePopupExecDebug(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePopupExecSelAll(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePopupExecClear(CCmdUI *pCmdUI);
	afx_msg void OnPopupExecClearTrace();
	afx_msg void OnUpdatePopupExecClearTrace(CCmdUI *pCmdUI);
public:
	CStringList		m_szSource;
public:
	bool			m_bRunning;
	int				m_nCommunicateMode;
	DWORD			m_dwSerialParameter;
	DWORD			m_dwIPAddress;
	DWORD			m_dwServerPort;
	DWORD			m_dwClientPort;
	BOOL			m_bIpTransfer;
	DWORD			m_dwIpAddress1;
	DWORD			m_dwIpAddress2;
	int				m_nIpPort1;
	int				m_nIpPort2;
	bool			m_bEcho;
	bool			m_bDisp;
private:
	CWinThreadExec*	m_threadExec;
	CObList			m_listEth;
private:
	void				m_ClearThread(void);
	HANDLE				m_CreateThread(void);
	void				m_ClearEthList(void);
	CNetworkIf*			m_EthListAddFromString(CString info);
public:
	CSourceView*		m_GetSourceView();
	CShowDataView*		m_GetShowDataView();
	CSignalListView*	m_GetSignalView();
	CRuntimeView*		m_GetRuntimeView();
	CWinThreadExec*		m_GetThread();
	int					m_EthListToCombBox(CComboBox& combo);
	int					m_EthListIfs(void);
};


