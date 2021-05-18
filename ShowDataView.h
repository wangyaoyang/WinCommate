#pragma once


// CShowDataView 视图
#include "CommateDoc.h"
#include "WinThreadExec.h"
#include <sys/timeb.h>

#define		DISPLAY_HEX		0
#define		DISPLAY_ASCII	1
#define		DISPLAY_FORCE	2

#define		FLAG_CHAR_SEND		0x01
#define		FLAG_CHAR_RECV		0xff


class CProbeDoc;
class CRecvBuff : public CObject
{
private:
	bool	m_bSendRecv;
	DWORD	m_dwAddress;
	struct	__timeb64 m_oTimeB;
	long	m_nSerial;
	long	m_nBuffer;
	char*	m_sBuffer;
public:
	CRecvBuff();
	~CRecvBuff();
	void		m_Append( bool bSendRecv,DWORD dwAddress,char* sBuffer,long nLength,long nSerial );
	void		Serialize(CArchive &ar);
	bool		m_IsSend()	{ return m_bSendRecv; }
	DWORD		m_GetAddress()	{ return m_dwAddress; }
	long		m_GetSerial() { return m_nSerial; }
	long		m_GetLength() { return m_nBuffer; }
	char*		m_GetBuffer() { return m_sBuffer; }
	struct	__timeb64&		m_GetTime() { return m_oTimeB; }
};

class CChildFrame;

class CShowDataView : public CSizingControlBarG
{
friend class CChildFrame;
	DECLARE_DYNCREATE(CShowDataView)
private:
	int		m_nTimer;
protected:
	CShowDataView();           // 动态创建所使用的受保护的构造函数
	virtual ~CShowDataView();
	CProbeDoc*	GetDocument(); // non-debug version is inline

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
private:
	CFont*			m_pFont; 
	LOGFONT			m_lFont;
	CProbeDoc*		m_pDoc;
private:
	char			m_cColor;
	bool			m_bEffect;
	CRichEditCtrl	m_ctrlEdit;
	CBrush			m_Brush;
	DWORD			m_counter;
	int				m_maxline;
	int				m_current;
	bool			m_bCaretPosChanged;
	char			m_buffer[RECV_BUFF_SIZE];
	int				m_nBuff;
	CObList			m_oHistory;
private:
	void		m_SetEffect( long nHeight,COLORREF cText,COLORREF cBackground );
	bool		m_ChangeEffect( bool bEffect = 0);
	void		m_AppendEcho( CString szText );
	void		m_AppendText( CString& szText );
	void		m_DisplayData( DWORD nData,LPSTR sData,bool bSend );
	void		m_Display( DWORD nData = 0,LPSTR sData = NULL,bool bSend = false );	//默认值表示显示所有历史数据
	void		m_ClearHistory();
	bool		m_IsPermissible(CObList& aFilters,CString szIP);
	bool		m_IsPermissible(CObList& aFilters,DWORD dwIP);
public:
	void		OnInitialUpdate(CProbeDoc& doc);
	void		m_Refresh( BOOL bHexAscii);
	int			m_Append( bool bSendRecv,DWORD nBlocks,DWORD dwAddress,LPSTR pData,DWORD nCounter );
	void		m_OnRecvListViewClear();
	void		m_OnRecvListViewSelectAll();
	void		m_OnRecvListViewCopy();
	void		m_OnRecvListViewReview();
	void		m_OnRecvListViewSavelog();
	void		m_OnRecvListViewOpenlog();
	void		m_OnViewColour();
	void		m_OnViewFilter();
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};


