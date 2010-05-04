//
// WndNetwork.cpp
//

#include "stdafx.h"
#include "Xmonitor.h"
#include "WndNetwork.h"
#include "CoolInterface.h"
#include "Settings.h"
#include "Profiles.h"
#include "Network.h"
#include "Skin.h"
#include "XML.h"

#include "ChannelMonitor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CNetworkWnd, CChildWnd)
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
#define PANEL_WIDTH		200
#define BAR_HEIGHT		28
#define SPLIT_SIZE		6

//////////////////////////////////////////////////////////////////////
// CNetworkWnd construction

CNetworkWnd::CNetworkWnd() : CChildWnd(TRUE)
{
	m_nText		= 102;
	m_pMonitor	= NULL;
	
	Create( IDR_HOMEFRAME );
}

CNetworkWnd::~CNetworkWnd()
{
	if ( m_pMonitor ) delete m_pMonitor;
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkWnd message handlers

int CNetworkWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CChildWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	SetOwner( GetParent() );
	
	// Panel

	m_wndPanel.Create( WS_VISIBLE, rectDefault, this );
	
	// Header
	
	if ( ! m_wndHeaderBar.Create( this, WS_CHILD|WS_VISIBLE|CBRS_NOALIGN, AFX_IDW_TOOLBAR ) ) return -1;
	m_wndHeaderBar.SetBarStyle( m_wndHeaderBar.GetBarStyle() | CBRS_TOOLTIPS|CBRS_BORDER_BOTTOM );
	
	Skin.CreateToolBar( _T("CNetworkWnd.Header"), &m_wndHeaderBar );
	m_wndHeaderBar.SetOwner( this );
	
	m_wndHeaderBar.SetIcon( CoolInterface.m_pImages.ExtractIcon( 22 ) );
	m_wndHeaderBar.SetCaption( "96031城市总机" );
	
	// Child
	
	m_wndChild.Create( WS_VISIBLE, rectDefault, this, IDC_NETWORK_CHILD );

	// CtrlText

	m_wndText.Create( WS_VISIBLE, rectDefault, this, IDC_NETWORK_TEXT );
	ShowStartupText();
	
//	CChannel* pChannel = new CCoreChannel;
//	pChannel->ConnectTo( Settings.Network.Host, Settings.Network.Port );
//	CXMLElement* pXML = Profiles.FindProcess( NULL );
	
	ASSERT( m_pMonitor == NULL );
	m_pMonitor = new CMonitorChannel;
	Network.SetChannel( m_pMonitor );
	
	m_pMonitor->SetOwner( &m_wndChild );
	m_pMonitor->LinkRestart();
	m_wndPanel.ExecuteMonitor();
	
	return 0;
}

void CNetworkWnd::OnDestroy() 
{
	CChildWnd::OnDestroy();
}

