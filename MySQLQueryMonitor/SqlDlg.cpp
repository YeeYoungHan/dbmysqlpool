// SqlDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MySQLQueryMonitor.h"
#include "SqlDlg.h"


// CSqlDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSqlDlg, CDialog)

CSqlDlg::CSqlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSqlDlg::IDD, pParent)
	, m_strSQL(_T(""))
{

}

CSqlDlg::~CSqlDlg()
{
}

void CSqlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SQL, m_strSQL);
}


BEGIN_MESSAGE_MAP(CSqlDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CSqlDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSqlDlg 메시지 처리기입니다.

BOOL CSqlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	UpdateData( TRUE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CSqlDlg::OnBnClickedOk()
{
	OnOK();
}
