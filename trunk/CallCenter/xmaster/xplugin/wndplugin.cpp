//
// WndPlugin.cpp
//

#include "stdafx.h"
#include "Xmaster.h"
#include "Settings.h"
#include "Network.h"
#include "WndPlugin.h"
#include "CoolInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CPluginWnd, CChildWnd, 0)

BEGIN_MESSAGE_MAP(CPluginWnd, CChildWnd)
	//{{AFX_MSG_MAP(CPluginWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_PLUGIN_LIST, OnCustomDrawList)
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_USER + 111, OnNextTry)
END_MESSAGE_MAP()

#define SIZE_INTERNAL	1982
#define PANEL_WIDTH		200
#define SPLIT_SIZE		6

#define DS_TEMP		(1<<13)
#define DS_ERROR	(1<<14)
#define DS_NORMAL	(1<<15)

#define MAX_MESSAGE_COUNT	1024

// Registry

const TCHAR	gcszWindowName[]	= _T("CallCenter.Xmaster.Xplugin");
const TCHAR	gcszWindowClass[]	= _T("CallCenter.Xmaster.XpluginEx");

/////////////////////////////////////////////////////////////////////////////
// CPluginWnd construction

CPluginWnd::CPluginWnd() : CChildWnd(FALSE)
{
	m_hThread		= NULL;
	m_bThread		= FALSE;

#ifdef _DEBUG
	m_tUserSub		= 0;
	m_tUserPay		= 0;
	m_tUserSmSj		= 0;
#else
	COleDateTime tNow = COleDateTime::GetCurrentTime();
	DWORD tCurrChk = tNow.GetDay() - 1;
	tCurrChk *= 24; tCurrChk += tNow.GetHour();
	
	m_tUserSub		= tCurrChk * 60 + tNow.GetMinute() / 6;;
	m_tUserPay		= tCurrChk * 60 + tNow.GetMinute() / 15;;
	m_tUserSmSj		= tCurrChk;;
#endif
	
	
	Create( IDR_PLUGINFRAME );
}

CPluginWnd::~CPluginWnd()
{
}

/////////////////////////////////////////////////////////////////////////////
// CPluginWnd message handlers

int CPluginWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CChildWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	SetOwner( GetParent() );
	
	// Panel

	m_wndPanel.Create( this );

	// ListCtrl
	
	m_wndList.Create( WS_CHILD|WS_VISIBLE|LVS_REPORT|LVS_NOLABELWRAP|WS_VSCROLL|WS_TABSTOP|LVS_SHOWSELALWAYS,
		rectDefault, this, IDC_PLUGIN_LIST );
	
	m_wndList.SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP );
	m_wndList.SetImageList( &CoolInterface.m_pImages, LVSIL_SMALL );

	m_wndList.InsertColumn( 0, _T("发生时间"), LVCFMT_LEFT, 120 );
	m_wndList.InsertColumn( 1, _T("事件对象"), LVCFMT_LEFT,  94 );
	m_wndList.InsertColumn( 2, _T("事件代码"), LVCFMT_LEFT,  80 );
	m_wndList.InsertColumn( 3, _T("事件描述"), LVCFMT_LEFT, 250 );
	
	StartThread();

	return 0;
}

void CPluginWnd::OnDestroy() 
{
	StopThread();
	
	CChildWnd::OnDestroy();
}

void CPluginWnd::OnSize(UINT nType, int cx, int cy) 
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

/////////////////////////////////////////////////////////////////////////////
// CPluginWnd custom message handlers

void CPluginWnd::OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult)
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
		else if ( (dwExisting & DS_ERROR) == DS_ERROR )
		{
			pDraw->clrText = RGB( 169, 0, 0 );
		}
		else
		if ( (dwExisting & DS_NORMAL) == DS_NORMAL )
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

