//
// WndMain.cpp
//

#include "stdafx.h"
#include "Xmaster.h"
#include "Network.h"
#include "Settings.h"
#include "CoolInterface.h"
#include "Skin.h"

#include "WndMain.h"
#include "WndNetwork.h"
#include "WndPlugin.h"
#include "WndReport.h"

#include "DlgAbout.h"
#include "DlgSettingsManager.h"
#include "DlgProfilesManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CMainWnd, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainWnd, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_MEASUREITEM()
	ON_WM_DRAWITEM()
	ON_WM_INITMENUPOPUP()
	ON_WM_SYSCOMMAND()
	ON_UPDATE_COMMAND_UI(ID_WINDOW_TOOLBAR, OnUpdateWindowToolBar)
	ON_COMMAND(ID_WINDOW_TOOLBAR, OnWindowToolBar)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_STATUS_BAR, OnUpdateWindowStatusBar)
	ON_COMMAND(ID_WINDOW_STATUS_BAR, OnWindowStatusBar)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_DEBUG, OnUpdateWindowDebug)
	ON_COMMAND(ID_WINDOW_DEBUG, OnWindowDebug)
	ON_UPDATE_COMMAND_UI(ID_TAB_CONNECT, OnUpdateTabConnect)
	ON_COMMAND(ID_TAB_CONNECT, OnTabConnect)
	ON_UPDATE_COMMAND_UI(ID_TAB_HOME, OnUpdateTabNetwork)
	ON_COMMAND(ID_TAB_HOME, OnTabNetwork)
	ON_UPDATE_COMMAND_UI(ID_TAB_PLUGIN, OnUpdateTabPlugin)
	ON_COMMAND(ID_TAB_PLUGIN, OnTabPlugin)
	ON_UPDATE_COMMAND_UI(ID_TAB_REPORT, OnUpdateTabReport)
	ON_COMMAND(ID_TAB_REPORT, OnTabReport)
	ON_UPDATE_COMMAND_UI(ID_NETWORK_CONNECT, OnUpdateNetworkConnect)
	ON_COMMAND(ID_NETWORK_CONNECT, OnNetworkConnect)
	ON_UPDATE_COMMAND_UI(ID_NETWORK_DISCONNECT, OnUpdateNetworkDisconnect)
	ON_COMMAND(ID_NETWORK_DISCONNECT, OnNetworkDisconnect)
	ON_COMMAND(ID_TOOLS_SETTINGS, OnToolsSettings)
	ON_COMMAND(ID_TOOLS_PROFILES, OnToolsProfiles)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_MESSAGE(WM_LOG, OnLog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainWnd construction

CMainWnd::CMainWnd()
{
	theApp.m_pMainWnd	= this;
	
	LoadFrame( IDR_MAINFRAME, WS_OVERLAPPEDWINDOW );
}

CMainWnd::~CMainWnd()
{
}

/////////////////////////////////////////////////////////////////////////////
// CMainWnd create window

BOOL CMainWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	WNDCLASS wndcls;
	
	ZeroMemory( &wndcls, sizeof(WNDCLASS) );
	wndcls.style			= CS_DBLCLKS;
	wndcls.lpfnWndProc		= AfxWndProc;
	wndcls.hInstance		= AfxGetInstanceHandle();
	wndcls.hIcon			= theApp.LoadIcon( IDR_MAINFRAME );
	wndcls.hCursor			= theApp.LoadStandardCursor( IDC_ARROW );
	wndcls.hbrBackground	= NULL;
	wndcls.lpszMenuName		= NULL;
	wndcls.lpszClassName	= gcszWindowClass;
	
	AfxRegisterClass( &wndcls );
	
	cs.lpszName = gcszWindowName;
	cs.lpszClass = wndcls.lpszClassName;
	
	return CMDIFrameWnd::PreCreateWindow( cs );
}

/////////////////////////////////////////////////////////////////////////////
// CMainWnd create window

int CMainWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CMDIFrameWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	// Icon
	
	SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), FALSE );
	
	// Status Bar
	
	UINT wID[2] = { ID_SEPARATOR, ID_SEPARATOR };
	if ( ! m_wndStatusBar.Create( this ) ) return -1;
	m_wndStatusBar.SetIndicators( wID, 2 );
	m_wndStatusBar.SetPaneInfo( 0, ID_SEPARATOR, SBPS_STRETCH, 0 );
	m_wndStatusBar.SetPaneInfo( 1, ID_SEPARATOR, SBPS_NORMAL, 210 );
	
	EnableDocking( CBRS_ALIGN_ANY );
	
	// Menu Bar
	
	SetMenu( NULL );
	
	if ( CMenu* pMenu = Skin.GetMenu( _T("CMainWnd") ) )
	{
		m_wndMenuBar.SetMenu( pMenu->Detach() );
	}

	if ( ! m_wndMenuBar.Create( this, WS_CHILD|WS_VISIBLE|CBRS_TOP, IDW_MENU_BAR ) ) return -1;
	m_wndMenuBar.SetWindowText( _T("菜单栏") );
	m_wndMenuBar.EnableDocking( CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM );
	DockControlBar( &m_wndMenuBar, AFX_IDW_DOCKBAR_TOP );
	
	// Tool Bar
	
	if ( ! m_wndToolBar.Create( this, WS_CHILD|CBRS_TOP, IDW_TOOL_BAR ) ) return -1;
	m_wndToolBar.SetWindowText( _T("工具栏") );
	m_wndToolBar.EnableDocking( CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM );
	m_wndToolBar.SetBarStyle( m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS );
	m_wndToolBar.SetGripper( TRUE );
	DockControlBar( &m_wndToolBar, AFX_IDW_DOCKBAR_TOP );
	
	Skin.CreateToolBar( _T("CMainWnd"), &m_wndToolBar );
	ShowControlBar( &m_wndToolBar, TRUE, TRUE );
	
	// Default Size
	
	SetWindowPos( NULL,	GetSystemMetrics( SM_CXSCREEN ) * 1 / 10,
						GetSystemMetrics( SM_CYSCREEN ) * 1 / 10,
						GetSystemMetrics( SM_CXSCREEN ) * 8 / 10,
						GetSystemMetrics( SM_CYSCREEN ) * 8 / 10, 0 );
	
	// Window Setup
	
	if ( ! m_wndMenuBar.IsVisible() ) ShowControlBar( &m_wndMenuBar, TRUE, TRUE );
	if ( ! m_wndToolBar.IsVisible() ) ShowControlBar( &m_wndToolBar, TRUE, TRUE );
	
	m_pWindows.SetOwner( this );

	SendMessage( WM_COMMAND, ID_TAB_PLUGIN );
	SendMessage( WM_COMMAND, ID_TAB_HOME );
	
	m_bInTimer = FALSE;
	SetTimer( 1, 1000, NULL );
	PostMessage( WM_TIMER, 1 );

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMainWnd destroy window

void CMainWnd::OnClose() 
{
#ifndef _DEBUG
	if (AfxMessageBox(IDS_QUITPROMPT, MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2) == IDCANCEL)
	{
		return;
	}
#endif
	
	CWaitCursor pCursor;
	
	Network.Disconnect();
	KillTimer( 1 );
	
	m_pWindows.m_bClosing = TRUE;
	m_pWindows.Close();	

	CMDIFrameWnd::OnClose();
}

/////////////////////////////////////////////////////////////////////////////
// CMainWnd command architecture

void CMainWnd::OnSysCommand(UINT nID, LPARAM lParam) 
{
	if ( ( nID & 0xFFF0 ) == SC_KEYMENU )
	{
		if ( lParam != ' ' && lParam != '-' )
		{
			if ( lParam )
				m_wndMenuBar.OpenMenuChar( lParam );
			else
				m_wndMenuBar.OpenMenuBar();
				
			return;
		}
	}
	
	CMDIFrameWnd::OnSysCommand( nID, lParam );
}

