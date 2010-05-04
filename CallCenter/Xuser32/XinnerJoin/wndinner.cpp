//
// WndInner.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "WndInner.h"
#include "CoolInterface.h"
#include "Network.h"
#include "skin.h"
#include "XML.h"

#include "CtrlNetworkPanel.h"
#include "DlgUserMeeting.h"
#include "Datagrams.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MIN_GROUP		4001
#define MAX_GROUP		4999

IMPLEMENT_SERIAL(CInnerWnd, CChildWnd, 0)

BEGIN_MESSAGE_MAP(CInnerWnd, CChildWnd)
	//{{AFX_MSG_MAP(CInnerWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_MDIACTIVATE()
	ON_BN_CLICKED(AFX_IDW_TOOLBAR, OnToolbarReturn)
	ON_BN_DOUBLECLICKED(AFX_IDW_TOOLBAR, OnToolbarEscape)
	ON_UPDATE_COMMAND_UI(ID_NETWORK_CONNECT, OnUpdateNetworkConnect)
	ON_COMMAND(ID_NETWORK_CONNECT, OnNetworkConnect)
	ON_COMMAND(ID_INNER_REFRESH, OnInnerRefresh)
	ON_COMMAND(ID_INNER_REPLY, OnInnerReply)
	ON_COMMAND_RANGE(ID_DTMF_ONE, ID_DTMF_WELL, OnDtmfKeydown)
//	ON_UPDATE_COMMAND_UI_RANGE(ID_DTMF_ONE, ID_DTMF_WELL, OnUpdateDtmfKeydown)
	ON_UPDATE_COMMAND_UI_RANGE(MIN_GROUP, MAX_GROUP, OnUpdateUserGroup)
	ON_COMMAND(ID_SEARCH_GROUP, OnSearchGroup)
	ON_COMMAND(ID_USER_MEETING, OnUserMeeting)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SYNCNOTIFY, OnSyncNotify)
END_MESSAGE_MAP()

#define SIZE_INTERNAL	1982
#define PANEL_WIDTH		200
#define BAR_HEIGHT		28

#define MAX_PHONE		21
#define MAX_TIMEOUT		(30* 1000)

#define DTMF_ToneTime	(0.1)
#define DTMF_BreakTime	(0.04)

//////////////////////////////////////////////////////////////////////
// CInnerWnd construction

CInnerWnd::CInnerWnd() : CChildWnd(FALSE)
{
	m_pWindow	= NULL;
	
	Create( IDR_LINERFRAME );
}

CInnerWnd::~CInnerWnd()
{
	ASSERT( m_pWindow == NULL );
}

/////////////////////////////////////////////////////////////////////////////
// CInnerWnd message handlers

int CInnerWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CChildWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	// Load Definitions
	
	Skin.LoadFromResource( AfxGetResourceHandle(), IDR_XML_DEFINITIONS );
	Skin.LoadFromResource( AfxGetResourceHandle(), IDR_XML_DEFAULT );
	
	// Panel
	
	//m_wndPanel.Create( WS_VISIBLE, rectDefault, this );
	
	// Header
	
	if ( ! m_wndHeaderBar.Create( this, WS_CHILD|WS_VISIBLE|CBRS_NOALIGN, AFX_IDW_TOOLBAR ) ) return -1;
	m_wndHeaderBar.SetBarStyle( m_wndHeaderBar.GetBarStyle() | CBRS_TOOLTIPS|CBRS_BORDER_BOTTOM );
	Skin.CreateToolBar( _T("CInnerWnd.Header"), &m_wndHeaderBar );
