#pragma once


// CSqlDlg 대화 상자입니다.

class CSqlDlg : public CDialog
{
	DECLARE_DYNAMIC(CSqlDlg)

public:
	CSqlDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSqlDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SQL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strSQL;

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
