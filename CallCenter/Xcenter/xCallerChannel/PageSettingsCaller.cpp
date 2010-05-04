//
// PageSettingsCaller.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Registry.h"

#include "PageSettingsCaller.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CCallerSettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CCallerSettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CCallerSettingsPage)
	ON_BN_CLICKED(IDC_USER_NO, OnNoUser)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCallerSettingsPage property page

CCallerSettingsPage::CCallerSettingsPage() : CSettingsPage( CCallerSettingsPage::IDD )
{
	//{{AFX_DATA_INIT(CCallerSettingsPage)
	//}}AFX_DATA_INIT
}

CCallerSettingsPage::~CCallerSettingsPage()
{
}

void CCallerSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCallerSettingsPage)
	DDX_Control(pDX, IDC_RING_MAX, m_wndMaxRing);
	DDX_Control(pDX, IDC_USER_NO, m_wndNoUser);
	DDX_Control(pDX, IDC_USER_BUSY, m_wndUsrBusy);
	DDX_Control(pDX, IDC_USER_TIME, m_wndUsrTime);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CCallerSettingsPage message handlers

BOOL CCallerSettingsPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();
	
	LoadSettings();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCallerSettingsPage::OnOK()
{
	EditSettings();

	CSettingsPage::OnOK();
}

BOOL CCallerSettingsPage::LoadSettings()
{
	CString strValue;
	CRegistry pRegistry;
	
	strValue = FormatTimeString( pRegistry.GetInt( "Caller", "MaxRing", 15 ) );
	m_wndMaxRing.SetCurSel( m_wndMaxRing.FindStringExact( -1, (LPCTSTR)strValue ) );
	
	if ( m_wndMaxRing.GetCurSel() == CB_ERR )
	{
		int nItem = m_wndMaxRing.AddString( strValue );
		m_wndMaxRing.SetCurSel( nItem );
	}
	
	m_wndUsrBusy.SetCheck( pRegistry.GetInt( "Caller", "UserBusy", FALSE ) );
	m_wndNoUser.SetCheck( pRegistry.GetInt( "Caller", "NoUser", TRUE ) );
	m_wndUsrTime.SetWindowText( pRegistry.GetString( "Caller", "UserTime", "0800-2200" ) );
	SendMessage(WM_COMMAND, MAKELONG(m_wndNoUser.GetDlgCtrlID(),BN_CLICKED), (LPARAM)m_hWnd);

	return TRUE;
}

BOOL CCallerSettingsPage::EditSettings() 
{
	CString strValue;
	CRegistry pRegistry;
	
	m_wndMaxRing.GetWindowText( strValue );
	pRegistry.SetInt( "Caller", "MaxRing", FormatTimeValue( strValue ) );
	
	pRegistry.SetInt( "Caller", "UserBusy", m_wndUsrBusy.GetCheck() );
	pRegistry.SetInt( "Caller", "NoUser", m_wndNoUser.GetCheck() );
	m_wndUsrTime.GetWindowText( strValue );
	pRegistry.SetString( "Caller", "UserTime", strValue );
	
	return TRUE;
}

void CCallerSettingsPage::OnNoUser() 
{
	BOOL bNoUser = m_wndNoUser.GetCheck();
	m_wndUsrTime.EnableWindow( bNoUser );
}

CString CCallerSettingsPage::FormatTimeString(DWORD nTime) const
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

int CCallerSettingsPage::FormatTimeValue(LPCTSTR xTime) const
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
