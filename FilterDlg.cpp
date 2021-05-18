// FilterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Commate.h"
#include "CommateDoc.h"
#include "FilterDlg.h"


// CFilterDlg 对话框

IMPLEMENT_DYNAMIC(CFilterDlg, CDialog)

CFilterDlg::CFilterDlg(CObList& aFilters,CWnd* pParent /*=NULL*/)
	: CDialog(CFilterDlg::IDD, pParent),m_aFilters(aFilters)
{
	m_nItem = -1;
}

CFilterDlg::~CFilterDlg()
{
}

void CFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILTER_IP, m_ctrlIP);
	DDX_Control(pDX, IDC_LIST_FILTER, m_listIP);
	DDX_Control(pDX, IDC_CHECK_SIMPLIFY_MODE, m_simplifyMode);
}


BEGIN_MESSAGE_MAP(CFilterDlg, CDialog)
	ON_BN_CLICKED(IDC_FILTER_INSERT, &CFilterDlg::OnBnClickedFilterInsert)
	ON_BN_CLICKED(IDC_FILTER_DELETE, &CFilterDlg::OnBnClickedFilterDelete)
	ON_BN_CLICKED(IDC_FILTER_MODIFY, &CFilterDlg::OnBnClickedFilterModify)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_FILTER, &CFilterDlg::OnLvnItemchangedListFilter)
	ON_BN_CLICKED(IDC_CHECK_SIMPLIFY_MODE, &CFilterDlg::OnBnClickedCheckSimplifyMode)
END_MESSAGE_MAP()


// CFilterDlg 消息处理程序

void CFilterDlg::m_Refresh()
{
	m_listIP.DeleteAllItems();
	for( POSITION pos = m_aFilters.GetHeadPosition(); pos; m_aFilters.GetNext(pos) )
	{
		CFilter*	filter = (CFilter*) m_aFilters.GetAt(pos);
		int			nItem = m_listIP.GetItemCount();
		CString		szIP;
		DWORD		b0 = 0x000000ff & (filter->m_dwIP >> 24);
		DWORD		b1 = 0x000000ff & (filter->m_dwIP >> 16);
		DWORD		b2 = 0x000000ff & (filter->m_dwIP >> 8);
		DWORD		b3 = 0x000000ff & (filter->m_dwIP);
		szIP.Format( _T("%d.%d.%d.%d"),b0,b1,b2,b3 );
		m_listIP.InsertItem( nItem,szIP );
	}
}

void CFilterDlg::OnBnClickedFilterInsert()
{
	BYTE	b0 = 0,b1 = 0,b2 = 0,b3 = 0;
	CString	szIP;
	int		nItem = m_listIP.GetItemCount();
	m_ctrlIP.GetAddress(b0,b1,b2,b3);
	CFilter*	filter = new CFilter(b0,b1,b2,b3);
	m_aFilters.AddTail( filter );
	m_Refresh();
}

void CFilterDlg::OnBnClickedFilterDelete()
{
	if( m_nItem >= 0 )
	{
		CString	szIP = m_listIP.GetItemText( m_nItem,0 );
		DWORD	dwIP = 0;
		DWORD	b0 = 0,b1 = 0,b2 = 0,b3 = 0;
		swscanf_s( szIP,_T("%d.%d.%d.%d"),&b0,&b1,&b2,&b3 );
		dwIP = (b0<<24) | (b1<<16) | (b2<<8) | b3;
		for( POSITION pos = m_aFilters.GetHeadPosition(); pos; )
		{
			CFilter*	filter = (CFilter*) m_aFilters.GetAt(pos);
			if( filter->m_dwIP == dwIP )
			{
				POSITION	posDel = pos;
				m_aFilters.GetNext(pos);
				m_aFilters.RemoveAt(posDel);
				delete (CFilter*) filter;
			}
			else m_aFilters.GetNext(pos);
		}
	}
	m_Refresh();
}

void CFilterDlg::OnBnClickedFilterModify()
{
	if( m_nItem >= 0 )
	{
		CString	szIP = m_listIP.GetItemText( m_nItem,0 );
		DWORD	dwIP0 = 0;
		DWORD	dwIP1 = 0;
		DWORD	b0 = 0,b1 = 0,b2 = 0,b3 = 0;
		m_ctrlIP.GetAddress(dwIP1);
		swscanf_s( szIP,_T("%d.%d.%d.%d"),&b0,&b1,&b2,&b3 );
		dwIP0 = (b0<<24) | (b1<<16) | (b2<<8) | b3;
		for( POSITION pos = m_aFilters.GetHeadPosition(); pos; m_aFilters.GetNext(pos) )
		{
			CFilter*	filter = (CFilter*) m_aFilters.GetAt(pos);
			if( filter->m_dwIP == dwIP0 ) filter->m_dwIP = dwIP1;
		}
	}
	m_Refresh();
}

BOOL CFilterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_listIP.InsertColumn( 0,_T("允许显示的地址"),LVCFMT_LEFT,100 );
	m_Refresh();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CFilterDlg::OnLvnItemchangedListFilter(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_nItem = pNMLV->iItem;
	CString	szIP = m_listIP.GetItemText( m_nItem,0 );
	DWORD	dwIP = 0;
	DWORD	b0 = 0,b1 = 0,b2 = 0,b3 = 0;
	swscanf_s( szIP,_T("%d.%d.%d.%d"),&b0,&b1,&b2,&b3 );
	dwIP = (b0<<24) | (b1<<16) | (b2<<8) | b3;
	m_ctrlIP.SetAddress( dwIP );
	*pResult = 0;
}

void CFilterDlg::OnBnClickedCheckSimplifyMode()
{
	m_bSimplifyMode = m_simplifyMode.GetCheck();
}
