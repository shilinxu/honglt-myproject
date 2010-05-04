//
// PageSettingsPlay.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Settings.h"
#include "Registry.h"

#include "PageSettingsPlay.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CPlaySettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CPlaySettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CPlaySettingsPage)
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
	DDX_Control(pDX, IDC_PLAY_RETRY, m_wndRetry);
	DDX_Control(pDX, IDC_BLACKLIST, m_wndBklst);
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
	CRegistry pRegistry;
	
	strValue = FormatTimeString( pRegistry.GetInt( "Profit", "PlayRetry", 180 ) );
	m_wndRetry.SetCurSel( m_wndRetry.FindStringExact( -1, (LPCTSTR)strValue ) );
	
	if ( m_wndRetry.GetCurSel() == CB_ERR )
	{
		int nItem = m_wndRetry.AddString( strValue );
		m_wndRetry.SetCurSel( nItem );
	}
	
	return TRUE;
}

BOOL CPlaySettingsPage::EditSettings() 
{
	CString strValue;
	CRegistry pRegistry;
	
	m_wndRetry.GetWindowText( strValue );
	pRegistry.SetInt( "Profit", "PlayRetry", FormatTimeValue( strValue ) );
	
	return TRUE;
}

CString CPlaySettingsPage::FormatTimeString(DWORD nTime) const
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

int CPlaySettingsPage::FormatTimeValue(LPCTSTR xTime) const
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
