//
// CtrlMailPanel.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Settings.h"
#include "CoolInterface.h"
#include "CtrlMailPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CMailPanel, CTaskPanel)
BEGIN_MESSAGE_MAP(CMailPanel, CTaskPanel)
	//{{AFX_MSG_MAP(CMailPanel)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CMailTaskBox, CTaskBox)
BEGIN_MESSAGE_MAP(CMailTaskBox, CTaskBox)
	//{{AFX_MSG_MAP(CMailTaskBox)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_COMMAND(IDC_MAIL_CLEAR, OnClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMailPanel construction

CMailPanel::CMailPanel()
{
}

CMailPanel::~CMailPanel()
{
}

int CMailPanel::ReportItem(LPCTSTR xCatalog)
{
	return m_wndTask.ReportItem( xCatalog );
}

/////////////////////////////////////////////////////////////////////////////
// CMailPanel message handlers

BOOL CMailPanel::Create(CWnd* pParentWnd)
{
	return CTaskPanel::Create( WS_VISIBLE, CRect(0,0,0,0), pParentWnd, IDC_MAIL_PANEL );
}

int CMailPanel::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskPanel::OnCreate( lpCreateStruct ) == -1 )return -1;
	
	m_wndTask.Create( this, 112, _T("今日排行榜"), IDI_TASKPANEL );
	
	SetStretchBox( &m_wndTask );
	
	AddBox( &m_wndTask );
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMailTaskBox construction

CMailTaskBox::CMailTaskBox()
{
}

CMailTaskBox::~CMailTaskBox()
{
}

/////////////////////////////////////////////////////////////////////////////
// CMailTaskBox message handlers

int CMailTaskBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskBox::OnCreate( lpCreateStruct ) == -1 ) return -1;

	CRect rc( 0, 0, 0, 0 );
	
	m_wndList.Create( WS_CHILD|WS_VISIBLE|LVS_REPORT|LVS_NOLABELWRAP|WS_VSCROLL|WS_TABSTOP|LVS_SHOWSELALWAYS,
		rc, this, IDC_MAIL_LIST );
	
	m_wndList.SetExtendedStyle( LVS_EX_FULLROWSELECT/*|LVS_EX_HEADERDRAGDROP*/ );
	
	m_wndList.InsertColumn( 0, _T("类别"), LVCFMT_LEFT, 92 );
	m_wndList.InsertColumn( 1, _T("数量"), LVCFMT_LEFT, 56 );
	
	m_wndClear.Create( rc, this, IDC_MAIL_CLEAR );
	m_wndClear.SetWindowText( "清除..." );
	m_wndClear.SetIcon( CoolInterface.m_pImages.ExtractIcon( 11 ) );
	m_wndClear.SetHandCursor( TRUE );
	
	SetTimer( 5, 1000, NULL );
	PostMessage( WM_TIMER, 5 );
	
	SetPrimary( TRUE );
	
	return 0;
}

void CMailTaskBox::OnDestroy()
{
}

void CMailTaskBox::OnSize(UINT nType, int cx, int cy) 
{
	CTaskBox::OnSize( nType, cx, cy );
	
	CRect rc; GetClientRect( &rc );
	rc.top += 1;
	
	HDWP hDWP = BeginDeferWindowPos( 2 );
	
	DeferWindowPos( hDWP, m_wndList.GetSafeHwnd(), NULL,
		rc.left, rc.top, rc.Width(), rc.Height() - 34, SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndClear, NULL, 88, rc.bottom - 28, 80, 24, SWP_SHOWWINDOW|SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CMailTaskBox::OnPaint() 
{
	CTaskBox::OnPaint();
	CPaintDC dc( this );
	CRect rc;
	
	GetClientRect( &rc );
	
	dc.Draw3dRect( 0, 0, rc.right, 1,
		RGB( 128, 128, 128 ), RGB( 128, 128, 128 ) );
}

void CMailTaskBox::OnTimer(UINT nIDEvent) 
{
	CTime tNow = CTime::GetCurrentTime();
	DWORD tToday = tNow.GetYear() * 10000 + tNow.GetMonth() * 100 + tNow.GetDay();
	
	if ( m_tToday != tToday )
	{
		SendMessage( WM_COMMAND, IDC_MAIL_CLEAR );
		m_tToday = tToday;
	}
	
	CTaskBox::OnTimer(nIDEvent);
}

void CMailTaskBox::OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*)pNMHDR;
	
	if ( pDraw->nmcd.dwDrawStage == CDDS_PREPAINT )
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if ( pDraw->nmcd.dwDrawStage == CDDS_ITEMPREPAINT )
	{
		DWORD dwExisting	= m_wndList.GetItemState( pDraw->nmcd.dwItemSpec, 0xFFFF );
		BOOL bSelected		= ( dwExisting & LVIS_SELECTED ) ? TRUE : FALSE;
		
		if ( ! bSelected == TRUE ) pDraw->clrText = RGB( 34, 93, 217 );

		*pResult = CDRF_DODEFAULT;
	}
	else
	{
		*pResult = 0;
	}
}

void CMailTaskBox::OnClear()
{
	CSingleLock pLock( &m_pSection, TRUE );
	m_wndList.DeleteAllItems();
}

int CMailTaskBox::ReportItem(LPCTSTR xCatalog)
{
	for ( int nItem = m_wndList.GetNextItem( -1, LVNI_ALL ) ; nItem >= 0 ;
		nItem = m_wndList.GetNextItem( nItem, LVNI_ALL ) )
	{
		CString strText = m_wndList.GetItemText( nItem, 0 );
		if ( ! strText.CompareNoCase( xCatalog ) ) break;
	}
	
	if ( nItem == -1 )
	{
		nItem = m_wndList.InsertItem( m_wndList.GetItemCount(), xCatalog );
	}
	
	CString strText = m_wndList.GetItemText( nItem, 1 );
	int nCaCount = _ttoi( strText );
	strText.Format( "%i", ++nCaCount );
	m_wndList.SetItemText( nItem, 1, strText );
	
	return nCaCount;
}
