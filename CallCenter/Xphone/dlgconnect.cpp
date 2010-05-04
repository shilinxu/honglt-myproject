//
// DlgConnect.cpp
//

#include "stdafx.h"
#include "Xphone.h"
#include "DlgConnect.h"
#include "CoolInterface.h"
#include "Settings.h"
#include "Network.h"
#include "skin.h"

#include "CaptureGraph.h"
#include "RTPSession2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CConnectDlg, CDialog)
BEGIN_MESSAGE_MAP(CConnectDlg, CDialog)
	//{{AFX_MSG_MAP(CConnectDlg)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_SYSCOMMAND()
	ON_WM_MEASUREITEM()
	ON_WM_DRAWITEM()
	ON_COMMAND(ID_TRAY_OPEN, OnTrayOpen)
	ON_COMMAND(ID_NETWORK_CONNECT, OnNetworkConnect)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_TRAY, OnTray)
	ON_MESSAGE(WM_STARTPREVIEW, OnStartPreview)
END_MESSAGE_MAP()

#if 1
DECLARE_INTERFACE_(ITaskbarList,IUnknown)
{
   STDMETHOD(QueryInterface)(THIS_ REFIID riid,LPVOID* ppvObj) PURE;
   STDMETHOD_(ULONG,AddRef)(THIS) PURE;
   STDMETHOD_(ULONG,Release)(THIS) PURE;
   STDMETHOD(ActivateTab)(HWND) PURE;
   STDMETHOD(AddTab)(HWND) PURE;
   STDMETHOD(DeleteTab)(HWND) PURE;
   STDMETHOD(HrInit)(void) PURE;
};
#endif
#define BAR_HEIGHT		28

//////////////////////////////////////////////////////////////////////
// CConnectDlg construction

CConnectDlg::CConnectDlg() : CDialog( CConnectDlg::IDD, NULL )
{
	//{{AFX_DATA_INIT(CConnectDlg)
	//}}AFX_DATA_INIT
	
	theApp.m_pMainWnd	= this;
	ZeroMemory( &m_pTray, sizeof(NOTIFYICONDATA) );
	
	CreateReal( IDD );
}

CConnectDlg::~CConnectDlg()
{
	theApp.m_pMainWnd	= NULL;
}

void CConnectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectDlg)
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CConnectDlg operations

BOOL CConnectDlg::CreateReal(UINT nID)
{
	LPCTSTR lpszTemplateName = MAKEINTRESOURCE( nID );

	HINSTANCE hInst		= AfxFindResourceHandle( lpszTemplateName, RT_DIALOG );
	HRSRC hResource		= ::FindResource( hInst, lpszTemplateName, RT_DIALOG );
	HGLOBAL hTemplate	= LoadResource( hInst, hResource );

	LPCDLGTEMPLATE lpDialogTemplate = (LPCDLGTEMPLATE)LockResource( hTemplate );

	BOOL bResult = CreateDlgIndirect( lpDialogTemplate, NULL, hInst );

	UnlockResource( hTemplate );

	FreeResource( hTemplate );

	return bResult;
}

BOOL CConnectDlg::PreCreateWindow(CREATESTRUCT& cs) 
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
	
	return CDialog::PreCreateWindow( cs );
}

void CConnectDlg::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
	delete this;
}

/////////////////////////////////////////////////////////////////////////////
// CConnectDlg message handlers

