// RuntimeView.cpp : 实现文件
//

#include "stdafx.h"
#include "Commate.h"
#include "CommateDoc.h"
#include "SourceView.h"
#include "RuntimeView.h"
#include ".\runtimeview.h"
#include "WinThreadExec.h"


// CRuntimeView

IMPLEMENT_DYNCREATE(CRuntimeView, CSizingControlBarG)

CRuntimeView::CRuntimeView()
{
	m_pDoc = NULL;
	m_nSentence = -1;
}

CRuntimeView::~CRuntimeView()
{
}

BEGIN_MESSAGE_MAP(CRuntimeView, CSizingControlBarG)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_RUNTIME_VIEW, &CRuntimeView::OnLvnItemchanged)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_RUNTIME_VIEW, &CRuntimeView::OnNMDblclk)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CRuntimeView 诊断

#ifdef _DEBUG
void CRuntimeView::AssertValid() const
{
	CSizingControlBarG::AssertValid();
}

void CRuntimeView::Dump(CDumpContext& dc) const
{
	CSizingControlBarG::Dump(dc);
}
#endif //_DEBUG

// CRuntimeView 消息处理程序
CListCtrl& CRuntimeView::GetListCtrl()
{
	return m_ctrlList;
}

CProbeDoc* CRuntimeView::GetDocument()
{
	return m_pDoc;
}

int CRuntimeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CSizingControlBarG::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_ctrlList.CreateEx(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP|WS_EX_CLIENTEDGE,
		WS_CHILD|WS_VISIBLE|LVS_REPORT|LVS_SHOWSELALWAYS|LVS_SINGLESEL,
		CRect(0,0,0,0), this, IDC_LIST_RUNTIME_VIEW))
		return -1;
	GetListCtrl().ModifyStyle(0,LVS_REPORT|LVS_SHOWSELALWAYS|LVS_SINGLESEL);//|LVS_NOCOLUMNHEADER);
	GetListCtrl().SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,0,
						LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP);

	return 0;
}

void CRuntimeView::OnInitialUpdate(CProbeDoc& doc)
{
	m_pDoc = &doc;
	doc.m_pViewRuntime = this;

	CSizingControlBarG::OnInitialUpdate();

	m_ImageList.Create(IDB_BITMAP_STEPS,16,0,RGB(0,128,128));
	m_ImageList.SetBkColor(GetSysColor(COLOR_WINDOW));
	GetListCtrl().ModifyStyle(0,LVS_REPORT|LVS_SHOWSELALWAYS|LVS_SINGLESEL);
	GetListCtrl().SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,0,
						LVS_EX_GRIDLINES|LVS_EX_INFOTIP|LVS_EX_FULLROWSELECT);
	GetListCtrl().SetImageList(&m_ImageList,LVSIL_SMALL);
	GetListCtrl().SetBkColor( (COLORREF)RGB(0,0,48) );
	GetListCtrl().SetTextBkColor( (COLORREF)RGB(0,0,48) );
	GetListCtrl().SetTextColor( (COLORREF)RGB(0,255,255) );
	GetListCtrl().InsertColumn( 0,_T("序号"),LVCFMT_LEFT,100 );
	GetListCtrl().InsertColumn( 1,_T("模拟程序"),LVCFMT_LEFT,150 );
	GetListCtrl().InsertColumn( 2,_T("程序命令"),LVCFMT_LEFT,400 );
}

int CRuntimeView::GetCurrItem() { return this->m_nSentence; }

void CRuntimeView::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	m_nSentence = pNMLV->iItem;
	*pResult = 0;
}

void CRuntimeView::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	if( m_nSentence >= 0 )
	{
		CString	szKeyword = GetListCtrl().GetItemText( m_nSentence,1 );
		CString	szContent = GetListCtrl().GetItemText( m_nSentence,2 );
		this->MessageBox( szContent,_T("当前命令为：\"") + szKeyword + _T("\"；内容为："),MB_ICONINFORMATION );
	}

	*pResult = 0;
}

void CRuntimeView::OnSize(UINT nType, int cx, int cy)
{
	CSizingControlBarG::OnSize(nType, cx, cy);

	m_ctrlList.MoveWindow(0,0,cx,cy);
}

