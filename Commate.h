// Commate.h : Commate 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h” 
#endif

#include "resource.h"       // 主符号


// CProbeApp:
// 有关此类的实现，请参阅 Commate.cpp
//

class CProbeApp : public CWinApp
{
public:
	CProbeApp();


// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现
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
