//
// DlgConnect.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "DlgConnect.h"
#include "Registry.h"
#include "Settings.h"

#include "Network.h"
#include "ChannelUser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CConnectDlg, CDialog)

BEGIN_MESSAGE_MAP(CConnectDlg, CDialog)
	//{{AFX_MSG_MAP(CConnectDlg)
	ON_CBN_SELCHANGE(IDC_USER_ID, OnSelChangeUserId)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_BINDUSER, OnBindUser)
END_MESSAGE_MAP()

#ifdef _DEBUG
#define MAX_ITEM	1
#else
#define MAX_ITEM	10
#endif

//////////////////////////////////////////////////////////////////////
// CConnectDlg construction

CConnectDlg::CConnectDlg() : CDialog( CConnectDlg::IDD, NULL )
{
	//{{AFX_DATA_INIT(CConnectDlg)
	//}}AFX_DATA_INIT
	
	m_pChannel	= NULL;
}

CConnectDlg::~CConnectDlg()
{
	CSingleLock pLock( &Network.m_pSection );
	if ( ! pLock.Lock( 250 ) ) return;
	
	m_pChannel = (CChannel*)Network.Find( m_pChannel );
	if ( m_pChannel ) m_pChannel->Disconnect();
}

void CConnectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectDlg)
	DDX_Control(pDX, IDC_USER_ID, m_wndUserId);
	DDX_Control(pDX, IDC_PASSWORD, m_wndPassword);
	DDX_Control(pDX, IDC_SAVE_PASSWORD, m_wndSaveing);
	DDX_Control(pDX, IDOK, m_wndOK);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CConnectDlg message handlers

BOOL CConnectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CString strCaption;
	strCaption.LoadString( IDS_NETWORK_CONNECT );
	SetWindowText( (LPCTSTR)strCaption );
	
	LoadHistory();
	
	m_wndUserId.SetCurSel( 0 );
	SendMessage(WM_COMMAND, MAKELONG(m_wndUserId.GetDlgCtrlID(),CBN_SELCHANGE), (LPARAM)m_hWnd);
	
	return TRUE;
}

int CConnectDlg::LoadHistory()
{
	CRegistry pRegistry;
	int nCookie = pRegistry.GetInt( _T("Connect"), _T("Cookie") );
	
	for ( int nItem = 0; nItem < MAX_ITEM; nItem++, nCookie -= nCookie > 0 ? 1 : -MAX_ITEM+1 )
	{
		CString strKey;
		strKey.Format( _T("usr%i"), UINT(nCookie) % MAX_ITEM );
		
		CString strValue = pRegistry.GetString( _T("Connect"), strKey );
		if ( strValue.IsEmpty() ) break;
		
		strValue	+= '|';
		CString str	= strValue.SpanExcluding( _T("|;,") );
		InsertItem( nItem, (LPCTSTR)str, nCookie );
	}
	
	return nItem;
}

void CConnectDlg::SaveHistory()
{
	CString xUserId;
	m_wndUserId.GetWindowText( xUserId );
	if ( xUserId.IsEmpty() ) return;
	
	CRegistry pRegistry;
	int nCookie = pRegistry.GetInt( _T("Connect"), _T("Cookie"), -1 );
	nCookie = ++nCookie % MAX_ITEM;
	
	InsertItem( 0, xUserId, nCookie );
	
	CString strKey;
	strKey.Format( _T("usr%i"), nCookie );
	
	CString strValue = xUserId;
	if ( m_wndSaveing.GetCheck() )
	{
		CString str;
		m_wndPassword.GetWindowText( str );
		strValue += '|'; strValue += str;
	}
	
	pRegistry.SetString( _T("Connect"), strKey, strValue );
	pRegistry.SetInt( _T("Connect"), _T("Cookie"), nCookie );
}

