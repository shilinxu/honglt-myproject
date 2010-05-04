//
// PageSettingsTrunk.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Settings.h"

#include "PageSettingsTrunk.h"
#include "Network.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CTrunkSettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CTrunkSettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CTrunkSettingsPage)
	ON_BN_CLICKED(IDC_INBOUND_LIMIT, OnInboundLimit)
	ON_BN_CLICKED(IDC_OUTBOUND_LIMIT, OnOutboundLimit)
	ON_BN_CLICKED(IDC_REC_FILE_DEFAULT, OnRecFileDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTrunkSettingsPage property page

CTrunkSettingsPage::CTrunkSettingsPage() : CSettingsPage( CTrunkSettingsPage::IDD )
{
	//{{AFX_DATA_INIT(CTrunkSettingsPage)
	//}}AFX_DATA_INIT
}

CTrunkSettingsPage::~CTrunkSettingsPage()
{
}

void CTrunkSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrunkSettingsPage)
	DDX_Control(pDX, IDC_TIMEOUT, m_wndTimeOut);
	DDX_Control(pDX, IDC_INBOUND_LIMIT, m_wndInLimit);
	DDX_Control(pDX, IDC_INBOUND_LIMITED, m_wndInLimited);
	DDX_Control(pDX, IDC_LOOKOUT, m_wndLookOut);
	DDX_Control(pDX, IDC_OUTBOUND_PHONE, m_wndOutPhone);
	DDX_Control(pDX, IDC_OUTBOUND_RESERVED, m_wndOutReserved);
	DDX_Control(pDX, IDC_OUTBOUND_LIMIT, m_wndOutLimit);
	DDX_Control(pDX, IDC_OUTBOUND_LIMITED, m_wndOutLimited);
	DDX_Control(pDX, IDC_REC_FILE, m_wndRecFile);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CTrunkSettingsPage message handlers

BOOL CTrunkSettingsPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();

	LoadSettings();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTrunkSettingsPage::OnOK()
{
	EditSettings();

	CSettingsPage::OnOK();
}

BOOL CTrunkSettingsPage::LoadSettings()
{
	CString strValue;
	
	strValue.Format( "%i", Settings.Trunk.TimeOut );
	m_wndTimeOut.SetWindowText( (LPCTSTR)strValue );
	
	m_wndInLimit.SetCheck( Settings.Trunk.InLimit == TRUE );
	m_wndInLimited.SetWindowText( Settings.Trunk.InLimited );
	SendMessage(WM_COMMAND, MAKELONG(m_wndInLimit.GetDlgCtrlID(),BN_CLICKED), (LPARAM)m_hWnd);
	
	m_wndLookOut.SetWindowText( Settings.Trunk.LookOut );
	m_wndOutPhone.SetWindowText( Settings.Trunk.OutPhone );
	m_wndOutLimit.SetCheck( Settings.Trunk.OutLimit );
	strValue.Format( "%i", Settings.Trunk.OutLimited );
	m_wndOutLimited.SetWindowText( (LPCTSTR)strValue );
	SendMessage(WM_COMMAND, MAKELONG(m_wndOutLimit.GetDlgCtrlID(),BN_CLICKED), (LPARAM)m_hWnd);
	m_wndOutReserved.SetWindowText( Settings.Trunk.OutReserved );
	
	strValue = Settings.Trunk.RecFile;
	m_wndRecFile.SetWindowText( (LPCTSTR)strValue );
	
	return TRUE;
}

BOOL CTrunkSettingsPage::EditSettings() 
{
	CString strValue;
	
	m_wndTimeOut.GetWindowText( strValue );
	Settings.Trunk.TimeOut = _ttoi( strValue );
	
	Settings.Trunk.InLimit = m_wndInLimit.GetCheck();
	m_wndInLimited.GetWindowText( Settings.Trunk.InLimited );
	
	m_wndLookOut.GetWindowText( Settings.Trunk.LookOut );
	m_wndOutPhone.GetWindowText( Settings.Trunk.OutPhone );
	Settings.Trunk.OutLimit = m_wndOutLimit.GetCheck();
	m_wndOutLimited.GetWindowText( strValue );
	Settings.Trunk.OutLimited = _ttoi( strValue );
	m_wndOutReserved.GetWindowText( Settings.Trunk.OutReserved );
	
	m_wndRecFile.GetWindowText( strValue );
	Settings.Trunk.RecFile = strValue;
	
	return TRUE;
}

void CTrunkSettingsPage::OnInboundLimit() 
{
	BOOL bCheck = m_wndInLimit.GetCheck();
	m_wndInLimited.EnableWindow( bCheck );
}

void CTrunkSettingsPage::OnOutboundLimit() 
{
	BOOL bCheck = m_wndOutLimit.GetCheck();
	m_wndOutLimited.EnableWindow( bCheck );
}

void CTrunkSettingsPage::OnRecFileDefault() 
{
	LPCTSTR xRule = _T("<CHAN>\\<REMOTE>-<DATE><TIME>.pcm");
	
	m_wndRecFile.SetWindowText( xRule );
}
