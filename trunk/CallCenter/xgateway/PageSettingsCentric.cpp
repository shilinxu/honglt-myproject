//
// PageSettingsCentric.cpp
//

#include "stdafx.h"
#include "Xgateway.h"
#include "Settings.h"

#include "PageSettingsCentric.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CCentricSettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CCentricSettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CCentricSettingsPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCentricSettingsPage property page

CCentricSettingsPage::CCentricSettingsPage() : CSettingsPage( CCentricSettingsPage::IDD )
{
	//{{AFX_DATA_INIT(CCentricSettingsPage)
	//}}AFX_DATA_INIT
}

CCentricSettingsPage::~CCentricSettingsPage()
{
}

void CCentricSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCentricSettingsPage)
	DDX_Control(pDX, IDC_LOCAL, m_wndLocal);
	DDX_Control(pDX, IDC_REMOTE, m_wndRemote);
	DDX_Control(pDX, IDC_USER_ID, m_wndUserId);
	DDX_Control(pDX, IDC_PASSWORD, m_wndPassword);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CCentricSettingsPage message handlers

BOOL CCentricSettingsPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();

	LoadSettings();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCentricSettingsPage::OnOK()
{
	EditSettings();

	CSettingsPage::OnOK();
}

BOOL CCentricSettingsPage::LoadSettings()
{
	m_wndLocal.SetWindowText( Settings.Centric.Local );
	m_wndRemote.SetWindowText( Settings.Centric.Remote );
	
	m_wndUserId.SetWindowText( Settings.Centric.UserId );
	m_wndPassword.SetWindowText( Settings.Centric.Password );
	
	return TRUE;
}

BOOL CCentricSettingsPage::EditSettings() 
{
	CString strValue;
	
	m_wndLocal.GetWindowText( strValue );
	Settings.Centric.Local = strValue;
	
	m_wndRemote.GetWindowText( strValue );
	Settings.Centric.Remote = strValue;
	
	m_wndUserId.GetWindowText( Settings.Centric.UserId );
	m_wndPassword.GetWindowText( Settings.Centric.Password );
	
	return TRUE;
}
