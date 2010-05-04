//
// WndPager.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "WndPager.h"
#include "CoolInterface.h"
#include "Skin.h"

#include "Settings.h"
#include "Network.h"
#include "Channel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CPagerWnd, CChildWnd, 0)

BEGIN_MESSAGE_MAP(CPagerWnd, CChildWnd)
	//{{AFX_MSG_MAP(CPagerWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST, OnCustomDrawList)
	ON_COMMAND(ID_P2JOB_SEARCH, OnSearchNumber)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define SIZE_INTERNAL	1982
#define PANEL_WIDTH		200
#define BAR_HEIGHT		28

#define STATE_SUCCEED	(1<<12)
#define STATE_FAILED	(1<<13)

#define MAX_MESSAGE_COUNT	1024

/////////////////////////////////////////////////////////////////////////////
// CPagerWnd construction

CPagerWnd::CPagerWnd() : CChildWnd(FALSE)
{
	m_pRecordset	= NULL;
	m_bIsOpen		= FALSE;

	Create( IDR_PAGERFRAME, FALSE );
}

CPagerWnd::~CPagerWnd()
{
}

/////////////////////////////////////////////////////////////////////////////
// CPagerWnd message handlers

int CPagerWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CChildWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	SetOwner( GetParent() );
	
	// Panel
	
	m_wndPanel.Create( this );
	
	// ListCtrl
	
	m_wndList.Create( WS_CHILD|WS_VISIBLE|LVS_REPORT|LVS_NOLABELWRAP|WS_VSCROLL|WS_TABSTOP|LVS_SHOWSELALWAYS,
		rectDefault, this, IDC_LIST );
	
	m_wndList.SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP );
	m_wndList.SetImageList( &CoolInterface.m_pImages, LVSIL_SMALL );
	
	m_wndList.InsertColumn( 0, _T("发生时间"), LVCFMT_LEFT, 120 );
	m_wndList.InsertColumn( 1, _T("被叫号码"), LVCFMT_LEFT,  94 );
	m_wndList.InsertColumn( 2, _T("事件描述"), LVCFMT_LEFT, 250 );
	m_wndList.InsertColumn( 3, _T("主叫号码"), LVCFMT_LEFT,  94 );
	m_wndList.InsertColumn( 4, _T("寿命"), LVCFMT_LEFT,  94 );
	m_wndList.InsertColumn( 5, _T("状态"), LVCFMT_LEFT,  80 );
	m_wndList.InsertColumn( 6, _T("流水号"), LVCFMT_LEFT,  80 );
	
	// Toolbar
	
	if ( ! m_wndToolBar.Create( this, WS_CHILD|WS_VISIBLE|CBRS_NOALIGN, AFX_IDW_TOOLBAR ) ) return -1;
	m_wndToolBar.SetBarStyle( m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS|CBRS_BORDER_TOP );
	m_wndToolBar.SetOwner( this );
	
	if ( ! m_wndSearch.Create( WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL, rectDefault, &m_wndToolBar, IDC_SEARCH_NUMBER ) ) return -1;
	m_wndSearch.SetFont( &CoolInterface.m_fntNormal );
	
	Skin.CreateToolBar( _T("CPagerWnd"), &m_wndToolBar );
	
	SetTimer( 5, 6000, NULL );

	return 0;
}

void CPagerWnd::OnDestroy() 
{
	CChildWnd::OnDestroy();
}

