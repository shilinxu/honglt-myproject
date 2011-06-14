//
// PageSettingsTelecom.cpp
//

#include "stdafx.h"
#include "MakeReport.h"
#include "Settings.h"

#include "PageSettingsTelecom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CTelecomSettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CTelecomSettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CTelecomSettingsPage)
	ON_BN_CLICKED(IDC_TEST_USER, OnTestUser)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTelecomSettingsPage property page

CTelecomSettingsPage::CTelecomSettingsPage() : CSettingsPage( CTelecomSettingsPage::IDD )
{
	//{{AFX_DATA_INIT(CTelecomSettingsPage)
	//}}AFX_DATA_INIT
}

CTelecomSettingsPage::~CTelecomSettingsPage()
{
}

void CTelecomSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTelecomSettingsPage)
	DDX_Control(pDX, IDC_HOST, m_wndHost);
	DDX_Control(pDX, IDC_PORT, m_wndPort);
	DDX_Control(pDX, IDC_USERID, m_wndUserId);
	DDX_Control(pDX, IDC_PASSWORD, m_wndPassword);
	DDX_Control(pDX, IDC_PATH_USR, m_wndPathUsr);
	DDX_Control(pDX, IDC_TEST_USER, m_wndTest);
	DDX_Control(pDX, IDC_TEST_USERID, m_wndTestPhone);
	DDX_Control(pDX, IDC_BOOKMARK, m_wndBookmark);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CTelecomSettingsPage message handlers

BOOL CTelecomSettingsPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();

	LoadSettings();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTelecomSettingsPage::OnOK()
{
	EditSettings();

	CSettingsPage::OnOK();
}

BOOL CTelecomSettingsPage::LoadSettings()
{
	CString strValue;
	
	m_wndHost.SetWindowText( Settings.Telecom.Host );
	strValue.Format( "%i", Settings.Telecom.Port );
	m_wndPort.SetWindowText( (LPCTSTR)strValue );
	m_wndUserId.SetWindowText( Settings.Telecom.UserId );
	m_wndPassword.SetWindowText( Settings.Telecom.Password );
	m_wndPathUsr.SetWindowText( Settings.Telecom.PathUsr );
	
	m_wndTest.SetCheck( Settings.Telecom.Test == TRUE );
	m_wndTestPhone.SetWindowText( (LPCTSTR)Settings.Telecom.TestPhone );
	SendMessage(WM_COMMAND, MAKELONG(m_wndTest.GetDlgCtrlID(),BN_CLICKED), (LPARAM)m_hWnd);
	
	strValue.Format( "%i", Settings.Telecom.BookMark );
	m_wndBookmark.SetWindowText( (LPCTSTR)strValue );
	
	return TRUE;
}

BOOL CTelecomSettingsPage::EditSettings() 
{
	CString strValue;
		
	m_wndHost.GetWindowText( strValue );
	Settings.Telecom.Host = strValue;
	m_wndPort.GetWindowText( strValue );
	Settings.Telecom.Port = _ttoi( strValue );
	m_wndUserId.GetWindowText( strValue );
	Settings.Telecom.UserId = strValue;
	m_wndPassword.GetWindowText( strValue );
	Settings.Telecom.Password = strValue;
	m_wndPathUsr.GetWindowText( strValue );
	Settings.Telecom.PathUsr = strValue;
	
	Settings.Telecom.Test = m_wndTest.GetCheck() == TRUE;
	m_wndTestPhone.GetWindowText( strValue );
	Settings.Telecom.TestPhone = strValue;
	
	m_wndBookmark.GetWindowText( strValue );
	Settings.Telecom.BookMark = _ttoi( strValue );
	
	return TRUE;
}

void CTelecomSettingsPage::OnTestUser() 
{
	m_wndTestPhone.EnableWindow( m_wndTest.GetCheck() );
}
