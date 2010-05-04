//
// WndHome.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "WndHome.h"
#include "CoolInterface.h"

#include "ChannelUser.h"
#include "RTPSession2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CHomeWnd, CChildWnd)

BEGIN_MESSAGE_MAP(CHomeWnd, CChildWnd)
	//{{AFX_MSG_MAP(CHomeWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define SIZE_INTERNAL	1982
#define PANEL_WIDTH		200

//////////////////////////////////////////////////////////////////////
// CHomeWnd construction

CHomeWnd::CHomeWnd() : CChildWnd(TRUE)
{
	Create( IDR_HOMEFRAME );
}

CHomeWnd::~CHomeWnd()
{
}

/////////////////////////////////////////////////////////////////////////////
// CHomeWnd message handlers

int CHomeWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CChildWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	SetOwner( GetParent() );
	
	// Panel

	m_wndPanel.Create( this );
	
	return 0;
}

void CHomeWnd::OnDestroy() 
{
	CChildWnd::OnDestroy();
	
	if ( UserInterface )
	UserInterface->Release();
}

void CHomeWnd::OnSize(UINT nType, int cx, int cy) 
{
	if ( nType != SIZE_INTERNAL ) CChildWnd::OnSize( nType, cx, cy );

	CRect rc;
	GetClientRect( &rc );

	HDWP hDWP = BeginDeferWindowPos( 1 );
	
	DeferWindowPos( hDWP, m_wndPanel.GetSafeHwnd(), NULL,
		rc.left, rc.top, PANEL_WIDTH, rc.Height(), SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

BOOL CHomeWnd::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect( &rc );
	
	pDC->FillSolidRect( &rc, CoolInterface.GetDialogBkColor() );
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CHomeWnd operations

BOOL CHomeWnd::StartCapture()
{
	BOOL bResult = TRUE;
	UserInterface->Setup();
	
	if ( bResult ) bResult =
	UserInterface->BuildGraph();
	
	if ( bResult ) //bResult =
	m_wndPanel.StartPreview( UserInterface );
	
	if ( ! bResult ) return bResult;
	int nError = RTPSession.Connect();
	
	if ( nError < 0 )
	{
		CString str;
		std::string stdErrStr = RTPGetErrorString(nError);
		
		str.Format( "Error[%i]: %s", nError, stdErrStr.c_str() );
		AfxMessageBox( (LPCTSTR)str );
		
		return FALSE;
	}
	
	return bResult;
}
