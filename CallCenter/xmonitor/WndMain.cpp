//
// WndMain.cpp
//

#include "stdafx.h"
#include "Xmonitor.h"
#include "CoolInterface.h"
#include "Network.h"
#include "Skin.h"

#include "WndMain.h"
#include "WndNetwork.h"
#include "WndAgency.h"

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
	ON_UPDATE_COMMAND_UI(ID_TASK_AGENCY, OnUpdateTaskAgency)
	ON_COMMAND(ID_TASK_AGENCY, OnTaskAgency)
	ON_COMMAND(ID_TOOLS_PROFILE, OnToolsProfile)
	ON_COMMAND(ID_TOOLS_SETTINGS, OnToolsSettings)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_TOOLBAR, OnUpdateWindowToolBar)
	ON_COMMAND(ID_WINDOW_TOOLBAR, OnWindowToolBar)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_STATUS_BAR, OnUpdateWindowStatusBar)
	ON_COMMAND(ID_WINDOW_STATUS_BAR, OnWindowStatusBar)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_TAB_CONNECT, OnTabConnect)
	ON_COMMAND(ID_TAB_NETWORK, OnTabNetwork)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_PACKETREQ, OnPacketReq)
	ON_MESSAGE(WM_LOG, OnLog)
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
	
	// Tool Bar
	
	if ( ! m_wndToolBar.Create( this, WS_CHILD|CBRS_TOP, IDW_TOOL_BAR ) ) return -1;
	m_wndToolBar.SetWindowText( _T("工具栏") );
	m_wndToolBar.EnableDocking( CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM );
	m_wndToolBar.SetBarStyle( m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS );
	//	m_wndToolBar.SetGripper( TRUE );
	DockControlBar( &m_wndToolBar, AFX_IDW_DOCKBAR_TOP );
	
	Skin.CreateToolBar( _T("CMainWnd"), &m_wndToolBar );
	ShowControlBar( &m_wndToolBar, TRUE, TRUE );
	
	// Default Size
	
	SetWindowPos( NULL,	GetSystemMetrics( SM_CXSCREEN ) * 1 / 6,
						GetSystemMetrics( SM_CYSCREEN ) * 1 / 6,
						GetSystemMetrics( SM_CXSCREEN ) * 4 / 6,
						GetSystemMetrics( SM_CYSCREEN ) * 4 / 6, 0 );
	
	m_pWindows.SetOwner( this );
	SendMessage( WM_COMMAND, ID_TAB_NETWORK );
	
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
	
	KillTimer( 1 );
	Network.Disconnect();
	
	m_pWindows.m_bClosing = TRUE;
	m_pWindows.Close();	
	
	CMDIFrameWnd::OnClose();
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
	
	if ( CNetworkWnd* pWnd = (CNetworkWnd*)m_pWindows.Find( RUNTIME_CLASS(CNetworkWnd) ) )
	{
		pWnd->Message( (int)wParam, pszLog );
	}

	free( pszLog );
	return 0;
}

LONG CMainWnd::OnPacketReq(WPARAM wParam, LPARAM lParam)
{
	CPacket* pPacket = (CPacket*)wParam;
/*	
	if ( pPacket->IsCommandId( ICore_SyncTrunkView ) )
	{
		CNetworkWnd* pWnd = (CNetworkWnd*)m_pWindows.Find( RUNTIME_CLASS(CNetworkWnd) );
//		ASSERT( pWnd ); pWnd->LayoutThumb( ((CSyncTrunkView*)pPacket)->m_pXML );
	}
	else
	if ( pPacket->IsCommandId( ICore_SyncLineState ) )
	{
		CNetworkWnd* pWnd = (CNetworkWnd*)m_pWindows.Find( RUNTIME_CLASS(CNetworkWnd) );
//		ASSERT( pWnd ); pWnd->m_wndChild.UpdateChState( (CSyncLineState*)pPacket );
	}
*/	
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

void CMainWnd::OnTabConnect() 
{
	Network.Connect();
	
//	CChildWnd* pChild = m_pWindows.Find( RUNTIME_CLASS(CNetworkWnd) );
//	if	( pChild ) pChild->SendMessage( WM_COMMAND, ID_TAB_CONNECT );
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

/////////////////////////////////////////////////////////////////////////////
// CMainWnd task menu

void CMainWnd::OnUpdateTaskAgency(CCmdUI* pCmdUI)
{
	CChildWnd* pChild = m_pWindows.GetActive();
	pCmdUI->SetCheck( pChild && pChild->IsKindOf( RUNTIME_CLASS(CAgencyWnd) ) );
}

void CMainWnd::OnTaskAgency() 
{
	m_pWindows.Open( RUNTIME_CLASS(CAgencyWnd) );
}

/////////////////////////////////////////////////////////////////////////////
// CMainWnd tools menu

void CMainWnd::OnToolsProfile()
{
	CProfilesManagerDlg dlg;
	dlg.DoModal();
}

void CMainWnd::OnToolsSettings() 
{
	CSettingsManagerDlg dlg;
	if ( dlg.DoModal() != IDOK ) return;
	
	AfxGetMainWnd()->SendMessage( WM_COMMAND, ID_TAB_CONNECT );
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

/////////////////////////////////////////////////////////////////////////////
// CMainWnd help menu

void CMainWnd::OnAppAbout() 
{
	CAboutDlg dlg;
	dlg.DoModal();
}
