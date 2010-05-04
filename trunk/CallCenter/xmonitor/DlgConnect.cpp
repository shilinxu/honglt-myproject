//
// DlgConnect.cpp
//

#include "stdafx.h"
#include "Xmonitor.h"
#include "DlgConnect.h"

#include "Settings.h"
#include "Registry.h"
#include "XML.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CConnectDlg, CDialog)
BEGIN_MESSAGE_MAP(CConnectDlg, CDialog)
	//{{AFX_MSG_MAP(CConnectDlg)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_USER_ID, OnSelChangeUserId)
	ON_BN_CLICKED(IDC_SETTINGS, OnSettings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// CConnectDlg construction

CConnectDlg::CConnectDlg() : CDialog( CConnectDlg::IDD, NULL )
{
	//{{AFX_DATA_INIT(CConnectDlg)
	//}}AFX_DATA_INIT
	
	m_pXML	= NULL;
}

CConnectDlg::~CConnectDlg()
{
	if ( m_pXML ) delete m_pXML;
}

void CConnectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectDlg)
	DDX_Control(pDX, IDC_SETTINGS, m_wndSettings);
	DDX_Control(pDX, IDC_DATABASE, m_wndDatabase);
	DDX_Control(pDX, IDC_HOST, m_wndHost);
	DDX_Control(pDX, IDC_REMOTE, m_wndRemote);
	DDX_Control(pDX, IDC_USER_ID, m_wndUserId);
	DDX_Control(pDX, IDC_PASSWORD, m_wndPassword);
	DDX_Control(pDX, IDC_SAVE_PASSWORD, m_wndSaveing);
	DDX_Control(pDX, IDOK, m_wndOK);
	//}}AFX_DATA_MAP
}

int CConnectDlg::DoModal()
{
	this->LoadXML();
	
	if ( ! theApp.m_pMainWnd )
	theApp.m_pMainWnd	= this;
	
	return CDialog::DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CConnectDlg XML

BOOL CConnectDlg::LoadXML()
{
	m_pXML = CXMLElement::FromFile( _T("CookieAdmin.xml"), TRUE );
	
	if ( ! m_pXML ) m_pXML = CreateXML();
	return TRUE;
}

CXMLElement* CConnectDlg::CreateXML()
{
	WORD nVersion[4];
	theApp.GetVersionNumber( nVersion );
	
	CString strVersion;
	strVersion.Format( _T("%i.%i.%i.%i"),
		nVersion[0], nVersion[1], nVersion[2], nVersion[3] );
	
	CXMLElement* pXML = new CXMLElement( NULL, _T("CallCenter") );
	pXML->AddAttribute( _T("Author"), _T("Honglt") );
	pXML->AddAttribute( _T("Version"), strVersion );
	
	return pXML;
}

BOOL CConnectDlg::SaveXML()
{
	CFile pFile;
	if ( ! pFile.Open( _T("CookieAdmin.xml"), CFile::modeWrite|CFile::modeCreate ) ) return FALSE;
	
	CString strXML;
	strXML = m_pXML->ToString( TRUE, TRUE );
	
	pFile.Write( (LPCSTR)strXML, strXML.GetLength() );
	
	pFile.Close();
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CConnectDlg message handlers

BOOL CConnectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
/*	
	CString strCaption;
	strCaption.LoadString( IDS_NETWORK_CONNECT );
	SetWindowText( (LPCTSTR)strCaption );
*/	
	LoadXML( m_pXML->GetElementByName( _T("History"), TRUE ) );
	
	m_wndUserId.SetCurSel( 0 );
	SendMessage(WM_COMMAND, MAKELONG(m_wndUserId.GetDlgCtrlID(),CBN_SELCHANGE), (LPARAM)m_hWnd);

	CRegistry pRegistry;
	CString strValue = pRegistry.GetString( "Database", "Host", "." );
	m_wndHost.SetWindowText( (LPCTSTR)strValue );
	
	strValue = pRegistry.GetString( "Database", "Database", "xcallcenter" );
	m_wndDatabase.SetWindowText( (LPCTSTR)strValue );
	
	CenterWindow();
	ShowWindow( SW_SHOWNORMAL );
	ShowSettings( FALSE );
	
	return TRUE;
}


void CConnectDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	if ( theApp.m_pMainWnd == this )
	theApp.m_pMainWnd	= NULL;
}

int CConnectDlg::LoadXML(CXMLElement* pXML)
{
	int nCount = 0;
	ASSERT( pXML != NULL );
	
	for ( POSITION pos = pXML->GetElementIterator() ; pos ; nCount++ )
	{
		CXMLElement* pMap = pXML->GetNextElement( pos );
		if ( ! pMap->IsNamed( _T("Cookie") ) ) continue;
		
		if ( ! pMap->GetAttribute( _T("UserId") ) ) continue;
		
		InsertItem( 0, pMap );
	}
	
	return nCount;
}

int CConnectDlg::InsertItem(int nIndex, CXMLElement* pXML)
{
	ASSERT( pXML != NULL );
	CString strValue = pXML->GetAttributeValue( _T("UserId") );
	
	int nItem = m_wndUserId.FindString( -1, strValue );
	if ( nItem != -1 ) RemoveItem( nItem );
	
	nItem = m_wndUserId.InsertString( nIndex, strValue );
	m_wndUserId.SetItemData( nItem, (DWORD)pXML );
	
	m_pXML->GetElementByName( _T("History"), TRUE )	->AddElement( pXML );
	return nItem;
}

void CConnectDlg::RemoveItem(int nItem)
{
	CXMLElement* pXML = (CXMLElement*)m_wndUserId.GetItemData( nItem );
	ASSERT( pXML != NULL ); pXML->Delete();
	
	m_wndUserId.DeleteString( nItem );
}

