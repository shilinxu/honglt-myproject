//
// CtrlNetworkChild.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "CoolInterface.h"
#include "CtrlNetworkChild.h"
#include "Network.h"
#include "Channel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CNetworkChild, CWnd)
BEGIN_MESSAGE_MAP(CNetworkChild, CWnd)
	//{{AFX_MSG_MAP(CNetworkChild)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetworkChild construction

CNetworkChild::CNetworkChild(int nChType) : m_nChType( nChType )
{
	m_nChFirst		= -1;
	m_pChState		= NULL;
	m_nChCount		= 0;
	m_nRetained		= -1;
}

CNetworkChild::~CNetworkChild()
{
	if ( m_pChState ) free( m_pChState );
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkChild message handlers

BOOL CNetworkChild::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	dwStyle |= WS_CHILD|WS_VSCROLL;
	return CWnd::Create( NULL, NULL, dwStyle, rect, pParentWnd, nID, NULL );
}

void CNetworkChild::UpdateScroll()
{
	SCROLLINFO pInfo;
	CRect rc;
	
	GetClientRect( &rc );
	pInfo.cbSize	= sizeof(pInfo);
	pInfo.fMask		= SIF_RANGE|SIF_PAGE|SIF_DISABLENOSCROLL;
	pInfo.nMin		= 0;
	pInfo.nPage		= rc.Height() / 16;
	pInfo.nMax		= GetChCount() / (rc.Width() / 32);
	
	SetScrollInfo( SB_VERT, &pInfo, TRUE );
}

int CNetworkChild::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	SCROLLINFO pScroll;
	ZeroMemory( &pScroll, sizeof(pScroll) );
	pScroll.cbSize	= sizeof(pScroll);
	pScroll.fMask	= SIF_RANGE|SIF_PAGE|SIF_DISABLENOSCROLL;
	pScroll.nMin	= 0;
	pScroll.nMax	= 0; 
	pScroll.nPage	= 1;
	SetScrollInfo( SB_VERT, &pScroll, TRUE );
	
	return 0;
}

void CNetworkChild::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	SCROLLINFO pInfo;
	
	pInfo.cbSize	= sizeof(pInfo);
	pInfo.fMask		= SIF_ALL & ~SIF_TRACKPOS;
	
	GetScrollInfo( SB_VERT, &pInfo );
	int nDelta = pInfo.nPos;
	
	switch ( nSBCode )
	{
	case SB_BOTTOM:
		pInfo.nPos = pInfo.nMax - pInfo.nPage;
		break;
	case SB_LINEDOWN:
		pInfo.nPos ++;
		break;
	case SB_LINEUP:
		pInfo.nPos --;
		break;
	case SB_PAGEDOWN:
		pInfo.nPos += pInfo.nPage;
		break;
	case SB_PAGEUP:
		pInfo.nPos -= pInfo.nPage;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		pInfo.nPos = nPos;
		break;
	case SB_TOP:
		pInfo.nPos = 0;
		break;
	}
	
	pInfo.nPos = max( 0, min( pInfo.nPos, pInfo.nMax - (int)pInfo.nPage + 1 ) );
	if ( pInfo.nPos == nDelta ) return;
	
	SetScrollInfo( SB_VERT, &pInfo, TRUE );
	
	Invalidate();
}

BOOL CNetworkChild::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CNetworkChild::OnPaint() 
{
	CRect rc;
	GetClientRect( &rc );
	
	CPaintDC dc( this );
	dc.FillSolidRect( &rc, RGB( 250, 250, 255 ) );
	
	int nScroll = GetScrollPos( SB_VERT );
	DoPaint( &dc, rc, nScroll );
	
	UpdateScroll();
}

void CNetworkChild::DoPaint(CDC* pDC, CRect& rcClient, int nScroll)
{
	int nChFirst = nScroll * (rcClient.Width() / 32);
	for ( int nChan = nChFirst; nChan < GetChCount();  nChan++ )
	{
		int nCol = (nChan - nChFirst) % ( rcClient.Width() / 32 );
		int nRow = (nChan - nChFirst) / ( rcClient.Width() / 32 );
		
		if ( rcClient.top + nRow * 16 > rcClient.bottom ) break;
		
		DrawChImage( pDC, rcClient.left + nCol * 32, rcClient.top + nRow * 16, nChan, TRUE );
	}
}

