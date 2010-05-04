//
// PageSettingsMaster.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Settings.h"

#include "PageSettingsMaster.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CMasterSettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CMasterSettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CMasterSettingsPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMasterSettingsPage property page

CMasterSettingsPage::CMasterSettingsPage() : CSettingsPage( CMasterSettingsPage::IDD )
{
	//{{AFX_DATA_INIT(CMasterSettingsPage)
	//}}AFX_DATA_INIT
}

CMasterSettingsPage::~CMasterSettingsPage()
{
}

void CMasterSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMasterSettingsPage)
	DDX_Control(pDX, IDC_HOST, m_wndHost);
	DDX_Control(pDX, IDC_PORT, m_wndPort);
	DDX_Control(pDX, IDC_TIMEOUT, m_wndTimeOut);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CMasterSettingsPage message handlers

BOOL CMasterSettingsPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();

	LoadSettings();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMasterSettingsPage::OnOK()
{
	EditSettings();

	CSettingsPage::OnOK();
}

BOOL CMasterSettingsPage::LoadSettings()
{
	CString strValue;
	
	m_wndHost.SetWindowText( Settings.Master.Host );
	strValue.Format( _T("%i"), Settings.Master.Port );
	m_wndPort.SetWindowText( strValue );
	
	strValue.Format( _T("%i"), Settings.Master.TimeOut );
	m_wndTimeOut.SetWindowText( strValue );
	
	return TRUE;
}

BOOL CMasterSettingsPage::EditSettings() 
{
	CString strValue;
	
	m_wndHost.GetWindowText( Settings.Master.Host );
	m_wndPort.GetWindowText( strValue );
	Settings.Master.Port = _ttoi( strValue );
	
	m_wndTimeOut.GetWindowText( strValue );
	Settings.Master.TimeOut = _ttoi( strValue );
	
	return TRUE;
}