LONG CPluginWnd::OnNextTry(WPARAM wParam, LPARAM lParam)
{
	CSingleLock pLock( &m_pSection );
	if ( ! pLock.Lock( 250 ) ) return 0;
	
	m_tUserSub = m_tUserPay = m_tUserSmSj = 0;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CPluginWnd Text operations

int CPluginWnd::AddItem(LPCTSTR xPhone, LPCTSTR xFeeCode, LPCTSTR xContent, DWORD nState)
{
	if ( m_wndList.GetItemCount() >= MAX_MESSAGE_COUNT )
	{
		m_wndList.DeleteAllItems();
	}
	
	LV_ITEM pItem;
	ZeroMemory( &pItem, sizeof(pItem) );
	pItem.mask		= LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	
	pItem.stateMask = LVIS_STATEIMAGEMASK;
	
	if ( nState == DS_NORMAL ) pItem.iImage = CoolInterface.ImageForID( ID_NETWORK_COMPLETE );
	else
	if ( nState == DS_ERROR ) pItem.iImage = CoolInterface.ImageForID( ID_NETWORK_REJECTED );
	
	CString str = COleDateTime::GetCurrentTime().Format("%m-%d %H:%M:%S");
	pItem.pszText	= str.GetBuffer( 0 );
	
	pItem.iItem		= m_wndList.GetItemCount();
	pItem.iItem		= m_wndList.InsertItem( &pItem );
	
	pItem.mask		= LVIF_TEXT;
	pItem.pszText	= (LPSTR)xPhone;
	pItem.iSubItem	= 1;
	m_wndList.SetItem( &pItem );
	
	pItem.mask		= LVIF_TEXT;
	pItem.pszText	= (LPSTR)xFeeCode;
	pItem.iSubItem	= 2;
	m_wndList.SetItem( &pItem );
	
	pItem.mask		= LVIF_TEXT;
	pItem.pszText	= (LPSTR)xContent;
	pItem.iSubItem	= 3;
	m_wndList.SetItem( &pItem );
	
	SelectItem( pItem.iItem );
	
	return pItem.iItem;
}

void CPluginWnd::SelectItem(int nItem)
{
	for ( int i = m_wndList.GetItemCount() - 1 ; i >= 0 ; i-- )
	{
		if ( m_wndList.GetItemState( i, LVIS_SELECTED ) )
		{
			if ( i != nItem ) m_wndList.SetItemState( i, 0, LVIS_SELECTED );
		}
		else
		{
			if ( i == nItem ) m_wndList.SetItemState( i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
		}
	}
	
	LV_ITEM pItem;
	ZeroMemory( &pItem, sizeof(pItem) );
	
	pItem.iItem = nItem; pItem.mask = LVIF_STATE;
	pItem.state = 0xFFFFFFFF; pItem.stateMask = LVIS_SELECTED|LVIS_FOCUSED;
	m_wndList.SendMessage( LVM_SETITEMSTATE, pItem.iItem, (LPARAM)&pItem );
	m_wndList.PostMessage( LVM_ENSUREVISIBLE, pItem.iItem, FALSE );
}

//////////////////////////////////////////////////////////////////////
// CPluginWnd thread run

void CPluginWnd::StartThread()
{
	if ( m_hThread != NULL && m_bThread ) return;
	
	m_bThread = TRUE;
	CWinThread* pThread = AfxBeginThread( ThreadStart, this, THREAD_PRIORITY_NORMAL );
	m_hThread = pThread->m_hThread;
}

void CPluginWnd::StopThread()
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

		MSG msg;
		while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}
	
	if ( nAttempt == 0 )
	{
		TerminateThread( m_hThread, 0 );
		_RPTF0(_CRT_WARN,"WARNING: Terminating CPluginWnd thread.\n");
		Sleep( 100 );
	}
	
	m_hThread = NULL;
}

UINT CPluginWnd::ThreadStart(LPVOID pParam)
{
	CPluginWnd* pClass = (CPluginWnd*)pParam;
	pClass->OnRun();
	return 0;
}

