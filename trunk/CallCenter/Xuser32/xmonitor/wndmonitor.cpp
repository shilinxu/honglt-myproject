//
// WndMonitor.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "WndMonitor.h"
#include "CoolInterface.h"
#include "Settings.h"
#include "Registry.h"
#include "Network.h"
#include "skin.h"
#include "XML.h"

#include "CtrlNetworkThumb.h"
#include "DlgBlacklst.h"
#include "Datagrams.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CMonitorWnd, CChildWnd, 0)

BEGIN_MESSAGE_MAP(CMonitorWnd, CChildWnd)
	//{{AFX_MSG_MAP(CMonitorWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_MDIACTIVATE()
	ON_UPDATE_COMMAND_UI(ID_MEDIA_RECORD, OnUpdateMediaRecord)
	ON_COMMAND(ID_MEDIA_RECORD, OnMediaRecord)
	ON_UPDATE_COMMAND_UI(ID_MEDIA_REC_MASK, OnUpdateMediaRecMask)
	ON_COMMAND(ID_MEDIA_REC_MASK, OnMediaRecMask)
	ON_COMMAND(ID_USER_BLACKLST, OnUserBlacklst)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SYNCNOTIFY, OnSyncNotify)
END_MESSAGE_MAP()

#define SIZE_INTERNAL	1982
#define PANEL_WIDTH		200
#define BAR_HEIGHT		28

//////////////////////////////////////////////////////////////////////
// CMonitorWnd construction

CMonitorWnd::CMonitorWnd() : CChildWnd(FALSE)
{
	m_hSubMenu	= NULL;
	m_pDatagrams= NULL;
	
	Create( IDR_MONITORFRAME );
}

CMonitorWnd::~CMonitorWnd()
{
}

void CMonitorWnd::Apply()
{
	CRegistry pRegistry;
	
	DWORD nPayloadMask = pRegistry.GetInt( "Monitor", "RecMask", RecordPayload(PayloadMask_Audio) );
	pRegistry.SetInt( "Monitor", "RecMask", nPayloadMask );
}

/////////////////////////////////////////////////////////////////////////////
// CMonitorWnd message handlers

int CMonitorWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CChildWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	// Load Definitions
	
	Skin.LoadFromResource( AfxGetResourceHandle(), IDR_XML_DEFINITIONS );
	Skin.LoadFromResource( AfxGetResourceHandle(), IDR_XML_DEFAULT );
	
	// Panel
	
//	m_wndPanel.Create( WS_VISIBLE, rectDefault, this );
	
	// Header
	
	if ( ! m_wndHeaderBar.Create( this, WS_CHILD|WS_VISIBLE|CBRS_NOALIGN, AFX_IDW_TOOLBAR ) ) return -1;
	m_wndHeaderBar.SetBarStyle( m_wndHeaderBar.GetBarStyle() | CBRS_TOOLTIPS|CBRS_BORDER_BOTTOM );
	Skin.CreateToolBar( _T("CMonitorWnd.Header"), &m_wndHeaderBar );
	
	CString strText;
	strText.LoadString( IDS_NETWORK_WELCOME );
	m_wndHeaderBar.SetCaption( (LPCTSTR)strText );
	m_wndHeaderBar.SetIcon( CoolInterface.m_pImages.ExtractIcon( 22 ) );
	
	if ( ! m_wndTime.Create( "", WS_CHILD, rectDefault, &m_wndHeaderBar, 1001 ) ) return -1;
	m_wndTime.SetFont( &CoolInterface.m_fntBold );
//	if ( ! m_wndPhone.Create( WS_CHILD|ES_AUTOHSCROLL, rectDefault, &m_wndHeaderBar, IDC_USER_NUMBER ) ) return -1;
//	m_wndPhone.SetFont( &CoolInterface.m_fntBold );
	
	// Child
	
	m_wndChild.Create( WS_VISIBLE, rectDefault, this );
	
	// Bottom
	
	if ( ! m_wndBottom.Create( this, WS_CHILD|WS_VISIBLE|CBRS_NOALIGN, AFX_IDW_TOOLBAR ) ) return -1;
	m_wndBottom.SetBarStyle( m_wndBottom.GetBarStyle() | CBRS_TOOLTIPS|CBRS_BORDER_TOP );
	Skin.CreateToolBar( _T("CMonitorWnd.Bottom"), &m_wndBottom );
	
	if ( ! m_wndFind.Create( WS_CHILD|ES_AUTOHSCROLL, rectDefault, &m_wndBottom, IDC_USER_PHONE ) ) return -1;
	m_wndFind.SetFont( &CoolInterface.m_fntNormal );
	
	SyncMirrorView();
	PostMessage( WM_TIMER, 1 );
	
	Apply();
	
	return 0;
}

