//
// PageSettingsUnicom.cpp
//

#include "stdafx.h"
#include "MakeReport.h"
#include "Settings.h"

#include "PageSettingsUnicom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CUnicomSettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CUnicomSettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CUnicomSettingsPage)
	ON_BN_CLICKED(IDC_TEST_USER, OnTestUser)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CUnicomSettingsPage property page

CUnicomSettingsPage::CUnicomSettingsPage() : CSettingsPage( CUnicomSettingsPage::IDD )
{
	//{{AFX_DATA_INIT(CUnicomSettingsPage)
	//}}AFX_DATA_INIT
}

CUnicomSettingsPage::~CUnicomSettingsPage()
{
}

void CUnicomSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUnicomSettingsPage)
	DDX_Control(pDX, IDC_HOST, m_wndHost);
	DDX_Control(pDX, IDC_PORT, m_wndPort);
	DDX_Control(pDX, IDC_USERID, m_wndUserId);
	DDX_Control(pDX, IDC_PASSWORD, m_wndPassword);
	DDX_Control(pDX, IDC_PATH_USR, m_wndPathUsr);
	DDX_Control(pDX, IDC_INTERVAL, m_wndInterval);
	DDX_Control(pDX, IDC_BOOKMARK, m_wndBookmark);
	DDX_Control(pDX, IDC_TEST_USER, m_wndTest);
	DDX_Control(pDX, IDC_TEST_USERID, m_wndTestPhone);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CUnicomSettingsPage message handlers

BOOL CUnicomSettingsPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();

	LoadSettings();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUnicomSettingsPage::OnOK()
{
	EditSettings();

	CSettingsPage::OnOK();
}

BOOL CUnicomSettingsPage::LoadSettings()
{
	CString strValue;
	
	m_wndHost.SetWindowText( Settings.Unicom.Host );
	strValue.Format( "%i", Settings.Unicom.Port );
	m_wndPort.SetWindowText( (LPCTSTR)strValue );
	m_wndUserId.SetWindowText( Settings.Unicom.UserId );
	m_wndPassword.SetWindowText( Settings.Unicom.Password );
	m_wndPathUsr.SetWindowText( Settings.Unicom.PathUsr );
	
	strValue = FormatTimeString( Settings.Unicom.Interval );
	m_wndInterval.SetWindowText( (LPCTSTR)strValue );
// 	strValue.Format( "%i", Settings.Unicom.BookMark );
// 	m_wndBookmark.SetWindowText( (LPCTSTR)strValue );
	m_wndBookmark.SetWindowText( Settings.Unicom.IgnoreHour );
	
	m_wndTest.SetCheck( Settings.Unicom.Test == TRUE );
	m_wndTestPhone.SetWindowText( (LPCTSTR)Settings.Unicom.TestPhone );
	SendMessage(WM_COMMAND, MAKELONG(m_wndTest.GetDlgCtrlID(),BN_CLICKED), (LPARAM)m_hWnd);
	
	return TRUE;
}

BOOL CUnicomSettingsPage::EditSettings() 
{
	CString strValue;
		
	m_wndHost.GetWindowText( strValue );
	Settings.Unicom.Host = strValue;
	m_wndPort.GetWindowText( strValue );
	Settings.Unicom.Port = _ttoi( strValue );
	m_wndUserId.GetWindowText( strValue );
	Settings.Unicom.UserId = strValue;
	m_wndPassword.GetWindowText( strValue );
	Settings.Unicom.Password = strValue;
	m_wndPathUsr.GetWindowText( strValue );
	Settings.Unicom.PathUsr = strValue;
	
	m_wndInterval.GetWindowText( strValue );
	Settings.Unicom.Interval = FormatTimeValue( strValue );
// 	m_wndBookmark.GetWindowText( strValue );
// 	Settings.Unicom.BookMark = _ttoi( strValue );
	m_wndBookmark.GetWindowText( strValue );
	Settings.Unicom.IgnoreHour = strValue;
	
	Settings.Unicom.Test = m_wndTest.GetCheck() == TRUE;
	m_wndTestPhone.GetWindowText( strValue );
	Settings.Unicom.TestPhone = strValue;
	
	return TRUE;
}

void CUnicomSettingsPage::OnTestUser() 
{
	m_wndTestPhone.EnableWindow( m_wndTest.GetCheck() );
}

CString CUnicomSettingsPage::FormatTimeString(DWORD nTime) const
{
	CString strValue;
	
	if ( nTime < 60 ) 
		strValue.Format( "%i秒", nTime );
	else
	if ( nTime < 60 * 60 ) 
		strValue.Format( "%i分钟", nTime / 60 );
	else
	if ( nTime < 24 * 3600 )
		strValue.Format( "%i小时", nTime / 3600 );
	else
		strValue.Format( "%i天", nTime / 86400 );
	
	return strValue;
}

int CUnicomSettingsPage::FormatTimeValue(LPCTSTR xTime) const
{
	int nTime = 0;
	
	for ( LPCTSTR sptr = xTime; _istdigit(*sptr); sptr++ )
	{
		nTime = nTime * 10 + (*sptr) - '0';
	}
	
	if ( _tcsnistr( xTime, "分钟", 4 ) )
		nTime *= 60;
	else
	if ( _tcsnistr( xTime, "小时", 4 ) )
		nTime *= 3600;
	else
	if ( _tcsnistr( xTime, "天", 2 ) )
		nTime *= 86400;
	
	return nTime;
}
