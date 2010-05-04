//
// WndMain.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "Network.h"
#include "Settings.h"

#include "WndMain.h"
#include "WndHome.h"

#include "DlgAbout.h"
#include "DlgSettingsManager.h"

#include "ChannelUser.h"

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
	ON_COMMAND(ID_NETWORK_CONNECT, OnNetworkConnect)
	ON_COMMAND(ID_TOOLS_SETTINGS, OnToolsSettings)
	ON_COMMAND(ID_HELP_UPDATE, OnHelpUpdate)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_TAB_CONNECT, OnTabConnect)
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
	
	// Default Size
	
	SetWindowPos( NULL,	GetSystemMetrics( SM_CXSCREEN ) * 1 / 6,
						GetSystemMetrics( SM_CYSCREEN ) * 1 / 6,
						GetSystemMetrics( SM_CXSCREEN ) * 4 / 6,
						GetSystemMetrics( SM_CYSCREEN ) * 4 / 6, 0 );
	
	m_pWindows.SetOwner( this );
	SendMessage( WM_COMMAND, ID_NETWORK_CONNECT );

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMainWnd destroy window

void CMainWnd::OnClose() 
{
	CWaitCursor pCursor;
	KillTimer( 1 );
	
	Network.Disconnect();
	
	m_pWindows.m_bClosing = TRUE;
	m_pWindows.Close();	

	CMDIFrameWnd::OnClose();
}

/////////////////////////////////////////////////////////////////////////////
// CMainWnd tab menu

void CMainWnd::OnTabConnect() 
{
	CChildWnd* pChild = m_pWindows.Find( RUNTIME_CLASS(CHomeWnd) );
	if ( pChild ) ((CHomeWnd*)pChild)->StartCapture();
}

/////////////////////////////////////////////////////////////////////////////
// CMainWnd network menu

void CMainWnd::OnNetworkConnect()
{
	m_pWindows.Open( RUNTIME_CLASS( CHomeWnd ) );
}

/////////////////////////////////////////////////////////////////////////////
// CMainWnd tools menu

void CMainWnd::OnToolsSettings() 
{
	CSettingsManagerDlg dlg;
	if ( dlg.DoModal() == IDOK )
	AfxGetMainWnd()->SendMessage( WM_COMMAND, ID_TAB_CONNECT );
}

/////////////////////////////////////////////////////////////////////////////
// CMainWnd help menu

void CMainWnd::OnHelpUpdate()
{
	DWORD nProcessId = GetCurrentProcessId();
	CString strParam; strParam.Format( "%i", nProcessId );
	
	ShellExecute( NULL, _T("open"), _T("XLiveUpdate"),
				strParam, Settings.General.Path, SW_SHOWNORMAL );
}

/////////////////////////////////////////////////////////////////////////////
// CMainWnd help menu

void CMainWnd::OnAppAbout() 
{
	CAboutDlg dlg;
	dlg.DoModal();
}
