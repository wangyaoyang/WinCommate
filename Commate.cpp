// Commate.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "Commate.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "CommateDoc.h"
#include "SourceView.h"
#include "..\..\TmnLibs\SocketApi\CommonFacility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CProbeApp

BEGIN_MESSAGE_MAP(CProbeApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// �����ļ��ı�׼�ĵ�����
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CProbeApp ����

CProbeApp::CProbeApp()
{
	EnableHtmlHelp();

	m_nMaxRecord = 1024;
	m_bEffectReadOnly = true;
	m_colorSource = RGB(0,0,0);
	m_colorShowData = RGB(0,0,0);
}


// Ψһ��һ�� CProbeApp ����

CProbeApp theApp;

// CProbeApp ��ʼ��

BOOL CProbeApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControls()�����򣬽��޷��������ڡ�
	InitCommonControls();

	CWinApp::InitInstance();

	// ��ʼ�� OLE ��
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	LoadStdProfileSettings(8);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)
	m_LoadPrivateProfile();
	// ע��Ӧ�ó�����ĵ�ģ�塣�ĵ�ģ��
	// �������ĵ�����ܴ��ں���ͼ֮�������
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_ProbeTYPE,
		RUNTIME_CLASS(CProbeDoc),
		RUNTIME_CLASS(CChildFrame), // �Զ��� MDI �ӿ��
		RUNTIME_CLASS(CSourceView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	// ������ MDI ��ܴ���
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;
	// �������к�׺ʱ�ŵ��� DragAcceptFiles
	//  �� MDI Ӧ�ó����У���Ӧ������ m_pMainWnd ֮����������
	// ������/��
	m_pMainWnd->DragAcceptFiles();
	// ���á�DDE ִ�С�
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);
	// ������׼������DDE�����ļ�������������
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// ��������������ָ����������
	// �� /RegServer��/Register��/Unregserver �� /Unregister ����Ӧ�ó����򷵻� FALSE��
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// �������ѳ�ʼ���������ʾ����������и���
	pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
	pMainFrame->UpdateWindow();
	return TRUE;
}



// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// �������жԻ����Ӧ�ó�������
void CProbeApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CProbeApp ��Ϣ�������


int CProbeApp::ExitInstance()
{
	// TODO: �ڴ����ר�ô����/����û���
	m_SavePrivateProfile();
	return CWinApp::ExitInstance();
}

void CProbeApp::m_LoadPrivateProfile()
{
	m_nMaxRecord		= this->GetProfileInt( _T("Communication"),_T("Max Records"),0 );
	m_bEffectReadOnly	= this->GetProfileInt( _T("Communication"),_T("Effect Read Only"),0 ) ? true : false;
	m_colorSource		= this->GetProfileInt( _T("Desktop"),_T("Color of Source Window"),0x00ffffff );
	m_colorShowData		= this->GetProfileInt( _T("Desktop"),_T("Color of Show Data Window"),0x000000ff );
}

void CProbeApp::m_SavePrivateProfile()
{
	this->WriteProfileInt( _T("Communication"),_T("Max Records"),m_nMaxRecord );
	this->WriteProfileInt( _T("Communication"),_T("Effect Read Only"),m_bEffectReadOnly );
	this->WriteProfileInt( _T("Desktop"),_T("Color of Source Window"),m_colorSource );
	this->WriteProfileInt( _T("Desktop"),_T("Color of Show Data Window"),m_colorShowData );
}

CString CutStringHead(CString& src, wchar_t sep) {
	int	pos = -1;
	CString	szHead;
	src.TrimLeft(sep);
	if ((pos = src.Find(sep)) > 0) {
		szHead = src.Left(pos);
		src = src.Right(src.GetLength() - pos);
		return szHead;
	} else if (src.GetLength() > 0) {
		szHead = src;
		src.Empty();
		return szHead;
	}
	szHead.Empty();
	return szHead;
}

bool ShellExec(wchar_t* command, CStringList& listEcho, CString& szEcho) {
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
	HANDLE hRead, hWrite;
	if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {//���������ܵ��������еõ���д�ܵ��ľ��
		return false;
	}
	STARTUPINFO si = { sizeof(STARTUPINFO) }; //�ýṹ�����½��̵�����������
	GetStartupInfo(&si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE; //���ش��ڣ�
	si.hStdOutput = hWrite; //�ܵ�������˿����������е������
 
	PROCESS_INFORMATION pi;// �ýṹ�����й��½��̼������̵߳���Ϣ
    //��������
	if (!CreateProcess(NULL, command,
		NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi)) {
		return false;
	}
	CloseHandle(hWrite);//�رչܵ�������˿ڣ�
	// ��ȡ�����з���ֵ
	CString	szRead;
	char buff[1024];
	const wchar_t	sep = '\n';
	DWORD dwRead = 0;

	do {	//�ӹܵ�������˻�ȡ������д������ݣ�
		memset (buff, 0x00, 1024);
		ReadFile(hRead, buff, 1024, &dwRead, NULL);
		szRead += MultiByteStr_WideStr(buff);
	} while (dwRead);
	CloseHandle(hRead);//�رչܵ�������˿ڣ�
	szEcho = szRead;

	while (!szRead.IsEmpty()) {
		CString	szLine = CutStringHead(szRead, sep);
		if (!szLine.IsEmpty())
			listEcho.AddTail(szLine);
	}

	return true;
}
