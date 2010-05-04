//
// CtrlNetworkPanel.cpp
//

#include "stdafx.h"
#include "Xmaster.h"
#include "CoolInterface.h"
#include "CtrlNetworkPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CNetworkPanel, CTaskPanel)
BEGIN_MESSAGE_MAP(CNetworkPanel, CTaskPanel)
	//{{AFX_MSG_MAP(CNetworkPanel)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CNetworkTaskBox, CTaskBox)
BEGIN_MESSAGE_MAP(CNetworkTaskBox, CTaskBox)
	//{{AFX_MSG_MAP(CNetworkTaskBox)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CNetworkTextBox, CTaskBox)
BEGIN_MESSAGE_MAP(CNetworkTextBox, CTaskBox)
	//{{AFX_MSG_MAP(CNetworkTextBox)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define BOX_MARGIN	10

/////////////////////////////////////////////////////////////////////////////
// CNetworkPanel construction

CNetworkPanel::CNetworkPanel()
{
}

CNetworkPanel::~CNetworkPanel()
{
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkPanel message handlers

BOOL CNetworkPanel::Create(CWnd* pParentWnd)
{
	return CTaskPanel::Create( WS_VISIBLE, CRect(0,0,0,0), pParentWnd, IDC_NETWORK_PANEL );
}

int CNetworkPanel::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskPanel::OnCreate( lpCreateStruct ) == -1 )return -1;
	
	m_wndTask.Create( this, 76, _T("任务"), IDI_TASKPANEL );
	m_wndText.Create( this, 54, _T("日志"), IDI_TASKPANEL );
	
	AddBox( &m_wndTask );
	AddBox( &m_wndText );
	
	return 0;
}

void CNetworkPanel::ExecuteCopy()
{
	GetParent()->SendMessage( WM_COMMAND, ID_TEXT_OPEN );
}

