//
// PageSettingsRecord.cpp
//

#include "stdafx.h"
#include "Xmaster.h"
#include "Settings.h"

#include "PageSettingsRecord.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CRecordSettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CRecordSettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CRecordSettingsPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecordSettingsPage property page

CRecordSettingsPage::CRecordSettingsPage() : CSettingsPage( CRecordSettingsPage::IDD )
{
	//{{AFX_DATA_INIT(CRecordSettingsPage)
	//}}AFX_DATA_INIT
}

CRecordSettingsPage::~CRecordSettingsPage()
{
}

void CRecordSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRecordSettingsPage)
	DDX_Control(pDX, IDC_RECORD, m_wndRecord);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CRecordSettingsPage message handlers

BOOL CRecordSettingsPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();

	LoadSettings();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRecordSettingsPage::OnOK()
{
	EditSettings();

	CSettingsPage::OnOK();
}

BOOL CRecordSettingsPage::LoadSettings()
{
	m_wndRecord.SetCheck( Settings.Record.Enable == TRUE );
	SendMessage(WM_COMMAND, MAKELONG(m_wndRecord.GetDlgCtrlID(),BN_CLICKED), (LPARAM)m_hWnd);

	return TRUE;
}

BOOL CRecordSettingsPage::EditSettings() 
{
	Settings.Record.Enable = m_wndRecord.GetCheck();

	return TRUE;
}
