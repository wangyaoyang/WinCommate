#if !defined(AFX_WINTHREADEXEC_H__FC4EC024_795E_11D4_9458_00E09876FE60__INCLUDED_)
#define AFX_WINTHREADEXEC_H__FC4EC024_795E_11D4_9458_00E09876FE60__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WinThreadExec.h : header file
//
#include "Communication.h"

#define		TRACER_EXECUTABLE_EVENT		_T("232TRACER_EXECUTABLE_EVENT")
#define		TRACER_EXECFINISH_EVENT		_T("232TRACER_EXECFINISH_EVENT")
#define		TRACER_EXEREADING_EVENT		_T("232TRACER_EXEREADING_EVENT")
#define		TRACER_EXEC_BREAK_EVENT		_T("232TRACER_EXEC_BREAK_EVENT")

#define			KEYWORD_NOTHING		0
#define			KEYWORD_WAIT		1
#define			KEYWORD_WHEN		2
#define			KEYWORD_WHENTXT		3
#define			KEYWORD_THEN		4
#define			KEYWORD_THENTXT		5
#define			KEYWORD_GOTO		6
#define			KEYWORD_SLEEP		7
#define			KEYWORD_SEND		8
#define			KEYWORD_SENDTXT		9
#define			KEYWORD_LABEL		10
#define			KEYWORD_END			11

#define			MAX_PROGRAM_LINE	1024
#define			KB					1024
#define			SEND_BUFF_SIZE		MAX_BUFF_SIZE
#define			RECV_BUFF_SIZE		MAX_BUFF_SIZE
/////////////////////////////////////////////////////////////////////////////

#define			DISP_LINE_BEGIN		1
#define			DISP_LINE_OVER		0
// CWinThreadExec thread

/////////////////////////////////////////////////////////////
#include "..\..\TmnLibs\HashMap\HashMapping.h"

class CProgramLine;
class CLineSyntax : public CHashPerson
{
private:
	CProgramLine*	m_pEntry;
public:
	char			m_ptr[SEND_BUFF_SIZE];
	int				m_len;
	int				m_param;
public:
	CLineSyntax(CProgramLine* pEntry);
	int				m_SetSyntax(CString sz);
	int				m_GetLineIndex();
	bool			operator == (CHashPerson& target);
};
/////////////////////////////////////////////////////////////
class CCommBuffer : public CLineSyntax
{
public:
	HANDLE	m_eventUpdated;
	HANDLE	m_eventIdel;
public:
	CCommBuffer();
	void	m_Clear();
	bool	m_Update(int nBuf,char* sBuf);
	bool	m_IsContain(int nBuf,char* sBuf);
	int		m_Append(int nBuf,char* sBuf);
	CString	m_AsString();
	void	operator = (CCommBuffer& buf);

};
/////////////////////////////////////////////////////////////
class CProgramLine
{
public:
	int				m_lineIdx;
	UINT			m_keyword;
	CString			m_string;
	CLineSyntax		m_syntax;
public:
	CProgramLine();
	bool			m_Init(int index,CString szKey,CString szValue,CHashMapping& map);
	bool			m_Finalize(CHashMapping& map,int& waitIndex);
};
/////////////////////////////////////////////////////////////
class CProbeDoc;
class CRuntimeView;
class CShowDataView;
class CSignalListView;
class CWinThreadExec : public CWinThread
{
	DECLARE_DYNCREATE(CWinThreadExec)
public:
	CWinThreadExec();
	virtual ~CWinThreadExec();
	void	m_Attach(CProbeDoc* pDoc);
	// protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinThreadExec)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWinThreadExec)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CHashMapping	m_map;
	CProgramLine	m_prg[MAX_PROGRAM_LINE];
	HANDLE			m_isRun;
	HANDLE			m_isBreak;
private:
	int			Txt2Hex( CString szInput,char* output );
	BOOL		IsOver();
	bool		Execute();
	void		DispCurrentLine( CProgramLine& prgLine,int nNext,UINT uState );
	DWORD		m_SendHex(int nData,char* sData);
	DWORD		m_SendTxt(CString szData);
private:
	int					m_nMaxline;
	CProbeDoc*			m_pDoc;
	CRuntimeView*		m_pViewRuntime;
	CShowDataView*		m_pViewInput;
	CSignalListView*	m_pViewSignal;
	CCommunication		m_com;
	CCommBuffer			m_recentMessage;
public:
	CCommBuffer			m_buffer;
	bool				m_bDebug;
public:
	BOOL				LoadProgram();
	CCommunication&		m_GetComm() { return m_com; }
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINTHREADEXEC_H__FC4EC024_795E_11D4_9458_00E09876FE60__INCLUDED_)
