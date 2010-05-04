//
// WndNetwork.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Network.h"
#include "Traffic.h"
#include "Neighbour.h"
#include "Settings.h"
#include "WndNetwork.h"

#include "ChannelUser.h"
#include "ChannelFax.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CNetworkWnd, CChildWnd, 0)

BEGIN_MESSAGE_MAP(CNetworkWnd, CChildWnd)
	//{{AFX_MSG_MAP(CNetworkWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define SIZE_INTERNAL	1982
#define HEADER_HEIGHT	16
#define PANEL_WIDTH		200
#define SPLIT_SIZE		6

/////////////////////////////////////////////////////////////////////////////
// CNetworkWnd construction

CNetworkWnd::CNetworkWnd() : CChildWnd(TRUE)
{
	m_nPanel		= 136;
	m_nChild		= 102;
	m_hChildWnd		= m_hDefaultChild = NULL;
	
	Create( IDR_HOMEFRAME );
}

CNetworkWnd::~CNetworkWnd()
{
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkWnd message handlers

int CNetworkWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CChildWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	SetOwner( GetParent() );
	
	SetRedraw( FALSE ); Invalidate( FALSE);
	
	// CtrlText
	
	m_wndText.Create( WS_VISIBLE, rectDefault, this, IDC_NETWORK_TEXT );
	
	// Child
	
	m_wndChild.Create( WS_VISIBLE, rectDefault, this, IDC_NETWORK_CHILD );
	m_hChildWnd = m_hDefaultChild = m_wndChild.GetSafeHwnd();
	
	// Device
	
	m_wndTree.Create( WS_CHILD|WS_VISIBLE, rectDefault, this, IDC_NETWORK_TREE );
	
	SetRedraw( TRUE ); Invalidate( TRUE);
	
#if 1
	ShowStartupText();
#endif	
	
	return 0;
}

void CNetworkWnd::OnDestroy() 
{
	Disconnect();

	CChildWnd::OnDestroy();
}

