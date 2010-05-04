//
// WndClerk.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "WndClerk.h"
#include "CoolInterface.h"
#include "Settings.h"
#include "Traffic.h"
#include "Network.h"
#include "ChannelClerk.h"
#include "Skin.h"

#include "WndMain.h"
#include "WndUser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CClerkWnd, CChildWnd, 0)

BEGIN_MESSAGE_MAP(CClerkWnd, CChildWnd)
	//{{AFX_MSG_MAP(CClerkWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(ID_P2JOB_SEARCH, OnSearchNumber)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST, OnCustomDrawList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define SIZE_INTERNAL	1982
#define PANEL_WIDTH		200
#define BAR_HEIGHT		28

#define STATE_SUCCEED	(1<<12)
#define STATE_FAILED	(1<<13)

#define MAX_MESSAGE_COUNT	1024

/////////////////////////////////////////////////////////////////////////////
// CClerkWnd construction

CClerkWnd::CClerkWnd() : CChildWnd(FALSE)
{
	m_pRecordset	= NULL;
	m_tNavigated	= 0;
	m_bIsOpen		= FALSE;
	
	m_pUserWnd		= NULL;
	CClerkWnd::Setup();

	CString strCaption;
	strCaption.LoadString( IDR_CLERKFRAME );
	
	Create( IDI_TASKPANEL, (LPCTSTR)strCaption );
}

CClerkWnd::~CClerkWnd()
{
}

//////////////////////////////////////////////////////////////////////
// CClerkWnd attributes

void CClerkWnd::Setup()
{
	CMainWnd* pMainWnd	= (CMainWnd*)theApp.m_pMainWnd;
	ASSERT( pMainWnd->GetSafeHwnd() != NULL );

	m_pUserWnd			= (CUserWnd*)pMainWnd->GetWindow( RUNTIME_CLASS(CUserWnd) );
	ASSERT( m_pUserWnd->GetSafeHwnd() != NULL );
}

/////////////////////////////////////////////////////////////////////////////
// CClerkWnd operations

BOOL CClerkWnd::Create(UINT nID, LPCTSTR xCaption) 
{
	m_nResID = nID;
	
	return CMDIChildWnd::Create( NULL, xCaption, WS_CHILD |
		 WS_OVERLAPPEDWINDOW );
}

/////////////////////////////////////////////////////////////////////////////
// CClerkWnd message handlers

int CClerkWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
	m_wndList.InsertColumn( 4, _T("状态"), LVCFMT_LEFT,  80 );
	m_wndList.InsertColumn( 5, _T("流水号"), LVCFMT_LEFT,  80 );
	
	// Toolbar
	
	if ( ! m_wndToolBar.Create( this, WS_CHILD|WS_VISIBLE|CBRS_NOALIGN, AFX_IDW_TOOLBAR ) ) return -1;
	m_wndToolBar.SetBarStyle( m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS|CBRS_BORDER_TOP );
	m_wndToolBar.SetOwner( this );
	
	if ( ! m_wndSearch.Create( WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL, rectDefault, &m_wndToolBar, IDC_SEARCH_NUMBER ) ) return -1;
	m_wndSearch.SetFont( &CoolInterface.m_fntNormal );
	
	Skin.CreateToolBar( _T("CClerkWnd"), &m_wndToolBar );
	m_hThread	= NULL;
	m_bThread	= FALSE;
	
	return 0;
}

void CClerkWnd::OnDestroy() 
{
	StopThread();
	
	CChildWnd::OnDestroy();
}

void CClerkWnd::OnSize(UINT nType, int cx, int cy) 
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
// CClerkWnd custom message handlers

void CClerkWnd::OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult)
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

void CClerkWnd::OnSearchNumber() 
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
// CClerkWnd thread run

void CClerkWnd::StartThread()
{
	if ( m_hThread != NULL && m_bThread ) return;
	
	m_bThread = TRUE;
	CWinThread* pThread = AfxBeginThread( ThreadStart, this, THREAD_PRIORITY_NORMAL );
	m_hThread = pThread->m_hThread;
}

void CClerkWnd::StopThread()
{
	if ( m_hThread == NULL ) return;
	
	m_bThread = FALSE;
	m_pWakeup.SetEvent();
	
	for ( int nAttempt = 50 ; nAttempt > 0 ; nAttempt-- )
	{
		DWORD nCode;
		if ( ! GetExitCodeThread( m_hThread, &nCode ) ) break;
		if ( nCode != STILL_ACTIVE ) break;
		Sleep( 100 );
	}
	
	if ( nAttempt == 0 )
	{
		TerminateThread( m_hThread, 0 );
		theApp.Message( MSG_DEBUG, _T("WARNING: Terminating CClerkWnd thread.") );
		Sleep( 100 );
	}
	
	m_hThread = NULL;
}

UINT CClerkWnd::ThreadStart(LPVOID pParam)
{
	CClerkWnd* pClass = (CClerkWnd*)pParam;
	pClass->OnRun();
	return 0;
}

//////////////////////////////////////////////////////////////////////
// CClerkWnd clerk run

