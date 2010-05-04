//
// WndReport.cpp
//

#include "stdafx.h"
#include "Xmaster.h"
#include "WndReport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CReportWnd, CChildWnd, 0)

BEGIN_MESSAGE_MAP(CReportWnd, CChildWnd)
	//{{AFX_MSG_MAP(CReportWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
//	ON_COMMAND(ID_REPORT_PRINT, OnReportPrint)
	ON_COMMAND(ID_REPORT_SEARCH, OnReportSearch)
	ON_COMMAND(ID_REPORT_CLEAR, OnReportClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define SIZE_INTERNAL	1982
#define PANEL_WIDTH		200
#define SPLIT_SIZE		6

/////////////////////////////////////////////////////////////////////////////
// CReportWnd construction

CReportWnd::CReportWnd() : CChildWnd(FALSE)
{
	Create( IDR_REPORTFRAME );
}

CReportWnd::~CReportWnd()
{
}

/////////////////////////////////////////////////////////////////////////////
// CReportWnd message handlers

int CReportWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CChildWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	SetOwner( GetParent() );
	
	// Panel

	m_wndPanel.Create( this );

	// List
	
	m_wndList.Create( WS_CHILD|WS_VISIBLE|LVS_REPORT|LVS_NOLABELWRAP|WS_VSCROLL|WS_TABSTOP|LVS_SHOWSELALWAYS,
		rectDefault, this, IDC_REPORT_LIST );
	
	m_wndList.SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP );

	return 0;
}

void CReportWnd::OnDestroy() 
{
	CChildWnd::OnDestroy();
}

void CReportWnd::OnSize(UINT nType, int cx, int cy) 
{
	if ( nType != SIZE_INTERNAL ) CChildWnd::OnSize( nType, cx, cy );

	CRect rc;
	GetClientRect( &rc );

	HDWP hDWP = BeginDeferWindowPos( 2 );
	
	DeferWindowPos( hDWP, m_wndPanel.GetSafeHwnd(), NULL,
		rc.left, rc.top, PANEL_WIDTH, rc.Height(), SWP_NOZORDER );

	DeferWindowPos( hDWP, m_wndList.GetSafeHwnd(), NULL,
		rc.left + PANEL_WIDTH, rc.top, rc.Width() - PANEL_WIDTH, rc.Height(), SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CReportWnd::OnReportSearch() 
{
	int nIgnore = 0;
	CString strQuery = m_wndPanel.GetSearch();
	int nCount = m_wndList.LoadRecordset(strQuery, &nIgnore);
	
	m_wndPanel.SendMessage( WM_REPORTSEARCH, nCount, nIgnore );
}

void CReportWnd::OnReportClear() 
{
	m_wndList.ClearWindow();
}

void CReportWnd::OnReportPrint()
{
}