void CNetworkPanel::ExecuteClear()
{
	GetParent()->SendMessage( WM_COMMAND, ID_TEXT_CLEAR );
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkTaskBox construction

CNetworkTaskBox::CNetworkTaskBox()
{
	m_crWhite = CCoolInterface::GetDialogBkColor();
	m_brWhite.CreateSolidBrush( m_crWhite );
}

CNetworkTaskBox::~CNetworkTaskBox()
{
	if ( m_brWhite.m_hObject )
	{
		m_brWhite.DeleteObject();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkTaskBox message handlers

int CNetworkTaskBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskBox::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	CRect rc( 0, 0, 0, 0 );

	m_wndToday.Create( "0", WS_CHILD|WS_VISIBLE, rc, this );
	m_wndToday.SetFont( &CoolInterface.m_fntNormal );
	
	m_wndMonth.Create( "0", WS_CHILD|WS_VISIBLE, rc, this );
	m_wndMonth.SetFont( &CoolInterface.m_fntNormal );
	
	m_wndSearch.Create( "切换到帐单信息", WS_CHILD|WS_VISIBLE, rc, this );
	m_wndSearch.SetFont( &CoolInterface.m_fntNormal );
	
	SetPrimary( TRUE );
	SetTimer( 5, 1000, NULL );
	PostMessage( WM_TIMER, 5 );
	
	return 0;
}

void CNetworkTaskBox::OnDestroy()
{
	KillTimer( 5 );
}

void CNetworkTaskBox::OnSize(UINT nType, int cx, int cy) 
{
	CTaskBox::OnSize( nType, cx, cy );
	
	HDWP hDWP = BeginDeferWindowPos( 3 );

	DeferWindowPos( hDWP, m_wndToday, NULL, BOX_MARGIN + 106, BOX_MARGIN,
		cx - BOX_MARGIN * 2 - 106, 16, SWP_NOZORDER );
	DeferWindowPos( hDWP, m_wndMonth, NULL, BOX_MARGIN + 106, BOX_MARGIN + 18,
		cx - BOX_MARGIN * 2 - 106, 16, SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndSearch, NULL, BOX_MARGIN + 30, BOX_MARGIN + 44,
		cx - BOX_MARGIN * 2 - 8, 16, SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CNetworkTaskBox::OnPaint() 
{
	CPaintDC dc( this );
	CRect rc, rct;
	CString str;
	
	UINT nFlags = ETO_CLIPPED;
	CDC* pDC = &dc;
	
	GetClientRect( &rc );
	
	if ( m_bmWatermark.m_hObject != NULL )
	{
		pDC = CoolInterface.GetBuffer( dc, rc.Size() );
		CoolInterface.DrawWatermark( pDC, &rc, &m_bmWatermark );
		pDC->SetBkMode( TRANSPARENT );
	}
	else
	{
		pDC->SetBkMode( OPAQUE );
		pDC->SetBkColor( m_crWhite );
		nFlags |= ETO_OPAQUE;
	}
	
	CFont* pOldFont = (CFont*)pDC->SelectObject( &CoolInterface.m_fntNormal );
	
	pDC->SetTextColor( 0 );
	
	rct.SetRect( BOX_MARGIN, BOX_MARGIN + 1, rc.right - BOX_MARGIN, BOX_MARGIN + 16 );
	pDC->ExtTextOut( rct.left, rct.top, nFlags, &rct, "当天累计交易笔数:", NULL );
	pDC->ExcludeClipRect( &rct );

	rct.OffsetRect( 0, 28 - rct.top );
	pDC->ExtTextOut( rct.left, rct.top, nFlags, &rct, "当月累计交易笔数:", NULL );
	pDC->ExcludeClipRect( &rct );
	
	pDC->SelectObject( pOldFont );
	
	if ( pDC != &dc )
	{
		dc.BitBlt( 0, 0, rc.Width(), rc.Height(), pDC, 0, 0, SRCCOPY );
		pDC->SelectClipRgn( NULL );
	}
	else
	{
		pDC->FillSolidRect( &rc, m_crWhite );
	}
}

HBRUSH CNetworkTaskBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = (HBRUSH)CTaskBox::OnCtlColor( pDC, pWnd, nCtlColor );
	
	pDC->SetBkColor( m_crWhite );
	
	if ( pWnd == &m_wndToday || pWnd == &m_wndMonth )
	{
		pDC->SetTextColor( RGB( 0, 0, 255 ) );
		pDC->SelectObject( &CoolInterface.m_fntBold );
	}
	else
	if ( pWnd == &m_wndSearch )
	{
		pDC->SetTextColor( RGB( 0, 0, 255 ) );
		pDC->SelectObject( &CoolInterface.m_fntUnder );
	}

	return m_brWhite;
}

BOOL CNetworkTaskBox::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CPoint point;
	CRect rc;

	GetCursorPos( &point );
	m_wndSearch.GetWindowRect( &rc );

	if ( rc.PtInRect( point ) )
	{
		SetCursor( theApp.LoadCursor( IDC_HAND ) );
		return TRUE;
	}
	
	return CTaskBox::OnSetCursor( pWnd, nHitTest, message );
}

void CNetworkTaskBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CTaskBox::OnLButtonUp( nFlags, point );
	
	CRect rc;
	m_wndSearch.GetWindowRect( &rc );
	ScreenToClient( &rc );
	
	if ( rc.PtInRect( point ) )
	{
		theApp.m_pMainWnd->SendMessage( WM_COMMAND, ID_TAB_REPORT );
	}
}

void CNetworkTaskBox::OnTimer(UINT nIDEvent) 
{
	if ( m_pReport.CheckOut() )
	{
		if ( m_pReport.InitReport() )
		{
			LoadReport();
		}
	}
	
	CTaskBox::OnTimer(nIDEvent);
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkTaskBox History Report

BOOL CNetworkTaskBox::LoadReport()
{
	DWORD nTotal = m_pReport.GetTotal();
	DWORD nToday = m_pReport.GetToday();
	
	CString strValue;
	strValue.Format( _T("%lu"), nToday );
	m_wndToday.SetWindowText( strValue );
	
	strValue.Format( _T("%lu"), nTotal );
	m_wndMonth.SetWindowText( strValue );

	return TRUE;
}

void CNetworkTaskBox::Register(int nInk)
{
	m_pReport.Register( nInk );
	LoadReport();
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkTextBox construction

CNetworkTextBox::CNetworkTextBox()
{
	m_crWhite = CCoolInterface::GetDialogBkColor();
	m_brWhite.CreateSolidBrush( m_crWhite );
}

CNetworkTextBox::~CNetworkTextBox()
{
	if ( m_brWhite.m_hObject )
	{
		m_brWhite.DeleteObject();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkTextBox message handlers

int CNetworkTextBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskBox::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	CRect rc( 0, 0, 0, 0 );

	m_wndOpen.Create( "打开日志文件", WS_CHILD|WS_VISIBLE, rc, this );
	m_wndOpen.SetFont( &CoolInterface.m_fntNormal );
	
	m_wndClear.Create( "清空日志", WS_CHILD|WS_VISIBLE, rc, this );
	m_wndClear.SetFont( &CoolInterface.m_fntNormal );
	
	SetPrimary( TRUE );
	
	return 0;
}

void CNetworkTextBox::OnDestroy()
{
	KillTimer( 5 );
}

void CNetworkTextBox::OnSize(UINT nType, int cx, int cy) 
{
	CTaskBox::OnSize( nType, cx, cy );
	
	HDWP hDWP = BeginDeferWindowPos( 2 );

	DeferWindowPos( hDWP, m_wndOpen, NULL, BOX_MARGIN + 80, BOX_MARGIN,
		cx - BOX_MARGIN * 2 - 80, 16, SWP_NOZORDER );
	DeferWindowPos( hDWP, m_wndClear, NULL, BOX_MARGIN + 80, BOX_MARGIN + 22,
		cx - BOX_MARGIN * 2 - 80, 16, SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CNetworkTextBox::OnPaint() 
{
	CPaintDC dc( this );
	CRect rc, rct;
	CString str;
	
	UINT nFlags = ETO_CLIPPED;
	CDC* pDC = &dc;
	
	GetClientRect( &rc );
	
	if ( m_bmWatermark.m_hObject != NULL )
	{
		pDC = CoolInterface.GetBuffer( dc, rc.Size() );
		CoolInterface.DrawWatermark( pDC, &rc, &m_bmWatermark );
		pDC->SetBkMode( TRANSPARENT );
	}
	else
	{
		pDC->SetBkMode( OPAQUE );
		pDC->SetBkColor( m_crWhite );
		nFlags |= ETO_OPAQUE;
	}
	
	CFont* pOldFont = (CFont*)pDC->SelectObject( &CoolInterface.m_fntNormal );
	
	pDC->SetTextColor( 0 );
	
	rct.SetRect( BOX_MARGIN, BOX_MARGIN + 1, rc.right - BOX_MARGIN, BOX_MARGIN + 16 );
	pDC->ExtTextOut( rct.left, rct.top, nFlags, &rct, "日志输出:", NULL );
	pDC->ExcludeClipRect( &rct );

	rct.OffsetRect( 0, 31 - rct.top );
	pDC->ExtTextOut( rct.left, rct.top, nFlags, &rct, "日志清理:", NULL );
	pDC->ExcludeClipRect( &rct );
	
	pDC->SelectObject( pOldFont );
	
	if ( pDC != &dc )
	{
		dc.BitBlt( 0, 0, rc.Width(), rc.Height(), pDC, 0, 0, SRCCOPY );
		pDC->SelectClipRgn( NULL );
	}
	else
	{
		pDC->FillSolidRect( &rc, m_crWhite );
	}
}

HBRUSH CNetworkTextBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = (HBRUSH)CTaskBox::OnCtlColor( pDC, pWnd, nCtlColor );
	
	pDC->SetBkColor( m_crWhite );
	
	if ( pWnd == &m_wndOpen || pWnd == &m_wndClear )
	{
		pDC->SetTextColor( RGB( 0, 0, 255 ) );
		pDC->SelectObject( &CoolInterface.m_fntUnder );
	}

	return m_brWhite;
}

BOOL CNetworkTextBox::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CPoint point;
	CRect rc;

	GetCursorPos( &point );
	m_wndOpen.GetWindowRect( &rc );

	if ( rc.PtInRect( point ) )
	{
		SetCursor( theApp.LoadCursor( IDC_HAND ) );
		return TRUE;
	}

	GetCursorPos( &point );
	m_wndClear.GetWindowRect( &rc );

	if ( rc.PtInRect( point ) )
	{
		SetCursor( theApp.LoadCursor( IDC_HAND ) );
		return TRUE;
	}
	
	return CTaskBox::OnSetCursor( pWnd, nHitTest, message );
}

void CNetworkTextBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CTaskBox::OnLButtonUp( nFlags, point );
	
	CRect rc;
	m_wndOpen.GetWindowRect( &rc );
	ScreenToClient( &rc );
	
	if ( rc.PtInRect( point ) )
	{
		CNetworkPanel* pPanel = (CNetworkPanel*)GetPanel();
		pPanel->ExecuteCopy();
	}

	m_wndClear.GetWindowRect( &rc );
	ScreenToClient( &rc );
	
	if ( rc.PtInRect( point ) )
	{
		CNetworkPanel* pPanel = (CNetworkPanel*)GetPanel();
		pPanel->ExecuteClear();
	}
}
