//
// WndAgency.cpp
//

#include "stdafx.h"
#include "Xmonitor.h"
#include "WndAgency.h"
#include "CoolInterface.h"
#include "Skin.h"

#include "DlgAgencyProperties.h"
#include "DlgAgencyXproperty.h"
#include "DlgAgencyImport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CAgencyWnd, CChildWnd, 0)
BEGIN_MESSAGE_MAP(CAgencyWnd, CChildWnd)
	//{{AFX_MSG_MAP(CAgencyWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(IDC_AGENCY_SEARCH, OnQuickSearch)
	ON_COMMAND(IDC_AGENCY_CLEAR, OnQuickClear)
	ON_COMMAND(ID_AGENCY_ALBUM_NEW, OnAgencyAlbumNew)
	ON_COMMAND(ID_AGENCY_PROPERTIES, OnAgencyProperties)
	ON_COMMAND(ID_AGENCY_IMPORT, OnAgencyImport)
	ON_COMMAND(ID_AGENCY_EXPORT, OnAgencyExport)
	ON_UPDATE_COMMAND_UI(ID_AGENCY_XPROPERTY, OnUpdateAgencyXproperty)
	ON_COMMAND(ID_AGENCY_XPROPERTY, OnAgencyXproperty)
	ON_UPDATE_COMMAND_UI(ID_AGENCY_XREMOVE, OnUpdateAgencyXproperty)
	ON_COMMAND(ID_AGENCY_XREMOVE, OnAgencyXremove)
	ON_COMMAND(ID_AGENCY_XADD, OnAgencyXadd)
	ON_COMMAND(ID_AGENCY_XQUICK, OnAgencyXquick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define SIZE_INTERNAL	1982
#define PANEL_WIDTH		200
#define BAR_HEIGHT		28
#define SPLIT_SIZE		6

/////////////////////////////////////////////////////////////////////////////
// CAgencyWnd construction

CAgencyWnd::CAgencyWnd() : CChildWnd(FALSE)
{
	m_xsaleinfo	= NULL;
	
	Create( IDR_AGENCYFRAME );
}

CAgencyWnd::~CAgencyWnd()
{
	free( m_xsaleinfo ); m_xsaleinfo = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CAgencyWnd message handlers

int CAgencyWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CChildWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	// Header
	
	if ( ! m_wndHeaderBar.Create( this, WS_CHILD|WS_VISIBLE|CBRS_NOALIGN, AFX_IDW_TOOLBAR ) ) return -1;
	m_wndHeaderBar.SetBarStyle( m_wndHeaderBar.GetBarStyle() | CBRS_TOOLTIPS|CBRS_BORDER_BOTTOM );
	
	m_wndHeaderBar.SetIcon( CoolInterface.m_pImages.ExtractIcon( 22 ) );
	m_wndHeaderBar.SetCaption( _T("96031城市总机") );
	
	Skin.CreateToolBar( _T("CAgencyWnd.Header"), &m_wndHeaderBar );
	m_wndHeaderBar.CalcFixedLayout( FALSE, FALSE );
	
	// List
	
	m_wndList.Create( WS_CHILD|WS_VISIBLE, rectDefault, this, IDC_AGENCY_LIST );
	
	// Bottom
	
	if ( ! m_wndBottom.Create( this, WS_CHILD|WS_VISIBLE|CBRS_NOALIGN, AFX_IDW_TOOLBAR ) ) return -1;
	m_wndBottom.SetBarStyle( m_wndBottom.GetBarStyle() | CBRS_TOOLTIPS|CBRS_BORDER_TOP );
	
	if ( ! m_wndPhone.Create( WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL, rectDefault, &m_wndBottom, IDC_USER_NUMBER ) ) return -1;
	m_wndPhone.SetFont( &CoolInterface.m_fntNormal );
	
	Skin.CreateToolBar( _T("CAgencyWnd.Bottom"), &m_wndBottom );
	
	// Panel
	
	m_wndPanel.Create( WS_VISIBLE, rectDefault, this );
	m_wndPanel.ExecuteDefaultSearch( NULL );
	
	return 0;
}

void CAgencyWnd::OnDestroy() 
{
	CChildWnd::OnDestroy();
}

void CAgencyWnd::OnSize(UINT nType, int cx, int cy) 
{
	if ( nType != SIZE_INTERNAL ) CChildWnd::OnSize( nType, cx, cy );

	CRect rc;
	GetClientRect( &rc );
	
	if ( rc.Width() < 32 || rc.Height() < 32 ) return;
	
	HDWP hDWP = BeginDeferWindowPos( 4 );
	
	DeferWindowPos( hDWP, m_wndPanel.GetSafeHwnd(), NULL,
		rc.left, rc.top, PANEL_WIDTH, rc.Height(), SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndHeaderBar.GetSafeHwnd(), NULL,
		rc.left + PANEL_WIDTH, rc.top, rc.Width() - PANEL_WIDTH, BAR_HEIGHT, SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndList.GetSafeHwnd(), NULL,
		rc.left + PANEL_WIDTH, rc.top +BAR_HEIGHT, rc.Width() - PANEL_WIDTH, rc.Height() - BAR_HEIGHT * 2, SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndBottom.GetSafeHwnd(), NULL,
		rc.left + PANEL_WIDTH, rc.bottom - BAR_HEIGHT, rc.Width() - PANEL_WIDTH, BAR_HEIGHT, SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

BOOL CAgencyWnd::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if ( m_wndHeaderBar.m_hWnd )
	{
		if ( m_wndHeaderBar.OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) ) return TRUE;
	}
	if ( m_wndBottom.m_hWnd )
	{
		if ( m_wndBottom.OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) ) return TRUE;
	}
	
	return CChildWnd::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
}

