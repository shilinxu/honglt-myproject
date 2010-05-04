//
// PageSettingsOptimize.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Settings.h"

#include "PageSettingsOptimize.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(COptimizeSettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(COptimizeSettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(COptimizeSettingsPage)
	ON_BN_CLICKED(IDC_POSE_CHECK, OnPoseCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COptimizeSettingsPage property page

COptimizeSettingsPage::COptimizeSettingsPage() : CSettingsPage( COptimizeSettingsPage::IDD )
{
	//{{AFX_DATA_INIT(COptimizeSettingsPage)
	//}}AFX_DATA_INIT
}

COptimizeSettingsPage::~COptimizeSettingsPage()
{
}

void COptimizeSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptimizeSettingsPage)
	DDX_Control(pDX, IDC_POSE_CHECK, m_wndPoseChk);
	DDX_Control(pDX, IDC_POSE_NUMBER, m_wndPoseNumb);
	DDX_Control(pDX, IDC_POSE_CALLER, m_wndPoseCaller);
	DDX_Control(pDX, IDC_POSE_EXCEPT, m_wndPoseExcept);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// COptimizeSettingsPage message handlers

BOOL COptimizeSettingsPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();

	LoadSettings();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptimizeSettingsPage::OnOK()
{
	EditSettings();

	CSettingsPage::OnOK();
}

BOOL COptimizeSettingsPage::LoadSettings()
{
	CString strValue;
	
	strValue = Settings.Tricker.PhoneNum;
	m_wndPoseNumb.SetWindowText( (LPCTSTR)strValue );
	strValue = Settings.Tricker.TxCaller;
	m_wndPoseCaller.SetWindowText( (LPCTSTR)strValue );
	strValue = Settings.Tricker.ExceptNum;
	m_wndPoseExcept.SetWindowText( (LPCTSTR)strValue );
	
	m_wndPoseChk.SetCheck( Settings.Tricker.Enable == TRUE );
	SendMessage(WM_COMMAND, MAKELONG(m_wndPoseChk.GetDlgCtrlID(),BN_CLICKED), (LPARAM)m_hWnd);
	
	return TRUE;
}

BOOL COptimizeSettingsPage::EditSettings() 
{
	CString strValue;
	
	Settings.Tricker.Enable = m_wndPoseChk.GetCheck() == TRUE;
	m_wndPoseNumb.GetWindowText( strValue );
	Settings.Tricker.PhoneNum = strValue;
	m_wndPoseCaller.GetWindowText( strValue );
	Settings.Tricker.TxCaller = strValue;
	m_wndPoseExcept.GetWindowText( strValue );
	Settings.Tricker.ExceptNum = strValue;
	
	return TRUE;
}

void COptimizeSettingsPage::OnPoseCheck() 
{
	BOOL bPoseChk = m_wndPoseChk.GetCheck();
	m_wndPoseCaller.EnableWindow( bPoseChk );
	m_wndPoseNumb.EnableWindow( bPoseChk );
	m_wndPoseExcept.EnableWindow( bPoseChk );
}
