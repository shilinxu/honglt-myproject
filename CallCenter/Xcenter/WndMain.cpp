//
// WndMain.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Network.h"
#include "Settings.h"
#include "CoolInterface.h"
#include "Skin.h"

#include "WndMain.h"
#include "WndNetwork.h"
#include "WndMusic.h"
#include "WndUser.h"
#include "WndClerk.h"
#include "WndPager.h"
#include "WndMail.h"

#include "DlgAbout.h"
#include "DlgSettingsManager.h"
#include "DlgProfilesManager.h"
#include "DlgPresserManager.h"

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
	ON_UPDATE_COMMAND_UI(ID_WINDOW_DEBUGLOG, OnUpdateWindowDebugLog)
	ON_COMMAND(ID_WINDOW_DEBUGLOG, OnWindowDebugLog)
	ON_COMMAND(ID_TAB_CONNECT, OnTabConnect)
	ON_UPDATE_COMMAND_UI(ID_TAB_HOME, OnUpdateTabNetwork)
	ON_COMMAND(ID_TAB_HOME, OnTabNetwork)
	ON_UPDATE_COMMAND_UI(ID_TAB_USER, OnUpdateTabUser)
	ON_COMMAND(ID_TAB_USER, OnTabUser)
	ON_COMMAND(ID_NETWORK_CONNECT, OnNetworkConnect)
	ON_COMMAND(ID_NETWORK_DISCONNECT, OnNetworkDisconnect)
	ON_UPDATE_COMMAND_UI(ID_REPORT_MUSIC, OnUpdateReportMusic)
	ON_COMMAND(ID_REPORT_MUSIC, OnReportMusic)
	ON_UPDATE_COMMAND_UI(ID_TASK_PAGER, OnUpdateTaskPager)
	ON_COMMAND(ID_TASK_PAGER, OnTaskPager)
	ON_UPDATE_COMMAND_UI(ID_TASK_MAIL, OnUpdateTaskMail)
	ON_COMMAND(ID_TASK_MAIL, OnTaskMail)
	ON_UPDATE_COMMAND_UI(ID_TASK_CLERK, OnUpdateTaskClerk)
	ON_COMMAND(ID_TASK_CLERK, OnTaskClerk)
	ON_COMMAND(ID_TOOLS_SETTINGS, OnToolsSettings)
	ON_COMMAND(ID_TOOLS_PROFILE, OnToolsProfile)
	ON_COMMAND(ID_TOOLS_PRESSER, OnToolsPresser)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
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
	SendMessage( WM_COMMAND, ID_TAB_HOME );
	SendMessage( WM_COMMAND, ID_TAB_USER );
	
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
	KillTimer( 1 );
	
	SendMessage( WM_COMMAND, ID_NETWORK_DISCONNECT );
	
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
	
	if ( CNetworkWnd* pWnd = (CNetworkWnd*)m_pWindows.Find( RUNTIME_CLASS(CNetworkWnd) ) )
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

