//
// PageSettingsPlay.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Settings.h"

#include "PageSettingsPlay.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CPlaySettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CPlaySettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CPlaySettingsPage)
	ON_BN_CLICKED(IDC_ONLY_USER, OnOnlyUser)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlaySettingsPage property page

CPlaySettingsPage::CPlaySettingsPage() : CSettingsPage( CPlaySettingsPage::IDD )
{
	//{{AFX_DATA_INIT(CPlaySettingsPage)
	//}}AFX_DATA_INIT
}

CPlaySettingsPage::~CPlaySettingsPage()
{
}

void CPlaySettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlaySettingsPage)
	DDX_Control(pDX, IDC_ONLY_USER, m_wndOnlyUser);
	DDX_Control(pDX, IDC_ONLY_USERID, m_wndOnlyUserId);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CPlaySettingsPage message handlers

BOOL CPlaySettingsPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();

	LoadSettings();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPlaySettingsPage::OnOK()
{
	EditSettings();

	CSettingsPage::OnOK();
}

BOOL CPlaySettingsPage::LoadSettings()
{
	CString strValue;
	
	m_wndOnlyUser.SetCheck( Settings.Play.OnlyUser == TRUE );
	m_wndOnlyUserId.SetWindowText( (LPCTSTR)Settings.Play.OnlyUserId );
	SendMessage(WM_COMMAND, MAKELONG(m_wndOnlyUser.GetDlgCtrlID(),BN_CLICKED), (LPARAM)m_hWnd);
	
	return TRUE;
}

BOOL CPlaySettingsPage::EditSettings() 
{
	CString strValue;
	
	Settings.Play.OnlyUser = m_wndOnlyUser.GetCheck() == TRUE;
	m_wndOnlyUserId.GetWindowText( strValue );
	Settings.Play.OnlyUserId = strValue;
	
	return TRUE;
}

void CPlaySettingsPage::OnOnlyUser() 
{
	Settings.Play.OnlyUser = m_wndOnlyUser.GetCheck();
	m_wndOnlyUserId.EnableWindow( Settings.Play.OnlyUser );
}
