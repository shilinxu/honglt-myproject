//
// CtrlAgencyPanel.cpp
//

#include "stdafx.h"
#include "Xmonitor.h"
#include "CtrlAgencyPanel.h"
#include "CoolInterface.h"
#include "WndAgency.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CAgencyPanel, CTaskPanel)
BEGIN_MESSAGE_MAP(CAgencyPanel, CTaskPanel)
	//{{AFX_MSG_MAP(CAgencyPanel)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CAgencyTaskBox, CTaskBox)
BEGIN_MESSAGE_MAP(CAgencyTaskBox, CTaskBox)
	//{{AFX_MSG_MAP(CAgencyTaskBox)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_CBN_DROPDOWN(IDC_AGENCY_FAMILY, OnDropdownFamily)
	ON_CBN_CLOSEUP(IDC_AGENCY_FAMILY, OnCloseupFamily)
	ON_COMMAND(IDC_AGENCY_SEARCH, OnSearch)
	ON_COMMAND(IDC_AGENCY_CLEAR, OnClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define BOX_MARGIN	10

//////////////////////////////////////////////////////////////////////
// CAgencyPanel construction

CAgencyPanel::CAgencyPanel()
{
}

CAgencyPanel::~CAgencyPanel()
{
}

/////////////////////////////////////////////////////////////////////////////
// CAgencyPanel message handlers

BOOL CAgencyPanel::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd)
{
	return CTaskPanel::Create( dwStyle, rect, pParentWnd, IDC_AGENCY_PANEL );
}

int CAgencyPanel::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskPanel::OnCreate( lpCreateStruct ) == -1 )return -1;
	
	m_wndTask.Create( this, 140, _T("查询"), IDI_SEARCH_FOLDER );
	
	AddBox( &m_wndTask );
	
	SetFooter( NULL, TRUE );
	
	return 0;
}

void CAgencyPanel::ExecuteSearch()
{
	ExecuteClear();
	
	CWaitCursor pCursor;
	GetParent()->SendMessage( WM_COMMAND, IDC_AGENCY_SEARCH );
}

void CAgencyPanel::ExecuteDefaultSearch(LPCTSTR xsaleinfo)
{
	m_wndTask.SendMessage( WM_COMMAND, MAKELONG( IDC_AGENCY_FAMILY, CBN_DROPDOWN ),
			(LPARAM)m_wndTask.m_wndFamily.m_hWnd );
	
	if ( xsaleinfo == NULL )
	{
		m_wndTask.m_wndFamily.SetCurSel( 0 );
	}
	else
	{
		int nItem = m_wndTask.SeekByJobId( xsaleinfo );
		m_wndTask.m_wndFamily.SetCurSel( nItem - 1 );
	}
	
	m_wndTask.PostMessage( WM_COMMAND, IDC_AGENCY_SEARCH );
}

void CAgencyPanel::ExecuteClear()
{
	GetParent()->SendMessage( WM_COMMAND, IDC_AGENCY_CLEAR );
}

BOOL CAgencyPanel::GetSearch(CString& strFilter) const
{
	return m_wndTask.GetSearch( strFilter );
}

/////////////////////////////////////////////////////////////////////////////
// CAgencyTaskBox construction

CAgencyTaskBox::CAgencyTaskBox()
{
}

CAgencyTaskBox::~CAgencyTaskBox()
{
}

/////////////////////////////////////////////////////////////////////////////
// CAgencyTaskBox message handlers

int CAgencyTaskBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskBox::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	CRect rc( 0, 0, 0, 0 );
	
	m_wndFamily.Create( WS_CHILD|WS_VSCROLL|WS_VISIBLE|CBS_DROPDOWN|CBS_HASSTRINGS,//|CBS_SORT,
		rc, this, IDC_AGENCY_FAMILY );
	
	m_wndFamily.SetFont( &CoolInterface.m_fntNormal );
	m_wndFamily.SetCurSel(0);
	
	m_wndFilter.Create( WS_CHILD|WS_VSCROLL|WS_VISIBLE|CBS_DROPDOWN|CBS_HASSTRINGS,
		rc, this, IDC_AGENCY_FILTER );
	
	m_wndFilter.SetFont( &CoolInterface.m_fntNormal );
	m_wndFilter.AddString( "任意" );
	m_wndFilter.SetCurSel( 0 );
	
	m_wndStart.Create( rc, this, IDC_AGENCY_SEARCH );
	m_wndStart.SetWindowText( "开始查询" );
	m_wndStart.SetIcon( CoolInterface.m_pImages.ExtractIcon( 22 ) );
	m_wndStart.SetHandCursor( TRUE );
	
	m_wndClear.Create( rc, this, IDC_AGENCY_CLEAR );
	m_wndClear.SetWindowText( "清除" );
	m_wndClear.SetIcon( CoolInterface.m_pImages.ExtractIcon( 7 ) );
	m_wndClear.SetHandCursor( TRUE );
	
	SetPrimary( TRUE );
	
	return 0;
}