void CNetworkWnd::OnSize(UINT nType, int cx, int cy) 
{
	if ( nType != SIZE_INTERNAL ) CChildWnd::OnSize( nType, cx, cy );

	CRect rc;
	GetClientRect( &rc );

	if ( rc.Width() < 32 || rc.Height() < 32 ) return;
	
	if ( rc.Width() < m_nPanel + SPLIT_SIZE )
	{
		m_nPanel = max( 0, rc.Width() - SPLIT_SIZE );
	}

	HDWP hDWP = BeginDeferWindowPos( 3 );
	
	DeferWindowPos( hDWP, m_wndText.GetSafeHwnd(), NULL,
		rc.left, rc.top, rc.Width() - SPLIT_SIZE - m_nPanel,
		rc.Height() - SPLIT_SIZE - m_nChild, SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_hChildWnd, NULL,
		rc.left, rc.bottom - m_nChild, rc.Width(),
		m_nChild, SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndTree.GetSafeHwnd(), NULL,
		rc.right - m_nPanel, rc.top + HEADER_HEIGHT, m_nPanel,
		rc.Height() - HEADER_HEIGHT - SPLIT_SIZE - m_nChild, SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CNetworkWnd::OnPaint() 
{
	CPaintDC dc( this );
	CRect rcClient, rc;

	GetClientRect( &rcClient );
	
	rc.SetRect(	rcClient.left,
				rcClient.bottom - m_nChild - SPLIT_SIZE,
				rcClient.right,
				rcClient.bottom - m_nChild );
	dc.FillSolidRect( rc.left, rc.top, rc.Width(), 1, GetSysColor( COLOR_BTNFACE ) );
	dc.FillSolidRect( rc.left, rc.top + 1, rc.Width(), 1, GetSysColor( COLOR_3DHIGHLIGHT ) );
	dc.FillSolidRect( rc.left, rc.bottom - 1, rc.Width(), 1, GetSysColor( COLOR_3DSHADOW ) );
	dc.FillSolidRect( rc.left, rc.top + 2, rc.Width(), rc.Height() - 3,
		GetSysColor( COLOR_BTNFACE ) );

	rc.SetRect(	rcClient.right - m_nPanel - SPLIT_SIZE,
				rcClient.top,
				rcClient.right - m_nPanel,
				rcClient.bottom - SPLIT_SIZE - m_nChild );
	dc.FillSolidRect( rc.left, rc.top, 1, rc.Height() , GetSysColor( COLOR_BTNFACE ) );
	dc.FillSolidRect( rc.left + 1, rc.top, 1, rc.Height(), GetSysColor( COLOR_3DHIGHLIGHT ) );
	dc.FillSolidRect( rc.right - 1, rc.top, 1, rc.Height(), GetSysColor( COLOR_3DSHADOW ) );
	dc.FillSolidRect( rc.left + 2, rc.top, rc.Width() - 3, rc.Height(),
		GetSysColor( COLOR_BTNFACE ) );
	
	rc.SetRect(	rcClient.right - m_nPanel, rcClient.top,
				rcClient.right,	rcClient.top + HEADER_HEIGHT );
	if ( dc.RectVisible( &rc ) ) PaintPanelHeader( dc, rc );
}

void CNetworkWnd::PaintPanelHeader(CDC& dc, CRect& rcBar)
{
	LPCTSTR lpszText = _T("设备资源表");
	
	CPoint pt = rcBar.CenterPoint();
	CSize szText = dc.GetTextExtent( lpszText, lstrlen(lpszText) );
	pt.x -= szText.cx / 2; pt.y -= szText.cy / 2;
	dc.SetBkMode( OPAQUE );
	dc.SetBkColor( RGB( 0, 0, 0x80 ) );
	dc.SetTextColor( RGB( 0xFF, 0xFF, 0 ) );
	dc.ExtTextOut( pt.x, pt.y, ETO_OPAQUE|ETO_CLIPPED, &rcBar, lpszText, NULL );
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkWnd resizing behaviour

BOOL CNetworkWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CRect rcClient, rc;
	CPoint point;

	GetCursorPos( &point );
	GetClientRect( &rcClient );
	ClientToScreen( &rcClient );
	
	rc.SetRect(	rcClient.left,
				rcClient.bottom - m_nChild - SPLIT_SIZE,
				rcClient.right,
				rcClient.bottom - m_nChild );
	
	if ( rc.PtInRect( point ) )
	{
		SetCursor( AfxGetApp()->LoadStandardCursor( IDC_SIZENS ) );
		return TRUE;
	}

	rc.SetRect(	rcClient.right - m_nPanel - SPLIT_SIZE,
				rcClient.top,
				rcClient.right - m_nPanel,
				rcClient.bottom - SPLIT_SIZE - m_nChild );

	if ( rc.PtInRect( point ) )
	{
		SetCursor( AfxGetApp()->LoadStandardCursor( IDC_SIZEWE ) );
		return TRUE;
	}
	
	return CChildWnd::OnSetCursor( pWnd, nHitTest, message );
}

void CNetworkWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect rcClient, rc;
	
	GetClientRect( &rcClient );

	rc.SetRect(	rcClient.left,
				rcClient.bottom - m_nChild - SPLIT_SIZE,
				rcClient.right,
				rcClient.bottom - m_nChild );
	
	if ( rc.PtInRect( point ) )
	{
		DoSizeSession(); return;
	}
	
	rc.SetRect(	rcClient.right - m_nPanel - SPLIT_SIZE,
				rcClient.top,
				rcClient.right - m_nPanel,
				rcClient.bottom - SPLIT_SIZE - m_nChild );
	
	if ( rc.PtInRect( point ) )
	{
		DoSizePanel(); return;
	}
	
	CChildWnd::OnLButtonDown( nFlags, point );
}

BOOL CNetworkWnd::DoSizeSession()
{
	CRect rcClient;
	CPoint point;
	
	GetClientRect( &rcClient );
	ClientToScreen( &rcClient );
	ClipCursor( &rcClient );
	SetCapture();

	GetClientRect( &rcClient );
	
	int nOffset = 0xFFFF;
	
	while ( GetAsyncKeyState( VK_LBUTTON ) & 0x8000 )
	{
		MSG msg;
		while ( ::PeekMessage( &msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE ) );

		if ( ! AfxGetThread()->PumpMessage() )
		{
			AfxPostQuitMessage( 0 );
			break;
		}

		GetCursorPos( &point );
		ScreenToClient( &point );

		int nSplit = rcClient.bottom - point.y;

		if ( nOffset == 0xFFFF ) nOffset = m_nChild - nSplit;
		nSplit += nOffset;

		nSplit = max( nSplit, 0 );
		nSplit = min( nSplit, rcClient.bottom - SPLIT_SIZE );

		if ( nSplit < 8 )
			nSplit = 0;
		if ( nSplit > rcClient.bottom - SPLIT_SIZE - 8 )
			nSplit = rcClient.bottom - SPLIT_SIZE;

		if ( nSplit != m_nChild )
		{
			m_nChild = nSplit;
			OnSize( SIZE_INTERNAL, 0, 0 );
			Invalidate();
		}
	}
	
	ReleaseCapture();
	ClipCursor( NULL );
	
	return TRUE;
}

BOOL CNetworkWnd::DoSizePanel()
{
	CRect rcClient;
	CPoint point;
	
	GetClientRect( &rcClient );
	ClientToScreen( &rcClient );
	ClipCursor( &rcClient );
	SetCapture();

	GetClientRect( &rcClient );
	
	int nOffset = 0xFFFF;
	
	while ( GetAsyncKeyState( VK_LBUTTON ) & 0x8000 )
	{
		MSG msg;
		while ( ::PeekMessage( &msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE ) );

		if ( ! AfxGetThread()->PumpMessage() )
		{
			AfxPostQuitMessage( 0 );
			break;
		}

		GetCursorPos( &point );
		ScreenToClient( &point );

		int nSplit = rcClient.right - point.x;

		if ( nOffset == 0xFFFF ) nOffset = m_nPanel - nSplit;
		nSplit += nOffset;

		nSplit = max( nSplit, 0 );
		nSplit = min( nSplit, rcClient.right - SPLIT_SIZE );

		if ( nSplit < 8 )
			nSplit = 0;
		if ( nSplit > rcClient.right - SPLIT_SIZE - 8 )
			nSplit = rcClient.right - SPLIT_SIZE;

		if ( nSplit != m_nPanel )
		{
			m_nPanel = nSplit;
			OnSize( SIZE_INTERNAL, 0, 0 );
			Invalidate();
		}
	}
	
	ReleaseCapture();
	ClipCursor( NULL );
	
	return TRUE;
}

void CNetworkWnd::SelectChild(HWND hCtrlChild)
{
	if ( m_hChildWnd && ::IsWindow( m_hChildWnd ) )
	{
		::ShowWindow( m_hChildWnd, SW_HIDE );
	}
	
	if ( ! hCtrlChild ) hCtrlChild = m_hDefaultChild;
	
	::ShowWindow( hCtrlChild, SW_SHOWNORMAL );
	m_hChildWnd = hCtrlChild;
	OnSize( SIZE_INTERNAL, 0, 0 );
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkWnd Text operations

void CNetworkWnd::Message(int nType, LPCTSTR pszText)
{
	if ( nType == MSG_DEBUG && ! Settings.General.Debug ) return;

	CTime pNow = CTime::GetCurrentTime();
	CString strLine;
	strLine.Format( _T("[%.2i:%.2i:%.2i] %s"),
		pNow.GetHour(), pNow.GetMinute(), pNow.GetSecond(), pszText );
	m_wndText.AddLine( nType, strLine );
}

void CNetworkWnd::ShowStartupText()
{
	m_wndText.AddLine( MSG_SYSTEM, "" );
	m_wndText.AddLine( MSG_SYSTEM, ":    **********  ********    **********  ****  ****" );
	m_wndText.AddLine( MSG_SYSTEM, ":    **    **      **    **  **  **  **    **  **" );
	m_wndText.AddLine( MSG_SYSTEM, ":          **      **    **      **        **  **" );
	m_wndText.AddLine( MSG_SYSTEM, ":        **        ******        **          **" );
	m_wndText.AddLine( MSG_SYSTEM, ":      **          **    **      **        **  **" );
	m_wndText.AddLine( MSG_SYSTEM, ":      **    **    **    **      **        **  **" );
	m_wndText.AddLine( MSG_SYSTEM, ":    **********  ********      ******    ****  ****" );
	m_wndText.AddLine( MSG_SYSTEM, "" );

	m_wndText.AddLine( MSG_SYSTEM, "欢迎使用城市热线交换平台！作者：honglt 信箱：hong_lt@163.com" );
	m_wndText.AddLine( MSG_SYSTEM, "" );
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkWnd Channel operations

CNetworkChild* CNetworkWnd::AddChannel(CNetworkChild* pCtrlChild, CChannel* pChannel)
{
	BOOL bNewNode = ! pCtrlChild;
	int nChan = pChannel->m_nChan;
	int nChType = SsmGetChType( nChan );

	if ( ! bNewNode )
	{
		int nChType2 = pCtrlChild->m_nChType;
		if ( nChType2 != nChType ) bNewNode = TRUE;
	}

#if 0	// 发现通道处不同板卡，则建立新节点
	if ( ! bNewNode )
	{
		int nBId = -1, nBCh = -1;
		int nBId2 = -1, nBCh2 = -1;
		int nChFirst = pCtrlChild->m_nChFirst;
		
		SsmGetChHdInfo( nChFirst, &nBId2, &nBCh2 );
		SsmGetChHdInfo( nChan, &nBId, &nBCh );

		if ( nBId != nBId2 ) bNewNode = TRUE;
	}
#endif
	
	if ( TRUE == bNewNode )
	{
		pCtrlChild = m_wndTree.AddItem( nChType );
	}
	
	pCtrlChild->AddItem( pChannel );
	Network.SetChannel( pChannel );
	
	return pCtrlChild;
}

CChannel* CNetworkWnd::FindChannel(int nChType, int nChState) const
{
	HTREEITEM pItem = NULL;
	CNetworkChild* pCtrlChild = NULL;

	for ( pCtrlChild = FindChild(nChType, &pItem); pCtrlChild;
		  pCtrlChild = FindChild(nChType, &pItem) )
	{
		CChannel* pChannel = pCtrlChild->FindChannel( nChState );
		if ( pChannel != NULL ) return pChannel;
	}
		
	return NULL;
}

CNetworkChild* CNetworkWnd::FindChild(int nChType, HTREEITEM* pItem) const
{
	return m_wndTree.FindNext( nChType, pItem );
}

int CNetworkWnd::GetChStateCount(int nChType, int nChState) const
{
	int nCount = 0;
	HTREEITEM pItem = NULL;
	CNetworkChild* pCtrlChild = NULL;
	
	for ( pCtrlChild = m_wndTree.FindNext(nChType, &pItem); pCtrlChild;
		  pCtrlChild = m_wndTree.FindNext(nChType, &pItem) )
	{
		nCount += pCtrlChild->GetChStateCount( nChState );
	}
	
	return nCount;
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkWnd connection

BOOL CNetworkWnd::Connect()
{
	CNetworkChild* pCtrlChild = NULL;
	
	for ( int nChan = 0; nChan < Network.m_nMaxChan; nChan++ )
	{
		CChannel* pChannel = NULL;
		int nChType = SsmGetChType( nChan );

		if ( nChType == CHN_USER )
		pCtrlChild = AddChannel( pCtrlChild, new CUserChannel( nChan ) );
		else
		if ( nChType == CHN_FAX || nChType == CHN_SOFTFAX )
		pCtrlChild = AddChannel( pCtrlChild, new CFaxChannel( nChan ) );
		else
		pCtrlChild = AddChannel( pCtrlChild, new CChannel( nChan ) );
	}
	
	if ( ! Network.Connect() ) return FALSE;
	theApp.Message( MSG_SYSTEM, "成功加载设备" );
	m_wndTree.SelectFirst();
	
	if ( ! Neighbour.Connect() ) return FALSE;
	theApp.Message( MSG_SYSTEM, "成功加载坐席" );
	
	Traffic.Setup();
	theApp.Message( MSG_SYSTEM, "成功加载队列" );
	
	return TRUE;
}

void CNetworkWnd::Disconnect()
{
	Neighbour.Disconnect();
	Traffic.Release();
	
	Network.Disconnect();
	Network.Release();
	m_wndTree.Clear();
	
//	theApp.Message( MSG_SYSTEM, "成功断开设备" );
}
