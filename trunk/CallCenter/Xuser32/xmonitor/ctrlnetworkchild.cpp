//
// CtrlNetworkChild.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "CoolInterface.h"
#include "CtrlNetworkChild.h"
#include "CtrlNetworkThumb.h"
#include "PacketBase.h"
#include "XML.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CNetworkChild, CTaskPanel)
BEGIN_MESSAGE_MAP(CNetworkChild, CTaskPanel)
	//{{AFX_MSG_MAP(CNetworkChild)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define CAPTION_HEIGHT	25

/////////////////////////////////////////////////////////////////////////////
// CNetworkChild construction

CNetworkChild::CNetworkChild(CNetworkTree* pPanel, LPCTSTR xGroupId) : m_pPanel( pPanel )
{
	m_pSelected		= NULL;
	m_xGroupId		= _tcsdup(xGroupId);
	
	m_nColumns		= 1;
	m_crPanelBack	= CoolInterface.m_crWindow;
	m_szBlock.cx	= m_nMargin + WINDOW_WIDTH;
	m_szBlock.cy	= m_nMargin + WINDOW_HEIGHT + CAPTION_HEIGHT;
}

CNetworkChild::~CNetworkChild()
{
	free( m_xGroupId ); m_xGroupId = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkChild operations

BOOL CNetworkChild::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd)
{
	dwStyle |= WS_CHILD | WS_VSCROLL;
	return CTaskPanel::Create( dwStyle, rect, pParentWnd, IDC_NETWORK_CHILD );
}

void CNetworkChild::UpdateScroll()
{
	CRect rc;
	GetClientRect( &rc );
	
	SCROLLINFO pInfo;
	pInfo.cbSize	= sizeof(pInfo);
	pInfo.fMask		= SIF_RANGE|SIF_PAGE|SIF_DISABLENOSCROLL;
	pInfo.nMin		= 0;
	pInfo.nPage		= rc.Height() - 1;
	pInfo.nMax		= ( GetBoxCount() + m_nColumns - 1 ) / m_nColumns * m_szBlock.cy;
	
	SetScrollInfo( SB_VERT, &pInfo, TRUE );
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkChild message handlers

int CNetworkChild::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	return 0;
}

void CNetworkChild::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize( nType, cx, cy );
	
	m_nColumns = cx / m_szBlock.cx;
	m_nColumns = max( m_nColumns, 1 );
	
	UpdateScroll();
	OnChanged();
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
	case SB_TOP:
		pInfo.nPos = 0;
		break;
	case SB_BOTTOM:
		pInfo.nPos = pInfo.nMax - 1;
		break;
	case SB_LINEUP:
		pInfo.nPos -= 16;
		break;
	case SB_LINEDOWN:
		pInfo.nPos += 16;
		break;
	case SB_PAGEUP:
		pInfo.nPos -= pInfo.nPage;
		break;
	case SB_PAGEDOWN:
		pInfo.nPos += pInfo.nPage;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		pInfo.nPos = nPos;
		break;
	}
	
	pInfo.fMask	= SIF_POS;
	pInfo.nPos	= max( 0, min( pInfo.nPos, GetScrollLimit( SB_VERT ) ) );
	
	if ( pInfo.nPos == nDelta ) return;
	SetScrollInfo( SB_VERT, &pInfo, TRUE );
	ScrollWindow( 0, nDelta - pInfo.nPos );
}

BOOL CNetworkChild::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	int nScrollWheelLines = 3;
	SystemParametersInfo ( SPI_GETWHEELSCROLLLINES, 0, &nScrollWheelLines, 0 );
	
	OnVScroll( SB_THUMBPOSITION, (int)( GetScrollPos( SB_VERT ) - zDelta / WHEEL_DELTA * nScrollWheelLines * 16 ), NULL );
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

void CNetworkChild::Layout(CRect& rcClient)
{
	int nScroll = GetScrollPos( SB_VERT );
	CPoint ptUp( rcClient.left, rcClient.top - nScroll );
	
	HDWP hDWP = BeginDeferWindowPos( GetBoxCount() );
	
	for ( POSITION posNext = GetBoxIterator(); posNext; )
	{
		CTaskBox* pWindow = GetNextBox( posNext );
		
		if ( ptUp.x + m_szBlock.cx > rcClient.right )
			ptUp.Offset( rcClient.left - ptUp.x, m_szBlock.cy );
		
		DeferWindowPos( hDWP, pWindow->GetSafeHwnd(), NULL, ptUp.x + m_nMargin,
			ptUp.y + m_nMargin, WINDOW_WIDTH, pWindow->GetOuterHeight(), SWP_SHOWWINDOW|SWP_NOZORDER );
		
		ptUp.Offset( WINDOW_WIDTH + m_nMargin, 0 );
	}
	
	EndDeferWindowPos( hDWP );
	m_bLayout = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkChild custom message handlers

void CNetworkChild::SelectTo(CNetworkThumb* pWindow)
{
	SetFocus();
	
	if ( m_pSelected ) m_pSelected->SetPrimary( FALSE );
	
	m_pSelected = pWindow;
	if ( pWindow ) pWindow->SetPrimary( TRUE );
}

BOOL CNetworkChild::AddUser(CXMLElement* pXML, CNetworkThumb*& pxVideo)
{
	ASSERT( pxVideo == NULL );
	ASSERT( pXML->IsNamed(ISyncMirrorView) );
	
	CString xUserId = pXML->GetAttributeValue( "UserId" );
	if ( pxVideo = FindUser(xUserId) ) return TRUE;
	
	pxVideo = new CNetworkThumb( this, xUserId );
	this->AddBox( pxVideo );
	SendMessage( WM_PAINT );
	
	if ( CXMLElement* pXML2 = pXML->GetFirstElement() )
	{
		SOCKADDR_IN& pHost = pxVideo->m_pHost;
		
		CXMLElement* pSync = pXML2->GetElementByName( "IP" );
		pHost.sin_addr.S_un.S_addr = inet_addr(  pSync->GetValue() );
		
		pSync = pXML2->GetElementByName( "Port" );
		pHost.sin_port	= htons( _ttoi( pSync->GetValue() ) );
		pHost.sin_family= PF_INET;
	}
	
	return TRUE;
}

CNetworkThumb* CNetworkChild::FindUser(LPCTSTR xUserId) const
{
	for ( POSITION posNext = GetBoxIterator(); posNext; )
	{
		CTaskBox* pWindow = GetNextBox( posNext );
		
		CString strCaption;
		pWindow->GetWindowText( strCaption );
		
		if ( strCaption.CompareNoCase( xUserId ) ) continue;
		if ( pWindow->IsKindOf( RUNTIME_CLASS(CNetworkThumb) ) ) return (CNetworkThumb*)pWindow;
	}
	
	return NULL;
}
