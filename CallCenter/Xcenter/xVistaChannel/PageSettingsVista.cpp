//
// PageSettingsVista.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Registry.h"

#include "PageSettingsVista.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CVistaSettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CVistaSettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CVistaSettingsPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CVistaSettingsPage property page

CVistaSettingsPage::CVistaSettingsPage() : CSettingsPage( CVistaSettingsPage::IDD )
{
	//{{AFX_DATA_INIT(CVistaSettingsPage)
	//}}AFX_DATA_INIT
}

CVistaSettingsPage::~CVistaSettingsPage()
{
}

void CVistaSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVistaSettingsPage)
	DDX_Control(pDX, IDC_HOST, m_wndHost);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CVistaSettingsPage message handlers

BOOL CVistaSettingsPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();
	
	LoadSettings();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CVistaSettingsPage::OnOK()
{
	EditSettings();

	CSettingsPage::OnOK();
}

BOOL CVistaSettingsPage::LoadSettings()
{
	CRegistry pRegistry;
	
	m_wndHost.SetWindowText( pRegistry.GetString( "Vista", "Host", "057496031" ) );
	
	return TRUE;
}

BOOL CVistaSettingsPage::EditSettings() 
{
	CString strValue;
	CRegistry pRegistry;
	
	m_wndHost.GetWindowText( strValue );
	pRegistry.SetString( "Vista", "Host", strValue );
	
	return TRUE;
}