/////////////////////////////////////////////////////////////////////////////
// CAgencyWnd menu GUI

void CAgencyWnd::OnUpdateAgencyXproperty(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( m_wndList.GetSelectedCount() > 0 );
}

void CAgencyWnd::OnAgencyXproperty()
{
	CString str;
	int nIndex = m_wndList.GetCurIndex( str );
	
	CAgencyXpropertyDlg dlg( (LPCTSTR)str );
	if ( dlg.DoModal() == IDCANCEL ) return;
	
	m_wndList.ResyncIndex( nIndex );
	m_wndList.ResyncItemWith( LVIS_SELECTED, nIndex );
}

void CAgencyWnd::OnAgencyXremove()
{
	m_wndList.RemoveItemOf( LVIS_SELECTED );
}

void CAgencyWnd::OnAgencyXadd()
{
	int nItem = m_wndList.AddNew( m_xsaleinfo );
	
	m_wndList.SelectIndex( nItem );
	PostMessage( WM_COMMAND, ID_AGENCY_XPROPERTY );
}

void CAgencyWnd::OnAgencyXquick()
{
	CString strValue;
	m_wndPhone.GetWindowText( strValue );
	
	strValue.TrimLeft(); strValue.TrimRight();
	if ( strValue.IsEmpty() ) return;
	
	int nItem = m_wndList.SeekByPhone( strValue );
}

/////////////////////////////////////////////////////////////////////////////
// CAgencyWnd custom message handlers

void CAgencyWnd::OnQuickSearch() 
{
	CString strQuery;
	m_wndPanel.GetSearch( strQuery );
	
	CString strFilter;
	strFilter = GetFilterString( strQuery, "xsaleinfo." );
	
	free( m_xsaleinfo ); m_xsaleinfo = NULL;
	if ( ! ReadGroupInfo( strFilter, m_xsaleinfo ) ) return;
	
	strFilter = GetFilterString( strQuery, "xsale." );
	if ( strFilter.GetLength() ) strFilter += " and ";
	CString str; str.Format( "xsaleinfo = '%s'", m_xsaleinfo );
	strFilter += str;
	
	m_wndList.LoadRecordset( strFilter );
}