void CPagerWnd::OnSize(UINT nType, int cx, int cy) 
{
	if ( nType != SIZE_INTERNAL ) CChildWnd::OnSize( nType, cx, cy );

	CRect rc;
	GetClientRect( &rc );

	HDWP hDWP = BeginDeferWindowPos( 3 );
	
	DeferWindowPos( hDWP, m_wndPanel.GetSafeHwnd(), NULL,
		rc.left, rc.top, PANEL_WIDTH, rc.Height(), SWP_NOZORDER );

	DeferWindowPos( hDWP, m_wndList.GetSafeHwnd(), NULL,
		rc.left + PANEL_WIDTH, rc.top, rc.Width() - PANEL_WIDTH, rc.Height() - BAR_HEIGHT, SWP_NOZORDER );
	DeferWindowPos( hDWP, m_wndToolBar.GetSafeHwnd(), NULL,
		rc.left + PANEL_WIDTH, rc.bottom - BAR_HEIGHT, rc.Width() - PANEL_WIDTH, BAR_HEIGHT, SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

/////////////////////////////////////////////////////////////////////////////
// CPagerWnd common timer

void CPagerWnd::OnTimer(UINT nIDEvent) 
{
	if ( nIDEvent == 5 ) OnRunPager();
	
	CChildWnd::OnTimer( nIDEvent );
}

/////////////////////////////////////////////////////////////////////////////
// CPagerWnd custom message handlers

void CPagerWnd::OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult)
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
		
		if ( bSelected == TRUE )
		{
			pDraw->clrText   = GetSysColor(COLOR_HIGHLIGHTTEXT);
		}
		else if ( (dwExisting & STATE_FAILED) == STATE_FAILED )
		{
			pDraw->clrText = RGB( 169, 0, 0 );
		}
		else
		if ( (dwExisting & STATE_SUCCEED) == STATE_SUCCEED )
		{
			pDraw->clrText = GetSysColor( COLOR_WINDOWTEXT );
		}

		*pResult = CDRF_DODEFAULT;
	}
	else
	{
		*pResult = 0;
	}
}

void CPagerWnd::OnSearchNumber() 
{
	CString strText;
	m_wndSearch.GetWindowText( strText );
	strText.TrimLeft(); strText.TrimRight();
	
	BOOL bScroll = FALSE;
	
	for ( int nDeselect = m_wndList.GetNextItem( -1, LVNI_SELECTED ) ;
		nDeselect != -1 ; nDeselect = m_wndList.GetNextItem( nDeselect, LVNI_SELECTED ) )
	{
		m_wndList.SetItemState( nDeselect, 0, LVIS_SELECTED );
	}
	
	for ( int nItem = 0; nItem < m_wndList.GetItemCount(); nItem ++ )
	{
		CString str = m_wndList.GetItemText( nItem, 1 );
		if ( _tcsnistr( str, strText, strText.GetLength() ) )
		{
			m_wndList.SetItemState( nItem, LVIS_SELECTED, LVIS_SELECTED );
			if ( ! bScroll ) m_wndList.EnsureVisible( nItem, FALSE == (bScroll = TRUE) );
		}
#if 1
		str = m_wndList.GetItemText( nItem, 3 );
		if ( _tcsnistr( str, strText, strText.GetLength() ) )
		{
			m_wndList.SetItemState( nItem, LVIS_SELECTED, LVIS_SELECTED );
			if ( ! bScroll ) m_wndList.EnsureVisible( nItem, FALSE == (bScroll = TRUE) );
		}
#endif
	}
}

//////////////////////////////////////////////////////////////////////
// CPagerWnd pager run

BOOL CPagerWnd::OnRunPager()
{
//#ifndef _DEBUG
	try
//#endif
	{
		CSingleLock pLock( &Network.m_pSection );
		if ( ! pLock.Lock( 250 ) ) return FALSE;
			
		if ( ! VerifyRecordset() ) return FALSE;
		if ( m_pRecordset->adoEOF ) return TRUE;
		
		if ( ! DoRunPager() ) m_bIsOpen = FALSE;
#ifdef _DEBUG
		KillTimer( 5 );
#endif
	}
//#ifndef _DEBUG
	catch(_com_error e) 
	{ 
		theApp.Message( MSG_ERROR, "error on CPagerWnd::OnRunPager: %s", e.ErrorMessage() );
		return m_bIsOpen = FALSE;
	}
	catch(...)
	{
		theApp.Message( MSG_ERROR, "error on CPagerWnd::OnRunPager: %s", "a fatal error" );
		return FALSE;
	}
//#endif
	
	return TRUE;
}

