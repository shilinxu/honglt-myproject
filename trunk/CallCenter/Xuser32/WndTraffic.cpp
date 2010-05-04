//
// WndTraffic.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "WndTraffic.h"
#include "CoolInterface.h"
#include "Skin.h"
#include "XML.h"

#include "CtrlNetworkPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CTrafficWnd, CChildWnd)

BEGIN_MESSAGE_MAP(CTrafficWnd, CChildWnd)
	//{{AFX_MSG_MAP(CTrafficWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_MDIACTIVATE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_TRAFFIC_ACCEPT, OnTrafficAccept)
	ON_COMMAND(ID_TRAFFIC_REJECT, OnTrafficReject)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define SIZE_INTERNAL	1982
#define PANEL_WIDTH		200
#define BAR_HEIGHT		28

//////////////////////////////////////////////////////////////////////
// CTrafficWnd construction

CTrafficWnd::CTrafficWnd() : CChildWnd(FALSE)
{
	Create( IDR_TRAFFICFRAME );
}

CTrafficWnd::~CTrafficWnd()
{
}

/////////////////////////////////////////////////////////////////////////////
// CTrafficWnd message handlers

int CTrafficWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CChildWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	SetOwner( GetParent() );
	
	// Panel

	m_wndPanel.Create( WS_VISIBLE, rectDefault, this );
	
	// Header
	
	if ( ! m_wndHeaderBar.Create( this, WS_CHILD|WS_VISIBLE|CBRS_NOALIGN, AFX_IDW_TOOLBAR ) ) return -1;
	m_wndHeaderBar.SetBarStyle( m_wndHeaderBar.GetBarStyle() | CBRS_TOOLTIPS|CBRS_BORDER_BOTTOM );
	Skin.CreateToolBar( _T("CTrafficWnd.Header"), &m_wndHeaderBar );
	
	m_wndHeaderBar.SetOwner( GetOwner() );
	m_wndHeaderBar.SetIcon( CoolInterface.m_pImages.ExtractIcon( 22 ) );
	
	// Bottom
	
	if ( ! m_wndBottom.Create( this, WS_CHILD|WS_VISIBLE|CBRS_NOALIGN, AFX_IDW_TOOLBAR ) ) return -1;
	m_wndBottom.SetBarStyle( m_wndBottom.GetBarStyle() | CBRS_TOOLTIPS|CBRS_BORDER_TOP );
	Skin.CreateToolBar( _T("CTrafficWnd.Bottom"), &m_wndBottom );
	
	return 0;
}

void CTrafficWnd::OnDestroy() 
{
	CChildWnd::OnDestroy();
}

void CTrafficWnd::OnSize(UINT nType, int cx, int cy) 
{
	if ( nType != SIZE_INTERNAL ) CChildWnd::OnSize( nType, cx, cy );

	CRect rc;
	GetClientRect( &rc );
	
	if ( rc.Width() < 32 || rc.Height() < 32 ) return;
	
	HDWP hDWP = BeginDeferWindowPos( 3 );
	
	DeferWindowPos( hDWP, m_wndPanel.GetSafeHwnd(), NULL,
		rc.left, rc.top, PANEL_WIDTH, rc.Height(), SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndHeaderBar.GetSafeHwnd(), NULL,
		rc.left + PANEL_WIDTH, rc.top, rc.Width() - PANEL_WIDTH, BAR_HEIGHT, SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndBottom.GetSafeHwnd(), NULL,
		rc.left + PANEL_WIDTH, rc.bottom - BAR_HEIGHT, rc.Width() - PANEL_WIDTH, BAR_HEIGHT, SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CTrafficWnd::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	if ( bActivate ) wndVideo.SetParent( &m_wndPanel);
	
	CChildWnd::OnMDIActivate( bActivate, pActivateWnd, pDeactivateWnd );
}

BOOL CTrafficWnd::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect( &rc );
	
	pDC->FillSolidRect( &rc, GetSysColor( COLOR_APPWORKSPACE ) );
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CTrafficWnd custom message handlers

BOOL CTrafficWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	return CChildWnd::OnCommand(wParam, lParam);
}

void CTrafficWnd::OnTrafficAccept()
{
}

void CTrafficWnd::OnTrafficReject()
{
}
