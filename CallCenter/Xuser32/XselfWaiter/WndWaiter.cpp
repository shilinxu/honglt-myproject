//
// WndInner.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "WndWaiter.h"
#include "CoolInterface.h"
#include "Settings.h"
#include "skin.h"
#include "XML.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CWaiterWnd, CChildWnd, 0)

BEGIN_MESSAGE_MAP(CWaiterWnd, CChildWnd)
	//{{AFX_MSG_MAP(CWaiterWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_MDIACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define SIZE_INTERNAL	1982
#define PANEL_WIDTH		200
#define BAR_HEIGHT		28

//////////////////////////////////////////////////////////////////////
// CWaiterWnd construction

CWaiterWnd::CWaiterWnd() : CChildWnd(FALSE)
{
	Create( IDR_WAITERFRAME );
}

CWaiterWnd::~CWaiterWnd()
{
}

/////////////////////////////////////////////////////////////////////////////
// CWaiterWnd message handlers

int CWaiterWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CChildWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	return 0;
}

void CWaiterWnd::OnDestroy() 
{
	CChildWnd::OnDestroy();
}

void CWaiterWnd::OnSize(UINT nType, int cx, int cy) 
{
	if ( nType != SIZE_INTERNAL ) CChildWnd::OnSize( nType, cx, cy );

	CRect rc;
	GetClientRect( &rc );
	
	if ( rc.Width() < 32 || rc.Height() < 32 ) return;
	
	HDWP hDWP = BeginDeferWindowPos( 0 );
	
	EndDeferWindowPos( hDWP );
}

void CWaiterWnd::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
//	if ( bActivate ) wndVideo.SetParent( &m_wndPanel);
	
	CChildWnd::OnMDIActivate( bActivate, pActivateWnd, pDeactivateWnd );
}