BOOL CPagerWnd::DoRunPager()
{
	_variant_t xRemoteId = m_pRecordset->GetCollect( "xPhoneNum" );
	_variant_t xFeeNumber = m_pRecordset->GetCollect( "xFeeNumber" );

	if ( VT_NULL == xFeeNumber.vt ) xFeeNumber = m_pRecordset->GetCollect( "xCaller" );
	
	HINSTANCE hInstance = LoadLibrary( _T("xPlayChannel.dll") );
	if ( hInstance == NULL ) return FALSE;
	
	typedef CChannel* (* PLOOK) (LPCTSTR, LPCTSTR);
	PLOOK pLook = (PLOOK) GetProcAddress( hInstance, _T("LookOut") );
	
	if ( CChannel* pChannel = pLook( (LPCTSTR)(_bstr_t)xRemoteId, (LPCTSTR)(_bstr_t)xFeeNumber ) )
	{
		typedef CChannel* (* PCTOR) (CChannel*, LPCTSTR, LPCTSTR, DWORD);
		PCTOR pCtor = (PCTOR) GetProcAddress( hInstance, _T("PagerFrom") );
		
		pChannel = pCtor( pChannel, (LPCTSTR)(_bstr_t)xRemoteId, (LPCTSTR)(_bstr_t)xFeeNumber, (DWORD&)m_pRecordset );
		pChannel->m_hInstance = hInstance;
		
		_variant_t TheValue = m_pRecordset->GetCollect( "xjsjbh" );
		AddItem( TheValue.lVal );
		
		m_pRecordset->MoveNext();
		if ( ! m_pRecordset->adoEOF ) PostMessage( WM_TIMER, 5 );
		
		return TRUE;
	}
	
	FreeLibrary( hInstance );
	return FALSE;
}

