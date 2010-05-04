//
// PageSettingsPlay.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "PageSettingsPlay.h"
#include "Registry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CPlaySettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CPlaySettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CPlaySettingsPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlaySettingsPage property page

CPlaySettingsPage::CPlaySettingsPage() : CSettingsPage( CPlaySettingsPage::IDD )
{
	//{{AFX_DATA_INIT(CPlaySettingsPage)
	//}}AFX_DATA_INIT
}

CPlaySettingsPage::~CPlaySettingsPage()
{
}

void CPlaySettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlaySettingsPage)
	DDX_Control(pDX, IDC_LOCK, m_wndLock);
	DDX_Control(pDX, IDC_NAV_OFFSET, m_wndNavOffset);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CPlaySettingsPage message handlers

BOOL CPlaySettingsPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();

	m_wndLock.SetCurSel( 0 );
	LoadSettings();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPlaySettingsPage::OnOK()
{
	EditSettings();

	CSettingsPage::OnOK();
}

BOOL CPlaySettingsPage::LoadSettings()
{
	CString strValue;
	CRegistry pRegistry;
	
	strValue = pRegistry.GetString( "Notify", "LockCh" );
	if ( ! strValue.IsEmpty() )
	{
		int nItem = m_wndLock.AddString( strValue );
		m_wndLock.SetCurSel( nItem );
	}
	
	m_wndNavOffset.SetWindowText( pRegistry.GetString( "Notify", "NavOffset" ) );
	
	return TRUE;
}

BOOL CPlaySettingsPage::EditSettings() 
{
	CString strValue;
	CRegistry pRegistry;
	
	m_wndLock.GetWindowText( strValue );
	int nItem = m_wndLock.FindStringExact( -1, strValue );
	
	if ( ! nItem ) strValue.Empty();
//	LockJob( strValue,  );
	
	pRegistry.SetString( "Notify", "LockCh", strValue );

	m_wndNavOffset.GetWindowText( strValue );
	pRegistry.SetString( "Notify", "NavOffset", strValue );

	return TRUE;
}

BOOL CPlaySettingsPage::LockJob(LPCTSTR pszText)
{
	CString strValue = pszText;
	
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		str.TrimLeft(); str.TrimRight();
		if ( str.IsEmpty() ) continue;
		
		LockWith( _ttoi(str) );
	}
	
	return TRUE;
}

BOOL CPlaySettingsPage::LockWith(int nChan)
{
//	CChannel* pChannel = Network.FindChannel( nChan );
//	if ( ! pChannel ) return FALSE;
	
	return TRUE;
}
