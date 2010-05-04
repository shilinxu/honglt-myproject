//
// WndChild.h
//

#include "stdafx.h"
#include "Xcenter.h"
#include "CoolInterface.h"

#include "WndChild.h"
#include "WndMain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CChildWnd, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildWnd, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_SYSCOMMAND()
	ON_WM_NCPAINT()
	ON_WM_NCHITTEST()
	ON_WM_NCCALCSIZE()
	ON_WM_NCACTIVATE()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CChildWnd* CChildWnd::m_pCmdMsg = NULL;

#define CAPTION_HEIGHT	20
#define CLOSEBOX		10


/////////////////////////////////////////////////////////////////////////////
// CChildWnd construction

CChildWnd::CChildWnd(BOOL bFixing)
{
	m_nResID		= 0;

	m_bPanelClose	= bFixing == FALSE;
	m_pCmdMsg		= NULL;
	
	m_crPanelBack	= RGB( 60, 60, 60 );
	m_crPanelText	= RGB( 255, 255, 255 );
	m_crPanelBorder	= RGB( 0, 0, 0 );
	m_bmPanelMark.LoadBitmap( IDB_PANEL_MARK );
}

CChildWnd::~CChildWnd()
{
	if ( m_bmPanelMark.m_hObject != NULL ) m_bmPanelMark.DeleteObject();
}

/////////////////////////////////////////////////////////////////////////////
// CChildWnd operations

BOOL CChildWnd::Create(UINT nID, BOOL bVisible) 
{
	m_nResID = nID;

	CString strCaption;
	strCaption.LoadString( m_nResID );
	
	return CMDIChildWnd::Create( NULL, strCaption, WS_CHILD |
		 WS_OVERLAPPEDWINDOW | ( bVisible ? WS_VISIBLE : 0 ) );
}

CMainWnd* CChildWnd::GetMainWnd()
{
	return (CMainWnd*)GetMDIFrame();
}

CWindowManager* CChildWnd::GetManager()
{
	return &GetMainWnd()->m_pWindows;
}

BOOL CChildWnd::IsActive(BOOL bFocused)
{
	CMainWnd* pMainWnd = GetMainWnd();
	
	if ( bFocused && GetForegroundWindow() != pMainWnd ) return FALSE;
	
	CChildWnd* pActive = (CChildWnd*)pMainWnd->MDIGetActive();
	
	if ( pActive == this ) return TRUE;
	if ( bFocused ) return FALSE;
	
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CChildWnd message handlers

int CChildWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CMDIChildWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	GetManager()->Add( this );

	HICON hIcon = (HICON)LoadImage( AfxGetResourceHandle(),
				MAKEINTRESOURCE( m_nResID ), IMAGE_ICON, 16, 16, 0 );
	SetIcon( hIcon, FALSE );
	
	CRect rc;
	GetParent()->GetClientRect( &rc );
	MoveWindow( &rc );
	
	return 0;
}

void CChildWnd::OnDestroy() 
{
	GetManager()->Remove( this );
	
	CMDIChildWnd::OnDestroy();
}

BOOL CChildWnd::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if ( m_pCmdMsg == this ) return FALSE;
	
	if ( CMDIChildWnd::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) ) return TRUE;
	
	if ( m_pCmdMsg != NULL ) return FALSE;
	
	m_pCmdMsg = this;
	BOOL bResult = GetMainWnd()->OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
	m_pCmdMsg = NULL;
	
	return bResult;
}

BOOL CChildWnd::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CChildWnd::OnSize(UINT nType, int cx, int cy) 
{
	CMDIChildWnd::OnSize( nType, cx, cy );
	
	BOOL bMinimized	= IsIconic();
	BOOL bVisible	= IsWindowVisible();
	
	if ( bMinimized && bVisible )
		ShowWindow( SW_HIDE );
	else if ( ! bMinimized && ! bVisible )
		ShowWindow( SW_SHOW );
	
	if ( ! IsIconic() )
	{
		CRect rc;
		GetWindowRect( &rc );
		rc.OffsetRect( -rc.left, -rc.top );
		rc.right++; rc.bottom++;
		SetWindowRgn( CreateRectRgnIndirect( &rc ), TRUE );
	}
}

void CChildWnd::OnSysCommand(UINT nID, LPARAM lParam) 
{
	switch ( nID & 0xFFF0 )
	{
	case SC_MINIMIZE:
	case SC_MAXIMIZE:
	case SC_MOVE:
	case SC_SIZE:
		break;
	
	case SC_CLOSE:
		if ( m_bPanelClose )
		CMDIChildWnd::OnSysCommand( nID, lParam );
		break;
	
	default:
		CMDIChildWnd::OnSysCommand( nID, lParam );
		break;
	}
}

