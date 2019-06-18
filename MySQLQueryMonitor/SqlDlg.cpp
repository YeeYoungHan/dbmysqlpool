// SqlDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MySQLQueryMonitor.h"
#include "SqlDlg.h"


// CSqlDlg ��ȭ �����Դϴ�.

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


// CSqlDlg �޽��� ó�����Դϴ�.

BOOL CSqlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	UpdateData( TRUE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CSqlDlg::OnBnClickedOk()
{
	OnOK();
}
