//
// CtrlMusicPanel.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "CtrlMusicPanel.h"
#include "CoolInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CMusicPanel, CTaskPanel)
BEGIN_MESSAGE_MAP(CMusicPanel, CTaskPanel)
	//{{AFX_MSG_MAP(CMusicPanel)
	ON_WM_CREATE()
	ON_MESSAGE(WM_REPORTSEARCH, OnReportSearch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CMusicTaskBox, CTaskBox)
BEGIN_MESSAGE_MAP(CMusicTaskBox, CTaskBox)
	//{{AFX_MSG_MAP(CMusicTaskBox)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_MUSIC_FAMILY, OnSelChangeFamily)
	ON_COMMAND(IDC_MUSIC_SEARCH, OnSearch)
	ON_COMMAND(IDC_MUSIC_CLEAR, OnClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CMusicFilterBox, CTaskBox)
BEGIN_MESSAGE_MAP(CMusicFilterBox, CTaskBox)
	//{{AFX_MSG_MAP(CMusicFilterBox)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CMusicResultBox, CTaskBox)
BEGIN_MESSAGE_MAP(CMusicResultBox, CTaskBox)
	//{{AFX_MSG_MAP(CMusicResultBox)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define BOX_MARGIN	10

/////////////////////////////////////////////////////////////////////////////
// CMusicPanel construction

CMusicPanel::CMusicPanel()
{
}

CMusicPanel::~CMusicPanel()
{
}

/////////////////////////////////////////////////////////////////////////////
// CMusicPanel message handlers

BOOL CMusicPanel::Create(CWnd* pParentWnd)
{
	CRect rect;
	return CTaskPanel::Create( WS_VISIBLE, rect, pParentWnd, IDC_MUSIC_PANEL );
}

int CMusicPanel::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskPanel::OnCreate( lpCreateStruct ) == -1 )return -1;
	
	m_wndSearch.Create( this, 140, _T("帐单统计"), IDI_SEARCH_FOLDER );
	m_wndFilter.Create( this, 100, _T("个人信息"), IDI_SEARCH_FOLDER );
	m_wndResult.Create( this, 32, _T("查询结果"), IDI_SEARCH_FOLDER );
	
	AddBox( &m_wndSearch );
	AddBox( &m_wndFilter );
	AddBox( &m_wndResult );
	
//	SetStretchBox( &m_wndSearch );
	
	return 0;
}

void CMusicPanel::ExecuteSearch()
{
	ExecuteClear();
	
	CString strCaption;
	m_wndResult.GetWindowText(strCaption);
	m_wndResult.SetCaption( "正在查询...");
	
	CWaitCursor pCursor;
	GetParent()->SendMessage( WM_COMMAND, ID_MUSIC_SEARCH );
	m_wndResult.SetCaption( (LPCTSTR)strCaption );
}

void CMusicPanel::ExecuteClear()
{
	GetParent()->SendMessage( WM_COMMAND, ID_MUSIC_CLEAR );
	m_wndResult.ReportCount( 0 );
}

LONG CMusicPanel::OnReportSearch(WPARAM wParam, LPARAM lParam)
{
	m_wndResult.ReportCount( wParam );
	
	return 0;
}

CString CMusicPanel::GetSearch(CString& strGroup)
{
	CString strQuery = m_wndSearch.GetSearch();
	CString strFilter= m_wndFilter.GetFilter(strGroup);
	
	if ( ! strFilter.IsEmpty() )
	{
		strQuery += " AND " + strFilter;
	}

	return strQuery;
}

/////////////////////////////////////////////////////////////////////////////
// CMusicTaskBox construction

CMusicTaskBox::CMusicTaskBox()
{
}

CMusicTaskBox::~CMusicTaskBox()
{
}

/////////////////////////////////////////////////////////////////////////////
// CMusicTaskBox message handlers

int CMusicTaskBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskBox::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	CRect rc( 0, 0, 0, 0 );
	
	m_wndFamily.Create( WS_CHILD|WS_VSCROLL|WS_VISIBLE|WS_TABSTOP|CBS_DROPDOWNLIST|CBS_HASSTRINGS,//|CBS_SORT,
		rc, this, IDC_MUSIC_FAMILY );
	
	m_wndFamily.AddString( "查询当天信息");
	m_wndFamily.AddString( "查询当月帐单");
	m_wndFamily.AddString( "查询某月帐单");
	
	m_wndFamily.SetFont( &CoolInterface.m_fntNormal );
//	m_wndFamily.SendMessage( CB_SETDROPPEDWIDTH, 200 );
	
	m_wndObject.Create( WS_CHILD|WS_VISIBLE|WS_TABSTOP|DTS_RIGHTALIGN|DTS_UPDOWN|DTS_LONGDATEFORMAT,
		rc, this, IDC_MUSIC_OBJECT );
	
	m_wndObject.SetFont( &CoolInterface.m_fntNormal );
	
	m_wndStart.Create( rc, this, IDC_MUSIC_SEARCH );
	m_wndStart.SetWindowText( "开始查询" );
	m_wndStart.SetIcon( CoolInterface.m_pImages.ExtractIcon( 22 ) );
	m_wndStart.SetHandCursor( TRUE );
	
	m_wndClear.Create( rc, this, IDC_MUSIC_CLEAR );
	m_wndClear.SetWindowText( "清除" );
	m_wndClear.SetIcon( CoolInterface.m_pImages.ExtractIcon( 7 ) );
	m_wndClear.SetHandCursor( TRUE );

	m_wndFamily.SetCurSel( 0 );
	SendMessage( WM_COMMAND, MAKELONG( IDC_MUSIC_FAMILY, CBN_SELCHANGE ),
			(LPARAM)m_wndFamily.m_hWnd );
	
	SetPrimary( TRUE );
	
	return 0;
}

void CMusicTaskBox::OnDestroy()
{
}

void CMusicTaskBox::OnSize(UINT nType, int cx, int cy) 
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

void CMusicTaskBox::OnPaint() 
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

void CMusicTaskBox::OnSelChangeFamily()
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

void CMusicTaskBox::OnSearch()
{
	CMusicPanel* pPanel = (CMusicPanel*)GetPanel();
	pPanel->ExecuteSearch();
}

void CMusicTaskBox::OnClear()
{
	CMusicPanel* pPanel = (CMusicPanel*)GetPanel();
	pPanel->ExecuteClear();
}

CString CMusicTaskBox::GetSearch()
{
	CString strQuery;
	strQuery.Empty();

	COleDateTime tValue;
	m_wndObject.GetTime( tValue );
	
	int nItem = m_wndFamily.GetCurSel();

	if ( nItem == 1 || nItem == 2 )
	{
		strQuery.Format( "DatePart(year,xopertime) = %i"
						" AND DatePart(month,xopertime) = %i",
						tValue.GetYear(), tValue.GetMonth() );
	}
	else
	//if ( nItem == 0 )
	{
		tValue = COleDateTime::GetCurrentTime();
		m_wndObject.SetTime( tValue );

		strQuery.Format( "Convert(char(6),xopertime,12) = '%s'",
					tValue.Format("%y%m%d") );
	}

	return strQuery;
}

/////////////////////////////////////////////////////////////////////////////
// CMusicFilterBox construction

CMusicFilterBox::CMusicFilterBox()
{
}

CMusicFilterBox::~CMusicFilterBox()
{
}

/////////////////////////////////////////////////////////////////////////////
// CMusicFilterBox message handlers

int CMusicFilterBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskBox::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	CRect rc( 0, 0, 0, 0 );
	
	m_wndFilter.Create( WS_CHILD|WS_VSCROLL|WS_VISIBLE|WS_TABSTOP|CBS_DROPDOWN|CBS_SORT,
		rc, this, IDC_MUSIC_FILTER );
	m_wndOrder.Create( WS_CHILD|WS_VSCROLL|WS_VISIBLE|WS_TABSTOP|CBS_DROPDOWNLIST|CBS_SORT,
		rc, this, IDC_MUSIC_ORDER );
	
	m_wndFilter.SetFont( &CoolInterface.m_fntNormal );
	m_wndFilter.AddString( "任意" );
	m_wndFilter.SetCurSel( 0 );
	
	m_wndOrder.SetFont( &CoolInterface.m_fntNormal );
	m_wndOrder.AddString( "查询点歌清单");
	m_wndOrder.AddString( "点歌次数排行");
	m_wndOrder.AddString( "点歌歌曲排行");
	m_wndOrder.SetCurSel( 0 );
	
	return 0;
}

void CMusicFilterBox::OnDestroy()
{
}

void CMusicFilterBox::OnSize(UINT nType, int cx, int cy) 
{
	CTaskBox::OnSize( nType, cx, cy );
	
	HDWP hDWP = BeginDeferWindowPos( 2 );
	
	DeferWindowPos( hDWP, m_wndFilter, NULL, BOX_MARGIN, 27,
		cx - BOX_MARGIN * 2, 256, SWP_NOZORDER );
	DeferWindowPos( hDWP, m_wndOrder, NULL, BOX_MARGIN, 71,
		cx - BOX_MARGIN * 2, 256, SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CMusicFilterBox::OnPaint() 
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
	pDC->ExtTextOut( rct.left, rct.top, nFlags, &rct, "请输入要查询的信息类型:", NULL );
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

CString CMusicFilterBox::GetFilter(CString& xGroup, BOOL bPeek)
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
		strFilter += "xcallphone = '"; strFilter += strValue; strFilter += '\'';
	}

	int nItem = m_wndOrder.GetCurSel();
	if ( nItem == 1 ) xGroup = "xcallphone";
	else if ( nItem == 2 ) xGroup = "xmusicname";
	else xGroup.Empty();

	return strFilter;
}

/////////////////////////////////////////////////////////////////////////////
// CMusicResultBox construction

CMusicResultBox::CMusicResultBox()
{
	m_brBlue.CreateSolidBrush( CoolInterface.m_crTaskBoxClient );
}

CMusicResultBox::~CMusicResultBox()
{
	if ( m_brBlue.m_hObject )
	{
		m_brBlue.DeleteObject();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMusicResultBox message handlers

int CMusicResultBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskBox::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	CRect rc( 0, 0, 0, 0 );

	m_wndTotal.Create( "", WS_CHILD|WS_VISIBLE, rc, this );
	m_wndTotal.SetFont( &CoolInterface.m_fntNormal );
	
	return 0;
}

void CMusicResultBox::OnDestroy()
{
}

void CMusicResultBox::OnSize(UINT nType, int cx, int cy) 
{
	CTaskBox::OnSize( nType, cx, cy );
	
	HDWP hDWP = BeginDeferWindowPos( 1 );

	DeferWindowPos( hDWP, m_wndTotal, NULL, BOX_MARGIN + 106, BOX_MARGIN,
		cx - BOX_MARGIN * 2 - 106, 16, SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CMusicResultBox::OnPaint() 
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

HBRUSH CMusicResultBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = (HBRUSH)CTaskBox::OnCtlColor( pDC, pWnd, nCtlColor );
	
	pDC->SetBkColor( CoolInterface.m_crTaskBoxClient );
	
	if ( pWnd == &m_wndTotal )
	{
		pDC->SetTextColor( RGB( 0, 0, 192 ) );
		pDC->SelectObject( &CoolInterface.m_fntBold );
	}

	return m_brBlue;
}

void CMusicResultBox::ReportCount(int nCount)
{
	CString strValue;
	strValue.Format( _T("%lu"), nCount );
	m_wndTotal.SetWindowText( strValue );
}
