//
// PageSettingsGeneral.cpp
//

#include "stdafx.h"
#include "Xgateway.h"
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
	ON_BN_CLICKED(IDC_PATH_REC_BROWSE, OnPathRecBrowse)
	ON_BN_CLICKED(IDC_REC_FILE_DEFAULT, OnDefaultRecFile)
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
	DDX_Control(pDX, IDC_HOST, m_wndHost);
	DDX_Control(pDX, IDC_DATABASE, m_wndDatabase);
	DDX_Control(pDX, IDC_USERID, m_wndUserID);
	DDX_Control(pDX, IDC_PASSWORD, m_wndPassword);
	DDX_Control(pDX, IDC_PATH_REC, m_wndPathRec);
	DDX_Control(pDX, IDC_REC_FILE, m_wndRecFile);
	DDX_Control(pDX, IDC_TIMEOUT, m_wndTimeOut);
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

BOOL CGeneralSettingsPage::LoadSettings()
{
	CString strValue;
	
	m_wndHost.SetWindowText( Settings.Database.Host );
	m_wndDatabase.SetWindowText( Settings.Database.Database );
	m_wndUserID.SetWindowText( Settings.Database.UserID );
	m_wndPassword.SetWindowText( Settings.Database.Password );
	
	m_wndPathRec.SetWindowText( Settings.Record.PathRec );
	m_wndRecFile.SetWindowText( Settings.Record.RecFile );
	
	strValue = FormatTimeString( Settings.General.TimeOut );
	m_wndTimeOut.SetCurSel( m_wndTimeOut.FindStringExact( -1, (LPCTSTR)strValue ) );
	
	if ( m_wndTimeOut.GetCurSel() == CB_ERR )
	{
		int nItem = m_wndTimeOut.AddString( strValue );
		m_wndTimeOut.SetCurSel( nItem );
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
	m_wndUserID.GetWindowText( strValue );
	Settings.Database.UserID = strValue;
	m_wndPassword.GetWindowText( strValue );
	Settings.Database.Password = strValue;
	
	m_wndPathRec.GetWindowText( strValue );
	Settings.Record.PathRec = strValue;
	m_wndRecFile.GetWindowText( Settings.Record.RecFile );
	
	m_wndTimeOut.GetWindowText( strValue );
	Settings.General.TimeOut = FormatTimeValue( strValue );
	
	return TRUE;
}

void CGeneralSettingsPage::OnPathRecBrowse()
{
	TCHAR szPath[MAX_PATH];
	LPITEMIDLIST pPath;
	LPMALLOC pMalloc;
	BROWSEINFO pBI;
		
	ZeroMemory( &pBI, sizeof(pBI) );
	pBI.hwndOwner		= AfxGetMainWnd()->GetSafeHwnd();
	pBI.pszDisplayName	= szPath;
	pBI.lpszTitle		= _T("选择录音存放的路径:");
	pBI.ulFlags			= BIF_RETURNONLYFSDIRS;
	
	pPath = SHBrowseForFolder( &pBI );

	if ( pPath == NULL ) return;

	SHGetPathFromIDList( pPath, szPath );
	SHGetMalloc( &pMalloc );
	pMalloc->Free( pPath );
	pMalloc->Release();
	
	m_wndPathRec.SetWindowText( szPath );
}

void CGeneralSettingsPage::OnDefaultRecFile()
{
	LPCTSTR xRule = _T("<USER>\\<DATE>\\<USER>.<DATE><TIME>.AVI");
	
	m_wndRecFile.SetWindowText( xRule );
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