void CChildWnd::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	NCCALCSIZE_PARAMS* pSize = (NCCALCSIZE_PARAMS*)lpncsp;
	pSize->rgrc[0].top += CAPTION_HEIGHT;
	return; // ( bCalcValidRects ) ? WVR_REDRAW|WVR_ALIGNTOP|WVR_ALIGNLEFT : 0;
}

UINT CChildWnd::OnNcHitTest(CPoint point) 
{
	CRect rc;
	GetWindowRect( &rc );
	rc.bottom = rc.top + CAPTION_HEIGHT;
	return rc.PtInRect( point ) ? HTCAPTION : HTCLIENT;
}

void CChildWnd::OnNcPaint() 
{
	CWindowDC dc( this );
	PaintCaption( dc );
}

BOOL CChildWnd::OnNcActivate(BOOL bActive) 
{
	CWindowDC dc( this );
	PaintCaption( dc );
	return TRUE;
}

BOOL CChildWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if ( nHitTest == HTCAPTION && m_bPanelClose )
	{
		CPoint pt;
		GetCursorPos( &pt );
		
		if ( m_rcClose.PtInRect( pt ) )
		{
			SetCursor( theApp.LoadCursor(IDC_HAND) );
			return TRUE;
		}
	}
	
	return CMDIChildWnd::OnSetCursor( pWnd, nHitTest, message );
}

void CChildWnd::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	if ( nHitTest == HTCAPTION && m_bPanelClose )
	{
		if ( m_rcClose.PtInRect( point ) )
		{
			PostMessage( WM_SYSCOMMAND, SC_CLOSE );
			return;
		}
	}

	CMDIChildWnd::OnNcLButtonDown( nHitTest, point );
}

void CChildWnd::PaintCaption(CDC& dc)
{
	CString strCaption;
	CRect rc, rcWnd;
	
	GetWindowRect( &rcWnd );
	rc.SetRect( 0, 0, rcWnd.Width(), CAPTION_HEIGHT );
	GetWindowText( strCaption );
	
	CDC* pBuffer = CoolInterface.GetBuffer( dc, rc.Size() );
	
	if ( ! CoolInterface.DrawWatermark( pBuffer, &rc, &m_bmPanelMark, 0, 0 ) )
	{
		pBuffer->FillSolidRect( &rc, m_crPanelBack );
	}
	
	int nIconY = rc.Height() / 2 - 8;
	DrawIconEx( pBuffer->GetSafeHdc(), 4, nIconY,
		GetIcon( FALSE ), 16, 16, 0, NULL, DI_NORMAL );
	
	//CFont* pOldFont	= (CFont*)pBuffer->SelectObject( &theApp.m_gdiFont );
	CFont* pOldFont	= (CFont*)pBuffer->SelectObject( &CoolInterface.m_fntCaption );
	CSize szCaption	= pBuffer->GetTextExtent( strCaption );
	
	pBuffer->SetBkMode( TRANSPARENT );
	
	if ( m_crPanelBorder != CLR_NONE )
	{
		pBuffer->SetTextColor( m_crPanelBorder );
		pBuffer->ExtTextOut( 8 + 16 - 1, rc.Height() / 2 - szCaption.cy / 2 - 1,
			ETO_CLIPPED, &rc, strCaption, NULL );
		pBuffer->ExtTextOut( 8 + 16 + 1, rc.Height() / 2 - szCaption.cy / 2 - 1,
			ETO_CLIPPED, &rc, strCaption, NULL );
		pBuffer->ExtTextOut( 8 + 16, rc.Height() / 2 - szCaption.cy / 2 - 1 - 1,
			ETO_CLIPPED, &rc, strCaption, NULL );
		pBuffer->ExtTextOut( 8 + 16, rc.Height() / 2 - szCaption.cy / 2 - 1 + 1,
			ETO_CLIPPED, &rc, strCaption, NULL );
	}
	
	pBuffer->SetTextColor( m_crPanelText );
	pBuffer->ExtTextOut( 8 + 16, rc.Height() / 2 - szCaption.cy / 2 - 1,
		ETO_CLIPPED, &rc, strCaption, NULL );
	
	if ( m_bPanelClose )
	{
		pBuffer->SelectObject( &CoolInterface.m_fntUnder );
		CString strText	= _T("¹Ø±Õ");
		CSize szText	= pBuffer->GetTextExtent( strText );
		
		m_rcClose.SetRect( rc.right - szText.cx - 8, rc.top, rc.right, rc.bottom );
		pBuffer->ExtTextOut( m_rcClose.left + 2,
			( m_rcClose.top + m_rcClose.bottom ) / 2 - szText.cy / 2 - 1,
			ETO_CLIPPED, &m_rcClose, strText, NULL );
		m_rcClose.OffsetRect( rcWnd.left, rcWnd.top );
	}
	
	pBuffer->SelectObject( pOldFont );
	
	dc.BitBlt( rc.left, rc.top, rc.Width(), rc.Height(), pBuffer, 0, 0, SRCCOPY );
}
