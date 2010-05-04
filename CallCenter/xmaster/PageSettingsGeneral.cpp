//
// PageSettingsGeneral.cpp
//

#include "stdafx.h"
#include "Xmaster.h"
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
//	ON_CBN_SELCHANGE(IDC_GIVEN, OnSelChangeGiven)
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
	DDX_Control(pDX, IDC_SHORT_FREE, m_wndShtFree);
	DDX_Control(pDX, IDC_USER_LENGTH, m_wndUsrLength);
	DDX_Control(pDX, IDC_USER_HEADER, m_wndUsrHeader);
	DDX_Control(pDX, IDC_GIVEN_VALUE, m_wndGivenValue);
	DDX_Control(pDX, IDC_GIVEN, m_wndGiven);
	DDX_Control(pDX, IDC_HOST, m_wndHost);
	DDX_Control(pDX, IDC_DATABASE, m_wndDatabase);
	DDX_Control(pDX, IDC_USERID, m_wndUserId);
	DDX_Control(pDX, IDC_PASSWORD, m_wndPassword);
	DDX_Control(pDX, IDC_GATEWAY, m_wndGateway);
	DDX_Control(pDX, IDC_PORT, m_wndPort);
	DDX_Control(pDX, IDC_MAX_LENGTH, m_wndMaxLength);
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

void CGeneralSettingsPage::OnSelChangeGiven()
{
	int nItem = m_wndGiven.GetCurSel();
	if ( CB_ERR == nItem ) return;
	
	m_wndGivenValue.EnableWindow( nItem == 0 );
	m_wndUsrHeader.EnableWindow( nItem == 0 );
	m_wndUsrLength.EnableWindow( nItem == 0 );
}

BOOL CGeneralSettingsPage::LoadSettings()
{
	CString strValue;
	
	m_wndHost.SetWindowText( Settings.Database.Host );
	m_wndDatabase.SetWindowText( Settings.Database.Database );
	m_wndUserId.SetWindowText( Settings.Database.UserID );
	m_wndPassword.SetWindowText( Settings.Database.Password );
	
	if ( Settings.Network.Host.IsEmpty() )
		m_wndGateway.SetWindowText( _T("自动") );
	else
		m_wndGateway.SetWindowText( Settings.Network.Host );
	strValue.Format( _T("%i"), Settings.Network.Port );
	m_wndPort.SetWindowText( strValue );
	strValue.Format( _T("%i"), Settings.Network.ShortFree );
	m_wndShtFree.SetWindowText( strValue );

	m_wndGiven.SetCurSel( Settings.FreeTry.Enable != TRUE );
	strValue.Format( _T("%i"), Settings.FreeTry.GivenValue );
	m_wndGivenValue.SetWindowText( strValue );
	m_wndUsrHeader.SetWindowText( Settings.FreeTry.UserHeader );
	strValue.Format( _T("%i"), Settings.FreeTry.UserLength );
	m_wndUsrLength.SetWindowText( strValue );
	SendMessage(WM_COMMAND, MAKELONG(m_wndGiven.GetDlgCtrlID(),CBN_SELCHANGE), (LPARAM)m_hWnd);
	
	strValue = FormatTimeString( Settings.Network.MaxLength );
	m_wndMaxLength.SetCurSel( m_wndMaxLength.FindStringExact( -1, (LPCTSTR)strValue ) );
	
	if ( m_wndMaxLength.GetCurSel() == CB_ERR )
	{
		int nItem = m_wndMaxLength.AddString( strValue );
		m_wndMaxLength.SetCurSel( nItem );
	}

	return TRUE;
}

BOOL CGeneralSettingsPage::EditSettings() 
{
	CString strValue;
		
	m_wndHost.GetWindowText( strValue );
	Settings.Database.Host = strValue;
	m_wndDatabase.GetWindowText( strValue );
	Settings.Database.Database = strValue;
	m_wndUserId.GetWindowText( strValue );
	Settings.Database.UserID = strValue;
	m_wndPassword.GetWindowText( strValue );
	Settings.Database.Password = strValue;
	
	m_wndGateway.GetWindowText( strValue );
	if ( strValue.CompareNoCase( _T("自动") ) == 0 )
		Settings.Network.Host.Empty();
	else
		Settings.Network.Host = strValue;
	m_wndPort.GetWindowText( strValue );
	Settings.Network.Port = _ttoi( strValue );
	m_wndShtFree.GetWindowText( strValue );
	Settings.Network.ShortFree = _ttoi( strValue );
	
	Settings.FreeTry.Enable = m_wndGiven.GetCurSel() == 0;
	m_wndGivenValue.GetWindowText( strValue );
	Settings.FreeTry.GivenValue = _ttoi( strValue );
	m_wndUsrHeader.GetWindowText( strValue );
	Settings.FreeTry.UserHeader = strValue;
	m_wndUsrLength.GetWindowText( strValue );
	Settings.FreeTry.UserLength = _ttoi( strValue );
	
	m_wndMaxLength.GetWindowText( strValue );
	Settings.Network.MaxLength = FormatTimeValue( strValue );

	return TRUE;
}

CString CGeneralSettingsPage::FormatTimeString(DWORD nTime) const
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

int CGeneralSettingsPage::FormatTimeValue(LPCTSTR xTime) const
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
