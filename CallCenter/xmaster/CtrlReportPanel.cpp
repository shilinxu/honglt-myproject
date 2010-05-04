//
// CtrlReportPanel.cpp
//

#include "stdafx.h"
#include "Xmaster.h"
#include "CtrlReportPanel.h"
#include "CoolInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CReportPanel, CTaskPanel)
BEGIN_MESSAGE_MAP(CReportPanel, CTaskPanel)
	//{{AFX_MSG_MAP(CReportPanel)
	ON_WM_CREATE()
	ON_MESSAGE(WM_REPORTSEARCH, OnReportSearch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CReportTaskBox, CTaskBox)
BEGIN_MESSAGE_MAP(CReportTaskBox, CTaskBox)
	//{{AFX_MSG_MAP(CReportTaskBox)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_REPORT_FAMILY, OnSelChangeFamily)
	ON_COMMAND(IDC_REPORT_SEARCH, OnSearch)
	ON_COMMAND(IDC_REPORT_CLEAR, OnClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CReportFilterBox, CTaskBox)
BEGIN_MESSAGE_MAP(CReportFilterBox, CTaskBox)
	//{{AFX_MSG_MAP(CReportFilterBox)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CReportResultBox, CTaskBox)
BEGIN_MESSAGE_MAP(CReportResultBox, CTaskBox)
	//{{AFX_MSG_MAP(CReportResultBox)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define BOX_MARGIN	10

/////////////////////////////////////////////////////////////////////////////
// CReportPanel construction

CReportPanel::CReportPanel()
{
}

CReportPanel::~CReportPanel()
{
}

/////////////////////////////////////////////////////////////////////////////
// CReportPanel message handlers

BOOL CReportPanel::Create(CWnd* pParentWnd)
{
	CRect rect;
	return CTaskPanel::Create( WS_VISIBLE, rect, pParentWnd, IDC_REPORT_PANEL );
}

int CReportPanel::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskPanel::OnCreate( lpCreateStruct ) == -1 )return -1;
	
	m_wndSearch.Create( this, 140, _T("帐单统计"), IDI_SEARCH_FOLDER );
	m_wndFilter.Create( this, 100, _T("个人信息"), IDI_SEARCH_FOLDER );
	m_wndResult.Create( this, 48, _T("查询结果"), IDI_SEARCH_FOLDER );
	
	AddBox( &m_wndSearch );
	AddBox( &m_wndFilter );
	AddBox( &m_wndResult );
	
//	SetStretchBox( &m_wndSearch );
	
	return 0;
}

void CReportPanel::ExecuteSearch()
{
	ExecuteClear();
	
	CString strCaption;
	m_wndResult.GetWindowText(strCaption);
	m_wndResult.SetCaption( "正在查询...");
	
	CWaitCursor pCursor;
	GetParent()->SendMessage( WM_COMMAND, ID_REPORT_SEARCH );
	m_wndResult.SetCaption( (LPCTSTR)strCaption );
}

void CReportPanel::ExecuteClear()
{
	GetParent()->SendMessage( WM_COMMAND, ID_REPORT_CLEAR );
	m_wndResult.ReportCount( 0, 0 );
}

LONG CReportPanel::OnReportSearch(WPARAM wParam, LPARAM lParam)
{
	m_wndResult.ReportCount( wParam, lParam );
	
	return 0;
}

CString CReportPanel::GetSearch()
{
	CString strQuery = m_wndSearch.GetSearch();
	CString strFilter= m_wndFilter.GetFilter();
	
	if ( ! strFilter.IsEmpty() )
	{
		strQuery += " AND " + strFilter;
	}

	return strQuery;
}

/////////////////////////////////////////////////////////////////////////////
// CReportTaskBox construction

CReportTaskBox::CReportTaskBox()
{
}

CReportTaskBox::~CReportTaskBox()
{
}

/////////////////////////////////////////////////////////////////////////////
// CReportTaskBox message handlers

int CReportTaskBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskBox::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	CRect rc( 0, 0, 0, 0 );
	
	m_wndFamily.Create( WS_CHILD|WS_VSCROLL|WS_VISIBLE|WS_TABSTOP|CBS_DROPDOWNLIST|CBS_HASSTRINGS,//|CBS_SORT,
		rc, this, IDC_REPORT_FAMILY );
	
	m_wndFamily.AddString( "查询当天信息");
	m_wndFamily.AddString( "查询当月帐单");
	m_wndFamily.AddString( "查询某月帐单");
	
	m_wndFamily.SetFont( &CoolInterface.m_fntNormal );
