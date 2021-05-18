// ChildFrm.cpp : CChildFrame 类的实现
//
#include "stdafx.h"
#include "Commate.h"

#include "ChildFrm.h"
#include "SignalListView.h"
#include "SourceView.h"
#include "RuntimeView.h"
#include "ShowDataView.h"
#include ".\childfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_RUNTIME, &CChildFrame::OnViewRuntime)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RUNTIME, &CChildFrame::OnUpdateViewRuntime)
	ON_COMMAND(ID_VIEW_SIGNAL, &CChildFrame::OnViewSignal)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SIGNAL, &CChildFrame::OnUpdateViewSignal)
	ON_COMMAND(ID_RECV_LIST_VIEW_CLEAR, &CChildFrame::OnRecvListViewClear)
	ON_COMMAND(ID_RECV_LIST_VIEW_SAVELOG, &CChildFrame::OnRecvListViewSavelog)
	ON_COMMAND(ID_RECV_LIST_VIEW_OPENLOG, &CChildFrame::OnRecvListViewOpenlog)
	ON_COMMAND(ID_RECV_LIST_VIEW_REVIEW, &CChildFrame::OnRecvListViewReview)
	ON_COMMAND(ID_RECV_LIST_VIEW_COLOR, &CChildFrame::OnRecvListViewColor)
	ON_COMMAND(ID_RECV_LIST_VIEW_SELECT_ALL, &CChildFrame::OnRecvListViewSelectAll)
	ON_COMMAND(ID_RECV_LIST_VIEW_COPY, &CChildFrame::OnRecvListViewCopy)
	ON_COMMAND(ID_VIEW_REFRESH, &CChildFrame::OnViewRefresh)
	ON_COMMAND(ID_VIEW_SHOWDATA, &CChildFrame::OnViewShowdata)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOWDATA, &CChildFrame::OnUpdateViewShowdata)
	ON_COMMAND(ID_RECV_LIST_VIEW_FILTER, &CChildFrame::OnRecvListViewFilter)
END_MESSAGE_MAP()


// CChildFrame 构造/析构

CChildFrame::CChildFrame()
{
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	//if (!m_wndSplitter.CreateStatic(this, 1, 1))
	//	return FALSE;
	//if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CSourceView),		CSize(600,300), pContext) ||	//记录通信流程
	//	//!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CRuntimeView),	CSize(300,400), pContext) ||	//当前运行程序
	//	//!m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(CSignalListView),	CSize(300,300), pContext) ||	//记录接收数据
	//	!m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(CShowDataView),	CSize(600,400), pContext) )	//记录发送数据
	//{
	//	m_wndSplitter.DestroyWindow();
	//	return FALSE;
	//}
	//return TRUE;
	return CMDIChildWnd::OnCreateClient( lpcs,pContext );
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或样式
	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.style = WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
		| FWS_ADDTOTITLE | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE;

	return TRUE;
}


