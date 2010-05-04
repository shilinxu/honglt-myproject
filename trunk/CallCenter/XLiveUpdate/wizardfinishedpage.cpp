//
// WizardFinishedPage.cpp
//

#include "StdAfx.h"
#include "LiveUpdate.h"
#include "VersionChecker.h"
#include "WizardFinishedPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CWizardFinishedPage, CWizardPage)

BEGIN_MESSAGE_MAP(CWizardFinishedPage, CWizardPage)
	//{{AFX_MSG_MAP(CWizardFinishedPage)
	ON_WM_CTLCOLOR()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWizardFinishedPage property page

CWizardFinishedPage::CWizardFinishedPage() : CWizardPage(CWizardFinishedPage::IDD)
{
	//{{AFX_DATA_INIT(CWizardFinishedPage)
	//}}AFX_DATA_INIT
}

CWizardFinishedPage::~CWizardFinishedPage()
{
}

void CWizardFinishedPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizardFinishedPage)
	DDX_Control(pDX, IDC_WELCOME, m_wndWelcome);
	DDX_Control(pDX, IDC_VERSION, m_wndVersion);
	DDX_Control(pDX, IDC_WEB, m_wndWeb);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CWizardFinishedPage message handlers

BOOL CWizardFinishedPage::OnInitDialog()
{
	CWizardPage::OnInitDialog();
	
	WORD nVersion[4];
	CVersionChecker::GetVersionNumber( "Xuser32.exe", nVersion );
	
	CString strValue;
	strValue.Format( _T("Ver%i.%i.%i.%i"),
		nVersion[0], nVersion[1], nVersion[2], nVersion[3] );
	
	m_wndVersion.SetWindowText( (LPCTSTR)strValue );

	return TRUE;
}

BOOL CWizardFinishedPage::OnSetActive()
{
	SetWizardButtons( PSWIZB_FINISH );
	GetSheet()->GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
	
	return CWizardPage::OnSetActive();
}

HBRUSH CWizardFinishedPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if ( pWnd == &m_wndWelcome || pWnd == &m_wndVersion )
	{
		pDC->SelectObject( &theApp.m_fntBold );
	}
	else if ( pWnd == &m_wndWeb )
	{
		pDC->SetTextColor( RGB( 0, 0, 255 ) );
		pDC->SelectObject( &theApp.m_fntUnder );
	}
	
	return CWizardPage::OnCtlColor( pDC, pWnd, nCtlColor );
}

BOOL CWizardFinishedPage::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CPoint point;
	CRect rc;

	GetCursorPos( &point );
	m_wndWeb.GetWindowRect( &rc );

	if ( rc.PtInRect( point ) )
	{
		SetCursor( theApp.LoadCursor( IDC_HAND ) );
		return TRUE;
	}
	
	return CWizardPage::OnSetCursor( pWnd, nHitTest, message );
}

void CWizardFinishedPage::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CWizardPage::OnLButtonUp( nFlags, point );
	
	CRect rc;
	m_wndWeb.GetWindowRect( &rc );
	ScreenToClient( &rc );
	
	if ( rc.PtInRect( point ) )
	{
		ShellExecute( GetSafeHwnd(), _T("open"),
			_T("http://www.96031.com.cn"), NULL, NULL, SW_SHOWNORMAL );
	}
}

void CWizardFinishedPage::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CRect rc;

	m_wndWeb.GetWindowRect( &rc );
	ScreenToClient( &rc );
	
	if ( rc.PtInRect( point ) && ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) )
	{
		DWORD* pNullPtr = (DWORD*)NULL;
		*pNullPtr = 0xFFFFFFFF;
	}
}