BOOL CClerkWnd::OnRun()
{
	CSingleLock pLock( &m_pSection );
	
	while ( m_bThread )
	{
		WaitForSingleObject( m_pWakeup, INFINITE );
#if 1
		if ( ! m_bThread ) break;
		pLock.Lock();
		OnRunClerk();
		pLock.Unlock();
#endif
		
		m_wndPanel.ExecuteFlowBack( Settings.Clerk.Interval );
	}

	return TRUE;
}

BOOL CClerkWnd::VerifyRecordset()
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
	
	strQuery.Format( "select * from x_warn_record"
					" where datediff(day,xstart,getdate()) >= 0"
					" and datediff(day,xend,getdate()) <= 0" );
#ifdef _DEBUG
	afxDump << (LPCTSTR)strQuery << "\n";
#endif
	xOpenRecordset(m_pRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
	m_bIsOpen = TRUE; return SUCCEEDED(hr_rs);
}

BOOL CClerkWnd::OnRunClerk()
{
	try
	{
		if ( ! VerifyRecordset() ) return FALSE;
		if ( m_pRecordset->adoEOF ) return TRUE;
		
		COleDateTime xTime;
		COleDateTime tNow = COleDateTime::GetCurrentTime();
		if ( ! OnRunClerk( &tNow, &xTime ) ) return FALSE; 
		
		m_pRecordset->Update(); m_pRecordset->MoveNext();
		if ( ! m_pRecordset->adoEOF ) SetEvent( Traffic.m_pWakeup );
	}
	catch(_com_error e) 
	{ 
		theApp.Message( MSG_ERROR, "error on CClerkWnd::OnRunClerk: %s", e.ErrorMessage() );
		return FALSE;
	}
	
	return TRUE;
}

BOOL CClerkWnd::OnRunClerk(COleDateTime* pNow, COleDateTime* xTime)
{
	int nWeekday = pNow->GetDayOfWeek() - 1;
	if ( ! nWeekday ) nWeekday = 7;
	if ( ! WeekNeedClerk( nWeekday ) ) return TRUE;
	
	FieldPtr xPlyTime, xPlyBlob;
	_variant_t TheValue;

	TheValue = m_pRecordset->GetCollect( "xweektype" );
	if ( VT_NULL == TheValue.vt || TheValue.lVal == 0 )
	{
		xPlyTime = m_pRecordset->GetFields()->GetItem("xtime");
		xPlyBlob = m_pRecordset->GetFields()->GetItem("xblob");
	}
	else// if ( TheValue.lVal == 1 )
	{
		switch ( nWeekday )
		{
		case	1: xPlyTime = m_pRecordset->GetFields()->GetItem("xmondaytime");
				   xPlyBlob = m_pRecordset->GetFields()->GetItem("xmondayblob"); break;
		case	2: xPlyTime = m_pRecordset->GetFields()->GetItem("xTuesdaytime");
				   xPlyBlob = m_pRecordset->GetFields()->GetItem("xTuesdayblob"); break;
		case	3: xPlyTime = m_pRecordset->GetFields()->GetItem("xWednesdaytime");
				   xPlyBlob = m_pRecordset->GetFields()->GetItem("xWednesdayblob"); break;
		case	4: xPlyTime = m_pRecordset->GetFields()->GetItem("xThursdaytime");
				   xPlyBlob = m_pRecordset->GetFields()->GetItem("xThursdayblob"); break;
		case	5: xPlyTime = m_pRecordset->GetFields()->GetItem("xFridaytime");
				   xPlyBlob = m_pRecordset->GetFields()->GetItem("xFridayblob"); break;
		case	6: xPlyTime = m_pRecordset->GetFields()->GetItem("xSaturdaytime");
				   xPlyBlob = m_pRecordset->GetFields()->GetItem("xSaturdayblob"); break;
		case	7: xPlyTime = m_pRecordset->GetFields()->GetItem("xSundaytime");
				   xPlyBlob = m_pRecordset->GetFields()->GetItem("xSundayblob"); break;
		}
	}
	
	//COleDateTime xTime = (COleDateTime)xPlyTime;
	* xTime = (COleDateTime)xPlyTime->GetValue();
	COleDateTimeSpan xTimeSpan = (*pNow) - (*xTime);
	if ( xTimeSpan.GetTotalDays() < 1 ) return TRUE;
	
	TheValue = m_pRecordset->GetCollect( "xtype" );
	if ( VT_NULL == TheValue.vt || TheValue.lVal == 1 ) return OnClerkOfWalk(pNow, xTime);
	
	LONG lDataSize = xPlyBlob->ActualSize;
	if ( lDataSize <= 0 ) return TRUE;
	
	CSingleLock pLock( &Network.m_pSection );
	if ( ! pLock.Lock( 250 ) ) return FALSE;
#if 0
	CChannel* pChannel = Network.LookOutPcm( Settings.Optimize.TrunkFind );
#else
	TheValue = m_pRecordset->GetCollect( "xphone" );
	CChannel* pChannel = Network.LookOutPhs( VT_NULL == TheValue.vt ?
		"" : (LPCTSTR)(_bstr_t)TheValue );
#endif
	if ( pChannel == NULL ) return FALSE;
	
	CDialPacket* pPacket = new CDialPacket;
	pPacket->SetLocalId( Settings.Clerk.TxCallerId );
	
	TheValue = m_pRecordset->GetCollect( "xphone" );
	if ( VT_NULL == TheValue.vt ) pPacket->SetRemoteId( "" );
	else pPacket->SetRemoteId( (LPCTSTR)(_bstr_t)TheValue );
	
	CHAR xRemoteId[MAX_PHONE + 1]; ZeroMemory(xRemoteId, MAX_PHONE + 1);
	pPacket->GetRemoteId(xRemoteId); pPacket->SetFeeNumber( xRemoteId );
	
	pChannel->OnDialPacket( pPacket ); pPacket->Release();
	CClerkChannel* pChClerk = CClerkChannel::ConvertFrom(pChannel);
	
	TheValue = m_pRecordset->GetCollect( "xwarncode" );
	pChClerk->m_xClerkId = (LPCTSTR)(_bstr_t)TheValue;
	AddItem( (LPCTSTR)(_bstr_t)TheValue );
	
	TheValue = xPlyBlob->GetChunk(lDataSize);
	LPTSTR *pBuf = NULL;
	SafeArrayAccessData(TheValue.parray, (void **)&pBuf);
	pChClerk->SetBuffer( pBuf, lDataSize );
	SafeArrayUnaccessData( TheValue.parray );
	
	xTime->SetDateTime( pNow->GetYear(), pNow->GetMonth(), pNow->GetDay(),
				xTime->GetHour(), xTime->GetMinute(), xTime->GetSecond() );
	m_pRecordset->PutCollect( xPlyTime->GetName(), _variant_t(*xTime) );
	
	return TRUE;
}