void CNetworkChild::DrawChImage(CDC* pDC, int nX, int nY, int nChan, BOOL bGrid)
{
	CDC dcBuf;
	CBitmap bmBuf, *pbmOld;
	
	dcBuf.CreateCompatibleDC( pDC );
	bmBuf.CreateCompatibleBitmap( pDC, 32, 16 );

	pbmOld = (CBitmap*)dcBuf.SelectObject( &bmBuf );
	
	dcBuf.FillSolidRect( 0, 0, 31, 15, GetChColour( m_pChState[ nChan ] ) );
	if ( bGrid == TRUE )
	{
		dcBuf.Draw3dRect( 0, 16, 32, 0, RGB( 70, 70, 70 ), RGB( 70, 70, 70 ) );
		dcBuf.Draw3dRect( 32, 0, 0, 16, RGB( 70, 70, 70 ), RGB( 70, 70, 70 ) );
	}
	
	CFont* pOldFont = (CFont*)dcBuf.SelectObject( &CoolInterface.m_fntNormal );
	dcBuf.SetBkColor( GetChColour( m_pChState[ nChan ] ) );
	dcBuf.SetBkMode( TRANSPARENT );
	dcBuf.SetTextColor( CoolInterface.m_crCmdText );
	
	CString strText;
	strText.Format( "%i", nChan + m_nChFirst );
	
	CRect rc(1, 1, 31, 15);
	CSize sz = dcBuf.GetTextExtent( strText );
	CPoint pt = rc.CenterPoint();
	pt.x -= sz.cx / 2; pt.y -= sz.cy / 2 + 1;

	dcBuf.ExtTextOut( pt.x, pt.y, ETO_CLIPPED|ETO_OPAQUE, rc, strText, NULL );
	dcBuf.SelectObject( pOldFont );
	
	pDC->BitBlt( nX, nY, 32, 16, &dcBuf, 0, 0, SRCCOPY );
	
	dcBuf.SelectObject( pbmOld );
	bmBuf.DeleteObject();
	dcBuf.DeleteDC();
}

void CNetworkChild::DrawChImage(int nChan, COLORREF crColour)
{
	CRect rcClient;
	GetClientRect( &rcClient );
	if ( rcClient.IsRectEmpty() ) return;
	
	int nScroll = GetScrollPos( SB_VERT );
	int nChFirst = nScroll * (rcClient.Width() / 32);
	
	int nCol = (nChan - nChFirst) % ( rcClient.Width() / 32 );
	int nRow = (nChan - nChFirst) / ( rcClient.Width() / 32 );
	
	if ( rcClient.top + nRow * 16 > rcClient.bottom ) return;
	
	CClientDC dc( this );
	DrawChImage( &dc, rcClient.left + nCol * 32, rcClient.top + nRow * 16, nChan );
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkChild operations

int CNetworkChild::AddItem(CChannel* pChannel)
{
	ASSERT( pChannel->m_nChan > m_nChFirst );
	ASSERT( SsmGetChType( pChannel->m_nChan ) == m_nChType );
	
	if ( m_nChFirst == -1 ) m_nChFirst = pChannel->m_nChan;
	m_pChState = (BYTE*)realloc( m_pChState, ++m_nChCount );
	
	int nChState = SsmGetChState( pChannel->m_nChan );
	m_pChState[ m_nChCount - 1 ] = nChState;
	
	pChannel->SetOwner( this );
	
	return nChState;
}

void CNetworkChild::SetItem(int nChan, LPCTSTR pszText, int nType)
{
}

void CNetworkChild::SetItem(int nChan, int nChState)
{
	ASSERT( nChan >= m_nChFirst && nChan - m_nChFirst < GetChCount() );
	
	CClientDC dc( this );
	m_pChState[ nChan - m_nChFirst ] = nChState;
	DrawChImage( nChan - m_nChFirst, GetChColour( nChState ) );
}

CChannel* CNetworkChild::FindChannel(int nChState)
{
	int nCount = GetChCount();
	
	for ( int i = 0; i < nCount; i++ )
	{
		int nItem = m_nRetained + 1;
		m_nRetained = nItem = nItem % nCount;
		
		int nChan = nItem + m_nChFirst;
		if ( SsmGetChState( nChan ) != nChState ) continue;
		
		return Network.FindChannel( nChan );
	}
	
	return NULL;
}

int CNetworkChild::GetChStateCount(int nChState) const
{
	int nCount = 0;
	
	for ( int nItem = 0; nItem < GetChCount(); nItem++ )
	{
		int nChan = nItem + m_nChFirst;
		if ( SsmGetChState( nChan ) != nChState ) continue;
		
		nCount += 1;
	}
	
	return nCount;
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkChild Helpers

COLORREF CNetworkChild::GetChColour(DWORD nChState) const
{
	switch ( nChState )
	{
	case	S_CALL_STANDBY:
		return GetSysColor( COLOR_BTNFACE );//RGB( 192, 192, 192 );
		
	case	S_CALL_PICKUPED:
		return RGB( 255, 247, 88 );
		
	case	S_CALL_RINGING:
		return RGB( 249, 59, 55 );

	case	S_CALL_TALKING:
		return RGB( 0, 168, 0 );
		
	case	S_CALL_PENDING:
	case	S_CALL_RemoteBlock:
	case	S_CALL_LocalBlock:
		return RGB( 250, 250, 255 );
		
	case	S_CALL_WAIT_REMOTE_PICKUP:
		return RGB( 0, 125, 251 );
		
	case	S_CALL_UNAVAILABLE:
		return RGB( 222, 81, 42 );

	case	S_TUP_WaitPcmReset:
	case	S_ISUP_WaitReset:
		return RGB( 255, 0, 128 );
	
	default:
		return RGB( 192, rand(), 192 );
	}
}
