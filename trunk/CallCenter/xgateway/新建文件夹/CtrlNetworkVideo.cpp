//
// CtrlNetworkVideo.cpp
//

#include "stdafx.h"
#include "Xgateway.h"
#include "Settings.h"
#include "CoolInterface.h"
#include "CtrlNetworkVideo.h"
#include "CtrlNetworkChild.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CNetworkVideo, CWnd)
BEGIN_MESSAGE_MAP(CNetworkVideo, CWnd)
	//{{AFX_MSG_MAP(CNetworkVideo)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_NCHITTEST()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// CNetworkVideo construction

CNetworkVideo::CNetworkVideo(CNetworkChild* pPanel, LPCTSTR xUserId) : CUserChannel(xUserId)
{
	m_bSelected	= FALSE;
	m_pPanel	= pPanel;
	
	Create( pPanel, xUserId );
}

CNetworkVideo::~CNetworkVideo()
{
	m_pPanel->RemoveChild( this );
}

void CNetworkVideo::Disconnect()
{
	PostMessage( WM_NCDESTROY );
}

void CNetworkVideo::PostNcDestroy()
{
	CUserChannel::Disconnect();
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkVideo operations

BOOL CNetworkVideo::Create(CNetworkChild* pPanel, LPCTSTR pszCaption)
{
	CRect rect( 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT + CAPTION_HEIGHT );
	
	if ( ! CWnd::Create( NULL, pszCaption, WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
		rect, pPanel, 100, NULL ) ) return FALSE;
	
	return TRUE;
}

CNetworkChild* CNetworkVideo::GetPanel() const
{
	ASSERT( m_pPanel != NULL );
	ASSERT_KINDOF( CNetworkChild, m_pPanel );
	return m_pPanel;
}

void CNetworkVideo::SetCaption(LPCTSTR pszCaption)
{
	CString strOld;
	GetWindowText( strOld );
	
	if ( strOld != pszCaption )
	{
		SetWindowText( pszCaption );
		InvalidateNonclient();
	}
}

void CNetworkVideo::InvalidateNonclient()
{
	if ( m_hWnd )
	{
		CRect rc;
		GetWindowRect( &rc );
		rc.bottom = rc.top + CAPTION_HEIGHT;
		ScreenToClient( &rc );
		RedrawWindow( &rc, NULL, RDW_FRAME|RDW_INVALIDATE );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkVideo message handlers

void CNetworkVideo::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	NCCALCSIZE_PARAMS* pSize = (NCCALCSIZE_PARAMS*)lpncsp;

	pSize->rgrc[0].left ++;
	pSize->rgrc[0].top ++;
	pSize->rgrc[0].right --;
	pSize->rgrc[0].bottom -= CAPTION_HEIGHT;
}

UINT CNetworkVideo::OnNcHitTest(CPoint point) 
{
	return HTCLIENT;
}

void CNetworkVideo::OnNcPaint() 
{
	PaintBorders();
}

void CNetworkVideo::PaintBorders()
{
	CWindowDC dc( this );
	CBitmap bmBuf, *pbmOld;
	
	CRect rc;
	GetWindowRect( &rc );
	rc.OffsetRect( -rc.left, -rc.top );
	
	CString strCaption;
	GetWindowText( strCaption );
	
	COLORREF crBorder = GetSysColor( COLOR_BTNFACE );
	if ( m_bSelected ) crBorder = CoolInterface.m_crHighlight;
	
	CDC dcBuf;
	dcBuf.CreateCompatibleDC( &dc );
	bmBuf.CreateCompatibleBitmap( &dc, rc.Width(), rc.Height() );
	pbmOld = (CBitmap*)dcBuf.SelectObject( &bmBuf );
	
	dcBuf.BitBlt( rc.left, rc.top, rc.right, rc.bottom, &dc, 0, 0, SRCCOPY );
	
	dcBuf.Draw3dRect( 0, 0, rc.Width(), rc.Height() - CAPTION_HEIGHT, crBorder, crBorder );
	dcBuf.FillSolidRect( 0, rc.Height() - CAPTION_HEIGHT, rc.Width(), CAPTION_HEIGHT, crBorder );
	
	CFont* pOldFont	= (CFont*)dcBuf.SelectObject( &CoolInterface.m_fntNormal );
	CSize sz		= dcBuf.GetTextExtent( strCaption );
	
	if ( m_bSelected )
	{
		dcBuf.SetBkColor( CoolInterface.m_crHighlight );
		dcBuf.SetTextColor( CoolInterface.m_crHiText );
		dcBuf.SetBkMode( OPAQUE );
	}
	else
	{
		dcBuf.SetBkColor( CoolInterface.m_crBackNormal );
		dcBuf.SetTextColor( CoolInterface.m_crText );
		dcBuf.SetBkMode( TRANSPARENT );
	}
	
	CRect rcc;
	rcc.SetRect( 0, rc.Height() - CAPTION_HEIGHT, rc.Width(), rc.Height() );
	
	dcBuf.ExtTextOut( rcc.left + rc.Width() / 2 - sz.cx / 2, 
		rcc.top + CAPTION_HEIGHT / 2 - sz.cy / 2, ETO_CLIPPED, &rcc, strCaption, NULL );
	
	dcBuf.SelectObject( pOldFont );
	
	dc.BitBlt( 0, 0, rc.Width(), rc.Height(), &dcBuf, 0, 0, SRCCOPY );
	
	dcBuf.SelectObject( pbmOld );
	dcBuf.DeleteDC();
	bmBuf.DeleteObject();
}

int CNetworkVideo::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	SetOwner( GetParent() );
	
	BOOL bResult = TRUE;
	CUserChannel::Setup();
	
	if ( bResult ) bResult =
	AudioCapture( Settings.Audio.DevOutput );
	//
	return 0;
}

void CNetworkVideo::OnPaint() 
{
	CPaintDC dc( this );
	
	CRect rc;
	GetClientRect( &rc );
	
//	dc.FillSolidRect( &rc, RGB( 238, 242, 251 ) );
	dc.FillSolidRect( &rc, CCoolInterface::GetDialogBkColor() );
}

void CNetworkVideo::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_pPanel->SelectTo( this );
}

void CNetworkVideo::OnRButtonDown(UINT nFlags, CPoint point) 
{
	OnLButtonDown( nFlags, point );
}

CNetworkVideo* CNetworkVideo::Select()
{
	if ( m_bSelected ) return this;
	m_bSelected	= TRUE;
	
	SendMessage( WM_NCPAINT );
	RedrawWindow( NULL, NULL, RDW_INVALIDATE );
	
	return this;
}

void CNetworkVideo::Deselect()
{
	m_bSelected	= FALSE;
	
	SendMessage( WM_NCPAINT );
	RedrawWindow( NULL, NULL, RDW_INVALIDATE );
}

void CNetworkVideo::OnDestroy() 
{
	CWnd::OnDestroy();
	
	// TODO: Add your message handler code here
	
}