void CAgencyWnd::OnQuickClear() 
{
	m_wndList.DeleteAllItems();
}

void CAgencyWnd::OnAgencyAlbumNew()
{
	CAgencyPropertiesDlg dlg( NULL );
	if ( dlg.DoModal() != IDOK ) return;
	
	ASSERT( m_xsaleinfo ); free( m_xsaleinfo );
	m_xsaleinfo = _tcsdup( dlg.m_xsaleinfo );
	
	m_wndPanel.ExecuteDefaultSearch( m_xsaleinfo );
}

void CAgencyWnd::OnAgencyProperties()
{
	CAgencyPropertiesDlg dlg( m_xsaleinfo );
	if ( dlg.DoModal() != IDOK ) return;
	
	m_wndPanel.ExecuteDefaultSearch();
}

void CAgencyWnd::OnAgencyImport()
{
	CAgencyImportDlg dlg( m_xsaleinfo );
	if ( dlg.DoModal() != IDOK ) return;
	
	m_wndPanel.ExecuteDefaultSearch();
}

void CAgencyWnd::OnAgencyExport()
{
	CWaitCursor pCursor;
	
	try
	{
		_ApplicationPtr pApplication;
		pApplication.CreateInstance( "Excel.Application" );
		pApplication->PutVisible( 0, VARIANT_TRUE );
		
		_WorkbookPtr pWorkbook;
		pWorkbook = pApplication->GetWorkbooks()->Add();
		
		ResetWorkbook( pWorkbook );
		m_wndList.PrintToWorkbook( pWorkbook );
	}
	catch (_com_error e)
	{
		AfxMessageBox( IDS_ERROR_DATABASE );
		return;
	}
}

BOOL CAgencyWnd::ReadGroupInfo(LPCTSTR lpszFilter, LPTSTR& xsi)
{
	try
	{
	CString strQuery;
	strQuery.Format( "select * from xsaleinfo" );
	
	ASSERT( lpszFilter && _tcslen(lpszFilter) );
	strQuery += CString(" where ") + lpszFilter;
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr xRecordset;
	xRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
#if 1
	if ( xRecordset->adoEOF )
	{
		AfxMessageBox( IDS_ERROR_NOAGENCY ); return FALSE;
	}
#endif
	
	_variant_t TheValue = xRecordset->GetCollect( "xjob" );
	m_wndHeaderBar.SetCaption( (TheValue.vt != VT_NULL) ? (LPCTSTR)(_bstr_t)TheValue : "未知的业务名称" );
	
	TheValue = xRecordset->GetCollect( "xsi" );
	xsi = _tcsdup( (_bstr_t)TheValue );
	}
	catch (_com_error e)
	{
		AfxMessageBox( IDS_ERROR_DATABASE ); return FALSE;
		return FALSE;
	}
	
	return TRUE;
}

CString CAgencyWnd::GetFilterString(LPCTSTR lpszFilter, LPCTSTR lpszReq) const
{
	CString strFilter;
	CString strValue = lpszFilter;
	
	for ( strValue += ';' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		if ( _tcsnicmp( str, lpszReq, _tcslen(lpszReq) ) == 0 )
		{
			if ( strFilter.GetLength() ) strFilter += " AND ";
			strFilter += str;
		}
	}
	
	return strFilter;
}

/////////////////////////////////////////////////////////////////////////////
// CAgencyWnd import/export handlers

void CAgencyWnd::ResetWorkbook(_WorkbookPtr pWorkbook)
{
	_WorksheetPtr pWorksheet = NULL;
	int nSheets = pWorkbook->Sheets->GetCount();
	
	for ( pWorksheet = pWorkbook->Sheets->GetItem( (long)nSheets );
		nSheets > 1 ; pWorksheet = pWorkbook->Sheets->GetItem( (long)nSheets ) )
	{
		pWorksheet->Delete(); nSheets --;
	}

	CString strCaption;
	m_wndHeaderBar.GetWindowText( strCaption );
	
	pWorksheet->PutName( (_bstr_t)strCaption );
}
