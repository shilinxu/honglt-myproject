//
// WndMain.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "CoolInterface.h"
#include "Settings.h"
#include "Datagrams.h"
#include "Network.h"
#include "Skin.h"

#include "WndMain.h"
#include "WndNetwork.h"
#include "DlgAbout.h"
#include "DlgSettingsManager.h"

#include "CtrlNetworkPanel.h"

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
	ON_COMMAND(ID_TOOLS_SETTINGS, OnToolsSettings)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_TOOLBAR, OnUpdateWindowToolBar)
	ON_COMMAND(ID_WINDOW_TOOLBAR, OnWindowToolBar)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_STATUS_BAR, OnUpdateWindowStatusBar)
	ON_COMMAND(ID_WINDOW_STATUS_BAR, OnWindowStatusBar)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_TAB_CONNECT, OnTabConnect)
	ON_COMMAND(ID_TAB_HOME, OnTabNetwork)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_LOG, OnLog)
	ON_MESSAGE(WM_SYNCNOTIFY, OnSyncNotify)
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
	SendMessage( WM_COMMAND, ID_TAB_HOME );
	
	m_bInTimer = FALSE;
	SetTimer( 1, 1000, NULL );
	PostMessage( WM_TIMER, 1 );
	
	SetTimer( 5, 100, TimerProcA );

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
	
	m_pWindows.m_bClosing = TRUE;
	m_pWindows.Close();	
	
	Network.Disconnect(0);
	
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

void CALLBACK CMainWnd::TimerProcA(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
{
	CMainWnd *pView = (CMainWnd *) CWnd::FromHandle( hWnd );
	
	CXMLElement* pXML;
	if ( DWORD nResult = Network.CallWorker( &pXML, 0 ) )
	{
		if ( nResult != WAIT_TIMEOUT )
		pView->OnFaultResp( nResult ); return;
	}
	
	pView->SendMessage( WM_SYNCNOTIFY, (WPARAM)&Network, (LPARAM)pXML );
	pXML->Delete();
}

/////////////////////////////////////////////////////////////////////////////
// CMainWnd custom message handlers

LONG CMainWnd::OnLog(WPARAM wParam, LPARAM lParam)
{
	LPTSTR pszLog = (LPTSTR)lParam;
	
	OutputDebugString( pszLog );
	OutputDebugString( "\n" );
	
	free( pszLog );
	return 0;
}

BOOL CMainWnd::OnSyncNotify(WPARAM wParam, CXMLElement* pXML)
{
	CWnd* pActiveWnd = m_pWindows.GetActive(); if ( pActiveWnd )
	{
		if ( pActiveWnd->SendMessage(WM_SYNCNOTIFY, wParam, (LPARAM)pXML) ) return TRUE;
	}
	
	for ( POSITION pos = m_pWindows.GetIterator() ; pos ; )
	{
		CChildWnd* pChildWnd = m_pWindows.GetNext( pos );
		if ( pChildWnd->SendMessage(WM_SYNCNOTIFY, wParam, (LPARAM)pXML) ) return TRUE;
	}
	
	_RPTF1(_CRT_WARN,"CMainWnd::OnSyncNotify - Unable to parse XML data\n%s\n", pXML->ToString(TRUE, TRUE));
	return FALSE;
}

BOOL CMainWnd::OnFaultResp(int nError)
{
	Network.Disconnect( 0 );
	
	CString str;
	str.Format( IDS_NETWORK_CONNECT_ERROR, nError );
	
	AfxMessageBox( str );
	
	return TRUE;
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
		
		DWORD tNow = GetTickCount();
		DWORD nLength = ( tNow - Network.m_tNavigated ) / 1000;
		
		CString strText;
		if ( nLength >= 3600 )
		{
			strText.Format( _T("[%s]: %i:%.2i:%.2i"), Network.m_xCallType,
				nLength / 3600, ( nLength / 60 ) % 60, nLength % 60 );
		}
		else
		{
			strText.Format( _T("[%s]: %.2i:%.2i"), Network.m_xCallType,
				nLength / 60, nLength % 60 );
		}
		
		strMessage += " " + strText;
	}
	
	m_wndStatusBar.GetPaneText( 1, strOld );
	if ( strOld != strMessage ) m_wndStatusBar.SetPaneText( 1, strMessage );
}

void CMainWnd::GetMessageString(UINT nID, CString& rMessage) const
{
	HINSTANCE hPrevInst = AfxGetResourceHandle();
	
	CChildWnd* pChild = m_pWindows.GetActive();
	HINSTANCE hInstance = pChild ? (HINSTANCE)pChild->m_hInstance : NULL;
	
	if ( hInstance ) AfxSetResourceHandle( hInstance );

	if ( LoadString( rMessage, nID ) )
	{
		int nPos = rMessage.Find( '\n' );
		if ( nPos >= 0 ) rMessage.SetAt( nPos, 0 );
	}
	
	AfxSetResourceHandle( hPrevInst );
}

/////////////////////////////////////////////////////////////////////////////
// CMainWnd tab menu

void CMainWnd::OnTabConnect() 
{
	CWaitCursor pCursor;
	
	wndPanel.ClearBoxes();
	Datagrams.Release();
	
	CSingleLock pLock( &Network.m_pSection, TRUE );
	
	CTaskBox* pWindow = wndPanel.RenderUser( Network.m_xUserId );
	pWindow->SetCaption( _T("本地视频") ); pWindow->SetPrimary();
	
	DWORD nAddress = htonl( INADDR_ANY );
	CDatagrams*	pDatagrams = &Datagrams;
	
	SOCKADDR_IN	pHost;
	CopyMemory( &pHost, &Network.m_pHost, sizeof(pHost) );
	
	pDatagrams->m_nType = Payload_Alaw;
	pDatagrams->Connect( (IN_ADDR*)&nAddress, (WORD)Settings.Network.Port );
	
	pWindow->PostMessage( WM_STARTPREVIEW, RenderPayload(PayloadMask_Audio), (LPARAM)pDatagrams );
	pHost.sin_port = pDatagrams->m_pHost.sin_port;
	pDatagrams->JoinMirror( &pHost, Network.m_xUserId );
	
	pDatagrams = Datagrams.Attach( new CDatagrams(Payload_H263) );
	pDatagrams->Connect( (IN_ADDR*)&nAddress, (WORD)Settings.Network.Port + 1 );
	
	pWindow->PostMessage( WM_STARTPREVIEW, RenderPayload(PayloadMask_Video)|PreviewPayload(0), (LPARAM)pDatagrams );
	pHost.sin_port = pDatagrams->m_pHost.sin_port;
	pDatagrams->JoinMirror( &pHost, Network.m_xUserId );
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
// CMainWnd tools menu

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
