// Commate.cpp : 定义应用程序的类行为。
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
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CProbeApp 构造

CProbeApp::CProbeApp()
{
	EnableHtmlHelp();

	m_nMaxRecord = 1024;
	m_bEffectReadOnly = true;
	m_colorSource = RGB(0,0,0);
	m_colorShowData = RGB(0,0,0);
}


// 唯一的一个 CProbeApp 对象

CProbeApp theApp;

// CProbeApp 初始化

BOOL CProbeApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControls()。否则，将无法创建窗口。
	InitCommonControls();

	CWinApp::InitInstance();

	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	LoadStdProfileSettings(8);  // 加载标准 INI 文件选项(包括 MRU)
	m_LoadPrivateProfile();
	// 注册应用程序的文档模板。文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_ProbeTYPE,
		RUNTIME_CLASS(CProbeDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CSourceView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	// 创建主 MDI 框架窗口
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;
	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 MDI 应用程序中，这应在设置 m_pMainWnd 之后立即发生
	// 启用拖/放
	m_pMainWnd->DragAcceptFiles();
	// 启用“DDE 执行”
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);
	// 分析标准外壳命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// 调度在命令行中指定的命令。如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// 主窗口已初始化，因此显示它并对其进行更新
	pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
	pMainFrame->UpdateWindow();
	return TRUE;
}



// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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

// 用于运行对话框的应用程序命令
void CProbeApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CProbeApp 消息处理程序


int CProbeApp::ExitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类
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
	if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {//创建匿名管道，并从中得到读写管道的句柄
		return false;
	}
	STARTUPINFO si = { sizeof(STARTUPINFO) }; //该结构描述新进程的主窗口特性
	GetStartupInfo(&si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE; //隐藏窗口；
	si.hStdOutput = hWrite; //管道的输入端口连接命令行的输出；
 
	PROCESS_INFORMATION pi;// 该结构返回有关新进程及其主线程的信息
    //创建进程
	if (!CreateProcess(NULL, command,
		NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi)) {
		return false;
	}
	CloseHandle(hWrite);//关闭管道的输入端口；
	// 读取命令行返回值
	CString	szRead;
	char buff[1024];
	const wchar_t	sep = '\n';
	DWORD dwRead = 0;

	do {	//从管道的输出端获取命令行写入的数据；
		memset (buff, 0x00, 1024);
		ReadFile(hRead, buff, 1024, &dwRead, NULL);
		szRead += MultiByteStr_WideStr(buff);
	} while (dwRead);
	CloseHandle(hRead);//关闭管道的输出端口；
	szEcho = szRead;

	while (!szRead.IsEmpty()) {
		CString	szLine = CutStringHead(szRead, sep);
		if (!szLine.IsEmpty())
			listEcho.AddTail(szLine);
	}

	return true;
}
