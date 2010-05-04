//
// PageSettingsMeeting.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Settings.h"

#include "PageSettingsMeeting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CMeetingSettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CMeetingSettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CMeetingSettingsPage)
	ON_BN_CLICKED(IDC_REC_FILE_DEFAULT, OnDefaultRecFile)
	ON_BN_CLICKED(IDC_REC_PATH_BROWSE, OnBrowseRecPath)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMeetingSettingsPage property page

CMeetingSettingsPage::CMeetingSettingsPage() : CSettingsPage( CMeetingSettingsPage::IDD )
{
	//{{AFX_DATA_INIT(CMeetingSettingsPage)
	//}}AFX_DATA_INIT
}

CMeetingSettingsPage::~CMeetingSettingsPage()
{
}

void CMeetingSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMeetingSettingsPage)
	DDX_Control(pDX, IDC_OUT_MAXTIME, m_wndMaxOut);
	DDX_Control(pDX, IDC_CALL_SPACE, m_wndCallVal);
	DDX_Control(pDX, IDC_REC_FILE, m_wndRecFile);
	DDX_Control(pDX, IDC_REC_PATH, m_wndRecPath);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CMeetingSettingsPage message handlers

BOOL CMeetingSettingsPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();

	LoadSettings();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMeetingSettingsPage::OnOK()
{
	EditSettings();

	CSettingsPage::OnOK();
}

BOOL CMeetingSettingsPage::LoadSettings()
{
	CString strValue;
	
	strValue = FormatTimeString( Settings.Meeting.MaxOutTime );
	m_wndMaxOut.SetCurSel( m_wndMaxOut.FindStringExact( -1, (LPCTSTR)strValue ) );
	
	if ( m_wndMaxOut.GetCurSel() == CB_ERR )
	{
		int nItem = m_wndMaxOut.AddString( strValue );
		m_wndMaxOut.SetCurSel( nItem );
	}
	
	strValue = FormatTimeString( Settings.Meeting.CallSpace );
	m_wndCallVal.SetCurSel( m_wndCallVal.FindStringExact( -1, (LPCTSTR)strValue ) );
	
	if ( m_wndCallVal.GetCurSel() == CB_ERR )
	{
		int nItem = m_wndCallVal.AddString( strValue );
		m_wndCallVal.SetCurSel( nItem );
	}
	
	strValue = Settings.Meeting.RecFile;
	m_wndRecFile.SetWindowText( (LPCTSTR)strValue );
	strValue = Settings.Meeting.RecPath;
	m_wndRecPath.SetWindowText( (LPCTSTR)strValue );
	
	return TRUE;
}

BOOL CMeetingSettingsPage::EditSettings() 
{
	CString strValue;
	
	m_wndMaxOut.GetWindowText( strValue );
	Settings.Meeting.MaxOutTime = FormatTimeValue( strValue );
	
	m_wndCallVal.GetWindowText( strValue );
	Settings.Meeting.CallSpace = FormatTimeValue( strValue );
	
	m_wndRecFile.GetWindowText( strValue );
	Settings.Meeting.RecFile = strValue;
	m_wndRecPath.GetWindowText( strValue );
	Settings.Meeting.RecPath = strValue;
	
	return TRUE;
}

void CMeetingSettingsPage::OnDefaultRecFile()
{
	LPCTSTR xRule = _T("<REMOTE>-<DATE><TIME>.wav");
	
	m_wndRecFile.SetWindowText( xRule );
}

void CMeetingSettingsPage::OnBrowseRecPath()
{
	TCHAR szPath[MAX_PATH];
	LPITEMIDLIST pPath;
	LPMALLOC pMalloc;
	BROWSEINFO pBI;
		
	ZeroMemory( &pBI, sizeof(pBI) );
	pBI.hwndOwner		= AfxGetMainWnd()->GetSafeHwnd();
	pBI.pszDisplayName	= szPath;
	pBI.lpszTitle		= _T("选择传真录音的路径:");
	pBI.ulFlags			= BIF_RETURNONLYFSDIRS;
	
	pPath = SHBrowseForFolder( &pBI );

	if ( pPath == NULL ) return;

	SHGetPathFromIDList( pPath, szPath );
	SHGetMalloc( &pMalloc );
	pMalloc->Free( pPath );
	pMalloc->Release();
	
	m_wndRecPath.SetWindowText( szPath );
}

CString CMeetingSettingsPage::FormatTimeString(DWORD nTime) const
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

int CMeetingSettingsPage::FormatTimeValue(LPCTSTR xTime) const
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
