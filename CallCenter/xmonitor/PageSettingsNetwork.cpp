//
// PageSettingsNetwork.cpp
//

#include "stdafx.h"
#include "Xmonitor.h"
#include "Settings.h"

#include "PageSettingsNetwork.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CNetworkSettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CNetworkSettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CNetworkSettingsPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CNetworkSettingsPage property page

CNetworkSettingsPage::CNetworkSettingsPage() : CSettingsPage( CNetworkSettingsPage::IDD )
{
	//{{AFX_DATA_INIT(CNetworkSettingsPage)
	//}}AFX_DATA_INIT
}

CNetworkSettingsPage::~CNetworkSettingsPage()
{
}

void CNetworkSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNetworkSettingsPage)
	DDX_Control(pDX, IDC_HOST, m_wndHost);
	DDX_Control(pDX, IDC_PORT, m_wndPort);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkSettingsPage message handlers

BOOL CNetworkSettingsPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();

	LoadSettings();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNetworkSettingsPage::OnOK()
{
	EditSettings();

	CSettingsPage::OnOK();
}

BOOL CNetworkSettingsPage::LoadSettings()
{
	CString strValue;
	
	m_wndHost.SetWindowText( Settings.Network.Host );
	strValue.Format( _T("%i"), Settings.Network.Port );
	m_wndPort.SetWindowText( strValue );
	
	return TRUE;
}

BOOL CNetworkSettingsPage::EditSettings() 
{
	CString strValue;
	
	m_wndHost.GetWindowText( strValue );
	Settings.Network.Host = strValue;
	m_wndPort.GetWindowText( strValue );
	Settings.Network.Port = _ttoi( strValue );
	
	return TRUE;
}
