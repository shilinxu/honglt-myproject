//
// CtrlTelecomPanel.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Settings.h"
#include "CoolInterface.h"
#include "CtrlTelecomPanel.h"
//#include "DlgSettingsManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CTelecomPanel, CTaskPanel)
BEGIN_MESSAGE_MAP(CTelecomPanel, CTaskPanel)
	//{{AFX_MSG_MAP(CTelecomPanel)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/*
IMPLEMENT_DYNAMIC(CTelecomTaskBox, CTaskBox)
BEGIN_MESSAGE_MAP(CTelecomTaskBox, CTaskBox)
	//{{AFX_MSG_MAP(CTelecomTaskBox)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_COMMAND(IDC_TELECOM_START, OnStart)
	ON_COMMAND(IDC_TELECOM_CLEAR, OnClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define BOX_MARGIN	10
*/
/////////////////////////////////////////////////////////////////////////////
// CTelecomPanel construction

CTelecomPanel::CTelecomPanel()
{
}

CTelecomPanel::~CTelecomPanel()
{
}

/////////////////////////////////////////////////////////////////////////////
// CTelecomPanel message handlers

BOOL CTelecomPanel::Create(CWnd* pParentWnd)
{
	return CTaskPanel::Create( WS_VISIBLE, CRect(0,0,0,0), pParentWnd, IDC_TELECOM_PANEL );
}

int CTelecomPanel::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskPanel::OnCreate( lpCreateStruct ) == -1 )return -1;
	
	m_wndP2job.Create( this, 140, _T("任务"), IDI_TASKPANEL );
	
	AddBox( &m_wndP2job );
	
	return 0;
}
/*
/////////////////////////////////////////////////////////////////////////////
// CTelecomTaskBox construction

CTelecomTaskBox::CTelecomTaskBox()
{
}

CTelecomTaskBox::~CTelecomTaskBox()
{
}

/////////////////////////////////////////////////////////////////////////////
// CTelecomTaskBox message handlers

int CTelecomTaskBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskBox::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	CRect rc( 0, 0, 0, 0 );
	
	m_wndGroup.Create( WS_CHILD|WS_VSCROLL|WS_VISIBLE|WS_TABSTOP|CBS_DROPDOWNLIST|CBS_HASSTRINGS,//|CBS_SORT,
		rc, this, IDC_TELECOM_GROUP );
	m_wndGroup.SetFont( &CoolInterface.m_fntNormal );
//	m_wndGroup.SendMessage( CB_SETDROPPEDWIDTH, 200 );
	
	m_wndVoice.Create( WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,
		rc, this, IDC_TELECOM_VOICE );
	m_wndVoice.ModifyStyleEx( 0, WS_EX_CLIENTEDGE );
	m_wndVoice.SetFont( &CoolInterface.m_fntNormal );
	
	m_wndStart.Create( rc, this, IDC_TELECOM_START );
	m_wndStart.SetWindowText( "开始发布" );
	m_wndStart.SetIcon( CoolInterface.m_pImages.ExtractIcon( 22 ) );
	m_wndStart.SetHandCursor( TRUE );
	
	m_wndClear.Create( rc, this, IDC_TELECOM_CLEAR );
	m_wndClear.SetWindowText( "清除" );
	m_wndClear.SetIcon( CoolInterface.m_pImages.ExtractIcon( 7 ) );
	m_wndClear.SetHandCursor( TRUE );

//	m_wndFamily.SetCurSel( 0 );
//	SendMessage( WM_COMMAND, MAKELONG( IDC_REPORT_FAMILY, CBN_SELCHANGE ),
//			(LPARAM)m_wndFamily.m_hWnd );
	
	SetPrimary( TRUE );
	
	return 0;
}

void CTelecomTaskBox::OnDestroy()
{
}

void CTelecomTaskBox::OnSize(UINT nType, int cx, int cy) 
{
	CTaskBox::OnSize( nType, cx, cy );
	
	HDWP hDWP = BeginDeferWindowPos( 4 );
	
	DeferWindowPos( hDWP, m_wndGroup, NULL, BOX_MARGIN, 27,
		cx - BOX_MARGIN * 2, 23, SWP_NOZORDER );
	DeferWindowPos( hDWP, m_wndVoice, NULL, BOX_MARGIN, 71,
		cx - BOX_MARGIN * 2, 19, SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndStart, NULL, BOX_MARGIN, 105, 86, 24, SWP_SHOWWINDOW|SWP_NOZORDER );
	DeferWindowPos( hDWP, m_wndClear, NULL, BOX_MARGIN + 94, 105, 60, 24, SWP_SHOWWINDOW|SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CTelecomTaskBox::OnPaint() 
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
	pDC->ExtTextOut( rct.left, rct.top, nFlags, &rct, "号码列表:", NULL );
	pDC->ExcludeClipRect( &rct );

	rct.OffsetRect( 0, 54 - rct.top );
	pDC->ExtTextOut( rct.left, rct.top, nFlags, &rct, "输入日期或月份:", NULL );
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

void CTelecomTaskBox::OnStart()
{
	CTelecomPanel* pPanel = (CTelecomPanel*)GetPanel();
//	pPanel->ExecuteSearch();
}

void CTelecomTaskBox::OnClear()
{
	CTelecomPanel* pPanel = (CTelecomPanel*)GetPanel();
//	pPanel->ExecuteClear();
}
*/
