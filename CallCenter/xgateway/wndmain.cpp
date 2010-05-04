//
// WndMain.cpp
//

#include "stdafx.h"
#include "Xgateway.h"
#include "CoolInterface.h"
#include "Settings.h"
#include "Network.h"
#include "Neighbour.h"
#include "Skin.h"

#include "WndMain.h"
#include "WndNetwork.h"

#include "DlgAbout.h"
#include "DlgSettingsManager.h"
#include "DlgProfileManager.h"

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
	ON_COMMAND(ID_TAB_CONNECT, OnTabConnect)
	ON_COMMAND(ID_TAB_NETWORK, OnTabNetwork)
	ON_COMMAND(ID_TOOLS_SETTINGS, OnToolsSettings)
	ON_COMMAND(ID_TOOLS_PROFILE, OnToolsProfile)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_TOOLBAR, OnUpdateWindowToolBar)
	ON_COMMAND(ID_WINDOW_TOOLBAR, OnWindowToolBar)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_STATUS_BAR, OnUpdateWindowStatusBar)
	ON_COMMAND(ID_WINDOW_STATUS_BAR, OnWindowStatusBar)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_UPDATE_COMMAND_UI(ID_TAB_NETWORK, OnUpdateTabNetwork)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_LOG, OnLog)
	ON_MESSAGE(WM_USERCHANGED, OnUserChanged)
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
	
	SetWindowPos( NULL,	GetSystemMetrics( SM_CXSCREEN ) * 1 / 10,
						GetSystemMetrics( SM_CYSCREEN ) * 1 / 10,
						GetSystemMetrics( SM_CXSCREEN ) * 8 / 10,
						GetSystemMetrics( SM_CYSCREEN ) * 8 / 10, 0 );
	
	// Window Setup
	
	m_pWindows.SetOwner( this );
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
	
	Neighbour.Disconnect();
	Network.Disconnect();
	
	MSG msg;
	while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
	
	m_pWindows.m_bClosing = TRUE;
	m_pWindows.Close();	

	CMDIFrameWnd::OnClose();
}

/////////////////////////////////////////////////////////////////////////////
// CMainWnd command architecture

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

LONG CMainWnd::OnUserChanged(WPARAM wParam, LPARAM lParam)
{
	if ( CNetworkWnd* pWnd = (CNetworkWnd*)m_pWindows.Find( RUNTIME_CLASS(CNetworkWnd) ) )
	{
		return pWnd->OnUserChanged( *(LPCTSTR*)lParam, *(CChannel**)lParam );
	}
	
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
	if ( Network.Connect() && Neighbour.Connect() )
	{
		theApp.Message( MSG_SYSTEM, "成功加载设备" );
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

/////////////////////////////////////////////////////////////////////////////
// CMainWnd network menu

/////////////////////////////////////////////////////////////////////////////
// CMainWnd report menu

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

void CMainWnd::OnToolsProfile()
{
	CProfileManagerDlg dlg;
	dlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CMainWnd help menu
BOOL FindFilterByName(REFCLSID clsidDeviceClass, LPCWSTR lpszName, IBaseFilter** ppFilter)
{
	CComPtr<ICreateDevEnum> icDevEnum;
	if ( CoCreateInstance( CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (void**)&icDevEnum ) != S_OK ) return FALSE;
	
	CComPtr<IEnumMoniker> ieMoniker = NULL;
	if ( icDevEnum->CreateClassEnumerator( clsidDeviceClass,
		&ieMoniker, 0 ) != S_OK || ieMoniker == NULL ) return 0;
	
	IMoniker* pMoniker = NULL;
	while ( ieMoniker->Next(1, &pMoniker, NULL) == S_OK )
	{
		CComPtr<IPropertyBag> pBag = NULL;
		if ( pMoniker->BindToStorage( 0, 0, IID_IPropertyBag,
			(void **)&pBag ) != S_OK ) continue;
		
		CComVariant TheValue;
		pBag->Read( L"FriendlyName", &TheValue, NULL );
		
		theApp.Message(MSG_ERROR, (CString)TheValue.bstrVal );
		
		if ( pMoniker->BindToObject( 0, 0, IID_IBaseFilter,
			(void **)ppFilter ) != S_OK ) continue;
		CLSID ID;
		(*ppFilter)->GetClassID(&ID);
/*		if ( CLSID_FG729Render == ID)
		{
			TRACE("**********************\n");
		}
		*/
	wchar_t szGUID[39];
	szGUID[ StringFromGUID2( *(GUID*)&ID, szGUID, 39 ) - 2 ] = 0;
	theApp.Message(MSG_TEMP,(CString)&szGUID[1]);
	}
	
	return 0;
}

void CMainWnd::OnAppAbout() 
{
/*CComPtr<IBaseFilter> pCapFilter = NULL;
theApp.Message(MSG_TEMP,"以下音频压缩");
FindFilterByName(CLSID_AudioCompressorCategory, L"G.729", &pCapFilter);
pCapFilter.Release();
theApp.Message(MSG_TEMP,"以下视频压缩");
FindFilterByName(CLSID_VideoCompressorCategory, L"H.263", &pCapFilter);
*/
#ifdef _DEBUG
	CSingleLock pLock( &Network.m_pSection, TRUE );
	
	const char ppp[] =
		"SDV 516\r\n"
		"\r\n"
		"<SyncDataView method=\"select\" hKey=\"0x10000000\">\r\n"
		"<column_expression>xPCMName, xRecDate=convert(varchar(19),xRecDate,120), xCaller, xTheState, xjsjbh,xphonenum=case when left(xphonenum,1) in ('0','1') then '' else '0574' end + xphonenum</column_expression>\r\n"
		"<search_condition>left(case when left(xphonenum,1) in ('0','1') then '' else '0574' end+ xphonenum,11) = '13884415107'</search_condition>\r\n"
		"<order_by_expression>xRecDate desc</order_by_expression>\r\n"
		"</SyncDataView>\r\n"
		"\r\n";
//	CChannel* pChannel = Network.FindChannel( "13867877813");
//	if ( pChannel ) pChannel->m_pInput->Add( ppp, _tcslen(ppp) );
#endif
	CAboutDlg dlg;
	dlg.DoModal();
}
