//
// PageSettingsGeneral.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
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
	ON_BN_CLICKED(IDC_PATH_FAX_BROWSE, OnPathFaxBrowse)
	ON_BN_CLICKED(IDC_PATH_REC_BROWSE, OnPathRecBrowse)
	ON_BN_CLICKED(IDC_TEST_ENBALE, OnTestEnable)
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
	DDX_Control(pDX, IDC_PATH_REC, m_wndPathRec);
	DDX_Control(pDX, IDC_PATH_FAX, m_wndPathFax);
	DDX_Control(pDX, IDC_HOST, m_wndHost);
	DDX_Control(pDX, IDC_DATABASE, m_wndDatabase);
	DDX_Control(pDX, IDC_USERID, m_wndUserId);
	DDX_Control(pDX, IDC_PASSWORD, m_wndPassword);
	DDX_Control(pDX, IDC_TEST_PHONE, m_wndTestPhone);
	DDX_Control(pDX, IDC_TEST_ENBALE, m_wndTestEnable);
	DDX_Control(pDX, IDC_TEST_CALLER, m_wndTestCaller);
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
	m_wndUserId.SetWindowText( Settings.Database.UserID );
	m_wndPassword.SetWindowText( Settings.Database.Password );
	
	m_wndPathFax.SetWindowText( Settings.General.PathFax );
	m_wndPathRec.SetWindowText( Settings.General.PathRec );
	
	m_wndTestEnable.SetCheck( Settings.Test.Enable == TRUE );
	m_wndTestCaller.SetWindowText( Settings.Test.Caller );
	m_wndTestPhone.SetWindowText( Settings.Test.Phone );
	SendMessage(WM_COMMAND, MAKELONG(m_wndTestEnable.GetDlgCtrlID(),BN_CLICKED), (LPARAM)m_hWnd);

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

	m_wndPathFax.GetWindowText( strValue );
	Settings.General.PathFax = strValue;
	m_wndPathRec.GetWindowText( strValue );
	Settings.General.PathRec = strValue;
	
	Settings.Test.Enable = m_wndTestEnable.GetCheck() == TRUE;
	m_wndTestCaller.GetWindowText( strValue );
	Settings.Test.Caller = strValue;
	m_wndTestPhone.GetWindowText( strValue );
	Settings.Test.Phone = strValue;
	
	return TRUE;
}

void CGeneralSettingsPage::OnPathFaxBrowse()
{
	TCHAR szPath[MAX_PATH];
	LPITEMIDLIST pPath;
	LPMALLOC pMalloc;
	BROWSEINFO pBI;
		
	ZeroMemory( &pBI, sizeof(pBI) );
	pBI.hwndOwner		= AfxGetMainWnd()->GetSafeHwnd();
	pBI.pszDisplayName	= szPath;
	pBI.lpszTitle		= _T("选择传真存放的路径:");
	pBI.ulFlags			= BIF_RETURNONLYFSDIRS;
	
	pPath = SHBrowseForFolder( &pBI );

	if ( pPath == NULL ) return;

	SHGetPathFromIDList( pPath, szPath );
	SHGetMalloc( &pMalloc );
	pMalloc->Free( pPath );
	pMalloc->Release();
	
	m_wndPathFax.SetWindowText( szPath );
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

void CGeneralSettingsPage::OnTestEnable() 
{
	Settings.Test.Enable = m_wndTestEnable.GetCheck();
	m_wndTestCaller.EnableWindow( Settings.Test.Enable );
	m_wndTestPhone.EnableWindow( Settings.Test.Enable );
}