//	m_wndHeaderBar.SetOwner( this );
	
	CString strText;
	strText.LoadString( IDS_WELCOME );
	m_wndHeaderBar.SetCaption( (LPCTSTR)strText );
	m_wndHeaderBar.SetIcon( CoolInterface.m_pImages.ExtractIcon( 22 ) );
	
	m_wndDtmfBar.CreateEx( &m_wndHeaderBar, TBSTYLE_FLAT, WS_CHILD|CBRS_ALIGN_TOP );
	m_wndDtmfBar.LoadToolBar( IDR_DTMFBAR );
	m_wndDtmfBar.SetOwner( this );

	if ( ! m_wndPhone.Create( WS_CHILD|ES_AUTOHSCROLL|ES_NUMBER, rectDefault, &m_wndHeaderBar, IDC_USER_PHONE ) ) return -1;
	m_wndPhone.SetFont( &CoolInterface.m_fntBold );
	
	// List
	
	m_wndList.Create( WS_CHILD|WS_VISIBLE, rectDefault, this, IDC_INNER_LIST );
	
	// Bottom
	
	if ( ! m_wndBottom.Create( this, WS_CHILD|WS_VISIBLE|CBRS_NOALIGN, AFX_IDW_TOOLBAR ) ) return -1;
	m_wndBottom.SetBarStyle( m_wndBottom.GetBarStyle() | CBRS_TOOLTIPS|CBRS_BORDER_TOP );
	Skin.CreateToolBar( _T("CInnerWnd.Bottom"), &m_wndBottom );
	
	if ( ! m_wndFind.Create( WS_CHILD|ES_AUTOHSCROLL, rectDefault, &m_wndBottom, IDC_USER_SEARCH ) ) return -1;
	m_wndFind.SetFont( &CoolInterface.m_fntNormal );
	
	if ( ! m_wndBalan.Create( "", WS_CHILD, rectDefault, &m_wndBottom, IDC_USER_BALANCE ) ) return -1;
	m_wndBalan.SetFont( &CoolInterface.m_fntBold );
	
	PostMessage( WM_COMMAND, ID_INNER_REFRESH );
	
	return 0;
}

void CInnerWnd::OnDestroy() 
{
	CChildWnd::OnDestroy();
	
	if ( m_pWindow )
	{
		m_pWindow->DestroyWindow();
		wndPanel.RemoveBox( m_pWindow );
		m_pWindow = NULL;
	}
	
	Network.LeaveTalk();
	StopRinging();
}