BOOL CConnectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetIcon( AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE );
	
	CRect rc; 
	GetClientRect(&rc);
	
	// Toolbar
	
	if ( ! m_wndToolBar.Create( this, WS_CHILD|WS_VISIBLE|CBRS_NOALIGN, AFX_IDW_TOOLBAR ) ) return FALSE;
	m_wndToolBar.SetBarStyle( m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS|CBRS_BORDER_TOP );
	
	Skin.CreateToolBar( _T("CMainWnd"), &m_wndToolBar );
	m_wndToolBar.SetWindowPos( NULL, rc.left, rc.bottom - BAR_HEIGHT, rc.Width(), BAR_HEIGHT, SWP_NOZORDER );
	
	m_wndToolBar.SetIcon( CoolInterface.m_pImages.ExtractIcon( 22 ) );
	m_wndToolBar.SetCaption( "宁波中北通信" );
	
	// Default Size
	
	SetWindowPos( &wndTopMost,
				GetSystemMetrics(SM_CXFULLSCREEN) - rc.Width(),
				GetSystemMetrics(SM_CYFULLSCREEN) - rc.Height() - GetSystemMetrics(SM_CYDLGFRAME),
				0, 0, SWP_NOSIZE|SWP_SHOWWINDOW );
	
	// Window Setup
#if 0
	CComPtr<ITaskbarList> pTaskbar;
	HRESULT hr = CoCreateInstance( CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_ITaskbarList, (LPVOID*)&pTaskbar ); 
	
//	if ( SUCCEEDED( hr ) ) pTaskbar->HrInit();
	if ( SUCCEEDED( hr ) ) pTaskbar-> DeleteTab(GetSafeHwnd()); 
#endif
	// Tray Icon
	
	Shell_NotifyIcon( NIM_DELETE, &m_pTray );
	
	m_pTray.cbSize				= sizeof(m_pTray);
	m_pTray.hWnd				= GetSafeHwnd();
	m_pTray.uID					= 0;
	m_pTray.uFlags				= NIF_ICON | NIF_MESSAGE | NIF_TIP;
	m_pTray.uCallbackMessage	= WM_TRAY;
	m_pTray.hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );
	
	CString strTip;
	strTip.LoadString( AFX_IDS_APP_TITLE );
	
	_tcscpy( m_pTray.szTip, (LPCTSTR)strTip );
	Shell_NotifyIcon( NIM_ADD, &m_pTray );
	
	m_bInTimer = FALSE;
	SetTimer( 1, 1000, NULL );
	PostMessage( WM_TIMER, 1 );
	
	return TRUE;
}