/////////////////////////////////////////////////////////////////////////////
// CMainWnd common timer

void CMainWnd::OnTimer(UINT nIDEvent) 
{
	// Propagate to children
	
	if ( m_bInTimer ) return;
	m_bInTimer = TRUE;
	
	for ( POSITION pos = m_pWindows.GetIterator() ; pos ; )
	{
		CChildWnd* pChild = m_pWindows.GetNext( pos );
		pChild->PostMessage( WM_TIMER, 1, 0 );
	}
	
	m_bInTimer = FALSE;
	
	// Menu Bar
	
	if ( m_wndMenuBar.IsWindowVisible() == FALSE ) ShowControlBar( &m_wndMenuBar, TRUE, FALSE );
	
	// Update messages

	if ( m_bInTimer ) return;
	m_bInTimer = TRUE;
	
	UpdateMessages();

	m_bInTimer = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainWnd custom message handlers

LONG CMainWnd::OnLog(WPARAM wParam, LPARAM lParam)
{
	LPTSTR pszLog = (LPTSTR)lParam;
	
	if ( CNetworkWnd* pWnd = (CNetworkWnd*)GetWindow( RUNTIME_CLASS(CNetworkWnd) ) )
	{
		pWnd->Message( (int)wParam, pszLog );
	}

	free( pszLog );
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMainWnd menu GUI

void CMainWnd::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
	CMDIFrameWnd::OnInitMenuPopup( pPopupMenu, nIndex, bSysMenu );
	UINT nID = pPopupMenu->GetMenuItemID( 0 );
	if ( nID != SC_RESTORE ) CoolMenu.AddMenu( pPopupMenu, TRUE );
}

void CMainWnd::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	CoolMenu.OnMeasureItem( lpMeasureItemStruct );
}

void CMainWnd::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CoolMenu.OnDrawItem( lpDrawItemStruct );
}

/////////////////////////////////////////////////////////////////////////////
// CMainWnd status message functionality

void CMainWnd::UpdateMessages()
{
	CString strMessage, strOld;
	
	if ( ! Network.IsConnected() )
	{
		strMessage = _T( "未连接网络" );
	}
	else
	{
		strMessage.Format( "已连接节点: %i", Network.IsWellConnected() );
	}
	
	m_wndStatusBar.GetPaneText( 1, strOld );
	if ( strOld != strMessage ) m_wndStatusBar.SetPaneText( 1, strMessage );
}

/////////////////////////////////////////////////////////////////////////////
// CMainWnd tab menu

void CMainWnd::OnUpdateTabConnect(CCmdUI* pCmdUI) 
{
	CCoolBarItem* pItem = m_wndToolBar.GetID( ID_TAB_CONNECT );
	
	UINT nTextID = 0;
	UINT nTipID = 0;
	
	if ( Network.IsWellConnected() )
	{
		//if ( pItem ) pItem->SetCheck( FALSE );
		if ( pItem ) pItem->SetTextColour( RGB( 255, 0, 0 ) );
		nTextID	= IDS_NETWORK_DISCONNECT;
		nTipID	= ID_NETWORK_DISCONNECT;
	}
	else if ( Network.IsConnected() )
	{
		if ( pItem ) pItem->SetCheck( TRUE );
		if ( pItem ) pItem->SetTextColour( CoolInterface.m_crCmdText == 0 ? RGB( 0, 127, 0 ) : CoolInterface.m_crCmdText );
		nTextID	= IDS_NETWORK_CONNECTING;
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
}

void CMainWnd::OnTabConnect() 
{
	if ( Network.IsWellConnected() )
	{
		CString strMessage;
		LoadString( strMessage, IDS_NETWORK_DISCONNECT_CONFIRM );

		if ( ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) ||
			AfxMessageBox( strMessage, MB_ICONQUESTION|MB_YESNO ) == IDYES )
		{
			Network.Disconnect();
		}
	}
	else
	if ( Network.IsConnected() )
	{
		Network.Disconnect();
	}
	else
	{
		Network.Connect();
	}
}

