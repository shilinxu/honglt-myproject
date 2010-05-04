//
// CtrlNetworkChild.cpp
//

#include "stdafx.h"
#include "Xgateway.h"
#include "CoolInterface.h"
#include "CtrlNetworkChild.h"
#include "CtrlNetworkVideo.h"
#include "CtrlNetworkTree.h"
#include "WndNetwork.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CNetworkChild, CWnd)
BEGIN_MESSAGE_MAP(CNetworkChild, CWnd)
	//{{AFX_MSG_MAP(CNetworkChild)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define CHILD_MARGIN	8

/////////////////////////////////////////////////////////////////////////////
// CNetworkChild construction

CNetworkChild::CNetworkChild(LPCTSTR xGroupId) : m_xGroupId( _tcsdup(xGroupId) )
{
	m_bLayout		= FALSE;
	m_pFocus		= NULL;
	m_pPanel		= NULL;
	
	m_nColumns		= 1;
	m_szBlock.cx	= CHILD_MARGIN+WINDOW_WIDTH;
	m_szBlock.cy	= CHILD_MARGIN+WINDOW_HEIGHT+CAPTION_HEIGHT;
}

CNetworkChild::~CNetworkChild()
{
	ClearChilds( TRUE );
	
	free( m_xGroupId ); m_xGroupId = NULL;
}

CNetworkTree* CNetworkChild::GetPanel() const
{
	ASSERT( m_pPanel && m_pPanel->IsKindOf( RUNTIME_CLASS(CNetworkTree) ) );
	return m_pPanel;
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkChild operations

BOOL CNetworkChild::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd)
{
	dwStyle |= WS_CHILD|WS_CLIPCHILDREN|WS_VSCROLL;
	return CWnd::Create( NULL, NULL, dwStyle, rect, pParentWnd, IDC_NETWORK_CHILD, NULL );
}

void CNetworkChild::UpdateScroll()
{
	SCROLLINFO pInfo;
	CRect rc;
	
	GetClientRect( &rc );
	
	pInfo.cbSize	= sizeof(pInfo);
	pInfo.fMask		= SIF_ALL & ~SIF_TRACKPOS;
	pInfo.nMin		= 0;
	pInfo.nMax		= ( ( GetChildCount() + m_nColumns - 1 ) / m_nColumns ) * m_szBlock.cy;
	pInfo.nPage		= rc.Height();
	pInfo.nPos		= GetScrollPos( SB_VERT );
	pInfo.nPos		= max( 0, min( pInfo.nPos, pInfo.nMax - (int)pInfo.nPage + 1 ) );

	SetScrollInfo( SB_VERT, &pInfo, TRUE );
	
//	Invalidate();
}

void CNetworkChild::ScrollBy(int nDelta)
{
	ScrollTo( max( 0, GetScrollPos( SB_VERT ) + nDelta ) );
}

void CNetworkChild::ScrollTo(int nPosition)
{
	if ( GetScrollPos( SB_VERT ) == nPosition ) return;
	
	SetScrollPos( SB_VERT, nPosition );
	
	UpdateScroll();
	RedrawWindow( NULL, NULL, RDW_INVALIDATE );
}

BOOL CNetworkChild::IsGroupCompatible(LPCTSTR xGroupId)
{
	return ( _tcsicmp(m_xGroupId, xGroupId) == 0 );
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkChild message handlers

int CNetworkChild::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	SetOwner( GetParent() );
	
	m_pPanel = &((CNetworkWnd*)GetParent())->m_wndTree;;
	
	return 0;
}

void CNetworkChild::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize( nType, cx, cy );
	
	m_nColumns = cx / (CHILD_MARGIN+WINDOW_WIDTH);
	m_nColumns = max( m_nColumns, 1 );
	
	UpdateScroll();
}

BOOL CNetworkChild::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CNetworkChild::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CRect rc;
	GetClientRect( &rc );

	SetFocus();

	switch ( nSBCode )
	{
	case SB_BOTTOM:
		ScrollTo( 0xFFFFFF );
		break;
	case SB_LINEDOWN:
		ScrollBy( 32 );
		break;
	case SB_LINEUP:
		ScrollBy( -32 );
		break;
	case SB_PAGEDOWN:
		ScrollBy( rc.Height() );
		break;
	case SB_PAGEUP:
		ScrollBy( -rc.Height() );
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		ScrollTo( nPos );
		break;
	case SB_TOP:
		ScrollTo( 0 );
		break;
	}
}