void CMonitorWnd::OnDestroy() 
{
	CChildWnd::OnDestroy();
	
	CMenu* pMainMenu = AfxGetMainWnd()->GetMenu();
	pMainMenu->DeleteMenu( (UINT)m_hSubMenu, MF_BYCOMMAND );
	
	pMainMenu->Detach();
	AfxGetMainWnd()->DrawMenuBar(); 
	
	m_wndChild.DestroyWindow();
	BreakMirrorView();
	
	Network.CallWorker( NULL, 1000 );
}

void CMonitorWnd::OnSize(UINT nType, int cx, int cy) 
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
	
	DeferWindowPos( hDWP, m_wndChild.GetSafeHwnd(), NULL,
		rc.left + PANEL_WIDTH, rc.top + BAR_HEIGHT, rc.Width() - PANEL_WIDTH,
		rc.Height() - BAR_HEIGHT * 2, SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndBottom.GetSafeHwnd(), NULL,
		rc.left + PANEL_WIDTH, rc.bottom - BAR_HEIGHT, rc.Width() - PANEL_WIDTH, BAR_HEIGHT, SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CMonitorWnd::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	if ( bActivate )
	{
		HINSTANCE hPrevInst = AfxGetResourceHandle();
		
		HINSTANCE hInstance = m_hInstance ? (HINSTANCE)m_hInstance : NULL;
		if ( hInstance ) AfxSetResourceHandle( hInstance );
		
		ApplyMenu( IDR_MONITORFRAME );
		AfxSetResourceHandle( hPrevInst );
		
		wndPanel.SetParent( this );
		OnSize( SIZE_INTERNAL, 0, 0 );
	}
	else
	{
		DestroyMenu();
	}
	
	CChildWnd::OnMDIActivate( bActivate, pActivateWnd, pDeactivateWnd );
}

BOOL CMonitorWnd::ApplyMenu(UINT nMenuId)
{
	CMenu pMenu;
	pMenu.LoadMenu( nMenuId );
	
	CString str;
	pMenu.GetMenuString( 0, str, MF_BYPOSITION );
	
	CMenu* pSubMenu = pMenu.GetSubMenu( 0 );
	m_hSubMenu = pMenu.GetSubMenu(0)->m_hMenu;
	
	CMenu* pMainMenu = AfxGetMainWnd()->GetMenu();
	pMainMenu->AppendMenu( MF_POPUP|MF_STRING, (UINT)m_hSubMenu, str ); 
	
	pMainMenu->Detach();
	pMenu.Detach();
	
	AfxGetMainWnd()->DrawMenuBar();
	return TRUE;
}