void CMainWnd::OnTabConnect() 
{
	SendMessage( WM_COMMAND, ID_NETWORK_CONNECT );
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

void CMainWnd::OnUpdateTabUser(CCmdUI* pCmdUI) 
{
	CChildWnd* pChild = m_pWindows.GetActive();
	pCmdUI->SetCheck( pChild && pChild->IsKindOf( RUNTIME_CLASS(CUserWnd) ) );
}

void CMainWnd::OnTabUser() 
{
	m_pWindows.Open( RUNTIME_CLASS(CUserWnd) );
}

/////////////////////////////////////////////////////////////////////////////
// CMainWnd network menu

void CMainWnd::OnNetworkConnect()
{
	CChildWnd* pChild = GetWindow( RUNTIME_CLASS(CNetworkWnd) );
	if ( pChild ) ((CNetworkWnd*)pChild)->Connect();
	
#ifndef _DEBUG
	pChild = (CChildWnd*)RUNTIME_CLASS(CPagerWnd)->CreateObject();
	pChild = (CChildWnd*)RUNTIME_CLASS(CMailWnd)->CreateObject();
	pChild = (CChildWnd*)RUNTIME_CLASS(CClerkWnd)->CreateObject();
	((CClerkWnd*)pChild)->m_wndPanel.ExecuteStart();
#endif
}

void CMainWnd::OnNetworkDisconnect()
{
	CChildWnd* pChild = GetWindow( RUNTIME_CLASS(CPagerWnd) );
	if ( pChild ) pChild->DestroyWindow();
	
	pChild = GetWindow( RUNTIME_CLASS(CMailWnd) );
	if ( pChild ) pChild->DestroyWindow();
	
	pChild = GetWindow( RUNTIME_CLASS(CClerkWnd) );
	if ( pChild ) pChild->DestroyWindow();
	
	pChild = GetWindow( RUNTIME_CLASS(CNetworkWnd) );
	if ( pChild ) ((CNetworkWnd*)pChild)->Disconnect();
}

/////////////////////////////////////////////////////////////////////////////
// CMainWnd report menu

void CMainWnd::OnUpdateReportMusic(CCmdUI* pCmdUI) 
{
	CChildWnd* pChild = GetWindow( RUNTIME_CLASS(CMusicWnd) );
	pCmdUI->SetCheck( pChild && pChild->IsKindOf( RUNTIME_CLASS(CMusicWnd) ) );
}

void CMainWnd::OnReportMusic()
{
	m_pWindows.Open( RUNTIME_CLASS(CMusicWnd) );
}

/////////////////////////////////////////////////////////////////////////////
// CMainWnd task's menu

void CMainWnd::OnUpdateTaskPager(CCmdUI* pCmdUI) 
{
	CChildWnd* pChild = GetWindow( RUNTIME_CLASS(CPagerWnd) );
	pCmdUI->SetCheck( pChild && pChild->IsKindOf( RUNTIME_CLASS(CPagerWnd) ) );
}

void CMainWnd::OnTaskPager()
{
	m_pWindows.Open( RUNTIME_CLASS(CPagerWnd) );
}

void CMainWnd::OnUpdateTaskMail(CCmdUI* pCmdUI) 
{
	CChildWnd* pChild = GetWindow( RUNTIME_CLASS(CMailWnd) );
	pCmdUI->SetCheck( pChild && pChild->IsKindOf( RUNTIME_CLASS(CMailWnd) ) );
}

void CMainWnd::OnTaskMail()
{
	m_pWindows.Open( RUNTIME_CLASS(CMailWnd) );
}

void CMainWnd::OnUpdateTaskClerk(CCmdUI* pCmdUI) 
{
	CChildWnd* pChild = GetWindow( RUNTIME_CLASS(CClerkWnd) );
	pCmdUI->SetCheck( pChild && pChild->IsKindOf( RUNTIME_CLASS(CClerkWnd) ) );
}

void CMainWnd::OnTaskClerk()
{
	m_pWindows.Open( RUNTIME_CLASS(CClerkWnd) );
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

void CMainWnd::OnUpdateWindowDebugLog(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( Settings.General.DebugLog == TRUE );
}

void CMainWnd::OnWindowDebugLog() 
{
	Settings.General.DebugLog = ! Settings.General.DebugLog;
}

/////////////////////////////////////////////////////////////////////////////
// CMainWnd tools menu

void CMainWnd::OnToolsSettings() 
{
	CSettingsManagerDlg dlg;
	dlg.DoModal();
}

void CMainWnd::OnToolsProfile()
{
	CProfilesManagerDlg dlg;
	dlg.DoModal();
}

void CMainWnd::OnToolsPresser() 
{
	CPresserManagerDlg dlg;
	dlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CMainWnd help menu

#include "ChannelUser2.h"
#include "Traffic.h"
void CMainWnd::OnAppAbout() 
{
#ifdef _DEBUG
	Traffic.AddUsrBack( "96031", "057410000");
#endif
#ifdef NPICKUP
	CSingleLock pLock( &Network.m_pSection, TRUE );
	if ( CChannel* pChannel = Network.FindChannel( 100 ) )
	if ( pChannel->m_hInstance || pChannel->m_hDllClosing ) return;
	
	CChannel* pChannel1 = new CChannel( 100);
	Network.SetChannel(  pChannel1 );
	
	static int aaa=0;
	aaa ++;
	if ((aaa % 3) == 0)
	{
		pChannel1 = Network.MakeChannel(100, "0");
		CopyMemory(pChannel1->LocalId, "96031", MAX_PHONE);
		CopyMemory(pChannel1->RemoteId, "13867877813", MAX_PHONE);
		CopyMemory(pChannel1->CallType, CA_BACK, MAX_CALLTYPE);
		Network.SetChannel(  pChannel1 );
		pChannel1->m_bConnected = 1;
		CUserChannel2* pChannel2 = new CUserChannel2(10, "test");
		pChannel2->m_bConnected = 1;
		Network.SetChannel(  pChannel2 );
		
		pChannel1->m_nTaskId = pChannel2->m_nChan;
		pChannel2->m_nTaskId = pChannel1->m_nChan;
	
		CString str = "SVR 96031555 WALK2";
		CPacket* pPacket = CPacketCore::ReadBuffer((BYTE*)str.GetBuffer(0), str.GetLength());
		pChannel2->OnPacket( pPacket );
		pPacket->Release();
	}
	else
	if ((aaa % 2) == 0)
	{
		pChannel1 = Network.MakeChannel(100, "0");
		CopyMemory(pChannel1->LocalId, "96031", MAX_PHONE);
		CopyMemory(pChannel1->RemoteId, "13867877813", MAX_PHONE);
		CopyMemory(pChannel1->CallType, CA_BACK, MAX_CALLTYPE);
		Network.SetChannel(  pChannel1 );
		pChannel1->m_bConnected = 1;
		CUserChannel2* pChannel2 = new CUserChannel2(10, "test");
		pChannel2->m_bConnected = 1;
		Network.SetChannel(  pChannel2 );
		
		pChannel1->m_nTaskId = pChannel2->m_nChan;
		pChannel2->m_nTaskId = pChannel1->m_nChan;
		
		CString str = "SVR 96031222 M13055206933#\\\\192.168.0.11\\info_file\\歌曲\\很爱很爱你.pcm";
		CPacket* pPacket = CPacketCore::ReadBuffer((BYTE*)str.GetBuffer(0), str.GetLength());
		pChannel2->OnPacket( pPacket );
		pPacket->Release();
	}
	else
	{
		pChannel1 = Network.MakeChannel(100, "1234");
		CopyMemory(pChannel1->LocalId, "96031234", MAX_PHONE);
		CopyMemory(pChannel1->RemoteId, "13867877813", MAX_PHONE);
		CopyMemory(pChannel1->CallType, CA_BACK, MAX_CALLTYPE);
		Network.SetChannel(  pChannel1 );
		pChannel1->m_bConnected = 1;
	}
#endif
#ifdef NPICKUP
	EVTBLK evtblk;
	evtblk.nReference=100;
#if 0	//pager发信
	evtblk.wEventCode=E_FEE_Login; evtblk.dwParam=S_FEE_Connected;
	Network.PutBlock( &evtblk);
	
	evtblk.wEventCode=E_CHG_ChState; evtblk.dwParam=S_CALL_TALKING;
	Network.PutBlock( &evtblk);
	
	evtblk.wEventCode=E_CHG_ChState; evtblk.dwParam=S_CALL_STANDBY;
	Network.PutBlock( &evtblk);
#endif
#if 0	//人工转传真发信
	evtblk.wEventCode=E_CHG_ChState; evtblk.dwParam=S_CALL_RINGING;
	Network.PutBlock( &evtblk);
	evtblk.wEventCode=E_CHG_ChState; evtblk.dwParam=S_CALL_TALKING;
	Network.PutBlock( &evtblk);
	evtblk.wEventCode=E_PROC_PlayEnd; evtblk.dwParam=0;
	Network.PutBlock( &evtblk);
	evtblk.wEventCode=E_PROC_FaxEnd; evtblk.dwParam=0;
	Network.PutBlock( &evtblk);
	evtblk.wEventCode=E_CHG_ChState; evtblk.dwParam=S_CALL_STANDBY;
	Network.PutBlock( &evtblk);
#endif
#if 1	//人工转222代留言
	if ((aaa % 2) ==0)
	{evtblk.wEventCode=E_PROC_PlayEnd; evtblk.dwParam=0;
	Network.PutBlock( &evtblk);
	evtblk.wEventCode=E_PROC_RecordEnd; evtblk.dwParam=0;
	Network.PutBlock( &evtblk);
	evtblk.wEventCode=E_CHG_ChState; evtblk.dwParam=S_CALL_STANDBY;
	Network.PutBlock( &evtblk);
	return;
	}
#endif
#if 1	//人工转555
	evtblk.wEventCode=E_CHG_ChState; evtblk.dwParam=S_CALL_RINGING;
	Network.PutBlock( &evtblk);
	evtblk.wEventCode=E_CHG_ChState; evtblk.dwParam=S_CALL_TALKING;
	Network.PutBlock( &evtblk);
	evtblk.wEventCode=E_PROC_PlayEnd; evtblk.dwParam=0;
	Network.PutBlock( &evtblk);
	evtblk.wEventCode=E_PROC_FaxEnd; evtblk.dwParam=0;
	Network.PutBlock( &evtblk);
	evtblk.wEventCode=E_CHG_ChState; evtblk.dwParam=S_CALL_STANDBY;
	Network.PutBlock( &evtblk);
#endif
	
	return;
#endif
	CAboutDlg dlg;
	dlg.DoModal();
}