BOOL CClerkWnd::OnClerkOfWalk(COleDateTime* pNow, COleDateTime* xTime)
{
	if ( ! m_pUserWnd ) return FALSE;
	
	CSingleLock pLock( &Network.m_pSection );
	if ( ! pLock.Lock( 250 ) ) return FALSE;
	
	CChannel* pChannel = m_pUserWnd->ReadItem("96031", S_CALL_STANDBY );
	if ( pChannel == NULL ) return FALSE;
	
	SsmStartRing( pChannel->m_nChan ); Sleep(250);
	_variant_t TheValue = m_pRecordset->GetCollect( "xwarncode" );
	pChannel->SendPacket( new CWalkPacket((LPCTSTR)(_bstr_t)TheValue) );
	
	xTime->SetDateTime( pNow->GetYear(), pNow->GetMonth(), pNow->GetDay(),
				xTime->GetHour(), xTime->GetMinute(), xTime->GetSecond() );
	m_pRecordset->PutCollect( "xtime", _variant_t(*xTime) );
	
	AddItem( (LPCTSTR)(_bstr_t)TheValue, "DELIVRD" );
	SsmStopRing( pChannel->m_nChan ); return TRUE;
}

BOOL CClerkWnd::WeekNeedClerk(int nDay)
{
	_variant_t TheValue = m_pRecordset->GetCollect( "xdatecount" );
	if ( VT_NULL == TheValue.vt ) return FALSE;
	
	CString strValue = (LPCTSTR)(_bstr_t)TheValue;
	
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		if ( str.GetLength() > 0 && _ttoi(str) == nDay ) return TRUE;
	}
	
	return FALSE;
}

BOOL CClerkWnd::AddItem(LPCTSTR xClerkId, LPCTSTR xChState)
{
	int nItem = m_wndList.GetItemCount();
	if ( nItem >= MAX_MESSAGE_COUNT ) m_wndList.DeleteItem(0);
	
	CString strValue = COleDateTime::GetCurrentTime().Format( "%x %X" );
	nItem = m_wndList.InsertItem( nItem, (LPCTSTR)strValue );
	
	_variant_t TheValue = m_pRecordset->GetCollect( "xphone" );
	if ( VT_NULL != TheValue.vt ) m_wndList.SetItemText( nItem, 1, (LPCTSTR)(_bstr_t)TheValue );
	
	TheValue = m_pRecordset->GetCollect( "xcontent" );
	if ( VT_NULL != TheValue.vt ) m_wndList.SetItemText( nItem, 2, (LPCTSTR)(_bstr_t)TheValue );
	
	//TheValue = m_pRecordset->GetCollect( "xCaller" );
	//if ( VT_NULL != TheValue.vt ) m_wndList.SetItemText( nItem, 3, (LPCTSTR)(_bstr_t)TheValue );
	
	TheValue = m_pRecordset->GetCollect( "xwarncode" );
	if ( VT_NULL != TheValue.vt ) m_wndList.SetItemText( nItem, 5, (LPCTSTR)(_bstr_t)TheValue );
	
	if ( xChState ) m_wndList.SetItemText( nItem, 4, xChState );
	
	return TRUE;
}