void CAgencyTaskBox::OnSize(UINT nType, int cx, int cy) 
{
	CTaskBox::OnSize( nType, cx, cy );
	
	HDWP hDWP = BeginDeferWindowPos( 4 );
	
	DeferWindowPos( hDWP, m_wndFamily, NULL, BOX_MARGIN, 27,
		cx - BOX_MARGIN * 2, 256, SWP_NOZORDER );
	DeferWindowPos( hDWP, m_wndFilter, NULL, BOX_MARGIN, 71,
		cx - BOX_MARGIN * 2, 256, SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndStart, NULL, BOX_MARGIN, 105, 86, 24, SWP_SHOWWINDOW|SWP_NOZORDER );
	DeferWindowPos( hDWP, m_wndClear, NULL, BOX_MARGIN + 94, 105, 60, 24, SWP_SHOWWINDOW|SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CAgencyTaskBox::OnPaint() 
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
	pDC->ExtTextOut( rct.left, rct.top, nFlags, &rct, "请选择业务名称:", NULL );
	pDC->ExcludeClipRect( &rct );
	
	rct.OffsetRect( 0, 54 - rct.top );
	pDC->ExtTextOut( rct.left, rct.top, nFlags, &rct, "输入营销员编号:", NULL );
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

void CAgencyTaskBox::OnDropdownFamily()
{
	m_wndFamily.ResetContent();
	LoadRecordset( &m_wndFamily );
}

void CAgencyTaskBox::OnCloseupFamily()
{
	if ( m_wndFamily.GetCurSel() == -1 )
	{
		CAgencyWnd* pAgencyWnd = (CAgencyWnd*)GetPanel()->GetParent();
		int nIndex = SeekByJobId( pAgencyWnd->m_xsaleinfo );
		
		m_wndFamily.SetCurSel( nIndex - 1 );
	}
}

void CAgencyTaskBox::OnSearch()
{
	CAgencyPanel* pPanel = (CAgencyPanel*)GetPanel();
	pPanel->ExecuteSearch();
}

void CAgencyTaskBox::OnClear()
{
	CAgencyPanel* pPanel = (CAgencyPanel*)GetPanel();
	pPanel->ExecuteClear();
}

BOOL CAgencyTaskBox::LoadRecordset(CComboBox* pCombo)
{
	try
	{
		CString strQuery;
		strQuery.Format( "select * from xsaleinfo"
			" where xtimeexpired >= getdate() or xtimeexpired is null" );
		TRACE( strQuery + "\n" );
		
		m_pRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
		
		for ( ; ! m_pRecordset->adoEOF ; m_pRecordset->MoveNext() )
		{
			_variant_t TheValue = m_pRecordset->GetCollect( "xjob" );
			if ( VT_NULL != TheValue.vt ) pCombo->AddString( (LPCTSTR)(_bstr_t)TheValue );
		}
		
	}
	catch (_com_error e)
	{
		return FALSE;
	}
	
	return TRUE;
}

BOOL CAgencyTaskBox::GetSearch(CString& strFilter) const
{
	CString strText;
	m_wndFamily.GetWindowText( strText );
		
	strText.TrimLeft(); strText.TrimRight();
	if ( strText.IsEmpty() ) return FALSE;
	
	strFilter = "xsaleinfo.xjob = '" + strText + "'";
	strFilter += ";" ;
	
	m_wndFilter.GetWindowText( strText );
	strText.TrimLeft(); strText.TrimRight();
	
	if ( ! strText.IsEmpty() && strText.CompareNoCase("任意") )
	{
		strFilter += "xsale.xuserid = '" + strText + "'";
		strFilter += ";" ;
	}
	
	return TRUE;
}

int CAgencyTaskBox::SeekByJobId(LPCTSTR xJobId)
{
	CString strQuery;
	
	strQuery.Format( "xsi = '%s'", xJobId );
	TRACE( strQuery + "\n" );
	
	try
	{
		m_pRecordset->MoveFirst();
		m_pRecordset->Find( (_bstr_t)strQuery, 0, adSearchForward );
		
		if ( m_pRecordset->adoEOF ) return -1;
		return m_pRecordset->AbsolutePosition;
	}
	catch (_com_error e)
	{
	}
	
	return -1;
}