void CMonitorWnd::DestroyMenu()
{
	CMenu* pMainMenu = AfxGetMainWnd()->GetMenu();
	pMainMenu->DeleteMenu( (UINT)m_hSubMenu, MF_BYCOMMAND );
	
	pMainMenu->Detach();
	AfxGetMainWnd()->DrawMenuBar(); 
	
	m_hSubMenu = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CMonitorWnd common timer

void CMonitorWnd::OnTimer(UINT nIDEvent) 
{
	COleDateTime tNow = COleDateTime::GetCurrentTime();
	m_wndTime.SetWindowText( tNow.Format( "%Y-%m-%d %H:%M:%S" ) );
}

void CMonitorWnd::JoinMonitor(BOOL bJoined)
{
	CXMLElement* pXML = new CXMLElement( NULL, ISyncOrderRelation );
	pXML->AddAttribute( "method", "monitor" );
	
	if ( ! bJoined ) pXML->AddAttribute( "leave" );
	
	Network.SendPacket( new CSyncOrderRelation(pXML) );
	delete pXML;
}

/////////////////////////////////////////////////////////////////////////////
// CMonitorWnd custom message handlers
#if 1
void CMonitorWnd::SyncMirrorView()
{
	ASSERT( m_pDatagrams == NULL );
	DWORD nAddress = htonl( INADDR_ANY );
	
	CRegistry pRegistry;
	DWORD nPort = pRegistry.GetInt( "Monitor", "Port", 9962 );
	
	SOCKADDR_IN	pHost;
	CopyMemory( &pHost, &Network.m_pHost, sizeof(pHost) );
	
	CString xUserId( Network.m_xUserId ); xUserId += ".Monitor";
	
	m_pDatagrams = Datagrams.Attach( new CDatagrams(Payload_Alaw) );
	m_pDatagrams->Connect( (IN_ADDR*)&nAddress, (WORD)nPort );
	
	pHost.sin_port = htons( (WORD)Settings.Network.Port );
	m_pDatagrams->JoinMirror( &pHost, xUserId );
	
	CDatagrams*	pDatagrams = m_pDatagrams->Attach( new CDatagrams(Payload_H263) );
	pDatagrams->Connect( (IN_ADDR*)&nAddress, (WORD)nPort + 1 );
	
	pHost.sin_port = htons( (WORD)Settings.Network.Port + 1 );
	pDatagrams->JoinMirror( &pHost, xUserId );
	
	this->JoinMonitor();
}
#else
void CMonitorWnd::SyncMirrorView()
{
	DWORD nAddress = inet_addr( Settings.Network.Host );
	CString xUserId( Network.m_xUserId ); xUserId += ".Monitor";
	
	if ( CDatagrams* pDatagrams = Datagrams.SeekPayload(Payload_Alaw) )
	{
		pDatagrams->JoinMirror( (IN_ADDR*)&nAddress, (WORD)Settings.Audio.Port, Network.m_xUserId );
	}
	if ( CDatagrams* pDatagrams = Datagrams.SeekPayload(Payload_H263) )
	{
		pDatagrams->JoinMirror( (IN_ADDR*)&nAddress, (WORD)Settings.Video.Port, Network.m_xUserId );
	}
}
#endif
void CMonitorWnd::BreakMirrorView()
{
	this->JoinMonitor( FALSE );
	
	m_pDatagrams->Release();
}

BOOL CMonitorWnd::OnSyncNotify(WPARAM, CXMLElement* pXML)
{
	CString strMsgType = pXML->GetName();
	
	if ( ! strMsgType.CompareNoCase(ISyncMirrorView) )
	{
		return OnSyncOrderRelation( pXML );
	}
	
	return FALSE;
}

BOOL CMonitorWnd::OnSyncOrderRelation(CXMLElement* pXML)
{
	CString strMethod = pXML->GetAttributeValue("method");
	
	if ( ! strMethod.CompareNoCase("mirror") )
	{
		return OnSyncMirrorView( pXML );
	}
	
	return FALSE;
}

BOOL CMonitorWnd::OnSyncMirrorView(CXMLElement* pXML)
{
	CNetworkThumb* pWindow = NULL;
	m_wndChild.AddUser( pXML, pWindow );
	
	BOOL bJoined = pXML->GetAttribute("leave") == NULL;
	if ( ! bJoined ) {delete pWindow; return TRUE;}
	
	CString xUserId = pXML->GetAttributeValue( "UserId" );
	
	for ( POSITION pos = pXML->GetElementIterator(); pos; )
	{
		CXMLElement* pSync = pXML->GetNextElement( pos );
		
		WORD nPayloadMask = 0;
		CDatagrams* pDatagram = JoinedMirror( xUserId, pSync, nPayloadMask );
		
		pWindow->SendMessage( WM_STARTPREVIEW, PreviewPayload(nPayloadMask), (LPARAM)pDatagram );
	}
	
	Invalidate();
	return TRUE;
}

BOOL CMonitorWnd::OnSyncMirrorTalk(CXMLElement* pXML)
{
	CString xUserId = pXML->GetAttributeValue( "UserId" );
	
	CNetworkThumb* pWindow = m_wndChild.FindUser( xUserId );
	if ( ! pWindow ) return FALSE;
	
	if ( pXML->GetAttribute("bye") ) return SUCCEEDED(pWindow->StopRecord());
	CString xRemoteId = pXML->GetAttributeValue( "RemoteId" );
	
	CRegistry pRegistry;
	DWORD nPayloadMask = pRegistry.GetInt( "Monitor", "RecMask" );
	
	CString strFile = pRegistry.GetString( "Monitor", "RecFile", "<USER>\\<DATE>\\<TIME>-<REMOTE>.AVI" );
	
//	if ( pWindow ) pWindow->OnRecordFile( nPayloadMask, strFile, xRemoteId );
	return TRUE;
}

CDatagrams* CMonitorWnd::JoinedMirror(LPCTSTR xUserId, CXMLElement* pXML, WORD& nPayloadMask)
{
	ASSERT( pXML->IsNamed("mirror") );
	
	CString strValue;
	strValue = pXML->GetAttributeValue( "Type" );
	
	CDatagrams* pDatagram = m_pDatagrams->SeekNext( _ttoi(strValue) );
	if ( ! pDatagram ) return NULL;
	
	if ( _ttoi(strValue) == Payload_H263 ) nPayloadMask = PayloadMask_Video;
	else nPayloadMask = PayloadMask_Audio;
	
	strValue = pXML->GetAttributeValue( "ID" );
	pDatagram->JoinedMirror( xUserId, _ttoi(strValue) );
	
	ParseMatch( pXML, pDatagram );
	
	return pDatagram;
}

BOOL CMonitorWnd::ParseMatch(CXMLElement* pXML, CDatagrams* pParent)
{
	SOCKADDR_IN pHost;
	ZeroMemory( &pHost, sizeof(pHost) );
	
	CXMLElement* pSync = pXML->GetElementByName( "IP" );
	pHost.sin_addr.S_un.S_addr = inet_addr(  pSync->GetValue() );
	
	pSync = pXML->GetElementByName( "Port" );
	pHost.sin_port	= htons( _ttoi( pSync->GetValue() ) );
	pHost.sin_family= PF_INET;
	
	CString strValue = pXML->GetAttributeValue( "ID" );
	return pParent->JoinedMirror( _ttoi(strValue), &pHost );
}

/////////////////////////////////////////////////////////////////////////////
// CMonitorWnd menu GUI

void CMonitorWnd::OnUpdateMediaRecord(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( m_wndChild.m_pSelected != 0 );
	
	HINSTANCE hPrevInst = AfxGetResourceHandle();
	AfxSetResourceHandle( m_hInstance );
	
	CCoolBarItem* pItem = m_wndBottom.GetID( ID_MEDIA_RECORD );
	
	UINT nTextID = 0;
	UINT nTipID = 0;
	
	CNetworkThumb* pWindow = m_wndChild.m_pSelected;
	if ( pWindow && pWindow->IsRecording() )
	{
		if ( pItem ) pItem->SetTextColour( RGB( 255, 0, 0 ) );
		
		nTextID	= IDS_MEDIA_RECORDING;
		nTipID	= ID_MEDIA_REC_STOP;
	}
	else if ( pItem )
	{
		if ( pItem->m_bEnabled )
			pItem->SetTextColour( CoolInterface.m_crCmdText );
		else
			pItem->SetTextColour( CoolInterface.m_crDisabled );
		
		nTextID	= IDS_MEDIA_RECORD;
		nTipID	= ID_MEDIA_RECORD;
	}
	
	CString strText;
	
	LoadString( strText, nTextID );
	if ( pItem ) pItem->SetText( strText );
	
	LoadString( strText, nTipID );
	if ( pItem ) pItem->SetTip( strText );
	
	if ( pItem ) pItem->SetImage( nTipID );
	
	AfxSetResourceHandle( hPrevInst );
}

void CMonitorWnd::OnMediaRecord()
{
	if ( ! m_wndChild.m_pSelected ) return;
	
	CNetworkThumb* pWindow = m_wndChild.m_pSelected;
	if ( pWindow->IsRecording() ) {pWindow->StopRecord(); return;}
	
	CRegistry pRegistry;
	DWORD nPayloadMask = pRegistry.GetInt( "Monitor", "RecMask" );
	
	CString strFile = pRegistry.GetString( "Monitor", "RecFile", "<USER>\\<DATE>\\<USER>.<DATE><TIME>.AVI" );
	pWindow->OnRecordFile( nPayloadMask, strFile );
}

void CMonitorWnd::OnUpdateMediaRecMask(CCmdUI* pCmdUI)
{
	CRegistry pRegistry;
	DWORD nPayloadMask = pRegistry.GetInt( "Monitor", "RecMask" );
	
	pCmdUI->SetCheck( (RecordPayload(nPayloadMask) & PayloadMask_Video) == PayloadMask_Video );
}

void CMonitorWnd::OnMediaRecMask() 
{
	CRegistry pRegistry;
	DWORD nPayloadMask = pRegistry.GetInt( "Monitor", "RecMask" );
	
	pRegistry.SetInt( "Monitor", "RecMask", RecordPayload(nPayloadMask) ^ PayloadMask_Video );
}

/////////////////////////////////////////////////////////////////////////////
// CMonitorWnd menu GUI

void CMonitorWnd::OnUserBlacklst()
{
	HINSTANCE hPrevInst = AfxGetResourceHandle();
	
	HINSTANCE hInstance = m_hInstance ? (HINSTANCE)m_hInstance : NULL;
	if ( hInstance ) AfxSetResourceHandle( hInstance );
	
	CBlacklstDlg dlg;
	dlg.DoModal();
	
	AfxSetResourceHandle( hPrevInst );
}