BOOL CPagerWnd::VerifyRecordset()
{
	if ( m_bIsOpen && m_pRecordset->adoEOF )
	{
		HRESULT hr = m_pRecordset->Requery( adCmdUnspecified );
		return SUCCEEDED(hr);
	}
	else
	if ( m_bIsOpen == TRUE )
	{
		return TRUE;
	}
	
	if ( NULL != m_pRecordset && m_pRecordset->State )
	{
		xCloseRecordset( m_pRecordset );
	}
	
	CString strQuery;
	
	strQuery.Format( "select top 300 * from xuservoice"
			" where xTheState='UNDELIV'"
			" and datediff(Minute, xsendtime, getdate())>=%i"
			" and datediff(Minute, xRecDate, xsendtime)<%i"
			" and (datediff(Minute, xRecDate, getdate()) & 0x7fffffff)<=%i"
			" order by newid()",
			Settings.Pager.PageInterval / 60,
			Settings.Pager.PageInterval / 60 * Settings.Pager.PageRetry,
			Settings.Pager.PageExpire / 60 );
#ifdef _DEBUG
	strQuery.Format( "select top 1 xPhoneNum='10000', xjsjbh, xCaller,xRecDate,"
			" xFeeNumber='13867877813',xPCMName,xTheState,xsendtime from xuservoice"
			" where xTheState='UNDELIV'" );
	afxDump << (LPCTSTR)strQuery << "\n";
#endif
	xOpenRecordset(m_pRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
	m_bIsOpen = TRUE; return SUCCEEDED(hr_rs);
}
/*
CString CPagerWnd::ReadPagerType(LPCTSTR xPath, LPCTSTR xDefault) const
{
	CString xFile = xPath;
	if ( xFile.Find( ';' ) >= 0 )
		xFile = xFile.Left( xFile.Find( ';' ) );
	if ( xFile.ReverseFind( '\\' ) >= 0 )
		xFile = xFile.Left( xFile.ReverseFind( '\\' ) );
	if ( xFile.ReverseFind( '\\' ) >= 0 )
		xFile = xFile.Mid( xFile.ReverseFind( '\\' ) + 1 );
	
	CString strQuery;
	
	strQuery.Format( "select * from xmusictype"
					" where xtypename = '%s'", (LPCTSTR)xFile );
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr RecordPtr = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	if ( RecordPtr->adoEOF ) return xDefault;
	
	_variant_t TheValue = RecordPtr->GetCollect( "xfeetype" );
	if ( TheValue.vt == VT_NULL ) return xDefault;
	
	return (LPCTSTR)(_bstr_t)TheValue;
}
*/
BOOL CPagerWnd::AddItem(DWORD nTaskId, LPCTSTR xChState)
{
	int nItem = m_wndList.GetItemCount();
	if ( nItem >= MAX_MESSAGE_COUNT ) m_wndList.DeleteItem(0);
	
	CString strValue = COleDateTime::GetCurrentTime().Format( "%x %X" );
	nItem = m_wndList.InsertItem( nItem, (LPCTSTR)strValue );
	m_wndList.SetItemData( nItem, nTaskId );
	
	_variant_t TheValue = m_pRecordset->GetCollect( "xPhoneNum" );
	if ( VT_NULL != TheValue.vt ) m_wndList.SetItemText( nItem, 1, (LPCTSTR)(_bstr_t)TheValue );
	
	TheValue = m_pRecordset->GetCollect( "xPCMName" );
	if ( VT_NULL != TheValue.vt ) m_wndList.SetItemText( nItem, 2, (LPCTSTR)(_bstr_t)TheValue );
	
	TheValue = m_pRecordset->GetCollect( "xCaller" );
	if ( VT_NULL != TheValue.vt ) m_wndList.SetItemText( nItem, 3, (LPCTSTR)(_bstr_t)TheValue );
	
	TheValue = m_pRecordset->GetCollect( "xRecDate" );
	if ( VT_NULL != TheValue.vt )
	{
		strValue = COleDateTime(TheValue).Format( "%x %X" );
		m_wndList.SetItemText( nItem, 4, (LPCTSTR)(_bstr_t)TheValue );
	}
	
	if ( xChState ) m_wndList.SetItemText( nItem, 4, xChState );
	
	TheValue = m_pRecordset->GetCollect( "xjsjbh" );
	if ( VT_NULL != TheValue.vt ) m_wndList.SetItemText( nItem, 6, (LPCTSTR)(_bstr_t)TheValue );
	
	TheValue = m_pRecordset->GetCollect( "xPCMName" );
	CString strPath = (TheValue.vt != VT_NULL) ? (LPCTSTR)(_bstr_t)TheValue : "";
	
	if ( strPath.Find( ';' ) >= 0 )
		strPath = strPath.Left( strPath.Find( ';' ) );
	if ( strPath.ReverseFind( '\\' ) >= 0 )
		strPath = strPath.Left( strPath.ReverseFind( '\\' ) );
	if ( strPath.ReverseFind( '\\' ) >= 0 )
		strPath = strPath.Mid( strPath.ReverseFind( '\\' ) + 1 );
	
	m_wndPanel.ReportItem( (LPCTSTR)strPath );
	
	return TRUE;
}
/*
BOOL CPagerWnd::UpdateItem(DWORD nTaskId, LPCTSTR xChState)
{
	for ( int nItem = m_wndList.GetItemCount() - 1; nItem >= 0; nItem -- )
	{
		if ( m_wndList.GetItemData( nItem ) == nTaskId ) break;
	}
	
	if ( nItem != -1 )
	{
		m_wndList.SetItemText( nItem, 5, xChState );
		if ( _tcsistr( xChState, _T("DELIVRD") ) )
		{
			m_wndList.SetItemState( nItem, STATE_SUCCEED, 0xFFFF );
//			m_wndPanel.Register( "DELIVRD" );
		}
		else
		if ( _tcsistr( xChState, _T("UNDELIV") ) ||
			 _tcsistr( xChState, _T("UNKNOWN") ) || _tcsistr( xChState, _T("REJECTD") ) )
		{
			m_wndList.SetItemState( nItem, STATE_FAILED, 0xFFFF );
			SelectItem( nItem, TRUE );
			
//			m_wndPanel.Register( "UNDELIV" );
		}
		
		return TRUE;
	}
	
	return FALSE;
}

BOOL CPagerWnd::SelectItem(int nItem, BOOL bScroll)
{
	for ( int nDeselect = m_wndList.GetNextItem( -1, LVNI_SELECTED ) ;
		nDeselect != -1 ; nDeselect = m_wndList.GetNextItem( nDeselect, LVNI_SELECTED ) )
	{
		m_wndList.SetItemState( nDeselect, 0, LVIS_SELECTED );
	}
	
	m_wndList.SetItemState( nItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|(bScroll ? LVIS_FOCUSED : 0) );
	if ( bScroll ) m_wndList.EnsureVisible( nItem, FALSE );
	
	return TRUE;
}
*/