void CConnectDlg::OnSelChangeUserId()
{
	int nItem = m_wndUserId.GetCurSel();
	if ( nItem < 0 ) return;
	
	CXMLElement* pXML = (CXMLElement*)m_wndUserId.GetItemData( nItem );
	ASSERT( pXML != NULL );
	
	if ( CXMLAttribute* pAttr = pXML->GetAttribute( _T("Password") ) )
	{
		m_wndPassword.SetWindowText( pAttr->GetValue() );
		m_wndSaveing.SetCheck( TRUE );
	}
	else
	{
		m_wndPassword.SetWindowText( _T("") );
		m_wndSaveing.SetCheck( FALSE );
	}
}

void CConnectDlg::SaveHistory()
{
	CXMLElement* pXML;
	
	CString strUserId;
	m_wndUserId.GetWindowText( strUserId );
	
	CString strPassword;
	m_wndPassword.GetWindowText( strPassword );
	
	int nItem = m_wndUserId.FindString( -1, strUserId );
	if ( nItem == -1 )
	{
		pXML = new CXMLElement( NULL, _T("Cookie") );
		pXML->AddAttribute( _T("UserId"), strUserId );
		
		if ( m_wndSaveing.GetCheck() )
		{
			CString strValue;
			m_wndPassword.GetWindowText( strValue );
			pXML->AddAttribute( _T("Password"), strValue );
		}
	}
	else
	{
		pXML = (CXMLElement*)m_wndUserId.GetItemData( nItem );
		pXML = pXML->Clone();
		
		if ( m_wndSaveing.GetCheck() )
		{
			CXMLAttribute* pAttr = pXML->GetAttribute( _T("Password") );
			if ( ! pAttr ) pAttr = pXML->AddAttribute( _T("Password") );
			
			CString strValue;
			m_wndPassword.GetWindowText( strValue );
			pAttr->SetValue( strValue );
		}
		else
		{
			pXML->DeleteAttribute( _T("Password") );
		}
	}
	
	InsertItem( 0, pXML );
	SaveXML();
}

void CConnectDlg::EnableMemvar(BOOL bEnable)
{
	m_wndOK.EnableWindow( bEnable );
	m_wndUserId.EnableWindow( bEnable );
	m_wndPassword.EnableWindow( bEnable );
	m_wndSaveing.EnableWindow( bEnable );
	
	m_wndSettings.ShowWindow( bEnable ? SW_SHOW : SW_HIDE );
}

void CConnectDlg::OnOK() 
{
	CString strUserId;
	m_wndUserId.GetWindowText( strUserId );
	
	strUserId.TrimLeft(); strUserId.TrimRight();
	if ( strUserId.IsEmpty() ) return;
	
	CString strPassword;
	m_wndPassword.GetWindowText( strPassword );
	
	EnableMemvar( FALSE );
	
	CString strValue;
	
	m_wndHost.GetWindowText( strValue );
	Settings.Database.Host = strValue;
	
	m_wndDatabase.GetWindowText( strValue );
	Settings.Database.Database = strValue;
	
	ShowSettings( FALSE );
	
	CWaitCursor pCursor;
	if ( ! theApp.OpenDatabase() )
	{
		EnableMemvar(); return;
	}
	
	if ( ! ChkUserState(strUserId, strPassword) )
	{
		CString strText;
		strText.LoadString( IDS_ERROR_NOTUSER );
			
		AfxMessageBox( (LPCTSTR)strText );
		EnableMemvar(); return;
	}
	else
	{
		SaveHistory();
		SaveSettings();
			
		CDialog::EndDialog( IDOK );
	}
}

void CConnectDlg::OnSettings() 
{
	ShowSettings( ! m_wndRemote.IsWindowVisible() );
}

void CConnectDlg::ShowSettings(BOOL bShow)
{
	if ( m_wndRemote.IsWindowVisible() == bShow ) return;
	
	CRect rect;
	this->GetWindowRect( rect );
	
	CRect rc2;
	m_wndRemote.GetWindowRect( rc2 );
	
	SetWindowPos( NULL, rect.left, rect.top, rect.Width(), 
		rect.Height() + (rc2.Height() + 10) * (bShow ? 1 : -1), SWP_NOMOVE );

	for ( CWnd* pChild = &m_wndRemote; pChild; pChild = pChild->GetNextWindow() )
	{
		pChild->ShowWindow( bShow ? SW_SHOW : SW_HIDE );
	}
}

void CConnectDlg::SaveSettings()
{
	CString strValue;
	CRegistry pRegistry;
	
	m_wndHost.GetWindowText( strValue );
	pRegistry.SetString( "Database", "Host", strValue );
	//Settings.Database.Host = strValue;
	
	m_wndDatabase.GetWindowText( strValue );
	pRegistry.SetString( "Database", "Database", strValue );
	//Settings.Database.Database = strValue;
}

BOOL CConnectDlg::ChkUserState(LPCTSTR xUsrNumber, LPCTSTR xPassword)
{
	try
	{
		CString strQuery;
		strQuery.Format( "select * from xuser_info"
			" where xuserid = '%s'", xUsrNumber );
		TRACE( strQuery + "\n" );
		
		_RecordsetPtr xRecordset;
		xRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
		if ( xRecordset->adoEOF ) return FALSE;
		
		_variant_t TheValue = xRecordset->GetCollect( "xuserpwd" );
		if ( VT_NULL == TheValue.vt || ! _tcscmp(xPassword, (LPCTSTR)(_bstr_t)TheValue) ) return TRUE;
	}
	catch (_com_error e)
	{
		return FALSE;
	}
	
	return FALSE;
}
