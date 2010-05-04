//
// PageSettingsMonitor.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Settings.h"

#include "PageSettingsMonitor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CMonitorSettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CMonitorSettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CMonitorSettingsPage)
	ON_CBN_EDITCHANGE(IDC_EXT_LIST, OnEditChangeExtList)
	ON_CBN_SELCHANGE(IDC_EXT_LIST, OnSelChangeExtList)
	ON_BN_CLICKED(IDC_EXT_ADD, OnExtAdd)
	ON_BN_CLICKED(IDC_EXT_REMOVE, OnExtRemove)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMonitorSettingsPage property page

CMonitorSettingsPage::CMonitorSettingsPage() : CSettingsPage( CMonitorSettingsPage::IDD )
{
	//{{AFX_DATA_INIT(CMonitorSettingsPage)
	//}}AFX_DATA_INIT
}

CMonitorSettingsPage::~CMonitorSettingsPage()
{
}

void CMonitorSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMonitorSettingsPage)
	DDX_Control(pDX, IDC_HOST, m_wndHost);
	DDX_Control(pDX, IDC_PORT, m_wndPort);
	DDX_Control(pDX, IDC_EXT_ADD, m_wndExtAdd);
	DDX_Control(pDX, IDC_EXT_REMOVE, m_wndExtRemove);
	DDX_Control(pDX, IDC_EXT_LIST, m_wndExtList);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CMonitorSettingsPage message handlers

BOOL CMonitorSettingsPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();

	LoadSettings();
	
	OnEditChangeExtList();
	OnSelChangeExtList();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMonitorSettingsPage::OnOK()
{
	EditSettings();

	CSettingsPage::OnOK();
}

BOOL CMonitorSettingsPage::LoadSettings()
{
	CString strValue;
	
	if ( Settings.Monitor.Host.IsEmpty() )
		m_wndHost.SetWindowText( _T("自动") );
	else
		m_wndHost.SetWindowText( Settings.Monitor.Host );
	strValue.Format( _T("%i"), Settings.Monitor.Port );
	m_wndPort.SetWindowText( strValue );
	
	strValue = Settings.Monitor.IpAddr;
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		str.TrimLeft(); str.TrimRight();
		if ( str.IsEmpty() ) continue;
		m_wndExtList.AddString( (LPCTSTR)str );
	}
	
	return TRUE;
}

BOOL CMonitorSettingsPage::EditSettings() 
{
	CString strValue;
	
	m_wndHost.GetWindowText( strValue );
	if ( strValue.CompareNoCase( _T("自动") ) == 0 )
		Settings.Monitor.Host.Empty();
	else
		Settings.Monitor.Host = strValue;
	m_wndPort.GetWindowText( strValue );
	Settings.Monitor.Port = _ttoi( strValue );
	
	CString str;
	strValue.Empty();
	for ( int nItem = 0; nItem < m_wndExtList.GetCount(); nItem++ )
	{
		m_wndExtList.GetLBText( nItem, str );
		str.TrimLeft(); str.TrimRight();
		if ( str.IsEmpty() ) continue;
		strValue += str + '|';
	}
	
	Settings.Monitor.IpAddr = strValue;
	
	return TRUE;
}

void CMonitorSettingsPage::OnEditChangeExtList() 
{
	m_wndExtAdd.EnableWindow( m_wndExtList.GetWindowTextLength() > 0 );
}

void CMonitorSettingsPage::OnSelChangeExtList() 
{
	m_wndExtRemove.EnableWindow( m_wndExtList.GetCurSel() >= 0 );
}

void CMonitorSettingsPage::OnExtAdd() 
{
	CString strExist;
	m_wndExtList.GetWindowText( strExist );

	strExist.TrimLeft(); strExist.TrimRight();
	if ( strExist.IsEmpty() ) return;

	if ( m_wndExtList.FindString( -1, strExist ) >= 0 ) return;

	m_wndExtList.AddString( strExist );
	m_wndExtList.SetWindowText( _T("") );
}

void CMonitorSettingsPage::OnExtRemove() 
{
	int nItem = m_wndExtList.GetCurSel();
	if ( nItem >= 0 ) m_wndExtList.DeleteString( nItem );
	m_wndExtRemove.EnableWindow( FALSE );
}
