// RecvDataDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Commate.h"
#include "RecvDataDlg.h"
#include "recvdatadlg.h"
#include "ShowDataView.h"
#include "..\..\TmnLibs\SocketApi\CommonFacility.h"

// CRecvDataDlg 对话框
static	CString		szDISP_MODE_STRING[2] = { _T("以ASCII方式显示"),_T("以HEX方式显示") };

IMPLEMENT_DYNAMIC(CRecvDataDlg, CDialog)
CRecvDataDlg::CRecvDataDlg(CObList& oHistory,CWnd* pParent /*=NULL*/)
	: CDialog(CRecvDataDlg::IDD, pParent)
{
	m_nItem = -1;
	m_nDispMode = 0;
	m_nFilterMode = FILTER_ALL;
	m_currPos = NULL;
	m_oHistory = &oHistory;
}

CRecvDataDlg::~CRecvDataDlg()
{
}

void CRecvDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RECV_DATA, m_ctrlList);
	DDX_Control(pDX, IDC_EDIT_RECV_DATA, m_ctrlEdit);
}


BEGIN_MESSAGE_MAP(CRecvDataDlg, CDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_RECV_DATA, OnLvnItemchangedListRecvData)
	ON_COMMAND(ID_RECV_BUFF_DELETE, OnRecvBuffDelete)
	ON_COMMAND(ID_RECV_BUFF_REFRESH, OnRecvBuffRefresh)
	ON_COMMAND(ID_RECV_BUFF_ASCII_HEX, OnRecvBuffAsciiHex)
	ON_WM_SIZE()
	ON_COMMAND(ID_RECV_BUFF_FILTER_RECV, OnRecvBuffFilterRecv)
	ON_COMMAND(ID_RECV_BUFF_FILTER_SEND, OnRecvBuffFilterSend)
	ON_COMMAND(ID_RECV_BUFF_FILTER_ALL, OnRecvBuffFilterAll)
	ON_UPDATE_COMMAND_UI(ID_RECV_BUFF_ASCII_HEX, &CRecvDataDlg::OnUpdateRecvBuffAsciiHex)
END_MESSAGE_MAP()


// CRecvDataDlg 消息处理程序
int CRecvDataDlg::m_RefreshTable(int nFilterMode)
{
	CObList&	oHistory = *m_oHistory;
	int			nItem = 0;

	m_ctrlList.DeleteAllItems();
	m_ctrlEdit.SetWindowText(_T(""));
	for(	POSITION pos = oHistory.GetHeadPosition();
			pos != NULL; oHistory.GetNext(pos) )
	{
		CRecvBuff*	pLine = (CRecvBuff*)oHistory.GetAt(pos);
		char*		pBuff = pLine->m_GetBuffer();
		long		nBuff = pLine->m_GetLength();
		long		nNo = pLine->m_GetSerial();
		DWORD		dwAdd = pLine->m_GetAddress();
		struct	__timeb64&	oTime = pLine->m_GetTime();
		struct tm	sTime;
		unsigned short		nMillin = oTime.millitm;
		CString		szNo;
		CString		szAdd;
		CString		szLeng;
		CString		szMesg;
		CString		szTime;
		int			nSendRecv = pLine->m_IsSend() ? 0 : 1;
		long		nFilter = pLine->m_IsSend() ? FILTER_SEND : FILTER_RECV;
		errno_t		error = _localtime64_s( &sTime,&(oTime.time) );

		if( nFilterMode & nFilter )
		{	//	显示报文
			int	i = 0;
			for( i = 0; i < nBuff && i < 16; i ++ )
			{
				CString	szByte;
				szByte.Format( _T("%02X "),0x00ff&pBuff[i] );
				szMesg += szByte;
			}
			if( i == 16 && nBuff != 16 ) szMesg += "...";
			szNo.Format( _T("%d"),nNo );
			szAdd.Format( _T("%d.%d.%d.%d"),0x00ff&(dwAdd>>24),0x00ff&(dwAdd>>16),
											0x00ff&(dwAdd>>8),0x00ff&dwAdd );
			szLeng.Format( _T("%d"),nBuff );
			szTime.Format( _T("%04d-%02d-%02d %02d:%02d:%02d.%d"),
							1900+sTime.tm_year,1+sTime.tm_mon,sTime.tm_mday,
							sTime.tm_hour,sTime.tm_min,sTime.tm_sec,nMillin );
			m_ctrlList.InsertItem( nItem, szNo,nSendRecv);
			m_ctrlList.SetItemText( nItem,1,szAdd );
			m_ctrlList.SetItemText( nItem,2,szTime );
			m_ctrlList.SetItemText( nItem,3,szLeng );
			m_ctrlList.SetItemText( nItem,4,szMesg );
			nItem ++;
		}
	}
	CString	szCount;
	szCount.Format( _T("%d 条报文"),nItem );
	m_ctrlList.InsertItem( nItem,_T("━") );
	m_ctrlList.SetItemText( nItem,1,_T("合计") );
	m_ctrlList.SetItemText( nItem,2,szCount );
	return nItem;
}

