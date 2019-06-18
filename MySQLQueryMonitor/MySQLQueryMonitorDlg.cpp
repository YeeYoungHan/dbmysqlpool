/* 
 * Copyright (C) 2012 Yee Young Han <websearch@naver.com> (http://blog.naver.com/websearch)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */

#include "stdafx.h"
#include "MySQLQueryMonitor.h"
#include "MySQLQueryMonitorDlg.h"
#include "QueryTimeList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMySQLQueryMonitorDlg dialog




CMySQLQueryMonitorDlg::CMySQLQueryMonitorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMySQLQueryMonitorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bStart = true;
}

void CMySQLQueryMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_START, m_btnStart);
	DDX_Control(pDX, IDC_SQL_LIST, m_clsSQLList);
}

BEGIN_MESSAGE_MAP(CMySQLQueryMonitorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CMySQLQueryMonitorDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_START, &CMySQLQueryMonitorDlg::OnBnClickedStart)
	ON_MESSAGE(WM_MYSQL_QUERY_THREAD, &CMySQLQueryMonitorDlg::OnMySQLQueryThread)
END_MESSAGE_MAP()


// CMySQLQueryMonitorDlg message handlers

BOOL CMySQLQueryMonitorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	m_clsSQLList.InsertColumn( 0, "Second", LVCFMT_LEFT, 80 );
	m_clsSQLList.InsertColumn( 1, "SQL", LVCFMT_LEFT, 510 );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMySQLQueryMonitorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMySQLQueryMonitorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMySQLQueryMonitorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMySQLQueryMonitorDlg::OnBnClickedOk()
{
}

void CMySQLQueryMonitorDlg::OnBnClickedStart()
{
	if( m_bStart )
	{
		if( IsMySQLQueryThreadRun() )
		{
			MessageBox( "이미 시작되어 있습니다.", "오류", MB_OK | MB_ICONERROR );
			return;
		}

		if( StartMySQLQueryThread( m_hWnd ) == false )
		{
			MessageBox( "시작하지 못 하였습니다.", "오류", MB_OK | MB_ICONERROR );
			return;
		}

		m_bStart = false;
		m_btnStart.SetWindowText( "Stop" );
	}
	else
	{
		if( IsMySQLQueryThreadRun() )
		{
			StopMySQLQueryThread();
		}

		m_bStart = true;
		m_btnStart.SetWindowText( "Start" );
	}
}

LRESULT CMySQLQueryMonitorDlg::OnMySQLQueryThread( WPARAM wParam, LPARAM lParam )
{
	bool bError = false;

	if( wParam == PARAM_REFRESH )
	{
		QUERY_TIME_LIST clsList;
		QUERY_TIME_LIST::iterator itQT;
		int iRow = 0;
		char szSecond[11];

		gclsQueryTimeList.Select( clsList );
		m_clsSQLList.DeleteAllItems();

		for( itQT = clsList.begin(); itQT != clsList.end(); ++itQT )
		{
			_snprintf( szSecond, sizeof(szSecond), "%d", itQT->m_iSecond );

			m_clsSQLList.InsertItem( iRow, szSecond );
			m_clsSQLList.SetItemText( iRow, 1, itQT->m_strSQL.c_str() );

			++iRow;
		}
	}
	else if( wParam == PARAM_SETUP_ERROR )
	{
		MessageBox( "설정 파일 읽기에 실패하였습니다. MySQLQueryMonitor.exe 프로그램이 존재하는 폴더에 MySQLQueryMonitor.xml 파일이 존재하는지 확인해 주시고 MySQLQueryMonitor.xml 파일의 내용을 확인해 주세요.", "오류", MB_OK | MB_ICONERROR );
		bError = true;
	}
	else if( wParam == PARAM_CONNECT_ERROR )
	{
		MessageBox( "MySQL DB 연결에 실패하였습니다. MySQLQueryMonitor.xml 파일에서 DB 연결 정보를 확인해 주세요.", "오류", MB_OK | MB_ICONERROR );
		bError = true;
	}

	if( bError )
	{
		m_bStart = true;
		m_btnStart.SetWindowText( "Start" );
	}

	return 0;
}
