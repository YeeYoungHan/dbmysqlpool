
// MySQLQueryMonitorDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CMySQLQueryMonitorDlg dialog
class CMySQLQueryMonitorDlg : public CDialog
{
// Construction
public:
	CMySQLQueryMonitorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MYSQLQUERYMONITOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	bool m_bStart;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CButton m_btnStart;
	CListCtrl m_clsSQLList;

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedStart();

	LRESULT afx_msg OnMySQLQueryThread( WPARAM wParam, LPARAM lParam );
	CString m_strTime;
};
