//
// PageSettingsTalker.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Registry.h"

#include "PageSettingsTalker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CTalkerSettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CTalkerSettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CTalkerSettingsPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTalkerSettingsPage property page

CTalkerSettingsPage::CTalkerSettingsPage() : CSettingsPage( CTalkerSettingsPage::IDD )
{
	//{{AFX_DATA_INIT(CTalkerSettingsPage)
	//}}AFX_DATA_INIT
}

CTalkerSettingsPage::~CTalkerSettingsPage()
{
}

void CTalkerSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTalkerSettingsPage)
	DDX_Control(pDX, IDC_TRICK_ENABLE, m_wndTrickEnable);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CTalkerSettingsPage message handlers

BOOL CTalkerSettingsPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();

	LoadSettings();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTalkerSettingsPage::OnOK()
{
	EditSettings();

	CSettingsPage::OnOK();
}

BOOL CTalkerSettingsPage::LoadSettings()
{
	CRegistry pRegistry;
	
	m_wndTrickEnable.SetCheck( pRegistry.GetInt( "Talker", "EnableTrick", FALSE ) );
	
	return TRUE;
}

BOOL CTalkerSettingsPage::EditSettings() 
{
	CRegistry pRegistry;
	
	pRegistry.SetInt( "Talker", "EnableTrick", m_wndTrickEnable.GetCheck() );
	
	return TRUE;
}
