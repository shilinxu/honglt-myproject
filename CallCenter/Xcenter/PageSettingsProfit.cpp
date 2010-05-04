//
// PageSettingsProfit.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Settings.h"

#include "PageSettingsProfit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CProfitSettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CProfitSettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CProfitSettingsPage)
	ON_BN_CLICKED(IDC_PLAY_NUMB, OnPlayNumb)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CProfitSettingsPage property page

CProfitSettingsPage::CProfitSettingsPage() : CSettingsPage( CProfitSettingsPage::IDD )
{
	//{{AFX_DATA_INIT(CProfitSettingsPage)
	//}}AFX_DATA_INIT
}

CProfitSettingsPage::~CProfitSettingsPage()
{
}

void CProfitSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProfitSettingsPage)
	DDX_Control(pDX, IDC_SEEK_NUMB, m_wndSeekNum);
	DDX_Control(pDX, IDC_PLAY_NUMB, m_wndPlyNumb);
	DDX_Control(pDX, IDC_GIVEN_LENG, m_wndMinGivLeng);
	DDX_Control(pDX, IDC_GIVEN_TOP, m_wndMaxGivLong);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CProfitSettingsPage message handlers

BOOL CProfitSettingsPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();

	LoadSettings();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CProfitSettingsPage::OnOK()
{
	EditSettings();

	CSettingsPage::OnOK();
}

BOOL CProfitSettingsPage::LoadSettings()
{
	CString strValue;
	
	m_wndPlyNumb.SetCheck( Settings.Profit.PlyNumb == TRUE );
	m_wndSeekNum.SetWindowText( (LPCTSTR)Settings.Profit.SeekNum );
	SendMessage(WM_COMMAND, MAKELONG(m_wndPlyNumb.GetDlgCtrlID(),BN_CLICKED), (LPARAM)m_hWnd);
	
	strValue.Format( "%i", Settings.Profit.MinGivSec );
	m_wndMinGivLeng.SetWindowText( (LPCTSTR)strValue );
	strValue.Format( "%i", Settings.Profit.MaxGivLong );
	m_wndMaxGivLong.SetWindowText( (LPCTSTR)strValue );
	
	return TRUE;
}

BOOL CProfitSettingsPage::EditSettings() 
{
	CString strValue;
	
	Settings.Profit.PlyNumb = m_wndPlyNumb.GetCheck() == TRUE;
	m_wndSeekNum.GetWindowText( strValue );
	Settings.Profit.SeekNum = strValue;
	
	m_wndMinGivLeng.GetWindowText( strValue );
	Settings.Profit.MinGivSec = _ttoi( strValue );
	m_wndMaxGivLong.GetWindowText( strValue );
	Settings.Profit.MaxGivLong = _ttoi( strValue );
	
	return TRUE;
}

void CProfitSettingsPage::OnPlayNumb() 
{
	Settings.Profit.PlyNumb = m_wndPlyNumb.GetCheck();
	m_wndSeekNum.EnableWindow( Settings.Profit.PlyNumb );
}
