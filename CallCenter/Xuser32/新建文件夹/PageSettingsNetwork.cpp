//
// PageSettingsNetwork.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
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
	DDX_Control(pDX, IDC_MAX_RATE, m_wndRate);
	DDX_Control(pDX, IDC_MAX_PACKET, m_wndPacket);
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
	
	if ( Settings.Network.Host.IsEmpty() )
		m_wndHost.SetWindowText( _T("自动") );
	else
		m_wndHost.SetWindowText( Settings.Network.Host );
	strValue.Format( _T("%i"), Settings.Network.Port );
	m_wndPort.SetWindowText( strValue );
	
	strValue.Format( "%.2lf frm/s", Settings.Network.MaxRate );
	m_wndRate.SetWindowText( strValue );
	
	strValue.Format( "%i", Settings.Network.MaxPacket );
	m_wndPacket.SetWindowText( strValue );

	return TRUE;
}

BOOL CNetworkSettingsPage::EditSettings() 
{
	CString strValue;
	
	m_wndHost.GetWindowText( strValue );
	if ( strValue.CompareNoCase( _T("自动") ) == 0 )
		Settings.Network.Host.Empty();
	else
		Settings.Network.Host = strValue;
	m_wndPort.GetWindowText( strValue );
	Settings.Network.Port = _ttoi( strValue );
	
	m_wndRate.GetWindowText( strValue );
	_stscanf( (LPCTSTR)strValue, _T("%lf"), &Settings.Network.MaxRate );
	m_wndPacket.GetWindowText( strValue );
	Settings.Network.MaxPacket = _ttoi( strValue );
	
	return TRUE;
}
