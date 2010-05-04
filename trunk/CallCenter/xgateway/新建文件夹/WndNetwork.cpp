//
// WndNetwork.cpp
//

#include "stdafx.h"
#include "WndNetwork.h"
#include "Settings.h"
#include "Xgateway.h"
#include "Network.h"
#include "Skin.h"

#include "CtrlNetworkVideo.h"

IMPLEMENT_SERIAL(CNetworkWnd, CChildWnd, 0)

BEGIN_MESSAGE_MAP(CNetworkWnd, CChildWnd)
	//{{AFX_MSG_MAP(CNetworkWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_UPDATE_COMMAND_UI(ID_NETWORK_VIDEO, OnUpdateNetworkVideo)
	ON_COMMAND(ID_NETWORK_VIDEO, OnNetworkVideo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define SIZE_INTERNAL	1982
#define HEADER_HEIGHT	16
#define BAR_HEIGHT		28
#define SPLIT_SIZE		6

/////////////////////////////////////////////////////////////////////////////
// CNetworkWnd construction

CNetworkWnd::CNetworkWnd() : CChildWnd(TRUE)
{
	m_nText		= 102;
	m_nPanel	= 172;
	m_hChildWnd	= m_hDefaultChild = NULL;
	
	Create( IDR_HOMEFRAME );
}

CNetworkWnd::~CNetworkWnd()
{
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkWnd message handlers

int CNetworkWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CChildWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	SetOwner( GetParent() );
	
	// Panel
	
	if ( ! m_wndTreeTop.Create( this, WS_CHILD|WS_VISIBLE|CBRS_NOALIGN, AFX_IDW_TOOLBAR ) ) return -1;
	m_wndTreeTop.SetBarStyle( m_wndTreeTop.GetBarStyle() | CBRS_TOOLTIPS|CBRS_BORDER_BOTTOM );
	Skin.CreateToolBar( _T("CNetworkTree.Top"), &m_wndTreeTop );
	m_wndTreeTop.SetOwner( GetOwner() );
	
	m_wndTree.Create( WS_VISIBLE, rectDefault, this );
	
	// Child
	
	if ( ! m_wndHeaderBar.Create( this, WS_CHILD|WS_VISIBLE|CBRS_NOALIGN, AFX_IDW_TOOLBAR ) ) return -1;
	m_wndHeaderBar.SetBarStyle( m_wndHeaderBar.GetBarStyle() | CBRS_TOOLTIPS|CBRS_BORDER_BOTTOM );
	Skin.CreateToolBar( _T("CNetworkWnd.Header"), &m_wndHeaderBar );
	m_wndHeaderBar.SetOwner( GetOwner() );
	
	m_wndChild.Create( WS_VISIBLE, rectDefault, this );
	m_hChildWnd = m_hDefaultChild = m_wndChild.GetSafeHwnd();
	
	// CtrlText

	m_wndText.Create( WS_VISIBLE, rectDefault, this, IDC_NETWORK_TEXT );

#if 1
	SelectChild( m_hDefaultChild );
	ShowStartupText();
#endif	
	
	return 0;
}

void CNetworkWnd::OnDestroy() 
{
	CChildWnd::OnDestroy();
}

void CNetworkWnd::OnSize(UINT nType, int cx, int cy) 
{
	if ( nType != SIZE_INTERNAL ) CChildWnd::OnSize( nType, cx, cy );

	CRect rc;
	GetClientRect( &rc );

	HDWP hDWP = BeginDeferWindowPos( 5 );
	
	DeferWindowPos( hDWP, m_wndTreeTop.GetSafeHwnd(), NULL,
		rc.left, rc.top, m_nPanel, BAR_HEIGHT, SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndTree.GetSafeHwnd(), NULL,
		rc.left, rc.top + BAR_HEIGHT, m_nPanel, rc.Height(), SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndHeaderBar.GetSafeHwnd(), NULL,
		rc.left + m_nPanel + SPLIT_SIZE, rc.top, rc.Width() - m_nPanel - SPLIT_SIZE, BAR_HEIGHT, SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_hChildWnd, NULL,
		rc.left + m_nPanel + SPLIT_SIZE, rc.top + BAR_HEIGHT, rc.Width() - m_nPanel - SPLIT_SIZE,
		rc.Height() - BAR_HEIGHT - SPLIT_SIZE - m_nText, SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndText.GetSafeHwnd(), NULL,
		rc.left + m_nPanel + SPLIT_SIZE, rc.bottom - m_nText,
		rc.Width() - m_nPanel - SPLIT_SIZE, m_nText, SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CNetworkWnd::OnPaint() 
{
	CPaintDC dc( this );
	CRect rcClient, rc;

	GetClientRect( &rcClient );
	
	rc.SetRect(	rcClient.left + m_nPanel,
				rcClient.top,
				rcClient.left + m_nPanel + SPLIT_SIZE,
				rcClient.bottom );

	dc.FillSolidRect( rc.left, rc.top, 1, rc.Height(), GetSysColor( COLOR_BTNFACE ) );
	dc.FillSolidRect( rc.left + 1, rc.top, 1, rc.Height(), GetSysColor( COLOR_3DHIGHLIGHT ) );
	dc.FillSolidRect( rc.right - 1, rc.top, 1, rc.Height(), GetSysColor( COLOR_3DSHADOW ) );
	dc.FillSolidRect( rc.left + 2, rc.top, rc.Width() - 3, rc.Height(),
		GetSysColor( COLOR_BTNFACE ) );
	
	rc.SetRect(	rcClient.left + m_nPanel + SPLIT_SIZE,
				rcClient.bottom - m_nText - SPLIT_SIZE,
				rcClient.right,
				rcClient.bottom - m_nText );
	dc.FillSolidRect( rc.left, rc.top, rc.Width(), 1, GetSysColor( COLOR_BTNFACE ) );
	dc.FillSolidRect( rc.left, rc.top + 1, rc.Width(), 1, GetSysColor( COLOR_3DHIGHLIGHT ) );
	dc.FillSolidRect( rc.left, rc.bottom - 1, rc.Width(), 1, GetSysColor( COLOR_3DSHADOW ) );
	dc.FillSolidRect( rc.left, rc.top + 2, rc.Width(), rc.Height() - 3,
		GetSysColor( COLOR_BTNFACE ) );
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkWnd resizing behaviour

BOOL CNetworkWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CRect rcClient, rc;
	CPoint point;

	GetCursorPos( &point );
	GetClientRect( &rcClient );
	ClientToScreen( &rcClient );
	
	rc.SetRect(	rcClient.left + m_nPanel,
				rcClient.top,
				rcClient.left + m_nPanel + SPLIT_SIZE,
				rcClient.bottom );
	
	if ( rc.PtInRect( point ) )
	{
		SetCursor( AfxGetApp()->LoadStandardCursor( IDC_SIZEWE ) );
		return TRUE;
	}
	
	rc.SetRect(	rcClient.left + m_nPanel + SPLIT_SIZE,
				rcClient.bottom - m_nText - SPLIT_SIZE,
				rcClient.right,
				rcClient.bottom - m_nText );
	
	if ( rc.PtInRect( point ) )
	{
		SetCursor( AfxGetApp()->LoadStandardCursor( IDC_SIZENS ) );
		return TRUE;
	}
	
	return CChildWnd::OnSetCursor( pWnd, nHitTest, message );
}

void CNetworkWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect rcClient, rc;
	
	GetClientRect( &rcClient );
	
	rc.SetRect(	rcClient.left + m_nPanel,
				rcClient.top,
				rcClient.left + m_nPanel + SPLIT_SIZE,
				rcClient.bottom );
	
	if ( rc.PtInRect( point ) )
	{
		DoSizePanel(); return;
	}

	rc.SetRect(	rcClient.left + m_nPanel,
				rcClient.bottom - m_nText - SPLIT_SIZE,
				rcClient.right,
				rcClient.bottom - m_nText );
	
	if ( rc.PtInRect( point ) )
	{
		DoSizeText(); return;
	}
	
	CChildWnd::OnLButtonDown( nFlags, point );
}

BOOL CNetworkWnd::DoSizePanel()
{
	CRect rcClient;
	CPoint point;
	
	GetClientRect( &rcClient );
	ClientToScreen( &rcClient );
	ClipCursor( &rcClient );
	SetCapture();

	GetClientRect( &rcClient );
	
	int nOffset = 0xFFFF;
	
	while ( GetAsyncKeyState( VK_LBUTTON ) & 0x8000 )
	{
		MSG msg;
		while ( ::PeekMessage( &msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE ) );

		if ( ! AfxGetThread()->PumpMessage() )
		{
			AfxPostQuitMessage( 0 );
			break;
		}

		GetCursorPos( &point );
		ScreenToClient( &point );

		int nSplit = point.x - rcClient.left;

		if ( nOffset == 0xFFFF ) nOffset = m_nPanel - nSplit;
		nSplit += nOffset;

		nSplit = max( nSplit, 0 );
		nSplit = min( nSplit, int(rcClient.right - SPLIT_SIZE) );

		if ( nSplit < 8 )
			nSplit = 0;
		if ( nSplit > rcClient.right - SPLIT_SIZE - 8 )
			nSplit = rcClient.right - SPLIT_SIZE;

		if ( nSplit != m_nPanel )
		{
			m_nPanel = nSplit;
			OnSize( SIZE_INTERNAL, 0, 0 );
			Invalidate();
		}
	}
	
	ReleaseCapture();
	ClipCursor( NULL );
	
	return TRUE;
}

BOOL CNetworkWnd::DoSizeText()
{
	CRect rcClient;
	CPoint point;
	
	GetClientRect( &rcClient );
	ClientToScreen( &rcClient );
	ClipCursor( &rcClient );
	SetCapture();

	GetClientRect( &rcClient );
	
	int nOffset = 0xFFFF;
	
	while ( GetAsyncKeyState( VK_LBUTTON ) & 0x8000 )
	{
		MSG msg;
		while ( ::PeekMessage( &msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE ) );

		if ( ! AfxGetThread()->PumpMessage() )
		{
			AfxPostQuitMessage( 0 );
			break;
		}

		GetCursorPos( &point );
		ScreenToClient( &point );

		int nSplit = rcClient.bottom - point.y;

		if ( nOffset == 0xFFFF ) nOffset = m_nText - nSplit;
		nSplit += nOffset;

		nSplit = max( nSplit, 0 );
		nSplit = min( nSplit, rcClient.bottom - SPLIT_SIZE );

		if ( nSplit < 8 )
			nSplit = 0;
		if ( nSplit > rcClient.bottom - SPLIT_SIZE - 8 )
			nSplit = rcClient.bottom - SPLIT_SIZE;

		if ( nSplit != m_nText )
		{
			m_nText = nSplit;
			OnSize( SIZE_INTERNAL, 0, 0 );
			Invalidate();
		}
	}
	
	ReleaseCapture();
	ClipCursor( NULL );
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkWnd custom message handlers

void CNetworkWnd::OnNetworkVideo()
{
}

void CNetworkWnd::OnUpdateNetworkVideo(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( TRUE );
}

void CNetworkWnd::SelectChild(HWND hCtrlChild)
{
	if ( m_hChildWnd && ::IsWindow( m_hChildWnd ) )
	{
		::ShowWindow( m_hChildWnd, SW_HIDE );
	}
	
	if ( ! hCtrlChild ) hCtrlChild = m_hDefaultChild;
	
	::ShowWindow( hCtrlChild, SW_SHOWNORMAL );
	m_hChildWnd = hCtrlChild;
	
	CNetworkChild* pChildCtrl = (CNetworkChild*)FromHandle( hCtrlChild );
	if ( pChildCtrl->IsKindOf( RUNTIME_CLASS(CNetworkChild) ) ) m_wndHeaderBar.Update( pChildCtrl );
	
	OnSize( SIZE_INTERNAL, 0, 0 );
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkWnd Text operations

void CNetworkWnd::Message(int nType, LPCTSTR pszText)
{
	if ( nType == MSG_DEBUG && ! Settings.General.Debug ) return;

	CTime pNow = CTime::GetCurrentTime();
	CString strLine;
	strLine.Format( _T("[%.2i:%.2i:%.2i] %s"),
		pNow.GetHour(), pNow.GetMinute(), pNow.GetSecond(), pszText );
	m_wndText.AddLine( nType, strLine );
}

void CNetworkWnd::ShowStartupText()
{
	m_wndText.AddLine( MSG_SYSTEM, "欢迎使用城市热线网关系统！作者：honglt 信箱：hong_lt@163.com" );
	m_wndText.AddLine( MSG_SYSTEM, "" );
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkWnd Channel operations

int CNetworkWnd::BindToUser(CChannel* pChannel, LPCTSTR xUserId)
{
	CNetworkVideo* pVideo = NULL;
	int nResult = m_wndTree.AddUser( xUserId, pVideo );
	
	pVideo->Clone( pChannel ); Network.SetChannel( pVideo );
	pVideo->OnHandshake();
	
	return nResult;
}
