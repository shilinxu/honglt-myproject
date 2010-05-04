//
// WizardSheet.cpp
//

#include "stdafx.h"
#include "LiveUpdate.h"
#include "WizardSheet.h"

#include "WizardWelcomePage.h"
#include "WizardCheckerPage.h"
#include "WizardUpdatePage.h"
#include "WizardFinishedPage.h"
#include "WizardNothingPage.h"
#include "WizardFailedPage.h"

BEGIN_MESSAGE_MAP(CWizardSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CWizardSheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CWizardPage, CPropertyPage)
BEGIN_MESSAGE_MAP(CWizardPage, CPropertyPage)
	//{{AFX_MSG_MAP(CWizardPage)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWizardSheet run wizard

BOOL CWizardSheet::RunWizard(CWnd* pParent)
{
	BOOL bSuccess = FALSE;
	
	CWizardSheet pSheet( pParent, 0 );
	
	CWizardWelcomePage		pWelcome;
	pSheet.AddPage( &pWelcome );
	
	CWizardCheckerPage		pChecker;
	pSheet.AddPage( &pChecker );
	
	CWizardUpdatePage		pUpdate;
	pSheet.AddPage( &pUpdate );
	
	CWizardFinishedPage		pFinished;
	pSheet.AddPage( &pFinished );
	
	CWizardNothingPage		pNothing;
	pSheet.AddPage( &pNothing );
	
	CWizardFailedPage		pFailed;
	pSheet.AddPage( &pFailed );
	
	bSuccess |= ( pSheet.DoModal() == IDOK );
	
	return bSuccess;
}

//////////////////////////////////////////////////////////////////////
// CWizardSheet construction

CWizardSheet::CWizardSheet(CWnd *pParentWnd, UINT iSelectPage)
{
	m_psh.dwFlags &= ~PSP_HASHELP;
	
	Construct( _T(""), pParentWnd, iSelectPage );
	SetWizardMode();
}

CWizardSheet::~CWizardSheet()
{
}

/////////////////////////////////////////////////////////////////////////////
// CWizardSheet message handlers

BOOL CWizardSheet::OnInitDialog()
{
	CPropertySheet::OnInitDialog();
	
	SetIcon( theApp.LoadIcon( IDR_MAINFRAME ), TRUE );
	
	DWORD nStyle = GetWindowLong( m_hWnd, GWL_STYLE );   
	SetWindowLong( m_hWnd, GWL_STYLE, nStyle | WS_MINIMIZEBOX );   	
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CWizardSheet operations

CWizardPage* CWizardSheet::FindPage(CRuntimeClass* pClass)
{
	for ( int nIndex = GetPageCount(); nIndex > 0; nIndex-- )
	{
		CWizardPage* pPage = (CWizardPage*)GetPage( nIndex - 1 );
		if ( ! pPage || pPage->IsKindOf( pClass ) ) return pPage;
	}
	
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CWizardPage construction

CWizardPage::CWizardPage(UINT nID) : CPropertyPage( nID )
{
	m_crWhite = RGB( 255, 255, 255 );
	m_brWhite.CreateSolidBrush( m_crWhite );
}

CWizardPage::~CWizardPage()
{
}

/////////////////////////////////////////////////////////////////////////////
// CWizardPage message handlers

HBRUSH CWizardPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkColor( m_crWhite );
	return (HBRUSH)m_brWhite.GetSafeHandle();
}

/////////////////////////////////////////////////////////////////////////////
// CWizardPage operations

CWizardSheet* CWizardPage::GetSheet()
{
	return (CWizardSheet*)GetParent();
}

void CWizardPage::SetWizardButtons(DWORD dwFlags)
{
	GetSheet()->SetWizardButtons( dwFlags );
}