BOOL CConnectDlg::OnEraseBkgnd(CDC* pDC) 
{
	CRect rc;
	GetClientRect( &rc );
	
	pDC->FillSolidRect( &rc, GetSysColor( COLOR_APPWORKSPACE ) );

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CConnectDlg destroy window

void CConnectDlg::OnDestroy() 
{
	CDialog::OnDestroy();
}

void CConnectDlg::OnClose() 
{
	CWaitCursor pCursor;
	
	Shell_NotifyIcon( NIM_DELETE, &m_pTray );
	
	Network.Disconnect();
	
	DestroyWindow();
}

void CConnectDlg::OnSysCommand(UINT nID, LPARAM lParam) 
{
	if ( nID == SC_CLOSE )
	{
		CDialog::OnSysCommand( SC_ICON, lParam );
		ShowWindow( SW_HIDE ); return;
	}
	else
	{
		CDialog::OnSysCommand( nID, lParam );
		if ( nID == SC_ICON ) ShowWindow( SW_HIDE );
	}
}

LRESULT CConnectDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if ( message == WM_SYSCOMMAND && wParam == SC_CLOSE )
	{
		wParam = SC_ICON;
	}
	
	return CDialog::DefWindowProc(message, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
// CConnectDlg common timer

void CConnectDlg::OnTimer(UINT nIDEvent) 
{
	// Update messages
	
	if ( m_bInTimer ) return;
	m_bInTimer = TRUE;
	
	UpdateMessages();
	m_bInTimer = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CConnectDlg tray functionality

void CConnectDlg::OpenFromTray(int nShowCmd)
{
	ShowWindow( nShowCmd );
	if ( IsIconic() ) SendMessage( WM_SYSCOMMAND, SC_RESTORE );
	
	SetForegroundWindow();
}

LONG CConnectDlg::OnTray(UINT wParam, LONG lParam)
{
	if ( LOWORD(lParam) == WM_LBUTTONDBLCLK )
	{
		OpenFromTray();
	}
	else if ( LOWORD(lParam) == WM_RBUTTONDOWN )
	{
		UINT nFlags = TPM_RIGHTBUTTON;
		CPoint pt;
		
		GetCursorPos( &pt );
		nFlags |= TPM_CENTERALIGN;
		
		SetForegroundWindow();
		
		CMenu* pMenu = Skin.GetMenu( _T("CMainWnd.Tray") );
		if ( pMenu == NULL ) return 0;
		
		MENUITEMINFO pInfo;
		pInfo.cbSize	= sizeof(pInfo);
		pInfo.fMask		= MIIM_STATE;
		GetMenuItemInfo( pMenu->GetSafeHmenu(), ID_TRAY_OPEN, FALSE, &pInfo );
		pInfo.fState	|= MFS_DEFAULT;
		SetMenuItemInfo( pMenu->GetSafeHmenu(), ID_TRAY_OPEN, FALSE, &pInfo );
		
		CoolMenu.AddMenu( pMenu, TRUE );		
		pMenu->TrackPopupMenu( nFlags | TPM_BOTTOMALIGN, pt.x, pt.y, this, NULL );
		
		PostMessage( WM_NULL );
	}
	
	return 0;
}

void CConnectDlg::OnTrayOpen() 
{
	OpenFromTray();
}

void CConnectDlg::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	CoolMenu.OnMeasureItem( lpMeasureItemStruct );
}

void CConnectDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CoolMenu.OnDrawItem( lpDrawItemStruct );
}

/////////////////////////////////////////////////////////////////////////////
// CConnectDlg custom message handlers

void CConnectDlg::OnNetworkConnect() 
{
	SendMessage( WM_SYSCOMMAND, SC_CLOSE );
	
	CaptureGraph.Setup();
	CaptureGraph.InstallAudio( NULL );
	CaptureGraph.InstallVideo( NULL );
	
	SendMessage( WM_COMMAND, ID_TRAY_OPEN );
	
	SendMessage( WM_STARTPREVIEW, 0L, (WPARAM)&CaptureGraph );
	
	CString xUserId, xPassword;
	VERIFY( theApp.CheckUsage( xUserId, xPassword ) );
	
	Network.Disconnect();
	RTPSession.Destroy();
	
	CSingleLock pLock( &Network.m_pSection, TRUE );
	Network.ConnectTo( Settings.Network.Host, Settings.Network.Port );
	Network.CUserChannel::ConnectTo( xUserId, xPassword );
	
	RTPSession.Connect();
}

LONG CConnectDlg::OnStartPreview(WPARAM wParam, LPARAM lParam)
{
	CRect rc;
	GetClientRect( &rc );
	
	CGraphCapture* pCapture = (CGraphCapture*)lParam;
	rc.bottom -= BAR_HEIGHT;
		
	pCapture->StartPreview( rc, GetSafeHwnd() );
	
	return 0;
}

void CConnectDlg::UpdateMessages()
{
	CString strMessage;
	LoadString( strMessage, AFX_IDS_APP_TITLE );
	
	CString str;
	LoadString( str, Network.IsConnected() ? IDS_NETWORK_CONNECTED : IDS_NETWORK_DISCONNECTED );
	
	strMessage += _T(" - ") + str;
	
	if ( strMessage != m_pTray.szTip )
	{
		m_pTray.uFlags = NIF_TIP | NIF_ICON;
		m_pTray.hIcon = AfxGetApp()->LoadIcon( Network.IsConnected() ? IDR_MAINFRAME : IDI_OFFLINE );
		
		_tcsncpy( m_pTray.szTip, strMessage, 63 );
		Shell_NotifyIcon( NIM_MODIFY, &m_pTray );
	}
}