int CConnectDlg::InsertItem(int nIndex, LPCTSTR xUserId, DWORD nData)
{
	int nItem = m_wndUserId.FindString( -1, xUserId );
	if ( nItem != -1 ) RemoveItem( nItem );
	
	if ( m_wndUserId.GetCount() >= MAX_ITEM )
		RemoveItem( MAX_ITEM - 1 );
	
	nItem = m_wndUserId.InsertString( nIndex, xUserId );
	m_wndUserId.SetItemData( nItem, nData );
	
	return nItem;
}

void CConnectDlg::RemoveItem(int nItem)
{
	int nIndex = m_wndUserId.GetItemData( nItem );
	m_wndUserId.DeleteString( nItem );
	
	CString strKey;
	strKey.Format( _T("usr%i"), nIndex );
	
	CRegistry pRegistry;
	pRegistry.RemoveValue( _T("Connect"), strKey );
}

void CConnectDlg::OnSelChangeUserId()
{
	int nItem = m_wndUserId.GetCurSel();
	if ( nItem < 0 ) return;
	
	CString strKey;
	DWORD nIndex = m_wndUserId.GetItemData( nItem );
	strKey.Format( _T("usr%i"), nIndex );
	
	CRegistry pRegistry;
	CString strValue = pRegistry.GetString( _T("Connect"), strKey );
	strValue		+= '|';
	
	CString str		= strValue.SpanExcluding( _T("|;,") );
	strValue		= strValue.Mid( str.GetLength() + 1 );
	
	if ( strValue.IsEmpty() == FALSE )
	{
		CString xPwd= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( xPwd.GetLength() + 1 );
		
		m_wndPassword.SetWindowText( (LPCTSTR)xPwd );
		m_wndSaveing.SetCheck( TRUE );
	}
	else
	{
		m_wndPassword.SetWindowText( _T("") );
	}
}

void CConnectDlg::OnOK() 
{
	EnableMemvar( FALSE );
	
	CSingleLock pLock( &Network.m_pSection, TRUE );
	
	m_pChannel = (CChannel*)Network.Find( m_pChannel );
	if ( m_pChannel ) m_pChannel->Disconnect();
	
	m_pChannel = new CChannel( GetSafeHwnd() );
	m_pChannel->ConnectTo( Settings.Network.Host, Settings.Network.Port );
	
	CString strUserId;
	m_wndUserId.GetWindowText( strUserId );
	
	CString strPassword;
	m_wndPassword.GetWindowText( strPassword );
	
	m_pChannel->Connect( strUserId, strPassword );
	
	pLock.Unlock();

	PostMessage( WM_TIMER, 1 );
	SetTimer( 1, 500, NULL );
}

void CConnectDlg::OnCancel()
{
	KillTimer( 1 );
	
	CDialog::OnCancel();
}

void CConnectDlg::EnableMemvar(BOOL bEnable)
{
	m_wndOK.EnableWindow( bEnable );
	m_wndUserId.EnableWindow( bEnable );
	m_wndPassword.EnableWindow( bEnable );
	m_wndSaveing.EnableWindow( bEnable );
}

/////////////////////////////////////////////////////////////////////////////
// CConnectDlg custom message handlers

LONG CConnectDlg::OnBindUser(BOOL bConnected, INT nError)
{
	if ( bConnected == TRUE )
	{
		OnConnected();
	}
	else
	{
		OnFailed( nError );
	}
	
	return 0;
}

void CConnectDlg::OnConnected()
{
	CSingleLock pLock( &Network.m_pSection, TRUE );
	UserInterface = new CUserChannel;
	UserInterface->Clone( m_pChannel );

	SaveHistory();
	CDialog::EndDialog( IDOK );
}

void CConnectDlg::OnFailed(int nErrCode)
{
	CString str;
	
	if ( nErrCode == ICP_NOUSER )
		str.Format( IDS_NETWORK_CONNECT_NOUSER, nErrCode );
	else
		str.Format( IDS_NETWORK_CONNECT_ERROR, nErrCode );
	
	AfxMessageBox( str );
	EnableMemvar();
}