void CInnerWnd::OnSize(UINT nType, int cx, int cy) 
{
	if ( nType != SIZE_INTERNAL ) CChildWnd::OnSize( nType, cx, cy );

	CRect rc;
	GetClientRect( &rc );
	
	if ( rc.Width() < 32 || rc.Height() < 32 ) return;
	
	HDWP hDWP = BeginDeferWindowPos( 4 );
	
	DeferWindowPos( hDWP, wndPanel.GetSafeHwnd(), NULL,
		rc.left, rc.top, PANEL_WIDTH, rc.Height(), SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndHeaderBar.GetSafeHwnd(), NULL,
		rc.left + PANEL_WIDTH, rc.top, rc.Width() - PANEL_WIDTH, BAR_HEIGHT, SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndList.GetSafeHwnd(), NULL,
		rc.left + PANEL_WIDTH, rc.top + BAR_HEIGHT, rc.Width() - PANEL_WIDTH, rc.Height() - BAR_HEIGHT * 2, SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndBottom.GetSafeHwnd(), NULL,
		rc.left + PANEL_WIDTH, rc.bottom - BAR_HEIGHT, rc.Width() - PANEL_WIDTH, BAR_HEIGHT, SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CInnerWnd::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	if ( bActivate )
	{
		wndPanel.SetParent( this );
		OnSize( SIZE_INTERNAL, 0, 0 );
	}
	
	CChildWnd::OnMDIActivate( bActivate, pActivateWnd, pDeactivateWnd );
}

void CInnerWnd::OnToolbarReturn()
{
	if ( GetFocus() == &m_wndFind )
	{
		if ( m_wndFind.GetWindowTextLength() > 0 )
			PostMessage( WM_COMMAND, ID_SEARCH_QUICK );
	}
}

void CInnerWnd::OnToolbarEscape()
{
	if ( GetFocus() == &m_wndFind )
	{
		m_wndFind.SetWindowText( _T("") );
		m_wndList.SetFocus();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CInnerWnd custom message handlers

void CInnerWnd::OnSyncOrderRelation()
{
	CXMLElement* pXML = new CXMLElement( NULL, ISyncDataView );
	pXML->AddAttribute( "method", "select top 100" );
	
	CXMLElement* pSync = pXML->AddElement( "column_expression" );
	pSync->SetValue( "a.jsjbh, b.xphone, c.xfeename, c.xtalktype,"
		" xtimestart=convert(varchar(19),a.xtimestart,120), a.xtimelength,"
		" xexpense=str(a.xexpense,5,2), a.xthirdid" );
	
	pSync = pXML->AddElement( "table_name" );
	pSync->SetValue( "from xuserdetail a"
		" inner join xuser b on a.xuserbindid = b.xuserbindid"
		" inner join xfeetable c on a.xfeetableid = c.xjsjbh" );
	
	CString str;
	str.Format( "b.xphone = '%s'", Network.m_xUserId );
	
	pSync = pXML->AddElement( "search_condition" );
	pSync->SetValue( (LPCTSTR)str );
	
	pSync = pXML->AddElement( "order_by_expression" );
	pSync->SetValue( "a.jsjbh desc" );
	
	Network.SendPacket( new CSyncDataView(pXML) );
	delete pXML;
}

BOOL CInnerWnd::OnSyncDataViewResp(CXMLElement* pXML)
{
#if 0
	CString strKey = pXML->GetAttributeValue( "hKey" );
	DWORD hKey = 0; _stscanf( strKey, "0x%x", &hKey );
	
	if ( hKey != (DWORD)m_hInstance ) return FALSE;
#endif
	return m_wndList.LoadRecordset( pXML );
}

void CInnerWnd::CalcUserBlance(LPCTSTR xUserId)
{
	CXMLElement* pXML = new CXMLElement( NULL, ISyncDataView );
	pXML->AddAttribute( "method", "select" );
	
	CXMLElement* pSync = pXML->AddElement( "column_expression" );
	pSync->SetValue( "xBalance=xBalance+xpreBala" );
	
	pSync = pXML->AddElement( "table_name" );
	pSync->SetValue( "from xuser_info" );
	
	CString str;
	str.Format( "xphone = '%s'", xUserId );
	
	pSync = pXML->AddElement( "search_condition" );
	pSync->SetValue( (LPCTSTR)str );
	
	Network.SendPacket( new CSyncDataView(pXML) );
	delete pXML;
}

BOOL CInnerWnd::OnCalcUserBlanceResp(CXMLElement* pXML)
{
	if ( CXMLElement* pSync = pXML->GetElementByName("SyncOrder") )
	{
		if ( CXMLAttribute* pAttri = pSync->GetAttribute("xBalance") )
		{
			m_wndBalan.SetWindowText( pAttri->GetValue() );
			return TRUE;
		}
	}
	
	return FALSE;
}

BOOL CInnerWnd::OnSyncNotify(WPARAM, CXMLElement* pXML)
{
	CString strMsgType = pXML->GetName();
	
	if ( ! strMsgType.CompareNoCase(ISyncHostFile) )
	{
		return OnSyncHostFile( pXML );
	}
	
	return FALSE;
}

BOOL CInnerWnd::OnSyncHostFile(CXMLElement* pXML)
{
	CString strMethod = pXML->GetAttributeValue("method");
	
	if ( ! strMethod.CompareNoCase( "ring" ) )
	{
		CXMLElement* pSync = pXML->GetElementByName( "RemoteId" );
		PutRinging( pSync->GetValue() );
	}
	else if ( ! strMethod.CompareNoCase( "bye" ) )
	{
		PostMessage(WM_COMMAND, ID_NETWORK_CONNECT);
	}
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CInnerWnd menu GUI

void CInnerWnd::OnUpdateNetworkConnect(CCmdUI* pCmdUI) 
{
	HINSTANCE hPrevInst = AfxGetResourceHandle();
	AfxSetResourceHandle( m_hInstance );

	CCoolBarItem* pItem = m_wndHeaderBar.GetID( ID_NETWORK_CONNECT );
	
	UINT nTextID = 0;
	UINT nTipID = 0;
	
	if ( Network.IsNavigated( ) )
	{
		if ( pItem ) pItem->SetCheck( TRUE );
		if ( pItem ) pItem->SetTextColour( CoolInterface.m_crCmdText == 0 ? RGB( 255, 0, 0 ) : CoolInterface.m_crCmdText );
		nTextID	= IDS_NETWORK_DISCONNECT;
		nTipID	= ID_NETWORK_DISCONNECT;
	}
	else
	{
		if ( pItem ) pItem->SetCheck( FALSE );
		if ( pItem ) pItem->SetTextColour( CoolInterface.m_crCmdText == 0 ? RGB( 0, 127, 0 ) : CoolInterface.m_crCmdText );
		nTextID	= IDS_NETWORK_CONNECT;
		nTipID	= ID_NETWORK_CONNECT;
	}

	CString strText;
	
	LoadString( strText, nTextID );
	if ( pItem ) pItem->SetText( strText );
	
	LoadString( strText, nTipID );
	if ( pItem ) pItem->SetTip( strText );
	
	if ( pItem ) pItem->SetImage( nTipID );
	
	AfxSetResourceHandle( hPrevInst );
}

void CInnerWnd::OnNetworkConnect()
{
	CString xRemoteId;
	m_wndPhone.GetWindowText( xRemoteId );
	
	xRemoteId.TrimLeft(); xRemoteId.TrimRight();
	if ( xRemoteId.IsEmpty() ) return;
	
	if ( Network.IsNavigated() )
	{
		ASSERT( m_pWindow != NULL );
		m_pWindow->DestroyWindow();
		wndPanel.RemoveBox( m_pWindow );
		m_pWindow = NULL;
		
		Network.LeaveTalk();
		PutReady();
	}
	else
	if ( Network.IsCallType( CA_TALK ) )
	{
		m_wndList.EnableWindow( FALSE );
		m_wndBottom.EnableWindow( FALSE );
		m_wndPhone.SetReadOnly( TRUE );
		
		CString xRemoteId;
		m_wndPhone.GetWindowText( xRemoteId );
		
		StopRinging();
	}
	else
	{
		if ( PutDialup(xRemoteId) ) PutConnecting( xRemoteId );
	}
}

void CInnerWnd::OnInnerReply()
{
	int nItem = m_wndList.GetNextItem( -1, LVIS_SELECTED );
	if ( nItem < 0 ) return;
	
	CString strKey = m_wndList.GetKey( nItem );
	PutReady( (LPCTSTR)strKey );
	
	SendMessage( WM_COMMAND, ID_NETWORK_CONNECT );
}

void CInnerWnd::OnInnerRefresh()
{
	CWaitCursor pCursor;
	
	OnSyncOrderRelation();
	
	CXMLElement* pXML;
	if ( ! Network.CallWorker( &pXML, MAX_TIMEOUT ) )
	{
		ASSERT( pXML->IsNamed( ISyncDataView ) );
		
		OnSyncDataViewResp( pXML );
		pXML->Delete();
	}
	
	CalcUserBlance( Network.m_xUserId );
	
	if ( ! Network.CallWorker( &pXML, MAX_TIMEOUT ) )
	{
		ASSERT( pXML->IsNamed( ISyncDataView ) );
		
		OnCalcUserBlanceResp( pXML );
		pXML->Delete();
	}
}
#if 0
void CInnerWnd::OnSearchGroup()
{
	CMenu pMenu;
	pMenu.CreatePopupMenu();
	
	CXMLElement* pXML;
	GetUserGroupInfo( Network.m_xUserId );
	
	if ( ! Network.CallWorker( &pXML, MAX_TIMEOUT ) )
	{
		ASSERT( pXML->IsNamed( ISyncDataView ) );
		
		OnUserGroupInfoResp( pXML, pMenu );
	}
	
	if ( ! pMenu.GetMenuItemCount() ) {pXML->Delete(); return;}
	
	CRect rcButton;
	m_wndHeaderBar.GetItemRect( m_wndHeaderBar.GetID(ID_SEARCH_GROUP), &rcButton );
	m_wndHeaderBar.ClientToScreen( &rcButton );
	rcButton.DeflateRect( 1, 2 );
	
	DWORD nFlags = TPM_LEFTALIGN | TPM_VERTICAL | TPM_RIGHTBUTTON;
	
	if ( UINT nMenuID = pMenu.TrackPopupMenu(nFlags | TPM_RETURNCMD, rcButton.left, rcButton.bottom, this) )
	{
		if ( ! Network.IsNavigated() ) PutIntoPhone( pXML, nMenuID - MIN_GROUP );
	}
	
	pXML->Delete();
}

void CInnerWnd::PutIntoPhone(CXMLElement* pXML, int nPos)
{
	int nIndex = 0;
	POSITION pos = pXML->GetElementIterator();
	
	for ( POSITION posNext = pos ; pos ; posNext = pos )
	{
		CXMLElement* pSync = pXML->GetNextElement( pos );
		
		if ( nIndex ++ >= nPos ) break;
	}
	
	if ( ! posNext ) return;
	
	CXMLElement* pSync = pXML->GetNextElement( posNext );
	m_wndPhone.SetWindowText( pSync->GetAttributeValue( _T("xphone") ) );
}
#else
void CInnerWnd::OnSearchGroup()
{
	CMenu pMenu;
	pMenu.CreatePopupMenu();
	
	HINSTANCE hInstance = LoadLibrary( _T("Xuserinfo.dll") );

	typedef DWORD (* USERGROUP) (LPCTSTR, HMENU);
	USERGROUP pfnUserInfo = (USERGROUP) GetProcAddress( hInstance, _T("GetUserGroupMenu") );
	
	if ( pfnUserInfo ) pfnUserInfo( Network.m_xUserId, pMenu );
	
	CRect rcButton;
	m_wndHeaderBar.GetItemRect( m_wndHeaderBar.GetID(ID_SEARCH_GROUP), &rcButton );
	m_wndHeaderBar.ClientToScreen( &rcButton );
	rcButton.DeflateRect( 1, 2 );
	
	DWORD nFlags = TPM_LEFTALIGN | TPM_VERTICAL | TPM_RIGHTBUTTON;
	
	if ( UINT nMenuID = pMenu.TrackPopupMenu(nFlags | TPM_RETURNCMD, rcButton.left, rcButton.bottom, this) )
	{
		if ( ! Network.IsNavigated() ) PutIntoPhone( pMenu, nMenuID );
	}
	
	FreeLibrary( hInstance );
}

void CInnerWnd::PutIntoPhone(HMENU hMenu, UINT nMenuId)
{
	CString strValue;
	GetMenuString( hMenu, nMenuId, strValue.GetBuffer(32), 32, MF_BYCOMMAND );
	strValue.ReleaseBuffer();
	
	int nPos = strValue.Find( '\t' );
	if ( nPos == -1 ) m_wndPhone.SetWindowText( strValue );
	else m_wndPhone.SetWindowText( strValue.Mid( nPos + 1 ) );
}
#endif

void CInnerWnd::GetUserGroupInfo(LPCTSTR xUserId)
{
	CXMLElement* pXML = new CXMLElement( NULL, ISyncDataView );
	pXML->AddAttribute( "method", "select" );
	
	CXMLElement* pSync = pXML->AddElement( "column_expression" );
	pSync->SetValue( "a.xphone, a.xname, a.xgroupnum, b.xgroupname" );
	
	pSync = pXML->AddElement( "table_name" );
	pSync->SetValue( "from xphonemeetinggroupmore a"
		" inner join xphonemeetinggroup b on a.xjsjbh=b.xjsjbh"
			" and a.xgroupnum=b.xgroupnum" );
	
	CString str;
	str.Format( "a.xjsjbh in (select xjsjbh from xuser_info "
				"where xphone = '%s')", Network.m_xUserId );
	
	pSync = pXML->AddElement( "search_condition" );
	pSync->SetValue( (LPCTSTR)str );
	
	Network.SendPacket( new CSyncDataView(pXML) );
	delete pXML; pXML = NULL;
}

BOOL CInnerWnd::OnUserGroupInfoResp(CXMLElement* pXML, HMENU hParent)
{
	UINT nMenuId = MIN_GROUP;
	
	for ( POSITION pos = pXML->GetElementIterator() ; pos ; )
	{
		CXMLElement* pSync = pXML->GetNextElement( pos );
		if ( ! pSync->IsNamed( "SyncOrder" ) ) continue;
		
		static LPCTSTR pszGroup[] = { _T("xgroupname"), _T("xgroupnum") };
		
		CXMLAttribute* pAttri = pSync->GetAttribute( pszGroup[0] );
		if ( ! pAttri || pAttri->GetValue().IsEmpty() ) pAttri = pSync->GetAttribute( pszGroup[1] );
		
		HMENU hMenu = FindMenuGroup( hParent, pAttri->GetValue() );	if ( ! hMenu )
		{
			hMenu = CreatePopupMenu();
			AppendMenu( hParent, MF_STRING|MF_POPUP, (UINT)hMenu, pAttri->GetValue() );
		}
		
		CString strText = pSync->GetAttributeValue( _T("xname") );
		if ( ! strText.IsEmpty() ) strText += '\t';
		
		strText += pSync->GetAttributeValue( _T("xphone") );
		AppendMenu( hMenu, MF_STRING, nMenuId++, strText );
		
		if ( nMenuId > MAX_GROUP ) break;
	}
	
	return TRUE;
}

HMENU CInnerWnd::FindMenuGroup(HMENU hParent, LPCTSTR lpszMenu)
{
	for ( int nItem = GetMenuItemCount( hParent ); nItem > 0; nItem-- )
	{
		TCHAR strItem[64];
		GetMenuString( hParent, nItem - 1, strItem, 64, MF_BYPOSITION );
		
		if ( ! _tcsicmp( strItem, lpszMenu ) ) return GetSubMenu( hParent, nItem - 1 );
	}
	
	return NULL;
}

void CInnerWnd::OnUpdateUserGroup(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( TRUE );
}

void CInnerWnd::OnUserMeeting()
{
	HINSTANCE hPrevInst = AfxGetResourceHandle();
	
	HINSTANCE hInstance = m_hInstance ? (HINSTANCE)m_hInstance : NULL;
	if ( hInstance ) AfxSetResourceHandle( hInstance );
	
	CUserMeetingDlg dlg;
	dlg.DoModal();
	
	AfxSetResourceHandle( hPrevInst );
}

/////////////////////////////////////////////////////////////////////////////
// CInnerWnd operations

void CInnerWnd::PutReady(LPCTSTR xRemoteId)
{
	HINSTANCE hPrevInst = AfxGetResourceHandle();
	AfxSetResourceHandle( m_hInstance );

	int nIcon = CoolInterface.ImageForID( ID_NETWORK_CONNECT );
	m_wndHeaderBar.SetIcon( CoolInterface.m_pImages.ExtractIcon( nIcon ) );
	
	CString strText;
	strText.LoadString( IDS_WELCOME );
	m_wndHeaderBar.SetCaption( (LPCTSTR)strText );
	
	m_wndPhone.SetReadOnly( FALSE );
	m_wndPhone.SetWindowText( xRemoteId );
	
	AfxSetResourceHandle( hPrevInst );
}

BOOL CInnerWnd::PutDialup(LPCTSTR xRemoteId, LPCTSTR xCallerId)
{
	CXMLElement* pXML = new CXMLElement( NULL, ISyncHostFile );
	pXML->AddAttribute( "method", "dialup" );
	
	CXMLElement* pSync = pXML->AddElement( "RemoteId" );
	pSync->SetValue( xRemoteId );
	
	if ( xCallerId && _tcslen(xCallerId) )
	{
		pSync = pXML->AddElement( "CallerId" );
		pSync->SetValue( xCallerId );
	}
	
	BOOL bResult = Network.DialupWith( pXML );
	pXML->Delete();
	
	if ( ! bResult ) return FALSE;
	
	ASSERT( m_pWindow == NULL );
	m_pWindow = wndPanel.RenderUser( Network.m_xUserId, 1 );
	m_pWindow->SetCaption( xRemoteId );
	
	m_pWindow->PostMessage( WM_STARTPREVIEW, PreviewPayload(PayloadMask_Audio), (LPARAM)&Datagrams );
	
	return bResult;
}

void CInnerWnd::PutRinging(LPCTSTR xRemoteId)
{
	HINSTANCE hPrevInst = AfxGetResourceHandle();
	AfxSetResourceHandle( m_hInstance );

	int nIcon = CoolInterface.ImageForID( ID_NETWORK_CONNECT );
	m_wndHeaderBar.SetIcon( CoolInterface.m_pImages.ExtractIcon( nIcon ) );
	
	CString strText;
	strText.LoadString( IDS_NETWORK_CONNECTING );
	m_wndHeaderBar.SetCaption( strText, RGB(222, 81, 42) );
	
	m_wndPhone.SetReadOnly( TRUE );
	m_wndPhone.SetWindowText( xRemoteId );
	
//	PlayRinging( AfxGetResourceHandle(), IDR_WAVE_RING );
	
	AfxSetResourceHandle( hPrevInst );
	
	ASSERT( m_pWindow == NULL );
	m_pWindow = wndPanel.RenderUser( Network.m_xUserId, 1 );
	m_pWindow->SetCaption( xRemoteId );
	
	m_pWindow->PostMessage( WM_STARTPREVIEW, PreviewPayload(PayloadMask_Audio), (LPARAM)&Datagrams );
}

BOOL CInnerWnd::PlayRinging(HINSTANCE hInstance, UINT nResourceID)
{
	HMODULE hModule = hInstance != NULL ? (HMODULE)hInstance : GetModuleHandle( NULL );
	HRSRC hRes = FindResource( hModule, MAKEINTRESOURCE( nResourceID ), _T("WAVE") );
	
	if ( hRes == NULL ) return FALSE;
	
	HGLOBAL hMemory		= ::LoadResource( hModule, hRes );
	LPCSTR pszInput		= (LPCSTR)LockResource( hMemory );
	
	return sndPlaySound( pszInput, SND_MEMORY | SND_LOOP | SND_ASYNC );
}

void CInnerWnd::StopRinging()
{
	sndPlaySound( NULL, SND_MEMORY | SND_LOOP | SND_ASYNC );
}

void CInnerWnd::PutConnecting(LPCTSTR xRemoteId)
{
	HINSTANCE hPrevInst = AfxGetResourceHandle();
	AfxSetResourceHandle( m_hInstance );

	int nIcon = CoolInterface.ImageForID( ID_NETWORK_CONNECT );
	m_wndHeaderBar.SetIcon( CoolInterface.m_pImages.ExtractIcon( nIcon ) );
	
	CString strText;
	strText.LoadString( IDS_NETWORK_CONNECTING );
	m_wndHeaderBar.SetCaption( strText, RGB(0, 127, 0) );
	
	m_wndPhone.SetReadOnly( TRUE );
	m_wndPhone.SetWindowText( xRemoteId );
	
	AfxSetResourceHandle( hPrevInst );
}

/////////////////////////////////////////////////////////////////////////////
// CInnerWnd keydown dtmf

void CInnerWnd::OnDtmfKeydown(UINT nID)
{
	const UINT nMinID = ID_DTMF_ONE;
	const UINT nMaxID = ID_DTMF_WELL;
	
	ASSERT( nID >= nMinID && nID <= nMaxID );
	int nIndex = nID - nMinID;
	
	HINSTANCE hPrevInst = AfxGetResourceHandle();
	AfxSetResourceHandle( m_hInstance );
	
	BYTE* pBuffer = LoadDtmfKey( AfxGetResourceHandle(), IDR_DTMF_SIGNAL );
	if ( pBuffer ) pBuffer = LoadDtmfKey( pBuffer, nIndex );
	
	AfxSetResourceHandle( hPrevInst );
	if ( ! pBuffer ) return;
	
	CSingleLock pLock( &Datagrams.m_pSection, TRUE );
	
	CDatagrams* pDatagram = Datagrams.SeekNext( Payload_Alaw );
	if ( ! pDatagram ) return;
	
	if ( CDatagramOut* pDG = pDatagram->LookupKey(Network.m_xUserId, pDG) )
	{
		const float nSamplesPerSec = 8000;
		float nTimePer = float( DTMF_ToneTime + DTMF_BreakTime );
		
		pDG->RenderBuffer( pBuffer, DWORD(nSamplesPerSec * nTimePer),
			Payload_Alaw, (DWORD)nTimePer, NULL, NULL );
	}
}

void CInnerWnd::OnUpdateDtmfKeydown(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( Network.IsNavigated( ) );
}

LPBYTE CInnerWnd::LoadDtmfKey(HINSTANCE hInstance, UINT nResourceID)
{
	HMODULE hModule = hInstance != NULL ? (HMODULE)hInstance : GetModuleHandle( NULL );
	HRSRC hRes = FindResource( hModule, MAKEINTRESOURCE( nResourceID ), _T("DTMF") );
	
	if ( hRes == NULL ) return NULL;
	
	DWORD nSize			= SizeofResource( hModule, hRes );
	HGLOBAL hMemory		= ::LoadResource( hModule, hRes );
	
	return (LPBYTE) LockResource( hMemory );
}

LPBYTE CInnerWnd::LoadDtmfKey(LPBYTE pBuffer, int nIndex)
{
	const float nSamplesPerSec = 8000;
	
	float nTimePer = float( DTMF_ToneTime + DTMF_BreakTime );
	pBuffer += nIndex * int( nSamplesPerSec * nTimePer );
	
	return pBuffer;
}
