//
// WndMusic.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "WndMusic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CMusicWnd, CChildWnd, 0)

BEGIN_MESSAGE_MAP(CMusicWnd, CChildWnd)
	//{{AFX_MSG_MAP(CMusicWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define SIZE_INTERNAL	1982
#define PANEL_WIDTH		200
#define SPLIT_SIZE		6

/////////////////////////////////////////////////////////////////////////////
// CMusicWnd construction

CMusicWnd::CMusicWnd() : CChildWnd(FALSE)
{
	Create( IDR_MUSICFRAME );
}

CMusicWnd::~CMusicWnd()
{
}

/////////////////////////////////////////////////////////////////////////////
// CMusicWnd message handlers

int CMusicWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CChildWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	SetOwner( GetParent() );

	return 0;
}

void CMusicWnd::OnDestroy() 
{
	CChildWnd::OnDestroy();
}

void CMusicWnd::OnSize(UINT nType, int cx, int cy) 
{
	if ( nType != SIZE_INTERNAL ) CChildWnd::OnSize( nType, cx, cy );

	CRect rc;
	GetClientRect( &rc );
}
