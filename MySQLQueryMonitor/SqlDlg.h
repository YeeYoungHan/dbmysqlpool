#pragma once


// CSqlDlg ��ȭ �����Դϴ�.

class CSqlDlg : public CDialog
{
	DECLARE_DYNAMIC(CSqlDlg)

public:
	CSqlDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CSqlDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SQL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strSQL;

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
