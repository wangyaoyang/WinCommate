// Commate.h : Commate Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h�� 
#endif

#include "resource.h"       // ������


// CProbeApp:
// �йش����ʵ�֣������ Commate.cpp
//

class CProbeApp : public CWinApp
{
public:
	CProbeApp();


// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
public:
	long			m_nMaxRecord;
	bool			m_bEffectReadOnly;
	COLORREF		m_colorSource;
	COLORREF		m_colorShowData;
private:
	void			m_LoadPrivateProfile();
	void			m_SavePrivateProfile();
public:
};

extern CProbeApp theApp;

CString CutStringHead(CString& src, wchar_t sep);
bool ShellExec(wchar_t* command, CStringList& listEcho, CString& szEcho);
