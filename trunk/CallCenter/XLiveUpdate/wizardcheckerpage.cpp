//
// WizardCheckerPage.cpp
//

#include "StdAfx.h"
#include "LiveUpdate.h"
#include "WizardCheckerPage.h"
#include "VersionChecker.h"

#include "Tlhelp32.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CWizardCheckerPage, CWizardPage)

BEGIN_MESSAGE_MAP(CWizardCheckerPage, CWizardPage)
	//{{AFX_MSG_MAP(CWizardCheckerPage)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_NEXTPAGE, OnNextPage)
	ON_MESSAGE(WM_CHECKOUT, OnCheckOut)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWizardCheckerPage property page

CWizardCheckerPage::CWizardCheckerPage() : CWizardPage(CWizardCheckerPage::IDD)
{
	//{{AFX_DATA_INIT(CWizardCheckerPage)
	//}}AFX_DATA_INIT
	
	m_nNextPage	= 0;
}

CWizardCheckerPage::~CWizardCheckerPage()
{
}

void CWizardCheckerPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizardCheckerPage)
	DDX_Control(pDX, IDC_WELCOME, m_wndWelcome);
	DDX_Control(pDX, IDC_PROGRESS, m_wndProgress);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CWizardCheckerPage message handlers

BOOL CWizardCheckerPage::OnInitDialog()
{
	CWizardPage::OnInitDialog();
	
	GetDlgItem(IDC_UPGRADER)->ShowWindow( SW_HIDE );
	SetWizardButtons( 0 );
	
	VersionChecker.SetOwner( GetSafeHwnd() );
	VersionChecker.Connect();
	
	return TRUE;
}

void CWizardCheckerPage::OnCheckOut(LPCTSTR lpszText)
{
	m_wndProgress.SetWindowText( lpszText );
}

LONG CWizardCheckerPage::OnNextPage(UINT nPageId, BOOL bContinue)
{
	m_nNextPage	= nPageId;
	
	SetWizardButtons( PSWIZB_NEXT );
	GetSheet()->PostMessage( WM_COMMAND, ID_WIZNEXT);
	
	return 0;
}

LRESULT CWizardCheckerPage::OnWizardNext()
{
	if ( m_nNextPage == -1 )
	GetDlgItem(IDC_UPGRADER)->ShowWindow( SW_SHOW );
	
	if ( m_nNextPage == -1 )
	m_wndProgress.SetWindowText( _T("准备更新程序") );
	
	CString str;
	if ( ! m_nNextPage && CheckProcess(str) )
	{
		CString strText;
		strText.Format( "以下进程正在运行\n"	"%s\n\n"
						"请确认关闭相关程序，然后点击下一步重试！", str );
		AfxMessageBox( (LPCTSTR)strText ); return CWizardCheckerPage::IDD;
	}
	
	int nNextPage = m_nNextPage;
	m_nNextPage = 0;
	
	return nNextPage;
}

BOOL CWizardCheckerPage::CheckProcess(CString& xName)
{
	CString strValue = theApp.m_lpCmdLine;
	strValue		+= '|';
	
	CString str		= strValue.SpanExcluding( _T(" |;,") );
	strValue		= strValue.Mid( str.GetLength() + 1 );
	DWORD nProcessId= _ttoi( str );
	
	if ( ! nProcessId ) return FALSE;
	
	return GetProcessNameByID( nProcessId, xName);
}

BOOL CWizardCheckerPage::GetProcessNameByID(DWORD nPId, CString& strName)
{
	HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	
	PROCESSENTRY32 pe;
	ZeroMemory( &pe, sizeof(pe) );
	
	pe.dwSize = sizeof(pe);
	for ( ::Process32First(hSnapshot,&pe); ; )
	{
		if ( pe.th32ProcessID == nPId )
		{
			strName = pe.szExeFile; return TRUE;
		}
		
		if ( ! ::Process32Next(hSnapshot,&pe) ) break;
	}
	
	return FALSE;
}
