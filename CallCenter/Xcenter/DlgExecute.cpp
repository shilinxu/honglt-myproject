//
// DlgExecute.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "DlgExecute.h"
#include "CoolInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CExecuteDlg, CDialog)

BEGIN_MESSAGE_MAP(CExecuteDlg, CDialog)
	//{{AFX_MSG_MAP(CExecuteDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExecuteDlg construction

CExecuteDlg::CExecuteDlg() : CDialog( CExecuteDlg::IDD, NULL )
{
	m_hThread		= NULL;
	m_bThread		= FALSE;
	
	Create( IDD );
}

CExecuteDlg::~CExecuteDlg()
{
}

void CExecuteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

/////////////////////////////////////////////////////////////////////////////
// CExecuteDlg message handlers

BOOL CExecuteDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CenterWindow();
	
	SetWindowPos( &wndTop, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW );
	UpdateWindow();
	
	return TRUE;
}

void CExecuteDlg::OnTimer(UINT nIDEvent)
{
	if ( ! IsRunning() )
	{
		KillTimer( 1 );
		Release();
	}
}

void CExecuteDlg::OnCancel()
{
	Stop(); Release();
}

void CExecuteDlg::Release()
{
	DestroyWindow();
	delete this;
}

void CExecuteDlg::Hide()
{
	ShowWindow( SW_HIDE );
}

BOOL CExecuteDlg::Execute(LPCTSTR xObject, LPCTSTR xFormat)
{
	CString strQuery;
	strQuery.Format( xFormat, xObject );
	
	try
	{
		CStatic* pWndObject = (CStatic*)GetDlgItem( IDC_EXECUTE_OBJECT );
		pWndObject->SetWindowText( xObject );
		
		xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
	}
	catch(_com_error e)
	{
		Hide();
		AfxMessageBox(CString("SQLÖ´ÐÐÊ§°Ü£º") + e.ErrorMessage());
		return FALSE;
	}
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CExecuteDlg operations

void CExecuteDlg::Start()
{
	if ( m_hThread != NULL && m_bThread ) return;
	
	m_bThread = TRUE;
	SetTimer( 1, 1000, NULL );
	CWinThread* pThread = AfxBeginThread( ThreadStart, this, THREAD_PRIORITY_NORMAL );
	m_hThread = pThread->m_hThread;
}

void CExecuteDlg::Stop()
{
	if ( m_hThread == NULL ) return;
	m_bThread = FALSE;
	
	WaitForSingleObject( m_hThread, INFINITE );
}

BOOL CExecuteDlg::IsRunning()
{
	if ( m_hThread == NULL ) return FALSE;
	DWORD nCode = 0;
	if ( ! GetExitCodeThread( m_hThread, &nCode ) ) return FALSE;
	return nCode == STILL_ACTIVE;
}

UINT CExecuteDlg::ThreadStart(LPVOID pParam)
{
	CExecuteDlg* pClass = (CExecuteDlg*)pParam;
	pClass->OnRun();
	return 0;
}