int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if( !m_wndRuntime.Create(_T("Runtime Bar"), this, 123) )
	{
		TRACE0(_T("Failed to create mybar\n"));
		return -1;
		// fail to create
	}
	if( !m_wndSignal.Create(_T("Signal Bar"), this, 124) )
	{
		TRACE0(_T("Failed to create mybar\n"));
		return -1;
		// fail to create
	}
	if( !m_wndShowData.Create(_T("Show Data Bar"), this, 125) )
	{
		TRACE0(_T("Failed to create mybar\n"));
		return -1;
		// fail to create
	}

	m_wndRuntime.SetBarStyle(m_wndRuntime.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_wndSignal.SetBarStyle(m_wndSignal.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_wndShowData.SetBarStyle(m_wndShowData.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// TODO: 如果不需要工具栏可停靠，则删除这三行
	m_wndRuntime.EnableDocking(CBRS_ALIGN_ANY);
	m_wndSignal.EnableDocking(CBRS_ALIGN_ANY);
	m_wndShowData.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndRuntime, AFX_IDW_DOCKBAR_RIGHT);
	DockControlBar(&m_wndShowData, AFX_IDW_DOCKBAR_BOTTOM);
	CRect rectAttr;
	RecalcLayout();
	m_wndRuntime.GetWindowRect(rectAttr);
	rectAttr.OffsetRect(0, 1);
	DockControlBar(&m_wndSignal, AFX_IDW_DOCKBAR_RIGHT,rectAttr );

	CString sProfile(_T("Commate Windows State"));
	if( VerifyBarState(sProfile) )
	{
		m_wndSignal.LoadState(sProfile);
		m_wndRuntime.LoadState(sProfile);
		m_wndShowData.LoadState(sProfile);
		LoadBarState(sProfile);
	}
	return 0;
}

// CChildFrame 诊断

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG


// CChildFrame 消息处理程序
BOOL CChildFrame::VerifyBarState(LPCTSTR lpszProfileName)
{
    CDockState state;
    state.LoadState(lpszProfileName);

    for (int i = 0; i < state.m_arrBarInfo.GetSize(); i++)
    {
        CControlBarInfo* pInfo = (CControlBarInfo*)state.m_arrBarInfo[i];
        ASSERT(pInfo != NULL);
        INT_PTR nDockedCount = pInfo->m_arrBarID.GetSize();
        if (nDockedCount > 0)
        {
            // dockbar
            for (int j = 0; j < nDockedCount; j++)
            {
                UINT nID = (UINT) pInfo->m_arrBarID[j];
                if (nID == 0) continue; // row separator
                if (nID > 0xFFFF)
                    nID &= 0xFFFF; // placeholder - get the ID
                if (GetControlBar(nID) == NULL)
                    return FALSE;
            }
        }
        
        if (!pInfo->m_bFloating) // floating dockbars can be created later
            if (GetControlBar(pInfo->m_nBarID) == NULL)
                return FALSE; // invalid bar ID
    }

    return TRUE;
}
BOOL CChildFrame::DestroyWindow()
{
	CString sProfile(_T("Commate Windows State"));
	m_wndRuntime.SaveState(sProfile);
	m_wndSignal.SaveState(sProfile);
	m_wndShowData.SaveState(sProfile);
	SaveBarState(sProfile);

	return CMDIChildWnd::DestroyWindow();
}

void CChildFrame::OnViewRuntime()
{
	BOOL bShow = m_wndRuntime.IsVisible();
	ShowControlBar(&m_wndRuntime, !bShow, FALSE);
}

void CChildFrame::OnViewSignal()
{
	BOOL bShow = m_wndSignal.IsVisible();
	ShowControlBar(&m_wndSignal, !bShow, FALSE);
}

void CChildFrame::OnViewShowdata()
{
	BOOL bShow = m_wndShowData.IsVisible();
	ShowControlBar(&m_wndShowData, !bShow, FALSE);
}

void CChildFrame::OnUpdateViewRuntime(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(m_wndRuntime.IsVisible());
}

void CChildFrame::OnUpdateViewSignal(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(m_wndSignal.IsVisible());
}

void CChildFrame::OnUpdateViewShowdata(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(m_wndShowData.IsVisible());
}

void CChildFrame::OnRecvListViewClear()
{
	this->m_wndShowData.m_OnRecvListViewClear();
}

void CChildFrame::OnRecvListViewSelectAll()
{
	this->m_wndShowData.m_OnRecvListViewSelectAll();
}

void CChildFrame::OnRecvListViewCopy()
{
	this->m_wndShowData.m_OnRecvListViewCopy();
}

void CChildFrame::OnRecvListViewSavelog()
{
	this->m_wndShowData.m_OnRecvListViewSavelog();
}

void CChildFrame::OnRecvListViewOpenlog()
{
	this->m_wndShowData.m_OnRecvListViewOpenlog();
}

void CChildFrame::OnRecvListViewReview()
{
	this->m_wndShowData.m_OnRecvListViewReview();
}

void CChildFrame::OnRecvListViewColor()
{
	this->m_wndShowData.m_OnViewColour();
}

void CChildFrame::OnViewRefresh()
{
	CRect	rect;
	//m_wndRuntime.GetClientRect(clientRect);
	//m_wndRuntime.MoveWindow(clientRect);
	//m_wndShowData.GetClientRect(clientRect);
	//m_wndShowData.MoveWindow(clientRect);
	//m_wndSignal.GetClientRect(clientRect);
	//m_wndSignal.MoveWindow(clientRect);
	//m_wndShowData.Invalidate();
	//AfxGetApp()->GetMainWnd()->GetClientRect(rect);
	//AfxGetApp()->GetMainWnd()->MoveWindow(rect);
	//AfxGetApp()->GetMainWnd()->Invalidate();
	//this->Invalidate();
}

void CChildFrame::OnRecvListViewFilter()
{
	this->m_wndShowData.m_OnViewFilter();
}
