// CalculaterDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Commate.h"
#include "CalculaterDlg.h"
#include ".\calculaterdlg.h"
#include "E:\Development\TmnDev\TmnCpps\Calculat.h"

// CCalculaterDlg �Ի���

IMPLEMENT_DYNAMIC(CCalculaterDlg, CDialog)
CCalculaterDlg::CCalculaterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCalculaterDlg::IDD, pParent)
{
}

CCalculaterDlg::~CCalculaterDlg()
{
}

void CCalculaterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CALCULATE_FORMULA, m_ctrlEditFormula);
	DDX_Control(pDX, IDC_EDIT_CALCULATE_RESULT, m_ctrlEditResult);
}


BEGIN_MESSAGE_MAP(CCalculaterDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_EXECUTE, OnBnClickedButtonExecute)
END_MESSAGE_MAP()


// CCalculaterDlg ��Ϣ�������

void CCalculaterDlg::OnBnClickedButtonExecute()
{
	int			nStart = 0,nEnd = 0;
	double		fResult = 0.0f;
	CCalculater	cal;
	CString		szText;
	m_ctrlEditFormula.GetWindowText(szText);
	m_ctrlEditFormula.GetSel(nStart,nEnd);
	cal.m_SourceString = ( nEnd > nStart ) ? szText.Mid( nStart,nEnd-nStart ) : szText;
	if( cal.Calculate( &fResult )!=0 )
		MessageBox( _T("��Ч�ļ�����ʽ.") );
	else
	{
		szText.Format(_T("%lf"),fResult);
		m_ctrlEditResult.SetWindowText( szText );
	}
}
