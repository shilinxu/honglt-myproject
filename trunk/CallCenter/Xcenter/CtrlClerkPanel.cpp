//
// CtrlClerkPanel.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Settings.h"
#include "CoolInterface.h"
#include "CtrlClerkPanel.h"

#include "WndClerk.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CClerkPanel, CTaskPanel)
BEGIN_MESSAGE_MAP(CClerkPanel, CTaskPanel)
	//{{AFX_MSG_MAP(CClerkPanel)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CClerkTaskBox, CTaskBox)
BEGIN_MESSAGE_MAP(CClerkTaskBox, CTaskBox)
	//{{AFX_MSG_MAP(CClerkTaskBox)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_COMMAND(IDC_CLERK_START, OnStart)
	ON_COMMAND(IDC_CLERK_CLEAR, OnClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CClerkResultBox, CTaskBox)
BEGIN_MESSAGE_MAP(CClerkResultBox, CTaskBox)
	//{{AFX_MSG_MAP(CClerkResultBox)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_MESSAGE(WM_TIMEROUT, OnTimerOut)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define BOX_MARGIN	10

/////////////////////////////////////////////////////////////////////////////
// CClerkPanel construction

CClerkPanel::CClerkPanel()
{
}

CClerkPanel::~CClerkPanel()
{
}

/////////////////////////////////////////////////////////////////////////////
// CClerkPanel operations

void CClerkPanel::ExecuteStart()
{
	m_wndTask.SendMessage( WM_COMMAND, IDC_CLERK_START );
}

void CClerkPanel::ExecuteClear()
{
	m_wndTask.SendMessage( WM_COMMAND, IDC_CLERK_CLEAR );
}

void CClerkPanel::ExecuteFlowBack(DWORD nFlowCount)
{
	m_wndResult.m_wndClock.SetStyle( CEDClock::EClockStyle::XDC_FLOWBACK, nFlowCount );
}

/////////////////////////////////////////////////////////////////////////////
// CClerkPanel message handlers

BOOL CClerkPanel::Create(CWnd* pParentWnd)
{
	return CTaskPanel::Create( WS_VISIBLE, CRect(0,0,0,0), pParentWnd, IDC_CLERK_PANEL );
}

int CClerkPanel::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskPanel::OnCreate( lpCreateStruct ) == -1 )return -1;
	
	m_wndTask.Create( this, 112, _T("任务"), IDI_TASKPANEL );
	m_wndResult.Create( this, 48, _T("结果"), IDI_SEARCH_FOLDER );
	
	AddBox( &m_wndTask );
	AddBox( &m_wndResult );
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CClerkTaskBox construction

CClerkTaskBox::CClerkTaskBox()
{
	m_crClient = CoolInterface.m_crTaskBoxClient;
	m_brClient.CreateSolidBrush( m_crClient );
}

CClerkTaskBox::~CClerkTaskBox()
{
}

/////////////////////////////////////////////////////////////////////////////
// CClerkTaskBox message handlers

int CClerkTaskBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskBox::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	CRect rc( 0, 0, 0, 0 );
	
	m_wndMode.Create( WS_CHILD|WS_VSCROLL|WS_VISIBLE|WS_TABSTOP|CBS_DROPDOWNLIST|CBS_HASSTRINGS,//|CBS_SORT,
		rc, this, IDC_CLERK_MODE );
	
	m_wndMode.AddString( "立即执行");
	m_wndMode.SetFont( &CoolInterface.m_fntNormal );
	m_wndMode.SetCurSel( 0 );
	
	m_wndProfile.Create( "重新制定规则", WS_CHILD|WS_VISIBLE, rc, this );
	m_wndProfile.SetFont( &CoolInterface.m_fntNormal );
	m_wndProfile.EnableWindow( FALSE );
	
	m_wndStart.Create( rc, this, IDC_CLERK_START );
	m_wndStart.SetWindowText( "开始执行" );
	m_wndStart.SetIcon( CoolInterface.m_pImages.ExtractIcon( 22 ) );
	m_wndStart.SetHandCursor( TRUE );
	
	m_wndClear.Create( rc, this, IDC_CLERK_CLEAR );
	m_wndClear.SetWindowText( "停止" );
	m_wndClear.SetIcon( CoolInterface.m_pImages.ExtractIcon( 7 ) );
	m_wndClear.SetHandCursor( TRUE );
	
	SetPrimary( TRUE );
	
	return 0;
}

void CClerkTaskBox::OnDestroy()
{
}

