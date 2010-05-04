//
// CtrlP2jobPanel.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Settings.h"
#include "CoolInterface.h"
#include "CtrlP2jobPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CP2jobPanel, CTaskPanel)
BEGIN_MESSAGE_MAP(CP2jobPanel, CTaskPanel)
	//{{AFX_MSG_MAP(CP2jobPanel)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CP2jobTaskBox, CTaskBox)
BEGIN_MESSAGE_MAP(CP2jobTaskBox, CTaskBox)
	//{{AFX_MSG_MAP(CP2jobTaskBox)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_COMMAND(IDC_P2JOB_START, OnStart)
	ON_COMMAND(IDC_P2JOB_CLEAR, OnClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define BOX_MARGIN	10

/////////////////////////////////////////////////////////////////////////////
// CP2jobPanel construction

CP2jobPanel::CP2jobPanel()
{
}

CP2jobPanel::~CP2jobPanel()
{
}

/////////////////////////////////////////////////////////////////////////////
// CP2jobPanel message handlers

BOOL CP2jobPanel::Create(CWnd* pParentWnd)
{
	return CTaskPanel::Create( WS_VISIBLE, CRect(0,0,0,0), pParentWnd, IDC_P2JOB_PANEL );
}

int CP2jobPanel::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskPanel::OnCreate( lpCreateStruct ) == -1 )return -1;
	
	m_wndTask.Create( this, 140, _T("任务"), IDI_TASKPANEL );
	
	AddBox( &m_wndTask );
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CP2jobTaskBox construction

CP2jobTaskBox::CP2jobTaskBox()
{
}

CP2jobTaskBox::~CP2jobTaskBox()
{
}

/////////////////////////////////////////////////////////////////////////////
// CP2jobTaskBox message handlers

int CP2jobTaskBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskBox::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	CRect rc( 0, 0, 0, 0 );
	
	m_wndPagStart.Create( WS_CHILD|WS_VISIBLE|WS_TABSTOP|DTS_RIGHTALIGN|DTS_UPDOWN|DTS_TIMEFORMAT,
		rc, this, IDC_TIME_START );
	m_wndPagStart.SetFont( &CoolInterface.m_fntNormal );
	m_wndPagStart.SetFormat( _T("HH时mm分") );
	
	m_wndPagClose.Create( WS_CHILD|WS_VISIBLE|WS_TABSTOP|DTS_RIGHTALIGN|DTS_UPDOWN|DTS_TIMEFORMAT,
		rc, this, IDC_TIME_CLOSE );
	m_wndPagClose.SetFont( &CoolInterface.m_fntNormal );
	m_wndPagClose.SetFormat( _T("HH时mm分") );
	
	m_wndStart.Create( rc, this, IDC_P2JOB_START );
	m_wndStart.SetWindowText( "开始发布" );
	m_wndStart.SetIcon( CoolInterface.m_pImages.ExtractIcon( 22 ) );
	m_wndStart.SetHandCursor( TRUE );
	
	m_wndClear.Create( rc, this, IDC_P2JOB_CLEAR );
	m_wndClear.SetWindowText( "清除" );
	m_wndClear.SetIcon( CoolInterface.m_pImages.ExtractIcon( 7 ) );
	m_wndClear.SetHandCursor( TRUE );
	
	COleDateTime tStart;
	tStart.ParseDateTime( Settings.P2job.PagStart, 0, VAR_TIMEVALUEONLY );
	if ( tStart.GetStatus() == COleDateTime::valid )
	{
		tStart.SetTime( tStart.GetHour(), tStart.GetMinute(), 0 );
		m_wndPagStart.SetTime( tStart );
	}
	
	COleDateTime tClose;
	tClose.ParseDateTime( Settings.P2job.PagClose, 0, VAR_TIMEVALUEONLY );
	if ( tClose.GetStatus() == COleDateTime::valid )
	{
		tClose.SetTime( tClose.GetHour(), tClose.GetMinute(), 0 );
		m_wndPagClose.SetTime( tClose );
	}
	
	SetPrimary( TRUE );
	
	return 0;
}

void CP2jobTaskBox::OnDestroy()
{
}

void CP2jobTaskBox::OnSize(UINT nType, int cx, int cy) 
{
	CTaskBox::OnSize( nType, cx, cy );
	
	HDWP hDWP = BeginDeferWindowPos( 4 );
	
	DeferWindowPos( hDWP, m_wndPagStart, NULL, BOX_MARGIN, 27,
		cx - BOX_MARGIN * 2, 23, SWP_NOZORDER );
	DeferWindowPos( hDWP, m_wndPagClose, NULL, BOX_MARGIN, 71,
		cx - BOX_MARGIN * 2, 19, SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndStart, NULL, BOX_MARGIN, 105, 86, 24, SWP_SHOWWINDOW|SWP_NOZORDER );
	DeferWindowPos( hDWP, m_wndClear, NULL, BOX_MARGIN + 94, 105, 60, 24, SWP_SHOWWINDOW|SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CP2jobTaskBox::OnPaint() 
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
	pDC->ExtTextOut( rct.left, rct.top, nFlags, &rct, "从:", NULL );
	pDC->ExcludeClipRect( &rct );

	rct.OffsetRect( 0, 54 - rct.top );
	pDC->ExtTextOut( rct.left, rct.top, nFlags, &rct, "到:", NULL );
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

void CP2jobTaskBox::OnStart()
{
	COleDateTime tt;
	m_wndPagStart.GetTime( tt );
	Settings.P2job.PagStart = tt.Format( "%H:%M" );
	m_wndPagClose.GetTime( tt );
	Settings.P2job.PagClose = tt.Format( "%H:%M" );
	
	CP2jobPanel* pPanel = (CP2jobPanel*)GetPanel();
	pPanel->GetParent()->SendMessage( WM_COMMAND, ID_P2JOB_START );
	
	m_wndStart.SetWindowText( "正在发布..." );
}

void CP2jobTaskBox::OnClear()
{
	CP2jobPanel* pPanel = (CP2jobPanel*)GetPanel();
	pPanel->GetParent()->SendMessage( WM_COMMAND, ID_P2JOB_CLEAR );
}
