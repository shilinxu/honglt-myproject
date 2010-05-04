//
// WndLiner.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "WndLiner.h"
#include "skin.h"
#include "XML.h"

#include "CtrlNetworkPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CLinerWnd, CChildWnd, 0)

BEGIN_MESSAGE_MAP(CLinerWnd, CChildWnd)
	//{{AFX_MSG_MAP(CLinerWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_MDIACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define SIZE_INTERNAL	1982
#define PANEL_WIDTH		200

//////////////////////////////////////////////////////////////////////
// CLinerWnd construction

CLinerWnd::CLinerWnd() : CChildWnd(FALSE)
{
	Create( IDR_LINERFRAME );
}

CLinerWnd::~CLinerWnd()
{
}

/////////////////////////////////////////////////////////////////////////////
// CLinerWnd message handlers

int CLinerWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CChildWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	// Load Definitions
	
	Skin.LoadFromResource( AfxGetResourceHandle(), IDR_XML_DEFINITIONS );
	Skin.LoadFromResource( AfxGetResourceHandle(), IDR_XML_DEFAULT );
	
	// Panel
	
	m_wndPanel.Create( WS_VISIBLE, rectDefault, this );
	
	m_wndPanel.OnChanged();
	
	return 0;
}

void CLinerWnd::OnDestroy() 
{
	CChildWnd::OnDestroy();
}

void CLinerWnd::OnSize(UINT nType, int cx, int cy) 
{
	if ( nType != SIZE_INTERNAL ) CChildWnd::OnSize( nType, cx, cy );

	CRect rc;
	GetClientRect( &rc );
	
	if ( rc.Width() < 32 || rc.Height() < 32 ) return;
	
	HDWP hDWP = BeginDeferWindowPos( 1 );
	
	DeferWindowPos( hDWP, m_wndPanel.GetSafeHwnd(), NULL,
		rc.left, rc.top, PANEL_WIDTH, rc.Height(), SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CLinerWnd::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	if ( bActivate ) wndVideo.SetParent( &m_wndPanel);
	
	CChildWnd::OnMDIActivate( bActivate, pActivateWnd, pDeactivateWnd );
}
