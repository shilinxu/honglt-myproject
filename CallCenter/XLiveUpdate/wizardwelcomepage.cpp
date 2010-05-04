//
// WizardWelcomePage.cpp
//

#include "StdAfx.h"
#include "LiveUpdate.h"
#include "WizardWelcomePage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CWizardWelcomePage, CWizardPage)

BEGIN_MESSAGE_MAP(CWizardWelcomePage, CWizardPage)
	//{{AFX_MSG_MAP(CWizardWelcomePage)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWizardWelcomePage property page

CWizardWelcomePage::CWizardWelcomePage() : CWizardPage(CWizardWelcomePage::IDD)
{
	//{{AFX_DATA_INIT(CWizardWelcomePage)
	//}}AFX_DATA_INIT
}

CWizardWelcomePage::~CWizardWelcomePage()
{
}

void CWizardWelcomePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizardWelcomePage)
	DDX_Control(pDX, IDC_WELCOME, m_wndWelcome);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CWizardWelcomePage message handlers

BOOL CWizardWelcomePage::OnInitDialog()
{
	CWizardPage::OnInitDialog();

	SetWizardButtons( PSWIZB_NEXT );

	return TRUE;
}

BOOL CWizardWelcomePage::OnSetActive()
{
	SetWizardButtons( PSWIZB_NEXT );
	return CWizardPage::OnSetActive();
}

HBRUSH CWizardWelcomePage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if ( pWnd == &m_wndWelcome )
	{
		pDC->SelectObject( &theApp.m_fntBold );
	}
	
	return CWizardPage::OnCtlColor( pDC, pWnd, nCtlColor );
}