void CRecvDataDlg::m_GetPosition(long nItem,POSITION& pos)
{
	CObList&	oHistory = *m_oHistory;
	if( oHistory.GetCount() > 0 && nItem >= 0 )
	{
		long		nSerial = 0;
		CString		szSerial = m_ctrlList.GetItemText(nItem,0);
		swscanf_s(	szSerial.GetBuffer(),_T("%d"),&nSerial );
		for( pos = oHistory.GetHeadPosition();pos;oHistory.GetNext(pos) )
		{
			CRecvBuff*	pLine = (CRecvBuff*)oHistory.GetAt( pos );
			if( nSerial == pLine->m_GetSerial() ) return;
		}
		pos = NULL;
	}
	else pos = NULL;
}

BOOL CRecvDataDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_nItem = -1;
	m_nDispMode = 0;
	m_nFilterMode = FILTER_ALL;
	m_currPos = NULL;
	m_ImageList.Create(IDB_BITMAP_SEND_RECV,16,0,RGB(0,128,128));
	m_ImageList.SetBkColor(GetSysColor(COLOR_WINDOW));
	m_ctrlList.ModifyStyle(0,LVS_REPORT|LVS_SHOWSELALWAYS|LVS_SINGLESEL);
	m_ctrlList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,0,
						LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_ctrlList.SetImageList(&m_ImageList,LVSIL_SMALL);
	m_ctrlList.InsertColumn(0,_T("序号"),LVCFMT_LEFT,40 );
	m_ctrlList.InsertColumn(1,_T("地址"),LVCFMT_LEFT,90 );
	m_ctrlList.InsertColumn(2,_T("时间"),LVCFMT_LEFT,115 );
	m_ctrlList.InsertColumn(3,_T("长度"),LVCFMT_LEFT,45 );
	m_ctrlList.InsertColumn(4,_T("报文"),LVCFMT_LEFT,300 );
	m_RefreshTable(FILTER_ALL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CRecvDataDlg::OnLvnItemchangedListRecvData(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if( (m_nItem = pNMLV->iItem) >= 0 )
	{
		m_currPos = NULL;
		m_GetPosition( m_nItem,m_currPos );
		if( m_currPos )
		{
			CObList&	oHistory = *m_oHistory;
			CRecvBuff*	pLine = (CRecvBuff*)oHistory.GetAt(m_currPos);
			char*		pBuff = pLine->m_GetBuffer();
			long		nBuff = pLine->m_GetLength();
			if( m_nDispMode )	m_ShowData( pBuff,nBuff,DISPLAY_HEX );
			else				m_ShowData( pBuff,nBuff,DISPLAY_ASCII );
		}
	}
	*pResult = 0;
}

void CRecvDataDlg::m_ShowData( char* pBuff,long nBuff,int nDispMode )
{
	CString	szAppend;
	for( long i=0;i<nBuff;i++ )
	{
		CString		szOneChar;
		short		cPrev	= 0x00ff&pBuff[i-1];
		short		cData	= 0x00ff&pBuff[i];
		short		cNext	= 0x00ff&pBuff[i+1];
		
		switch( nDispMode )
		{
		case DISPLAY_HEX:
			szOneChar.Format( _T("%02x "),cData );
			break;
		case DISPLAY_ASCII:
			//if( (cData == 0x0D && cNext == 0x0A) ||
			//	cData >= 0x20 || cData == 0x09 )
			//{	//可见ASCII字符
			//	szOneChar.Format( _T("%c"),cData );
			//}
			//else if( cPrev == 0x0D && cData == 0x0A )
			//{
			//	szOneChar.Format( _T("%c↓"),cData );
			//}
			//else
			//{	//特殊字符处理
			//	szOneChar.Format( _T("(%02x)"),cData );
			//}
			//szOneChar = CProbeApp::MultiByteStr_WideStr( (LPSTR) (&cData) );
			break;
		default:;
		}
		szAppend += szOneChar;
	}
	if( nDispMode == DISPLAY_ASCII )
		szAppend = MultiByteStr_WideStr( pBuff );
	m_ctrlEdit.SetWindowText( szAppend );
}

void CRecvDataDlg::OnRecvBuffDelete()
{
	if( m_nItem >= 0 && m_nItem < m_ctrlList.GetItemCount()-1 )
	{
		POSITION	position = NULL;
		m_GetPosition( m_nItem,position );
		if( position )
		{
			CObList&	oHistory = *m_oHistory;
			CRecvBuff*	pLine = (CRecvBuff*)oHistory.GetAt(position);
			oHistory.RemoveAt(position);
			delete pLine;
			m_RefreshTable(	m_nFilterMode );
		}
	}
}

void CRecvDataDlg::OnRecvBuffRefresh()
{
	m_RefreshTable(m_nFilterMode);
}

void CRecvDataDlg::OnRecvBuffAsciiHex()
{
	m_nDispMode = !m_nDispMode;
	if( m_currPos )
	{
		CObList&	oHistory = *m_oHistory;
		CRecvBuff*	pLine = (CRecvBuff*)oHistory.GetAt(m_currPos);
		char*		pBuff = pLine->m_GetBuffer();
		long		nBuff = pLine->m_GetLength();
		m_ShowData( pBuff,nBuff,m_nDispMode );
	}
}

void CRecvDataDlg::OnUpdateRecvBuffAsciiHex(CCmdUI *pCmdUI)
{
	pCmdUI->SetText( szDISP_MODE_STRING[this->m_nDispMode] );
}

void CRecvDataDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CRect	clientRect;

	GetClientRect( clientRect );
	if( m_ctrlList.GetSafeHwnd() && m_ctrlEdit.GetSafeHwnd() &&
		m_ctrlList.IsWindowVisible() && m_ctrlEdit.IsWindowVisible() )
	{
		m_ctrlList.MoveWindow( 0,0,cx*2/5,cy );
		m_ctrlEdit.MoveWindow( cx*2/5,0,cx*3/5,cy );
	}
}

void CRecvDataDlg::OnRecvBuffFilterRecv()
{
	m_nFilterMode = FILTER_RECV;
	m_RefreshTable(m_nFilterMode);
}

void CRecvDataDlg::OnRecvBuffFilterSend()
{
	m_nFilterMode = FILTER_SEND;
	m_RefreshTable(m_nFilterMode);
}

void CRecvDataDlg::OnRecvBuffFilterAll()
{
	m_nFilterMode = FILTER_ALL;
	m_RefreshTable(m_nFilterMode);
}
