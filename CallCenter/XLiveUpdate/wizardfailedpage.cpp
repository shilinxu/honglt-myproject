//
// WizardFailedPage.cpp
//

#include "StdAfx.h"
#include "LiveUpdate.h"
#include "WizardFailedPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CWizardFailedPage, CWizardPage)

BEGIN_MESSAGE_MAP(CWizardFailedPage, CWizardPage)
	//{{AFX_MSG_MAP(CWizardFailedPage)
	ON_WM_CTLCOLOR()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWizardFailedPage property page

CWizardFailedPage::CWizardFailedPage() : CWizardPage(CWizardFailedPage::IDD)
{
	//{{AFX_DATA_INIT(CWizardFailedPage)
	//}}AFX_DATA_INIT
}

CWizardFailedPage::~CWizardFailedPage()
{
}

void CWizardFailedPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizardFailedPage)
	DDX_Control(pDX, IDC_WELCOME, m_wndWelcome);
	DDX_Control(pDX, IDC_WEB, m_wndWeb);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CWizardFailedPage message handlers

BOOL CWizardFailedPage::OnSetActive()
{
	SetWizardButtons( PSWIZB_FINISH );
	GetSheet()->GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
	
	return CWizardPage::OnSetActive();
}

HBRUSH CWizardFailedPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if ( pWnd == &m_wndWelcome )
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

BOOL CWizardFailedPage::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
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

void CWizardFailedPage::OnLButtonDown(UINT nFlags, CPoint point) 
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

void CWizardFailedPage::OnRButtonDown(UINT nFlags, CPoint point) 
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
