//
// PageSettingsSubSm.cpp
//

#include "stdafx.h"
#include "Xmaster.h"
#include "Settings.h"

#include "PageSettingsSubSm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CSubSmSettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CSubSmSettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CSubSmSettingsPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSubSmSettingsPage property page

CSubSmSettingsPage::CSubSmSettingsPage() : CSettingsPage( CSubSmSettingsPage::IDD )
{
	//{{AFX_DATA_INIT(CSubSmSettingsPage)
	//}}AFX_DATA_INIT
}

CSubSmSettingsPage::~CSubSmSettingsPage()
{
}

void CSubSmSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSubSmSettingsPage)
	DDX_Control(pDX, IDC_USER_SUBID, m_wndSubId);
	DDX_Control(pDX, IDC_USER_TIME, m_wndUsrTime);
	DDX_Control(pDX, IDC_USER_DATE, m_wndUsrDate);
	DDX_Control(pDX, IDC_USER_REBACK, m_wndReBack);
	DDX_Control(pDX, IDC_USER_COOKIE, m_wndCookie);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CSubSmSettingsPage message handlers

BOOL CSubSmSettingsPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();

	LoadSettings();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSubSmSettingsPage::OnOK()
{
	EditSettings();

	CSettingsPage::OnOK();
}

BOOL CSubSmSettingsPage::LoadSettings()
{
	CString strValue;
	
	m_wndUsrTime.SetWindowText( Settings.SubSm.UsrTime );
	strValue.Format( _T("%i"), Settings.SubSm.UsrDate );
	m_wndUsrDate.SetWindowText( strValue );

	m_wndSubId.SetWindowText( Settings.SubSm.UsrRule );
	m_wndReBack.SetCheck( Settings.SubSm.ReBack == TRUE );
	
	strValue.Format( _T("%i"), Settings.SubSm.Cookie );
	m_wndCookie.SetWindowText( strValue );

	return TRUE;
}

BOOL CSubSmSettingsPage::EditSettings() 
{
	CString strValue;
		
	m_wndUsrTime.GetWindowText( strValue );
	Settings.SubSm.UsrTime = strValue;
	m_wndUsrDate.GetWindowText( strValue );
	Settings.SubSm.UsrDate = _ttoi( strValue );
	
	m_wndSubId.GetWindowText( strValue );
	Settings.SubSm.UsrRule = strValue;
	Settings.SubSm.ReBack = m_wndReBack.GetCheck();
	
	m_wndCookie.GetWindowText( strValue );
	Settings.SubSm.Cookie = _ttoi( strValue );

	return TRUE;
}
