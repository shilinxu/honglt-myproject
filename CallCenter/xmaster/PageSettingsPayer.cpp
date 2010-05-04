//
// PageSettingsPayer.cpp
//

#include "stdafx.h"
#include "Xmaster.h"
#include "Settings.h"

#include "PageSettingsPayer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CPayerSettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CPayerSettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CPayerSettingsPage)
	ON_BN_CLICKED(IDC_RED_ENABLE, OnRedEnable)
	ON_BN_CLICKED(IDC_PATH_BROWSE, OnPathBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPayerSettingsPage property page

CPayerSettingsPage::CPayerSettingsPage() : CSettingsPage( CPayerSettingsPage::IDD )
{
	//{{AFX_DATA_INIT(CPayerSettingsPage)
	//}}AFX_DATA_INIT
}

CPayerSettingsPage::~CPayerSettingsPage()
{
}

void CPayerSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPayerSettingsPage)
	DDX_Control(pDX, IDC_PAY_START, m_wndPayStart);
	DDX_Control(pDX, IDC_PAY_CLOSE, m_wndPayClose);
	DDX_Control(pDX, IDC_PAY_BALAN, m_wndPayBalan);
	DDX_Control(pDX, IDC_PATH, m_wndPath);
	DDX_Control(pDX, IDC_RED_BALAN, m_wndRedBalan);
	DDX_Control(pDX, IDC_RED_ENABLE, m_wndRedEnable);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CPayerSettingsPage message handlers

BOOL CPayerSettingsPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();

	LoadSettings();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPayerSettingsPage::OnOK()
{
	EditSettings();

	CSettingsPage::OnOK();
}

BOOL CPayerSettingsPage::LoadSettings()
{
	CString strValue;
	
	m_wndPayStart.SetWindowText( Settings.Payer.PayStart );
	m_wndPayClose.SetWindowText( Settings.Payer.PayClose );
	strValue.Format( _T("%i"), Settings.Payer.PayBalan );
	m_wndPayBalan.SetWindowText( strValue );
	m_wndPath.SetWindowText( Settings.Payer.PcmPager );

	m_wndRedEnable.SetCheck( Settings.Payer.RedEnable == TRUE );
	strValue.Format( _T("%.2f"), Settings.Payer.RedBalan );
	m_wndRedBalan.SetWindowText( strValue );
	SendMessage(WM_COMMAND, MAKELONG(m_wndRedEnable.GetDlgCtrlID(),BN_CLICKED), (LPARAM)m_hWnd);

	return TRUE;
}

BOOL CPayerSettingsPage::EditSettings() 
{
	CString strValue;
		
	m_wndPayStart.GetWindowText( strValue );
	Settings.Payer.PayStart = strValue;
	m_wndPayClose.GetWindowText( strValue );
	Settings.Payer.PayClose = strValue;
	m_wndPayBalan.GetWindowText( strValue );
	Settings.Payer.PayBalan = _ttoi( strValue );
	m_wndPath.GetWindowText( strValue );
	Settings.Payer.PcmPager = strValue;
	
	Settings.Payer.RedEnable = m_wndRedEnable.GetCheck();
	m_wndRedBalan.GetWindowText( strValue );
	_stscanf( strValue, _T("%lf"), &Settings.Payer.RedBalan );

	return TRUE;
}

void CPayerSettingsPage::OnPathBrowse() 
{
	CFileDialog dlg( TRUE, _T("pcm"), NULL , OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,
		_T("Audio Files|*.pcm|All Files|*.*||"), this );
	
	if ( dlg.DoModal() != IDOK ) return;
	
	m_wndPath.SetWindowText( dlg.GetPathName() );
}

void CPayerSettingsPage::OnRedEnable() 
{
	Settings.Payer.RedEnable = m_wndRedEnable.GetCheck();
	m_wndRedBalan.EnableWindow( Settings.Payer.RedEnable );
}
