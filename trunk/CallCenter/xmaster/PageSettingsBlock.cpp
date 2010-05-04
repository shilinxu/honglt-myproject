//
// PageSettingsBlock.cpp
//

#include "stdafx.h"
#include "Xmaster.h"
#include "Settings.h"

#include "PageSettingsBlock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CBlockSettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CBlockSettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CBlockSettingsPage)
	ON_BN_CLICKED(IDC_GIVEN_ENABLE, OnGivenEnable)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBlockSettingsPage property page

CBlockSettingsPage::CBlockSettingsPage() : CSettingsPage( CBlockSettingsPage::IDD )
{
	//{{AFX_DATA_INIT(CBlockSettingsPage)
	//}}AFX_DATA_INIT
}

CBlockSettingsPage::~CBlockSettingsPage()
{
}

void CBlockSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBlockSettingsPage)
	DDX_Control(pDX, IDC_USER_CYCLE, m_wndUsrCycle);
	DDX_Control(pDX, IDC_USER_TIME, m_wndUsrTime);
	DDX_Control(pDX, IDC_USER_DATE, m_wndUsrDate);
	DDX_Control(pDX, IDC_GIVEN_ENABLE, m_wndGiven);
	DDX_Control(pDX, IDC_GIVEN_LOCALID, m_wndGivenId);
	DDX_Control(pDX, IDC_GIVEN_LENGTH, m_wndGivenLen);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CBlockSettingsPage message handlers

BOOL CBlockSettingsPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();

	LoadSettings();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBlockSettingsPage::OnOK()
{
	EditSettings();

	CSettingsPage::OnOK();
}

BOOL CBlockSettingsPage::LoadSettings()
{
	CString strValue;
	
	m_wndUsrTime.SetWindowText( Settings.Block.UsrTime );
	strValue.Format( _T("%i"), Settings.Block.UsrDate );
	m_wndUsrDate.SetWindowText( strValue );
	strValue.Format( _T("%i"), Settings.Block.UsrCycle );
	m_wndUsrCycle.SetWindowText( strValue );

	m_wndGiven.SetCheck( Settings.GivenTry.Enable == TRUE );
	m_wndGivenId.SetWindowText( Settings.GivenTry.LocalId );
	strValue.Format( _T("%i"), Settings.GivenTry.MaxLength );
	m_wndGivenLen.SetWindowText( strValue );
	SendMessage(WM_COMMAND, MAKELONG(m_wndGiven.GetDlgCtrlID(),BN_CLICKED), (LPARAM)m_hWnd);

	return TRUE;
}

BOOL CBlockSettingsPage::EditSettings() 
{
	CString strValue;
		
	m_wndUsrTime.GetWindowText( strValue );
	Settings.Block.UsrTime = strValue;
	m_wndUsrDate.GetWindowText( strValue );
	Settings.Block.UsrDate = _ttoi( strValue );
	m_wndUsrCycle.GetWindowText( strValue );
	Settings.Block.UsrCycle = _ttoi( strValue );
	
	Settings.GivenTry.Enable = m_wndGiven.GetCheck();
	m_wndGivenId.GetWindowText( strValue );
	Settings.GivenTry.LocalId = strValue;
	m_wndGivenLen.GetWindowText( strValue );
	Settings.GivenTry.MaxLength = _ttoi( strValue );

	return TRUE;
}

void CBlockSettingsPage::OnGivenEnable() 
{
	Settings.GivenTry.Enable = m_wndGiven.GetCheck();
	m_wndGivenId.EnableWindow( Settings.GivenTry.Enable );
	m_wndGivenLen.EnableWindow( Settings.GivenTry.Enable );
}
