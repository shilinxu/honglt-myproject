//
// PageSettingsPlay.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
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
	DDX_Control(pDX, IDC_DAPAN_NEW, m_wndNew0);
	DDX_Control(pDX, IDC_DAPAN_MARKUP, m_wndMarkup0);
	DDX_Control(pDX, IDC_GEGU_NEW, m_wndNew1);
	DDX_Control(pDX, IDC_GEGU_CODE, m_wndCode1);
	DDX_Control(pDX, IDC_GEGU_MARKUP, m_wndMarkup1);
	DDX_Control(pDX, IDC_PAGER_EXPIRE, m_wndExpired);
	DDX_Control(pDX, IDC_REC_LENGTH, m_wndRecLeng);
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
	CRegistry pRegistry;
	
	m_wndNew0.SetWindowText( pRegistry.GetString( "Stock", "Offset_new0", "" ) );
	m_wndMarkup0.SetWindowText( pRegistry.GetString( "Stock", "Offset_markup0", "" ) );
	
	m_wndNew1.SetWindowText( pRegistry.GetString( "Stock", "Offset_new1", "" ) );
	m_wndCode1.SetWindowText( pRegistry.GetString( "Stock", "Offset_code1", "" ) );
	m_wndMarkup1.SetWindowText( pRegistry.GetString( "Stock", "Offset_markup1", "" ) );
	
	CString strValue;
	strValue = FormatTimeString( pRegistry.GetInt("Radio", "PageExpire", 7*24 * 3600) );
	m_wndExpired.SetCurSel( m_wndExpired.FindStringExact( -1, (LPCTSTR)strValue ) );
	
	if ( m_wndExpired.GetCurSel() == CB_ERR )
	{
		int nItem = m_wndExpired.AddString( strValue );
		m_wndExpired.SetCurSel( nItem );
	}
	
	strValue = FormatTimeString( pRegistry.GetInt("Radio", "RecLength", 3 * 60) );
	m_wndRecLeng.SetCurSel( m_wndRecLeng.FindStringExact( -1, (LPCTSTR)strValue ) );
	
	if ( m_wndRecLeng.GetCurSel() == CB_ERR )
	{
		int nItem = m_wndRecLeng.AddString( strValue );
		m_wndRecLeng.SetCurSel( nItem );
	}
	
	return TRUE;
}

BOOL CPlaySettingsPage::EditSettings() 
{
	CRegistry pRegistry;
	CString strValue;
	
	m_wndNew0.GetWindowText( strValue );
	pRegistry.SetString( "Stock", "Offset_new0", strValue );
	m_wndMarkup0.GetWindowText( strValue );
	pRegistry.SetString( "Stock", "Offset_markup0", strValue );
	
	m_wndNew1.GetWindowText( strValue );
	pRegistry.SetString( "Stock", "Offset_new1", strValue );
	m_wndCode1.GetWindowText( strValue );
	pRegistry.SetString( "Stock", "Offset_code1", strValue );
	m_wndMarkup1.GetWindowText( strValue );
	pRegistry.SetString( "Stock", "Offset_markup1", strValue );
	
	m_wndExpired.GetWindowText( strValue );
	pRegistry.SetInt( "Radio", "PageExpire", FormatTimeValue(strValue) );
	
	m_wndRecLeng.GetWindowText( strValue );
	pRegistry.SetInt( "Radio", "RecLength", FormatTimeValue(strValue) );
	
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
