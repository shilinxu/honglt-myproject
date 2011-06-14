//
// PageSettingsGeneral.cpp
//

#include "stdafx.h"
#include "MakeReport.h"
#include "Settings.h"

#include "PageSettingsGeneral.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CGeneralSettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CGeneralSettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CGeneralSettingsPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGeneralSettingsPage property page

CGeneralSettingsPage::CGeneralSettingsPage() : CSettingsPage( CGeneralSettingsPage::IDD )
{
	//{{AFX_DATA_INIT(CGeneralSettingsPage)
	//}}AFX_DATA_INIT
}

CGeneralSettingsPage::~CGeneralSettingsPage()
{
}

void CGeneralSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGeneralSettingsPage)
	DDX_Control(pDX, IDC_HOST, m_wndHost);
	DDX_Control(pDX, IDC_DATABASE, m_wndDatabase);
	DDX_Control(pDX, IDC_AUTO_UNICOM, m_wndAutoUnicom);
	DDX_Control(pDX, IDC_AUTO_TELECOM, m_wndAutoTelecom);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CGeneralSettingsPage message handlers

BOOL CGeneralSettingsPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();

	LoadSettings();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGeneralSettingsPage::OnOK()
{
	EditSettings();

	CSettingsPage::OnOK();
}

BOOL CGeneralSettingsPage::LoadSettings()
{
	CString strValue;
	
	m_wndHost.SetWindowText( Settings.Database.Host );
	m_wndDatabase.SetWindowText( Settings.Database.Database );
	
	m_wndAutoUnicom.SetCheck( Settings.General.AutoUnicom );
	m_wndAutoTelecom.SetCheck( Settings.General.AutoTelecom );
	
	return TRUE;
}

BOOL CGeneralSettingsPage::EditSettings() 
{
	CString strValue;
		
	m_wndHost.GetWindowText( strValue );
	Settings.Database.Host = strValue;
	m_wndDatabase.GetWindowText( strValue );
	Settings.Database.Database = strValue;
	
	Settings.General.AutoUnicom = m_wndAutoUnicom.GetCheck();
	Settings.General.AutoTelecom = m_wndAutoTelecom.GetCheck();
	
	return TRUE;
}
