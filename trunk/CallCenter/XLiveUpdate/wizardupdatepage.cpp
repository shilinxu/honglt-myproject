//
// WizardUpdatePage.cpp
//

#include "StdAfx.h"
#include "LiveUpdate.h"
#include "WizardUpdatePage.h"
#include "VersionChecker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CWizardUpdatePage, CWizardPage)

BEGIN_MESSAGE_MAP(CWizardUpdatePage, CWizardPage)
	//{{AFX_MSG_MAP(CWizardUpdatePage)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_NEXTPAGE, OnNextPage)
	ON_MESSAGE(WM_CHECKOUT, OnCheckOut)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWizardUpdatePage property page

CWizardUpdatePage::CWizardUpdatePage() : CWizardPage(CWizardUpdatePage::IDD)
{
	//{{AFX_DATA_INIT(CWizardUpdatePage)
	//}}AFX_DATA_INIT
	
	m_nNextPage	= 0;
}

CWizardUpdatePage::~CWizardUpdatePage()
{
}

void CWizardUpdatePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizardUpdatePage)
	DDX_Control(pDX, IDC_WELCOME, m_wndWelcome);
	DDX_Control(pDX, IDC_PROGRESS, m_wndProgress);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CWizardUpdatePage message handlers

BOOL CWizardUpdatePage::OnInitDialog()
{
	CWizardPage::OnInitDialog();
	
	SetWizardButtons( 0 );
	
	VersionChecker.SetOwner( GetSafeHwnd() );
	ResumeThread( VersionChecker.m_hThread );
	
	return TRUE;
}

void CWizardUpdatePage::OnCheckOut(LPCTSTR lpszText)
{
	m_wndProgress.SetWindowText( lpszText );
}

LONG CWizardUpdatePage::OnNextPage(UINT nPageId, BOOL bContinue)
{
	m_nNextPage	= nPageId;
	
	SetWizardButtons( PSWIZB_NEXT );
	GetSheet()->PostMessage( WM_COMMAND, ID_WIZNEXT);
	
	return 0;
}

LRESULT CWizardUpdatePage::OnWizardNext()
{
	return m_nNextPage;
}