void CMainWnd::OnUpdateTabNetwork(CCmdUI* pCmdUI)
{
	CChildWnd* pChild = m_pWindows.GetActive();
	pCmdUI->SetCheck( pChild && pChild->IsKindOf( RUNTIME_CLASS(CNetworkWnd) ) );
}

void CMainWnd::OnTabNetwork()
{
	m_pWindows.Open( RUNTIME_CLASS(CNetworkWnd) );
}

void CMainWnd::OnUpdateTabPlugin(CCmdUI* pCmdUI)
{
	CChildWnd* pChild = GetWindow( RUNTIME_CLASS(CPluginWnd) );
	pCmdUI->SetCheck( pChild && pChild->IsKindOf( RUNTIME_CLASS(CPluginWnd) ) );
}

void CMainWnd::OnTabPlugin()
{
	m_pWindows.Open( RUNTIME_CLASS(CPluginWnd) );
}

void CMainWnd::OnUpdateTabReport(CCmdUI* pCmdUI) 
{
	CChildWnd* pChild = m_pWindows.GetActive();
	pCmdUI->SetCheck( pChild && pChild->IsKindOf( RUNTIME_CLASS(CReportWnd) ) );
}

void CMainWnd::OnTabReport() 
{
	m_pWindows.Open( RUNTIME_CLASS(CReportWnd) );
}

/////////////////////////////////////////////////////////////////////////////
// CMainWnd network menu

void CMainWnd::OnUpdateNetworkConnect(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( TRUE );

	UINT nTextID = 0;

	if ( Network.IsWellConnected() )
	{
		nTextID = IDS_NETWORK_CONNECTED;
		pCmdUI->SetCheck( TRUE );
	}
	else
	if ( Network.IsConnected() )
	{
		nTextID = IDS_NETWORK_CONNECTING;
		pCmdUI->SetCheck( TRUE );
	}
	else
	{
		nTextID = IDS_NETWORK_CONNECT;
		pCmdUI->SetCheck( FALSE );
	}

	CString strText;
	LoadString( strText, nTextID );
	pCmdUI->SetText( strText );
}

void CMainWnd::OnNetworkConnect()
{
	OnTabNetwork();
	CNetworkWnd* pWnd = (CNetworkWnd*) GetWindow( RUNTIME_CLASS(CNetworkWnd) );
	pWnd->Connect();
}

void CMainWnd::OnUpdateNetworkDisconnect(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( Network.IsConnected() );
}

void CMainWnd::OnNetworkDisconnect() 
{
	CNetworkWnd* pWnd = (CNetworkWnd*) GetWindow( RUNTIME_CLASS(CNetworkWnd) );
	pWnd->Disconnect();
}

/////////////////////////////////////////////////////////////////////////////
// CMainWnd window menu

void CMainWnd::OnUpdateWindowToolBar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_wndToolBar.IsVisible() );
}

void CMainWnd::OnWindowToolBar() 
{
	ShowControlBar( &m_wndToolBar, ! m_wndToolBar.IsVisible(), TRUE );
}

void CMainWnd::OnUpdateWindowStatusBar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_wndStatusBar.IsVisible() );
}

void CMainWnd::OnWindowStatusBar() 
{
	ShowControlBar( &m_wndStatusBar, ! m_wndStatusBar.IsVisible(), TRUE );
}

void CMainWnd::OnUpdateWindowDebug(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( Settings.General.Debug == TRUE );
}

void CMainWnd::OnWindowDebug() 
{
	Settings.General.Debug = ! Settings.General.Debug;
}

/////////////////////////////////////////////////////////////////////////////
// CMainWnd tools menu

void CMainWnd::OnToolsSettings() 
{
	CSettingsManagerDlg dlg;
	dlg.DoModal();
}

void CMainWnd::OnToolsProfiles() 
{
	CProfilesManagerDlg dlg;
	dlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CMainWnd help menu

void CMainWnd::OnAppAbout() 
{
	CAboutDlg dlg;
	dlg.DoModal();
}