void CNetworkWnd::OnSize(UINT nType, int cx, int cy) 
{
	if ( nType != SIZE_INTERNAL ) CChildWnd::OnSize( nType, cx, cy );

	CRect rc;
	GetClientRect( &rc );
	
	if ( rc.Width() < 32 || rc.Height() < 32 ) return;
	
	HDWP hDWP = BeginDeferWindowPos( 3 );
	
	DeferWindowPos( hDWP, m_wndPanel.GetSafeHwnd(), NULL,
		rc.left, rc.top, PANEL_WIDTH, rc.Height(), SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndHeaderBar.GetSafeHwnd(), NULL,
		rc.left + PANEL_WIDTH, rc.top, rc.Width() - PANEL_WIDTH, BAR_HEIGHT, SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndChild.GetSafeHwnd(), NULL,
		rc.left + PANEL_WIDTH, rc.top + BAR_HEIGHT, rc.Width() - PANEL_WIDTH,
		rc.Height() - BAR_HEIGHT - SPLIT_SIZE - m_nText, SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndText.GetSafeHwnd(), NULL,
		rc.left + PANEL_WIDTH, rc.bottom - m_nText,
		rc.Width() - PANEL_WIDTH, m_nText, SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CNetworkWnd::OnPaint() 
{
	CPaintDC dc( this );
	CRect rcClient, rc;

	GetClientRect( &rcClient );
	
	rc.SetRect(	rcClient.left + PANEL_WIDTH,
				rcClient.bottom - m_nText - SPLIT_SIZE,
				rcClient.right,
				rcClient.bottom - m_nText );
	dc.FillSolidRect( rc.left, rc.top, rc.Width(), 1, GetSysColor( COLOR_BTNFACE ) );
	dc.FillSolidRect( rc.left, rc.top + 1, rc.Width(), 1, GetSysColor( COLOR_3DHIGHLIGHT ) );
	dc.FillSolidRect( rc.left, rc.bottom - 1, rc.Width(), 1, GetSysColor( COLOR_3DSHADOW ) );
	dc.FillSolidRect( rc.left, rc.top + 2, rc.Width(), rc.Height() - 3,
		GetSysColor( COLOR_BTNFACE ) );
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
	
	rc.SetRect(	rcClient.left + PANEL_WIDTH,
				rcClient.bottom - m_nText - SPLIT_SIZE,
				rcClient.right,
				rcClient.bottom - m_nText );
	
	if ( rc.PtInRect( point ) )
	{
		SetCursor( AfxGetApp()->LoadStandardCursor( IDC_SIZENS ) );
		return TRUE;
	}
	
	return CChildWnd::OnSetCursor( pWnd, nHitTest, message );
}

void CNetworkWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect rcClient, rc;
	
	GetClientRect( &rcClient );

	rc.SetRect(	rcClient.left + PANEL_WIDTH,
				rcClient.bottom - m_nText - SPLIT_SIZE,
				rcClient.right,
				rcClient.bottom - m_nText );
	
	if ( rc.PtInRect( point ) )
	{
		DoSizeText(); return;
	}
	
	CChildWnd::OnLButtonDown( nFlags, point );
}

BOOL CNetworkWnd::DoSizeText()
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

		if ( nOffset == 0xFFFF ) nOffset = m_nText - nSplit;
		nSplit += nOffset;

		nSplit = max( nSplit, 0 );
		nSplit = min( nSplit, rcClient.bottom - SPLIT_SIZE );

		if ( nSplit < 8 )
			nSplit = 0;
		if ( nSplit > rcClient.bottom - SPLIT_SIZE - 8 )
			nSplit = rcClient.bottom - SPLIT_SIZE;

		if ( nSplit != m_nText )
		{
			m_nText = nSplit;
			OnSize( SIZE_INTERNAL, 0, 0 );
			Invalidate();
		}
	}
	
	ReleaseCapture();
	ClipCursor( NULL );
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkWnd custom message handlers

BOOL CNetworkWnd::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if ( m_wndHeaderBar.m_hWnd )
	{
		if ( m_wndHeaderBar.OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) ) return TRUE;
	}
	
	return CMDIChildWnd::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
}

BOOL CNetworkWnd::LayoutThumb(CXMLElement* pXML)
{
#ifdef _DEBUG
	CFile pFile;
	if ( ! pFile.Open( "d:\\abc.xml", CFile::modeWrite|CFile::modeCreate ) ) return FALSE;
	
	CString strXML;
	strXML = pXML->ToString(TRUE,TRUE);
	
	pFile.Write( (LPCSTR)strXML, strXML.GetLength() );
	
	pFile.Close();
#endif
	
	m_wndChild.MapClassToXML( pXML );
	return TRUE;
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
	m_wndText.AddLine( MSG_SYSTEM, "欢迎使用城市热线班长席！作者：honglt 信箱：hong_lt@163.com" );
	//m_wndText.AddLine( MSG_SYSTEM, "" );
}