BOOL CPluginWnd::OnRun()
{
	CSingleLock pLock( &m_pSection );
	
	while ( m_bThread )
	{
		WaitForSingleObject( m_pWakeup, 6 * 1000 );
		
		if ( ! m_bThread ) break;
		pLock.Lock();
		ReadUserSub();
		pLock.Unlock();
		
		if ( ! m_bThread ) break;
		pLock.Lock();
		ReadUserPay();
		pLock.Unlock();

		if ( ! m_bThread ) break;
		pLock.Lock();
		ReadUserSmSj();
		pLock.Unlock();
	}
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CPluginWnd subscibe functionality

BOOL CPluginWnd::ReadUserSub()
{
	COleDateTime tNow = COleDateTime::GetCurrentTime();
	
	DWORD tCurrChk = tNow.GetDay() - 1;
	tCurrChk *= 24; tCurrChk += tNow.GetHour();
	tCurrChk *= 60;  tCurrChk += tNow.GetMinute() / 6;
	
	BOOL bRefresh = ! ( tCurrChk == m_tUserSub );
	
	COleDateTime tUsrTime;
	tUsrTime.ParseDateTime( Settings.Block.UsrTime, 0, VAR_TIMEVALUEONLY );
	if ( tUsrTime.GetStatus() != COleDateTime::valid ) return FALSE;
	
	if ( tUsrTime.GetHour() * 24 * 3600 + tUsrTime.GetMinute() * 60 >
		tNow.GetHour() * 24 * 3600 + tNow.GetMinute() * 60 ) return FALSE;
	
#ifndef _DEBUG
	try
#endif
	{
		CString strQuery;
		_RecordsetPtr Recordfee;
		
		strQuery.Format( "select * from xfeetable"
						" where xtalktype = 'FECE'" );
		TRACE( strQuery + "\n" );
		
		CSingleLock pLock( &Network.m_pSection );
		if ( ! pLock.Lock( 250 ) ) return FALSE;
		Recordfee = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
		pLock.Unlock();
		
		for ( ; ! Recordfee->adoEOF && m_bThread ; Recordfee->MoveNext() )
		{
			_variant_t xFeeCode = Recordfee->GetCollect( "xfeetableId" );
			_variant_t xFeeType = Recordfee->GetCollect( "xfeetype" );
			
			ReadUserSub((LPCTSTR)(_bstr_t)xFeeCode, (LPCTSTR)(_bstr_t)xFeeType, bRefresh);
		}
		
		strQuery.Format( "select * from xvoxzb" );
		TRACE( strQuery + "\n" );
		
		if ( ! pLock.Lock( 250 ) ) return FALSE;
		Recordfee = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
		pLock.Unlock();
		
		for ( ; ! Recordfee->adoEOF && m_bThread ; Recordfee->MoveNext() )
		{
			_variant_t xFeeCode = Recordfee->GetCollect( "xvoicecode" );
			_variant_t xFeeMoney= Recordfee->GetCollect( "xmonthmoney" );
			
			if ( xFeeMoney.vt == VT_NULL ) continue;
			ReadUserVox((LPCTSTR)(_bstr_t)xFeeCode, (double)xFeeMoney);
		}
	}
#ifndef _DEBUG
	catch(_com_error e) 
	{ 
		theApp.Message( MSG_ERROR, "ERROR[ReadUserSub]: %s", e.ErrorMessage() );
		m_tUserSub = 0; return FALSE;
	}
	catch(...)
	{
		theApp.Message( MSG_ERROR, "ABORT[ReadUserSub]!!!" );
		return FALSE;
	}
#endif
	
	m_tUserSub = tCurrChk; return TRUE;
}

BOOL CPluginWnd::ReadUserSub(LPCTSTR xFeeCode, LPCTSTR xFeeType, BOOL bRefresh)
{
	CString	strQuery;
	CSingleLock pLock( &Network.m_pSection );

#if 0
	strQuery.Format( "select a.*, b.xsubid, b.xfeedate, b.xjsjbh as b_xjsjbh, b.xfeecode as b_xfeecode"
		" from xusefee a left join"
		" xusefee_his b on a.xjsjbh = b.xjsjbh and a.xfeecode = b.xfeecode"
		" where a.xfeecode = '%s'", xFeeCode );
#else
	strQuery.Format( "select a.*, b.xsubid, b.xfeedate, b.xjsjbh as b_xjsjbh, b.xfeecode as b_xfeecode"
					",c.xstatus"
					" from xusefee a left join"
					" xusefee_his b on a.xjsjbh = b.xjsjbh and a.xfeecode = b.xfeecode"
					" inner join xuser_info c on a.xjsjbh = c.xjsjbh"
					" where a.xfeecode = '%s'", xFeeCode );
#endif

#if 1
	if ( ! bRefresh ) strQuery += " and datediff(minute, xdatetime, getdate()) < 6";
#endif
#ifdef _DEBUG
	strQuery += " and a.xjsjbh='xuser20060301000007'";
#endif
	TRACE( strQuery + "\n" );
	
	if ( ! pLock.Lock( 250 ) ) return FALSE;
	_RecordsetPtr pRecordset = NULL;
	xOpenRecordset(pRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
	pLock.Unlock();
	
	for ( ; ! pRecordset->adoEOF && m_bThread ; pRecordset->MoveNext() )
	{
		pLock.Unlock();
		if ( ! pLock.Lock( 250 ) ) return FALSE;
		
		_variant_t xUserId  = pRecordset->GetCollect( "xjsjbh" );
		if ( xUserId.vt == VT_NULL ) continue;
		
		_variant_t xSubDate = pRecordset->GetCollect( "xdatetime" );
		_variant_t xFeeDate = pRecordset->GetCollect( "xfeedate" );
//if ( xFeeDate.vt != VT_NULL) continue;
		
		int nElapse; double nExpense;
		if ( !ReadFeeType((LPCTSTR)(_bstr_t)xFeeType, &nElapse, &nExpense) ) continue;
		
#if 1
		_variant_t xStatus = pRecordset->GetCollect( "xstatus" );
		if ( _tcscmp((LPCTSTR)(_bstr_t)xStatus, "2") >= 0 && xFeeDate.vt == VT_NULL ) nExpense = 0;
#endif
		
		COleDateTime tSubDate;
		if ( xSubDate.vt != VT_NULL ) tSubDate = COleDateTime( xSubDate );
		else tSubDate = COleDateTime( 1900, 1, 1, 0, 0, 0 );
		
		COleDateTime tFeeDate;
		if ( xFeeDate.vt != VT_NULL ) tFeeDate = COleDateTime( xFeeDate );
		else tFeeDate = COleDateTime( 1900, 1, 1, 0, 0, 0 );
		
		if ( nElapse == 31 && ! CheckInDate(tSubDate, tFeeDate ) ) continue;
		if ( nElapse != 31 && ! CheckInDate(tSubDate, nElapse) ) continue;
		
		if ( ! WriteBalance((LPCTSTR)(_bstr_t)xUserId, nExpense) )
		{
			UnsubscibeUser( (LPCTSTR)(_bstr_t)xUserId, (LPCTSTR)(_bstr_t)xFeeCode );
			AddItem((LPCTSTR)(_bstr_t)xUserId, (LPCTSTR)(_bstr_t)xFeeCode, "退订成功", DS_NORMAL);
			continue;
		}
		
		_variant_t xBookmark= pRecordset->Bookmark;
		_variant_t xSubId   = pRecordset->GetCollect( "xbh" );
		
		if ( xFeeDate.vt == VT_NULL )
		{
			pRecordset->AddNew();
			
			CString xSubNumber = "sub"; xSubNumber += (LPCTSTR)(_bstr_t)xSubId;
			pRecordset->PutCollect( "xsubid", _variant_t(xSubNumber) );
			
			pRecordset->PutCollect( "b_xjsjbh", xUserId );
			pRecordset->PutCollect( "b_xfeecode", xFeeCode );
		}
		
		//tFeeDate = COleDateTime::GetCurrentTime();
		pRecordset->PutCollect( "xfeedate", _variant_t(tFeeDate) );
		
		pRecordset->Update();
		pRecordset->Bookmark= xBookmark;
		
#if 1
		AddItem((LPCTSTR)(_bstr_t)xUserId, (LPCTSTR)(_bstr_t)xFeeCode, "缴租成功", DS_NORMAL);
#else
		strQuery.Format( "select xphone from xuser_info"
						" where xjsjbh = '%s'", (LPCTSTR)(_bstr_t)xUserId );
		TRACE( strQuery + "\n" );
		
		_RecordsetPtr Recordusr = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
		_variant_t xRemoteId = !Recordusr->adoEOF ? Recordusr->GetCollect("xphone") : xUserId;
		
		AddItem((LPCTSTR)(_bstr_t)xRemoteId, (LPCTSTR)(_bstr_t)xFeeCode, "缴租成功", DS_NORMAL);
#endif
//		pLock.Unlock();
	}
	
	xCloseRecordset( pRecordset );
	
	return TRUE;
}

BOOL CPluginWnd::ReadUserVox(LPCTSTR xFeeCode, double nExpense)
{
	CString	strQuery;
	CSingleLock pLock( &Network.m_pSection );
	
	strQuery.Format( "select a.*, b.xsubid, b.xfeedate"
					" from xvoicesub a left join xusefee_his b"
					" on 'vox'+ltrim(str(a.jsjbh)) = b.xsubid"
					" where a.xvoicecode = '%s'", xFeeCode );
#ifdef _DEBUG
	strQuery += " and a.xjsjbh='xuser20060301000007'";
#endif
	TRACE( strQuery + "\n" );
	
	if ( ! pLock.Lock( 250 ) ) return FALSE;
	_RecordsetPtr pRecordset = NULL;
	xOpenRecordset(pRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
	pLock.Unlock();
	
	for ( ; ! pRecordset->adoEOF && m_bThread ; pRecordset->MoveNext() )
	{
		pLock.Unlock();
		if ( ! pLock.Lock( 250 ) ) return FALSE;
		
		_variant_t xUserId  = pRecordset->GetCollect( "xjsjbh" );
		if ( xUserId.vt == VT_NULL ) continue;
		
		_variant_t xSubDate = pRecordset->GetCollect( "xdate" );
		_variant_t xFeeDate = pRecordset->GetCollect( "xfeedate" );
		
		int nElapse = Settings.Block.UsrCycle;
		
		COleDateTime tSubDate;
		if ( xSubDate.vt != VT_NULL ) tSubDate = COleDateTime( xSubDate );
		else tSubDate = COleDateTime( 1900, 1, 1, 0, 0, 0 );
		
		COleDateTime tFeeDate;
		if ( xFeeDate.vt != VT_NULL ) tFeeDate = COleDateTime( xFeeDate );
		else tFeeDate = COleDateTime( 1900, 1, 1, 0, 0, 0 );
		
		if ( nElapse == 31 && ! CheckInDate(tSubDate, tFeeDate ) ) continue;
		if ( nElapse != 31 && ! CheckInDate(tSubDate, nElapse) ) continue;
		
		if ( ! WriteBalance((LPCTSTR)(_bstr_t)xUserId, nExpense) )
		{
			AddItem((LPCTSTR)(_bstr_t)xUserId, (LPCTSTR)(_bstr_t)xFeeCode, "退订成功[语音订阅]", DS_NORMAL);
			pRecordset->Delete( adAffectCurrent ); continue;
		}
		
		_variant_t xSubId   = pRecordset->GetCollect( "jsjbh" );
		//_variant_t xVoxName = pRecordset->GetCollect( "xvoicename" );
		_variant_t xBookmark= pRecordset->Bookmark;
		
		if ( xFeeDate.vt == VT_NULL )
		{
			pRecordset->AddNew();
			
			CString xSubNumber = "vox"; xSubNumber += (LPCTSTR)(_bstr_t)xSubId;
			pRecordset->PutCollect( "xsubid", _variant_t(xSubNumber) );
		}
		
		tFeeDate = COleDateTime::GetCurrentTime();
		pRecordset->PutCollect( "xfeedate", _variant_t(tFeeDate) );
		
		pRecordset->Update();
		pRecordset->Bookmark= xBookmark;
		
#if 1
		AddItem((LPCTSTR)(_bstr_t)xUserId, (LPCTSTR)(_bstr_t)xFeeCode, "缴租成功[语音订阅]", DS_NORMAL);
#else
		strQuery.Format( "select xphone from xuser_info"
						" where xjsjbh = '%s'", (LPCTSTR)(_bstr_t)xUserId );
		TRACE( strQuery + "\n" );
		
		_RecordsetPtr Recordusr = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
		_variant_t xRemoteId = !Recordusr->adoEOF ? Recordusr->GetCollect("xphone") : xUserId;
		
		AddItem((LPCTSTR)(_bstr_t)xRemoteId, (LPCTSTR)(_bstr_t)xFeeCode, "缴租成功[语音订阅]", DS_NORMAL);
#endif
//		pLock.Unlock();
	}
	
	xCloseRecordset( pRecordset );
	
	return TRUE;
}

BOOL CPluginWnd::WriteBalance(LPCTSTR xUserId, double nExpense)
{
	CString	strQuery;
	
	strQuery.Format( "select xBalance + xpreBala from xuser_info"
					" where xjsjbh = '%s'", (LPCTSTR)(_bstr_t)xUserId );
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr Recordusr = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	if ( Recordusr->adoEOF ) return FALSE;
	
	_variant_t TheValue = Recordusr->Fields->GetItem((long)0)->GetValue();
	if ( (double)TheValue < nExpense ) return FALSE;
	
	strQuery.Format( "update xuser_info"
					" set xBalance = xBalance - %.2f"
					" where xjsjbh = '%s'", 
					nExpense, (LPCTSTR)(_bstr_t)xUserId );
	TRACE( strQuery + "\n" );
	
	xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
	
	return TRUE;
}
/*
BOOL CPluginWnd::WriteBalance(_RecordsetPtr pRecordset, double nExpense)
{
	_variant_t TheValue = pRecordset->GetCollect( "xBalance" );
	if ( (double)TheValue < nExpense ) return FALSE;
	
	double xNewBala = (double)TheValue - nExpense;
	pRecordset->PutCollect( "xBalance", _variant_t(xNewBala) );
	
	pRecordset->Update();
	
	return TRUE;
}
*/

BOOL CPluginWnd::UnsubscibeUser(LPCTSTR xUserId, LPCTSTR xFeeCode)
{
	CString	strQuery;
	
	strQuery.Format( "delete xusefee"
					" where xjsjbh = '%s' and xfeecode = '%s'", xUserId, xFeeCode );
	TRACE( strQuery + "\n" );
	
	xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CPluginWnd pay functionality

BOOL CPluginWnd::ReadUserPay()
{
	COleDateTime tNow = COleDateTime::GetCurrentTime();

	DWORD tCurrChk = tNow.GetDay() - 1;
	tCurrChk *= 24; tCurrChk += tNow.GetHour();
	tCurrChk *= 60; tCurrChk += tNow.GetMinute() / 15;
	
	if ( tCurrChk == m_tUserPay ) return FALSE;
	
	COleDateTime tPayStart;
	tPayStart.ParseDateTime( Settings.Payer.PayStart, 0, VAR_TIMEVALUEONLY );
	if ( tPayStart.GetStatus() != COleDateTime::valid ) return FALSE;
	tPayStart.SetDateTime( tNow.GetYear(), tNow.GetMonth(), tNow.GetDay(),
					tPayStart.GetHour(), tPayStart.GetMinute(), 0 );
	
	COleDateTime tPayClose;
	tPayClose.ParseDateTime( Settings.Payer.PayClose, 0, VAR_TIMEVALUEONLY );
	if ( tPayClose.GetStatus() != COleDateTime::valid ) return FALSE;
	tPayClose.SetDateTime( tNow.GetYear(), tNow.GetMonth(), tNow.GetDay(),
					tPayClose.GetHour(), tPayClose.GetMinute(), 0 );
	
//theApp.Message(MSG_TEMP,"now:=%s",tNow.Format("%x %X"));
//theApp.Message(MSG_TEMP,"start:=%s",tPayStart.Format("%x %X"));
//theApp.Message(MSG_TEMP,"close:=%s",tPayClose.Format("%x %X"));
	
	if ( tNow < tPayStart || tNow > tPayClose ) return FALSE;
	
#ifndef _DEBUG
	try
#endif
	{
		ReadUserPay( tCurrChk );
	}
#ifndef _DEBUG
	catch(_com_error e) 
	{ 
		theApp.Message( MSG_ERROR, "ERROR[ReadUserPay]: %s", e.ErrorMessage() );
		return FALSE;
	}
	catch(...)
	{
		theApp.Message( MSG_ERROR, "ABORT[ReadUserPay]!!!" );
		return FALSE;
	}
#endif
	
	m_tUserPay = tCurrChk;
	return TRUE;
}

BOOL CPluginWnd::ReadUserPay(DWORD tCurrChk)
{
	CString	strQuery;
	CSingleLock pLock( &Network.m_pSection );
	
	strQuery.Format( "select * from xuser_info a"
					" where xpreBala+xBalance +"
					" (select sum(cast(right(xfeetype,3) as int)) from xfeetable where xtalktype='FECE' and xfeetableid in"
					" (select xfeecode from xusefee c where c.xjsjbh = a.xjsjbh)) < %i"
					" and xstatus = 1", Settings.Payer.PayBalan );
#ifdef _DEBUG
	strQuery += " and xjsjbh='xuser20080212000019'";
#endif
	TRACE( strQuery + "\n" );
	
	if ( ! pLock.Lock( 250 ) ) return FALSE;
	_RecordsetPtr pRecordset;
	xOpenRecordset(pRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
	pLock.Unlock();
	
	for ( ; ! pRecordset->adoEOF && m_bThread ; pRecordset->MoveNext() )
	{
		pLock.Unlock();
		if ( ! pLock.Lock( 250 ) ) return FALSE;
		
		_variant_t xUserId  = pRecordset->GetCollect( "xjsjbh" );
		if ( ! GoPayRecord((LPCTSTR)(_bstr_t)xUserId) ) continue;
		
		_variant_t xPhone   = pRecordset->GetCollect( "xphone" );
		_variant_t xBalance = pRecordset->GetCollect( "xBalance" );
		_variant_t xpreBala = pRecordset->GetCollect( "xpreBala" );
		
#if 1
		if ( (double)xBalance + (double)xpreBala <= 0 )
		{
			pRecordset->PutCollect( "xstatus", _variant_t("0") );
			pRecordset->Update();
		}
#endif
		
		BuildPayRecord( (LPCTSTR)(_bstr_t)xUserId );
		BuildPagerRec( (LPCTSTR)(_bstr_t)xPhone );
		
		CString xContent;
		xContent.Format( "催费: %.2f + %.2f",
				(VT_NULL == xBalance.vt) ? 0.00 : (double)xBalance,
				(VT_NULL == xpreBala.vt) ? 0.00 : (double)xpreBala );
		
		AddItem((LPCTSTR)(_bstr_t)xPhone, "N:=3", (LPCTSTR)xContent, DS_NORMAL);
		
		//pLock.Unlock();
		//Sleep( 500 );
	}
	
	xCloseRecordset( pRecordset );
	
	return TRUE;
}
/*
BOOL CPluginWnd::InSubElapse(LPCTSTR xSubId, COleDateTime xSubDate, int nElapse)
{
	CString	strQuery;
	
	strQuery.Format( "select top 1 xfeedate from xusefee_his"
					" where xsubid = '%s' order by xfeedate desc", xSubId );
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr Recordhis = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	COleDateTime xFeeDate = (Recordhis->adoEOF) ? xSubDate : Recordhis->GetCollect( "xfeedate" );
#if 1	// 先扣后用模式
	if ( Recordhis->adoEOF ) xFeeDate = COleDateTime(1900,1,1, 0,0,0);
#endif
	if ( xFeeDate.GetStatus() != COleDateTime::valid ) return FALSE;
	
	xFeeDate.SetDateTime( xFeeDate.GetYear(), xFeeDate.GetMonth(), xFeeDate.GetDay(), 0, 0, 0 );
	
	if ( xFeeDate + COleDateTimeSpan(nElapse,0,0,0) > COleDateTime::GetCurrentTime() ) return FALSE;
	
	return TRUE;
}
*/
/*	
BOOL CPluginWnd::AddExpense(LPCTSTR xUserId, LPCTSTR xSubId)
{
	CString	strQuery;
	
	strQuery.Format( "insert into xusefee_his (xsubid, xfeedate)"
					" values('%s', getdate())", xSubId );
	TRACE( strQuery + "\n" );
	
	xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
	
	return TRUE;
}
*/

BOOL CPluginWnd::GoPayRecord(LPCTSTR xUserId)
{
	CString	strQuery;
	
	strQuery.Format( "select top 1 xpaydate from xuserpay_his"
					" where xuserid = '%s' order by xpaydate desc", xUserId );
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr Recordhis = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	if ( Recordhis->adoEOF ) return TRUE;
	
	_variant_t xPayDate = Recordhis->GetCollect( "xpaydate" );
	if ( VT_NULL == xPayDate.vt ) return FALSE;
	
	strQuery.Format( "select top 1 xtimestart from xusercost"
					" where xjsjbh = '%s' order by xtimestart desc", xUserId );
	TRACE( strQuery + "\n" );
	
	Recordhis = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	if ( Recordhis->adoEOF ) return FALSE;
	
	_variant_t xXenDate = Recordhis->GetCollect( "xtimestart" );
	if ( VT_NULL == xXenDate.vt ) return FALSE;
	
	if ( COleDateTime(xXenDate) > xPayDate ) return TRUE;
	
	return FALSE;
}

BOOL CPluginWnd::BuildPayRecord(LPCTSTR xUserId)
{
	CString	strQuery;
	
	strQuery.Format( "insert into xuserpay_his (xuserid,xpaydate)"
					" values('%s',getdate())", xUserId );
	TRACE( strQuery + "\n" );
	
	xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
	
	return TRUE;
}

BOOL CPluginWnd::BuildPagerRec(LPCTSTR xUserId)
{
	CString	strQuery;
	
	strQuery.Format( "insert into xuservoice"
					" (xPCMName,xPhoneNum,xRecDate,xCaller)"
					" values('%s','%s',getdate(),'%s')",
					"Media\\prompt.nomorepay.pcm", xUserId, "057496031" );
	TRACE( strQuery + "\n" );
	
	xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CPluginWnd helper

BOOL CPluginWnd::ReadFeeType(LPCTSTR xFeeType, int* pnElapse, double* pnExpense) const
{
	if ( *xFeeType == 'A' )
	{
		*pnElapse  = 31;
		
		*pnExpense  = (double)_ttoi(xFeeType + 1);
		
		return TRUE;
	}
	if ( *xFeeType == 'B' )
	{
		*pnElapse  = 1;
		
		*pnExpense  = (double)_ttoi(xFeeType + 1) / 100;
		
		return TRUE;
	}
	
	return FALSE;
}

BOOL CPluginWnd::CheckInDate(COleDateTime dt, COleDateTime& et) const
{
TRACE("--- %s\n",dt.Format("%Y-%m-%d %X"));
TRACE("+++ %s\n",et.Format("%Y-%m-%d %X"));
	COleDateTime tNow = COleDateTime::GetCurrentTime();
	if ( tNow < dt || et > tNow ) return FALSE;
	
	dt.SetDate( dt.GetYear(), dt.GetMonth(), dt.GetDay() );
	et.SetDate( et.GetYear(), et.GetMonth(), et.GetDay() );
	//if ( et < dt ) { et = dt; return TRUE;}
	
	INT nMonthDay[] = {31,28,31,30,31,30, 31,31, 30,31,30,31};
	int nMonth = et.GetMonth(), nYear = et.GetYear();
	
	if ( IsLeapYear( nYear ) ) nMonthDay[1] = 29;
	
	et += COleDateTimeSpan( nMonthDay[nMonth - 1], tNow.GetHour(), tNow.GetMinute(), tNow.GetSecond() );
	et = max( dt, et );
TRACE("<<< %s\n",et.Format("%Y-%m-%d %X"));
	return ( et <= tNow );
}

BOOL CPluginWnd::CheckInDate(COleDateTime dt, int nElapse) const
{
	COleDateTime tNow = COleDateTime::GetCurrentTime();
	int nDays = COleDateTimeSpan( tNow - dt ).GetDays();
	
	return ( nDays >= nElapse );
}

BOOL CPluginWnd::IsLeapYear(int nYear) const
{
	return nYear % 4 == 0 && (nYear % 400 == 0 || nYear % 100 != 0);  
}

//////////////////////////////////////////////////////////////////////
// CPluginWnd read sm sj for 08208

BOOL CPluginWnd::ReadUserSmSj()
{
	COleDateTime tNow = COleDateTime::GetCurrentTime();

	DWORD tCurrChk = tNow.GetYear();
	tCurrChk *= 100; tCurrChk += tNow.GetDay();
	//tCurrChk *= 24; tCurrChk += tNow.GetHour();
	//tCurrChk *= 60; tCurrChk += tNow.GetMinute() / 15;
	
	if ( tCurrChk == m_tUserSmSj ) return FALSE;
#ifndef _DEBUG
	if ( tNow.GetDay() != Settings.SubSm.UsrDate ) return FALSE;
#endif
	
	COleDateTime tUsrTime;
	tUsrTime.ParseDateTime( Settings.SubSm.UsrTime, 0, VAR_TIMEVALUEONLY );
	if ( tUsrTime.GetStatus() != COleDateTime::valid ) return FALSE;
	
	if ( tUsrTime.GetHour() * 24 * 3600 + tUsrTime.GetMinute() * 60 >
		tNow.GetHour() * 24 * 3600 + tNow.GetMinute() * 60 ) return FALSE;
	
#ifndef _DEBUG
	try
#endif
	{
		ReadUserSmSj_add( );
		ReadUserSmSj_remove( );
	}
#ifndef _DEBUG
	catch(_com_error e) 
	{ 
		theApp.Message( MSG_ERROR, "ERROR[ReadUserSmSj]: %s", e.ErrorMessage() );
		return FALSE;
	}
	catch(...)
	{
		theApp.Message( MSG_ERROR, "ABORT[ReadUserSmSj]!!!" );
		return FALSE;
	}
#endif
	
	m_tUserSmSj = tCurrChk;
	return TRUE;
}

BOOL CPluginWnd::ReadUserSmSj_add()
{
	CString	strQuery;
	
	strQuery.Format( "select b.User_mobile_number, a.*"
					" from [msgsend]..subscibe a"
					" inner join [msgsend]..cust b"
					" on a.user_id = b.USER_ID"
#ifdef _DEBUG
					" where b.User_mobile_number = '13867877813'"
#endif
					);
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr Recordptr = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	
	for ( ; ! Recordptr->adoEOF && m_bThread ; Recordptr->MoveNext() )
	{
		_variant_t xPhone = Recordptr->GetCollect( "User_mobile_number" );
		if ( VT_NULL == xPhone.vt ) continue;
		
		CString xUserId;
		if ( ! GetUserId_Of( (LPCTSTR)(_bstr_t)xPhone, xUserId ) ) continue;
		
		double nGiven;
		_variant_t xSubId = Recordptr->GetCollect( "msg_min_id" );
		if ( VT_NULL == xSubId.vt || ! IsInSubRule(xSubId.lVal, nGiven) ) continue;
		if ( ! ChkAndUserCost( xUserId, xSubId.lVal, nGiven ) ) continue;
		
		strQuery.Format( "EXECUTE xaddBala '%s', %.2f",
			(LPCTSTR)(_bstr_t)xPhone, nGiven );
		TRACE( strQuery + "\n" );
		xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
		
		CString strValue;
		strValue.Format( "xaddBala: %.2f", nGiven );
		AddItem((LPCTSTR)(_bstr_t)xPhone, "<+SM>", (LPCTSTR)strValue, DS_NORMAL);
	}
	
	return TRUE;
}

BOOL CPluginWnd::ReadUserSmSj_remove()
{
	CString	strQuery;
	
	strQuery.Format( "select * from XuserCost"
					//" where xcostcode = '%i'"
					" where datediff(month,xtimestart,getdate()) = 0" );
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr pRecordset = NULL;
	xOpenRecordset(pRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
	
	for ( ; ! pRecordset->adoEOF && m_bThread ; pRecordset->MoveNext() )
	{
		_variant_t xUserId = pRecordset->GetCollect( "xjsjbh" );
		if ( VT_NULL == xUserId.vt ) continue;
		
		CString xPhoneNum;
		if ( ! GetPhoneNum_Of( (LPCTSTR)(_bstr_t)xUserId, xPhoneNum ) ) continue;
		
		double nGiven;
		_variant_t xSubId = pRecordset->GetCollect( "xcostcode" );
		if ( VT_NULL == xSubId.vt || ! IsInSubRule(_ttoi((LPCTSTR)(_bstr_t)xSubId), nGiven) ) continue;
		
		if ( ChkUserSubc( xPhoneNum, _ttoi((LPCTSTR)(_bstr_t)xSubId) ) ) continue;

		strQuery.Format( "EXECUTE xaddBala '%s', %.2f",
					(LPCTSTR)xPhoneNum, -nGiven );
		TRACE( strQuery + "\n" );
		xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
		pRecordset->Delete( adAffectCurrent ); continue;
		
		CString strValue;
		strValue.Format( "xaddBala: %.2f", -nGiven );
		AddItem((LPCTSTR)xPhoneNum, "<-SM>", (LPCTSTR)strValue, DS_NORMAL);
	}
	
	return TRUE;
}

BOOL CPluginWnd::GetUserId_Of(LPCTSTR xPhone, CString& xUserId)
{
	CSingleLock pLock( &Network.m_pSection, TRUE );
	CString	strQuery;
	
	strQuery.Format( "select xjsjbh from xuser_info"
					" where xPhone = '%s'", xPhone );
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr Recordusr = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	if ( Recordusr->adoEOF ) return FALSE;
	
	_variant_t TheValue = Recordusr->GetCollect( "xjsjbh" );
	if ( VT_NULL == TheValue.vt ) xUserId = "";
	else xUserId = (LPCTSTR)(_bstr_t)TheValue;
	
	return TRUE;
}

BOOL CPluginWnd::GetPhoneNum_Of(LPCTSTR xUserId, CString& xPhoneNum)
{
	CString	strQuery;
	
	strQuery.Format( "select xPhone from xuser_info"
					" where xjsjbh = '%s'", xUserId );
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr Recordusr = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	if ( Recordusr->adoEOF ) return FALSE;
	
	_variant_t TheValue = Recordusr->GetCollect( "xPhone" );
	if ( VT_NULL == TheValue.vt ) xPhoneNum = "";
	else xPhoneNum = (LPCTSTR)(_bstr_t)TheValue;
	
	return TRUE;
}

BOOL CPluginWnd::ChkAndUserCost(LPCTSTR xUserId, int nSubId, double nGiven)
{
	CString	strQuery;
	
	strQuery.Format( "select * from XuserCost"
					" where xjsjbh = '%s' and xcostcode = '%i'"
					" and datediff(month,xtimestart,getdate()) = 0", xUserId, nSubId );
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr pRecordset = NULL;
	xOpenRecordset(pRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
	
	if ( ! pRecordset->adoEOF ) return FALSE;
	
	pRecordset->AddNew();
	pRecordset->PutCollect( "xjsjbh", _variant_t(xUserId) );
	
	COleDateTime tNow = COleDateTime::GetCurrentTime();
	pRecordset->PutCollect( "xtimestart", _variant_t(tNow) );
	
	pRecordset->PutCollect( "xcostcode", _variant_t((long)nSubId) );
	pRecordset->PutCollect( "Xcode", _variant_t("SM") );
	pRecordset->PutCollect( "xpar_value", _variant_t(nGiven) );
	
	pRecordset->Update();
	
	return TRUE;
}

BOOL CPluginWnd::ChkUserSubc(LPCTSTR xPhoneNum, int nSubId)
{
	CString	strQuery;
	
	strQuery.Format( "select b.User_mobile_number, a.*"
					" from [msgsend]..subscibe a"
					" inner join [msgsend]..cust b"
					" on a.user_id = b.USER_ID"
					" where b.User_mobile_number = '%s' and msg_min_id = %i", xPhoneNum, nSubId );
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr Recordsub = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	if ( Recordsub->adoEOF ) return FALSE;
	
	return TRUE;
}

BOOL CPluginWnd::IsInSubRule(int nSubId, double& nGiven) const
{
	CString strValue;
	
	strValue = Settings.SubSm.UsrRule;
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		int xSubId; double xGiven=0;
		if ( _stscanf( str, "%i:%lf", &xSubId, &xGiven ) != 2 ) continue;
		
		if ( xSubId != nSubId ) continue;
		nGiven = xGiven; return TRUE;
	}
	
	return FALSE;
}
