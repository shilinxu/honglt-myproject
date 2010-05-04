//
// PageSettingsUser.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Settings.h"

#include "PageSettingsUser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CUserSettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CUserSettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CUserSettingsPage)
	ON_BN_CLICKED(IDC_REC_FILE_DEFAULT, OnRecFileDefault)
	ON_BN_CLICKED(IDC_USER_NO, OnNoUser)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CUserSettingsPage property page

CUserSettingsPage::CUserSettingsPage() : CSettingsPage( CUserSettingsPage::IDD )
{
	//{{AFX_DATA_INIT(CUserSettingsPage)
	//}}AFX_DATA_INIT
}

CUserSettingsPage::~CUserSettingsPage()
{
}

void CUserSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserSettingsPage)
	DDX_Control(pDX, IDC_REC_TRUNK, m_wndRecTrunk);
	DDX_Control(pDX, IDC_REC_TYPE, m_wndRecType);
	DDX_Control(pDX, IDC_REC_FILE, m_wndRecFile);
	DDX_Control(pDX, IDC_RING_MAX, m_wndMaxRing);
	DDX_Control(pDX, IDC_USER_NO, m_wndNoUser);
	DDX_Control(pDX, IDC_USER_BUSY, m_wndUsrBusy);
	DDX_Control(pDX, IDC_USER_TIME, m_wndUsrTime);
	DDX_Control(pDX, IDC_BRIDGE_PORT, m_wndBrgPort);
	DDX_Control(pDX, IDC_BRIDGE, m_wndBridge);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CUserSettingsPage message handlers

BOOL CUserSettingsPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();

	LoadSettings();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUserSettingsPage::OnOK()
{
	EditSettings();

	CSettingsPage::OnOK();
}

BOOL CUserSettingsPage::LoadSettings()
{
	CString strValue;
	
	m_wndRecType.SetCurSel( Settings.User.RecType != TRUE );
	m_wndRecFile.SetWindowText( Settings.User.RecFile );
	m_wndRecTrunk.SetWindowText( Settings.User.RecTrunk );
	
	strValue = FormatTimeString( Settings.User.MaxRing );
	m_wndMaxRing.SetCurSel( m_wndMaxRing.FindStringExact( -1, (LPCTSTR)strValue ) );
	
	if ( m_wndMaxRing.GetCurSel() == CB_ERR )
	{
		int nItem = m_wndMaxRing.AddString( strValue );
		m_wndMaxRing.SetCurSel( nItem );
	}
	
	m_wndUsrBusy.SetCheck( Settings.User.UserBusy == TRUE );
	m_wndNoUser.SetCheck( Settings.User.NoUser == TRUE );
	m_wndUsrTime.SetWindowText( Settings.User.UserTime );
	SendMessage(WM_COMMAND, MAKELONG(m_wndNoUser.GetDlgCtrlID(),BN_CLICKED), (LPARAM)m_hWnd);
	
	if ( Settings.User.Host.IsEmpty() )
		m_wndBridge.SetWindowText( _T("自动") );
	else
		m_wndBridge.SetWindowText( Settings.User.Host );
	strValue.Format( _T("%i"), Settings.User.Port );
	m_wndBrgPort.SetWindowText( strValue );

	return TRUE;
}

BOOL CUserSettingsPage::EditSettings() 
{
	CString strValue;
	
	Settings.User.RecType = m_wndRecType.GetCurSel() == 0;
	m_wndRecFile.GetWindowText( strValue );
	Settings.User.RecFile = strValue;
	m_wndRecTrunk.GetWindowText( strValue );
	Settings.User.RecTrunk = strValue;
	
	m_wndMaxRing.GetWindowText( strValue );
	Settings.User.MaxRing = FormatTimeValue( strValue );
	
	Settings.User.UserBusy = m_wndUsrBusy.GetCheck() == TRUE;
	Settings.User.NoUser = m_wndNoUser.GetCheck() == TRUE;
	m_wndUsrTime.GetWindowText( strValue );
	Settings.User.UserTime = strValue;
	
	m_wndBridge.GetWindowText( strValue );
	Settings.User.Host = strValue;
	m_wndBrgPort.GetWindowText( strValue );
	Settings.User.Port = _ttoi( strValue );
	
	return TRUE;
}

void CUserSettingsPage::OnNoUser() 
{
	Settings.User.NoUser = m_wndNoUser.GetCheck();
	m_wndUsrTime.EnableWindow( Settings.User.NoUser );
}

void CUserSettingsPage::OnRecFileDefault() 
{
	LPCTSTR xRule = _T("<CHAN>\\<REMOTE>-<DATE><TIME>.pcm");
	
	m_wndRecFile.SetWindowText( xRule );
}

CString CUserSettingsPage::FormatTimeString(DWORD nTime) const
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

int CUserSettingsPage::FormatTimeValue(LPCTSTR xTime) const
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