//	m_wndFamily.SendMessage( CB_SETDROPPEDWIDTH, 200 );
	
	m_wndObject.Create( WS_CHILD|WS_VISIBLE|WS_TABSTOP|DTS_RIGHTALIGN|DTS_UPDOWN|DTS_LONGDATEFORMAT,
		rc, this, IDC_REPORT_OBJECT );
	
	m_wndObject.SetFont( &CoolInterface.m_fntNormal );
	
	m_wndStart.Create( rc, this, IDC_REPORT_SEARCH );
	m_wndStart.SetWindowText( "开始查询" );
	m_wndStart.SetIcon( CoolInterface.m_pImages.ExtractIcon( 22 ) );
	m_wndStart.SetHandCursor( TRUE );
	
	m_wndClear.Create( rc, this, IDC_REPORT_CLEAR );
	m_wndClear.SetWindowText( "清除" );
	m_wndClear.SetIcon( CoolInterface.m_pImages.ExtractIcon( 7 ) );
	m_wndClear.SetHandCursor( TRUE );

	m_wndFamily.SetCurSel( 0 );
	SendMessage( WM_COMMAND, MAKELONG( IDC_REPORT_FAMILY, CBN_SELCHANGE ),
			(LPARAM)m_wndFamily.m_hWnd );
	
	SetPrimary( TRUE );
	
	return 0;
}

void CReportTaskBox::OnDestroy()
{
}