void CClerkTaskBox::OnSize(UINT nType, int cx, int cy) 
{
	CTaskBox::OnSize( nType, cx, cy );
	
	HDWP hDWP = BeginDeferWindowPos( 4 );
	
	DeferWindowPos( hDWP, m_wndMode, NULL, BOX_MARGIN, 27,
		cx - BOX_MARGIN * 2, 256, SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndProfile, NULL, BOX_MARGIN + 42, BOX_MARGIN + 52,
		cx - BOX_MARGIN * 2 - 74, 16, SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndStart, NULL, BOX_MARGIN, 82, 86, 24, SWP_SHOWWINDOW|SWP_NOZORDER );
	DeferWindowPos( hDWP, m_wndClear, NULL, BOX_MARGIN + 94, 82, 60, 24, SWP_SHOWWINDOW|SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CClerkTaskBox::OnPaint() 
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
		pDC->SetBkColor( CoolInterface.m_crTaskBoxClient );
		nFlags |= ETO_OPAQUE;
	}
	
	CFont* pOldFont = (CFont*)pDC->SelectObject( &CoolInterface.m_fntNormal );
	
	pDC->SetTextColor( 0 );
	rct.SetRect( BOX_MARGIN + 1, BOX_MARGIN, rc.right - BOX_MARGIN, BOX_MARGIN + 16 );
	pDC->ExtTextOut( rct.left, rct.top, nFlags, &rct, "请选择执行方式:", NULL );
	pDC->ExcludeClipRect( &rct );
	
	pDC->SelectObject( pOldFont );
	
	if ( pDC != &dc )
	{
		dc.BitBlt( 0, 0, rc.Width(), rc.Height(), pDC, 0, 0, SRCCOPY );
		pDC->SelectClipRgn( NULL );
	}
	else
	{
		pDC->FillSolidRect( &rc, CoolInterface.m_crTaskBoxClient );
	}
}

HBRUSH CClerkTaskBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = (HBRUSH)CTaskBox::OnCtlColor( pDC, pWnd, nCtlColor );
	
	pDC->SetBkColor( CoolInterface.m_crTaskBoxClient );
	
	if ( pWnd == &m_wndProfile )
	{
		pDC->SetTextColor( RGB( 0, 0, 255 ) );
		pDC->SelectObject( &CoolInterface.m_fntUnder );
	}
	
	return m_brClient;
}

void CClerkTaskBox::OnStart()
{
	m_wndMode.EnableWindow( FALSE );
	m_wndStart.SetWindowText( "运行中...");
	m_wndStart.EnableWindow( FALSE );
	
	CClerkPanel* pPanel = (CClerkPanel*)GetPanel();
	((CClerkWnd*)pPanel->GetParent())->StartThread();
	((CClerkWnd*)pPanel->GetParent())->m_pWakeup.SetEvent();
}

void CClerkTaskBox::OnClear()
{
	if ( m_wndStart.IsWindowEnabled() ) return;
	
	m_wndMode.EnableWindow( TRUE );
	m_wndStart.SetWindowText( "开始执行");
	m_wndStart.EnableWindow( TRUE );
	
	CClerkPanel* pPanel = (CClerkPanel*)GetPanel();
	((CClerkWnd*)pPanel->GetParent())->StopThread();
	
	if ( CTaskBox* pTaskBox = GetPanel()->FindBox( RUNTIME_CLASS(CClerkResultBox) ) )
	{
		((CClerkResultBox*)pTaskBox)->m_wndClock.SetStyle( CEDClock::EClockStyle::XDC_NORMAL );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CClerkResultBox construction

CClerkResultBox::CClerkResultBox()
{
}

CClerkResultBox::~CClerkResultBox()
{
}

/////////////////////////////////////////////////////////////////////////////
// CClerkResultBox message handlers

int CClerkResultBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskBox::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	CRect rc( 0, 0, 0, 0 );
	
	m_wndClock.Create( WS_CHILD|WS_VISIBLE, rc, this, IDC_CLERK_CLOCK);
	
	SetTimer( 5, 1000, NULL );
	PostMessage( WM_TIMER, 5 );
	
	return 0;
}

void CClerkResultBox::OnDestroy()
{
}

void CClerkResultBox::OnSize(UINT nType, int cx, int cy) 
{
	CTaskBox::OnSize( nType, cx, cy );
	
	HDWP hDWP = BeginDeferWindowPos( 1 );
	
	DeferWindowPos( hDWP, m_wndClock, NULL, BOX_MARGIN + 49, 7,
		13 * 8, 23, SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CClerkResultBox::OnPaint() 
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
		pDC->SetBkColor( CoolInterface.m_crTaskBoxClient );
		nFlags |= ETO_OPAQUE;
	}
	
	CFont* pOldFont = (CFont*)pDC->SelectObject( &CoolInterface.m_fntNormal );
	
	pDC->SetTextColor( 0 );
	
	rct.SetRect( BOX_MARGIN + 1, BOX_MARGIN + 3, rc.right - BOX_MARGIN, BOX_MARGIN + 16 );
	pDC->ExtTextOut( rct.left, rct.top, nFlags, &rct, "您还剩:", NULL );
	pDC->ExcludeClipRect( &rct );
	
	pDC->SelectObject( pOldFont );
	
	if ( pDC != &dc )
	{
		dc.BitBlt( 0, 0, rc.Width(), rc.Height(), pDC, 0, 0, SRCCOPY );
		pDC->SelectClipRgn( NULL );
	}
	else
	{
		pDC->FillSolidRect( &rc, CoolInterface.m_crTaskBoxClient );
	}
}

LONG CClerkResultBox::OnTimerOut(WPARAM wParam, LPARAM lParam)
{
	m_wndClock.SetStyle( CEDClock::EClockStyle::XDC_FLOWNEXT );
	
	CClerkPanel* pPanel = (CClerkPanel*)GetPanel();
	((CClerkWnd*)pPanel->GetParent())->StartThread();
	((CClerkWnd*)pPanel->GetParent())->m_pWakeup.SetEvent();
	
	return 0;
}