BOOL CNetworkChild::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	ScrollBy( zDelta * -m_szBlock.cy / WHEEL_DELTA / 2 );
	return TRUE;
}

void CNetworkChild::OnLButtonDown(UINT nFlags, CPoint point)
{
	SelectTo( NULL );
}

void CNetworkChild::OnRButtonDown(UINT nFlags, CPoint point)
{
	OnLButtonDown( nFlags, point );
}

void CNetworkChild::OnPaint() 
{
	CPaintDC dc( this );
	CRect rc;
	
	GetClientRect( &rc );
	
	dc.FillSolidRect( &rc, CoolInterface.m_crWindow );
	
	int nScroll = GetScrollPos( SB_VERT );
	if ( m_bLayout ) DoPaint( &dc, rc, nScroll );
	
	UpdateScroll();
}

void CNetworkChild::DoPaint(CDC* pDC, CRect& rcClient, int nScroll)
{
	CPoint ptUp( rcClient.left, rcClient.top - nScroll );
	
	HDWP hDWP = BeginDeferWindowPos( GetChildCount() );
	
	for ( POSITION posNext = GetChildIterator(); posNext; )
	{
		CNetworkVideo* pWindow = GetNextChild( posNext );
		
		if ( ptUp.x + m_szBlock.cx > rcClient.right )
			ptUp.Offset( rcClient.left - ptUp.x, m_szBlock.cy );
		
		if ( rcClient.PtInRect( ptUp ) ||
			 rcClient.PtInRect( ptUp + m_szBlock ) )
		{
			DeferWindowPos( hDWP, pWindow->GetSafeHwnd(), NULL, ptUp.x + CHILD_MARGIN,
				ptUp.y + CHILD_MARGIN, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOZORDER );
		}
		else
		{
			DeferWindowPos( hDWP, pWindow->GetSafeHwnd(), NULL, ptUp.x + CHILD_MARGIN,
				ptUp.y + CHILD_MARGIN, 0, 0, SWP_HIDEWINDOW|SWP_NOSIZE|SWP_NOZORDER );
		}
		
		ptUp.Offset( WINDOW_WIDTH + CHILD_MARGIN, 0 );
	}
	
	EndDeferWindowPos( hDWP );
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkChild custom message handlers

void CNetworkChild::SelectTo(CNetworkVideo* pWindow)
{
	SetFocus();
	
	if ( m_pFocus ) m_pFocus->Deselect();
	
	m_pFocus = pWindow ? pWindow->Select() : NULL;
}

void CNetworkChild::OnChanged()
{
	m_bLayout = TRUE;
	if ( m_hWnd ) Invalidate();
}

BOOL CNetworkChild::GetHeaderContent(CString& sContent)
{
	if ( ! m_pPanel ) return FALSE;
	
	if ( m_pPanel->GetSafeHwnd() ) m_pPanel->GetChildContent( this, sContent );
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkChild window list management

CNetworkVideo* CNetworkChild::AddChild(CNetworkVideo* pWindow, POSITION posBefore /* = NULL */)
{
	if ( ! posBefore ) m_pWindows.AddTail( pWindow );
	else m_pWindows.InsertBefore( posBefore, pWindow );
	
	OnChanged(); return pWindow;
}

POSITION CNetworkChild::GetChildIterator() const
{
	return m_pWindows.GetHeadPosition();
}

CNetworkVideo* CNetworkChild::GetNextChild(POSITION& pos) const
{
	return (CNetworkVideo*)m_pWindows.GetNext( pos );
}

int CNetworkChild::GetChildCount() const
{
	return m_pWindows.GetCount();
}

void CNetworkChild::RemoveChild(CNetworkVideo* pWindow)
{
	POSITION pos = m_pWindows.Find( pWindow );
	if ( pos == NULL ) return;
	
	m_pWindows.RemoveAt( pos );
	
	if ( ! GetChildCount() )
	{
		GetPanel()->RemoveItem( this );
	}
	else
	OnChanged();
}

void CNetworkChild::ClearChilds(BOOL bDelete)
{
	if ( bDelete )
	{
		for ( POSITION pos = GetChildIterator() ; pos ; ) delete GetNextChild( pos );
	}
	
	m_pWindows.RemoveAll();
	OnChanged();
}