void CReportTaskBox::OnSize(UINT nType, int cx, int cy) 
{
	CTaskBox::OnSize( nType, cx, cy );
	
	HDWP hDWP = BeginDeferWindowPos( 4 );
	
	DeferWindowPos( hDWP, m_wndFamily, NULL, BOX_MARGIN, 27,
		cx - BOX_MARGIN * 2, 256, SWP_NOZORDER );
	DeferWindowPos( hDWP, m_wndObject, NULL, BOX_MARGIN, 71,
		cx - BOX_MARGIN * 2, 23, SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndStart, NULL, BOX_MARGIN, 105, 86, 24, SWP_SHOWWINDOW|SWP_NOZORDER );
	DeferWindowPos( hDWP, m_wndClear, NULL, BOX_MARGIN + 94, 105, 60, 24, SWP_SHOWWINDOW|SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CReportTaskBox::OnPaint() 
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
	pDC->ExtTextOut( rct.left, rct.top, nFlags, &rct, "请选择查询分类:", NULL );
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

void CReportTaskBox::OnSelChangeFamily()
{
	int nItem = m_wndFamily.GetCurSel();
	if ( CB_ERR == nItem ) return;
	
	if ( nItem == 0 )
	{
		m_wndObject.SetTime( COleDateTime::GetCurrentTime() );
		m_wndObject.SetFormat("yyyy年MM月dd日");
		m_wndObject.EnableWindow( FALSE );
	}
	else
	if ( nItem == 1 )
	{
		m_wndObject.SetTime( COleDateTime::GetCurrentTime() );
		m_wndObject.SetFormat("yyyy年MM月");
		m_wndObject.EnableWindow( FALSE );
	}
	else
	if ( nItem == 2 )
	{
		m_wndObject.SetFormat("yyyy年MM月");
		m_wndObject.EnableWindow( TRUE );
	}
}

void CReportTaskBox::OnSearch()
{
	CReportPanel* pPanel = (CReportPanel*)GetPanel();
	pPanel->ExecuteSearch();
}

void CReportTaskBox::OnClear()
{
	CReportPanel* pPanel = (CReportPanel*)GetPanel();
	pPanel->ExecuteClear();
}

CString CReportTaskBox::GetSearch()
{
	CString strQuery;
	strQuery.Empty();

	COleDateTime tValue;
	m_wndObject.GetTime( tValue );
	
	int nItem = m_wndFamily.GetCurSel();

	if ( nItem == 1 || nItem == 2 )
	{
		strQuery.Format( "DatePart(year,xTimeStart) = %i"
						" AND DatePart(month,xTimeStart) = %i",
						tValue.GetYear(), tValue.GetMonth() );
	}
	else
	//if ( nItem == 0 )
	{
		tValue = COleDateTime::GetCurrentTime();
		m_wndObject.SetTime( tValue );

		strQuery.Format( "Convert(char(6),xTimeStart,12) = '%s'",
					tValue.Format("%y%m%d") );
	}

	return strQuery;
}

/////////////////////////////////////////////////////////////////////////////
// CReportFilterBox construction

CReportFilterBox::CReportFilterBox()
{
}

CReportFilterBox::~CReportFilterBox()
{
}

/////////////////////////////////////////////////////////////////////////////
// CReportFilterBox message handlers

int CReportFilterBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskBox::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	CRect rc( 0, 0, 0, 0 );
	
	m_wndFilter.Create( WS_CHILD|WS_VSCROLL|WS_VISIBLE|WS_TABSTOP|CBS_DROPDOWN|CBS_SORT,
		rc, this, IDC_REPORT_FILTER );
	m_wndDialer.Create( WS_CHILD|WS_VSCROLL|WS_VISIBLE|WS_TABSTOP|CBS_DROPDOWN|CBS_SORT,
		rc, this, IDC_REPORT_DIALER );
	
	m_wndFilter.SetFont( &CoolInterface.m_fntNormal );
	m_wndFilter.AddString( "任意" );
	m_wndFilter.SetCurSel( 0 );
	
	m_wndDialer.SetFont( &CoolInterface.m_fntNormal );
	m_wndDialer.AddString( "任意" );
	m_wndDialer.SetCurSel( 0 );
	
	return 0;
}

void CReportFilterBox::OnDestroy()
{
}

void CReportFilterBox::OnSize(UINT nType, int cx, int cy) 
{
	CTaskBox::OnSize( nType, cx, cy );
	
	HDWP hDWP = BeginDeferWindowPos( 2 );
	
	DeferWindowPos( hDWP, m_wndFilter, NULL, BOX_MARGIN, 27,
		cx - BOX_MARGIN * 2, 256, SWP_NOZORDER );
	DeferWindowPos( hDWP, m_wndDialer, NULL, BOX_MARGIN, 71,
		cx - BOX_MARGIN * 2, 256, SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CReportFilterBox::OnPaint() 
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
	pDC->ExtTextOut( rct.left, rct.top, nFlags, &rct, "请输入要查询的个人号码:", NULL );
	pDC->ExcludeClipRect( &rct );

	rct.OffsetRect( 0, 54 - rct.top );
	pDC->ExtTextOut( rct.left, rct.top, nFlags, &rct, "请输入要查询的局端号码:", NULL );
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

CString CReportFilterBox::GetFilter(BOOL bPeek)
{
	CString strValue;
	CString strFilter;

	strFilter.Empty();

	m_wndFilter.GetWindowText( strValue );
	strValue.TrimLeft(); strValue.TrimRight();

	if ( strValue.IsEmpty() && bPeek == TRUE )
	{
		m_wndFilter.SetCurSel( 0 );
	}
	else if ( strValue != "任意" )
	{
		if ( ! strFilter.IsEmpty() ) strFilter += " and ";
		strFilter += "b.xphone = '"; strFilter += strValue; strFilter += '\'';
	}

	m_wndDialer.GetWindowText( strValue );
	strValue.TrimLeft(); strValue.TrimRight();

	if ( strValue.IsEmpty() && bPeek == TRUE )
	{
		m_wndDialer.SetCurSel( 0 );
	}
	else if ( strValue != "任意" )
	{
		if ( ! strFilter.IsEmpty() ) strFilter += " and ";
		strFilter += "a.xFwdPhone = '"; strFilter += strValue; strFilter += '\'';
	}

	return strFilter;
}

/////////////////////////////////////////////////////////////////////////////
// CReportResultBox construction

CReportResultBox::CReportResultBox()
{
	m_brBlue.CreateSolidBrush( CoolInterface.m_crTaskBoxClient );
}

CReportResultBox::~CReportResultBox()
{
	if ( m_brBlue.m_hObject )
	{
		m_brBlue.DeleteObject();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CReportResultBox message handlers

int CReportResultBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskBox::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	CRect rc( 0, 0, 0, 0 );

	m_wndTotal.Create( "", WS_CHILD|WS_VISIBLE, rc, this );
	m_wndTotal.SetFont( &CoolInterface.m_fntNormal );
	
	m_wndIgnore.Create( "", WS_CHILD|WS_VISIBLE, rc, this );
	m_wndIgnore.SetFont( &CoolInterface.m_fntNormal );
	
	return 0;
}

void CReportResultBox::OnDestroy()
{
}

void CReportResultBox::OnSize(UINT nType, int cx, int cy) 
{
	CTaskBox::OnSize( nType, cx, cy );
	
	HDWP hDWP = BeginDeferWindowPos( 2 );

	DeferWindowPos( hDWP, m_wndTotal, NULL, BOX_MARGIN + 106, BOX_MARGIN,
		cx - BOX_MARGIN * 2 - 106, 16, SWP_NOZORDER );
	DeferWindowPos( hDWP, m_wndIgnore, NULL, BOX_MARGIN + 106, BOX_MARGIN + 18,
		cx - BOX_MARGIN * 2 - 106, 16, SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CReportResultBox::OnPaint() 
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
	pDC->ExtTextOut( rct.left, rct.top, nFlags, &rct, "累计符合条件:", NULL );
	pDC->ExcludeClipRect( &rct );

	rct.OffsetRect( 0, 28 - rct.top );
	pDC->ExtTextOut( rct.left, rct.top, nFlags, &rct, "尚未计费业务:", NULL );
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

HBRUSH CReportResultBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = (HBRUSH)CTaskBox::OnCtlColor( pDC, pWnd, nCtlColor );
	
	pDC->SetBkColor( CoolInterface.m_crTaskBoxClient );
	
	if ( pWnd == &m_wndTotal || pWnd == &m_wndIgnore )
	{
		pDC->SetTextColor( RGB( 0, 0, 192 ) );
		pDC->SelectObject( &CoolInterface.m_fntBold );
	}

	return m_brBlue;
}

void CReportResultBox::ReportCount(int nCount, int nIgnore)
{
	CString strValue;
	strValue.Format( _T("%lu"), nCount );
	m_wndTotal.SetWindowText( strValue );
	
	strValue.Format( _T("%lu"), nIgnore );
	m_wndIgnore.SetWindowText( strValue );
}
