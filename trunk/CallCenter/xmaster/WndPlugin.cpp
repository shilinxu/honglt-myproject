//
// WndPlugin.cpp
//

#include "stdafx.h"
#include "Xmaster.h"
#include "Settings.h"
#include "WndPlugin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CPluginWnd, CChildWnd)
BEGIN_MESSAGE_MAP(CPluginWnd, CChildWnd)
	//{{AFX_MSG_MAP(CPluginWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define SIZE_INTERNAL	1982
#define PANEL_WIDTH		200
#define SPLIT_SIZE		6

#define MAX_MESSAGE_COUNT	1024

/////////////////////////////////////////////////////////////////////////////
// CPluginWnd construction

CPluginWnd::CPluginWnd() : CChildWnd(FALSE)
{
	m_hThread		= NULL;
	m_bThread		= FALSE;
	
	COleDateTime tNow = COleDateTime::GetCurrentTime();
	DWORD tCurrChk = tNow.GetDay() - 1;
	
	tCurrChk *= 24; tCurrChk += tNow.GetHour();
	m_tUserSub		= tCurrChk * 60 + tNow.GetMinute() / 15;
	m_tUserPay		= tCurrChk * 60 + tNow.GetMinute() / 60;
	
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
	
	m_wndList.Create( WS_VISIBLE, rectDefault, this, IDC_PLUGIN_LIST );
	
	OpenDatabase();
	StartThread();

	return 0;
}

void CPluginWnd::OnDestroy() 
{
	StopThread();
	CloseDatabase();
	
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

BOOL CPluginWnd::OpenDatabase()
{
	CString sConnect = _T("");
	sConnect += _T("Provider=SQLOLEDB.1");
	sConnect += _T(";Persist Security Info=False");
	sConnect += _T(";User ID=") + Settings.Database.UserID;
	sConnect += _T(";Password=") + Settings.Database.Password;
	sConnect += _T(";Initial Catalog=") + Settings.Database.Database;
	sConnect += _T(";Data Source=") + Settings.Database.Host;
	
	TRACE( sConnect + "\n");
	
	try
	{
		xConnect(m_pConnection, (_bstr_t)sConnect);
	}
	catch(_com_error e)//捕捉异常
	{
		theApp.Message( MSG_ERROR, "ERROR[CPluginWnd]: %s", e.ErrorMessage() );
		return FALSE;
	} 
	
	return TRUE;
}

BOOL CPluginWnd::CloseDatabase()
{
	try
	{
		if (NULL != m_pConnection)
		{
			if ( m_pConnection->State ) xDisconnect( m_pConnection );
			m_pConnection.Release();
		}
	}
	catch(_com_error e)//捕捉异常
	{
		theApp.Message( MSG_ERROR, "ERROR[CPluginWnd]: %s", e.ErrorMessage() );
		return FALSE;
	} 

	return TRUE;
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
	tCurrChk *= 60;  tCurrChk += tNow.GetMinute() / 15;
	
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
		_RecordsetPtr pRecordset;
		
		strQuery.Format( "select * from xfeetable"
						" where xtalktype = 'FECE'" );
#ifdef _DEBUG
		strQuery += " and xfeetableid = '8888'";
#endif
		TRACE( strQuery + "\n" );
		
		pRecordset = xExecute(m_pConnection, (_bstr_t)strQuery, adCmdText);
		
		for ( ; ! pRecordset->adoEOF && m_bThread ; pRecordset->MoveNext() )
		{
			ReadUserSub( pRecordset, bRefresh );
		}
		
		strQuery.Format( "select * from xvoxzb" );
		TRACE( strQuery + "\n" );
		
		pRecordset = xExecute(m_pConnection, (_bstr_t)strQuery, adCmdText);
		
		for ( ; ! pRecordset->adoEOF && m_bThread ; pRecordset->MoveNext() )
		{
			_variant_t xFeeCode = pRecordset->GetCollect( "xvoicecode" );
			_variant_t xFeeMoney= pRecordset->GetCollect( "xmonthmoney" );
			
			if ( xFeeMoney.vt == VT_NULL ) continue;
			ReadUserVox((LPCTSTR)(_bstr_t)xFeeCode, (double)xFeeMoney, bRefresh);
		}
	}
#ifndef _DEBUG
	catch(_com_error e) 
	{ 
		theApp.Message( MSG_ERROR, "ERROR[ReadUserSub]: %s", e.ErrorMessage() );
		return FALSE;
	}
	catch(...)
	{
		theApp.Message( MSG_ERROR, "ABORT[ReadUserSub]!!!" );
		return FALSE;
	}
#endif
	
	m_tUserSub = tCurrChk; return TRUE;
}

BOOL CPluginWnd::ReadUserSub(_RecordsetPtr pRecordfee, BOOL bRefresh)
{
	CString	strQuery;
	_variant_t xFeeCode = pRecordfee->GetCollect( "xfeetableId" );
	
	strQuery.Format( "select a.*, b.xfeedate, b.xjsjbh as b_xjsjbh, b.xfeecode as b_xfeecode"
					",c.xstatus, c.xBalance, c.xpreBala"
					" from xusefee a left join"
					" xusefee_his b on a.xjsjbh = b.xjsjbh and a.xfeecode = b.xfeecode"
					" inner join xuser_info c on a.xjsjbh = c.xjsjbh"
					" where a.xfeecode = '%s'", (LPCTSTR)(_bstr_t)xFeeCode );

#if 1
	if ( ! bRefresh ) strQuery += " and datediff(minute, xdatetime, getdate()) < 6";
#endif
#ifdef _DEBUG
	strQuery += " and a.xjsjbh='xuser20060301000007'";
#endif
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr pRecordset;
	xOpenRecordset(pRecordset, m_pConnection, (_bstr_t)strQuery);
	
	for ( ; ! pRecordset->adoEOF && m_bThread ; pRecordset->MoveNext() )
	{
		_variant_t xUserId  = pRecordset->GetCollect( "xjsjbh" );
		if ( xUserId.vt == VT_NULL ) continue;
		
		int nElapse; double xpreBala;
		_variant_t TheValue = pRecordfee->GetCollect( "xfeetype" );
		if ( !ReadFeeType((LPCTSTR)(_bstr_t)TheValue, &nElapse, &xpreBala) ) continue;
		
		TheValue = pRecordfee->GetCollect( "xfixedfee" );
		double nExpense = (double)TheValue;
		
		TheValue = pRecordset->GetCollect( "xdatetime" );
		COleDateTime tSubDate = ( TheValue.vt != VT_NULL ) ? COleDateTime( TheValue ) : COleDateTime();
		
		TheValue = pRecordset->GetCollect( "xfeedate" );
		COleDateTime tFeeDate = ( TheValue.vt != VT_NULL ) ? COleDateTime( TheValue ) : COleDateTime();
#if 1
		_variant_t xStatus = pRecordset->GetCollect( "xstatus" );
		if ( _tcscmp((LPCTSTR)(_bstr_t)xStatus, "2") >= 0 && TheValue.vt == VT_NULL ) nExpense = xpreBala = 0;
#endif
		if ( nElapse == 365 && ! CheckInYear(tSubDate, tFeeDate) ) continue;
		if ( nElapse == 31 && ! CheckInMonth(tSubDate, tFeeDate) ) continue;
		if ( nElapse == 1 && ! CheckInDay(tSubDate, tFeeDate) ) continue;
		
		if ( ! WriteBalance( pRecordset, nExpense, xpreBala ) )
		{
			UnsubscibeUser( (LPCTSTR)(_bstr_t)xUserId, (LPCTSTR)(_bstr_t)xFeeCode );
			m_wndList.Print( MSG_DEFAULT, (LPCTSTR)(_bstr_t)xUserId, (LPCTSTR)(_bstr_t)xFeeCode, "退订成功" );
			continue;
		}
		
		_variant_t xBookmark= pRecordset->Bookmark;
		
		if ( pRecordset->GetCollect( "xfeedate" ).vt == VT_NULL )
		{
			pRecordset->AddNew();
			
			pRecordset->PutCollect( "b_xjsjbh", xUserId );
			pRecordset->PutCollect( "b_xfeecode", xFeeCode );
		}

		pRecordset->PutCollect( "xfeedate", _variant_t(tFeeDate) );
		pRecordset->Update();
		
		pRecordset->Bookmark= xBookmark;
		
		m_wndList.Print( MSG_DEFAULT, (LPCTSTR)(_bstr_t)xUserId, (LPCTSTR)(_bstr_t)xFeeCode, "缴租成功" );
	}
	
	xCloseRecordset( pRecordset );
	
	return TRUE;
}

BOOL CPluginWnd::ReadUserVox(LPCTSTR xFeeCode, double nExpense, BOOL bRefresh)
{
	CString	strQuery;
	
	strQuery.Format( "select a.*, b.xfeedate, b.xjsjbh as b_xjsjbh, b.xfeecode as b_xfeecode"
					" from xvoicesub a left join xusefee_his b on a.xjsjbh = b.xjsjbh"
					" where a.xvoicecode = '%s'", xFeeCode );
#if 1
	if ( ! bRefresh ) strQuery += " and datediff(minute, xdate, getdate()) < 6";
#endif
#ifdef _DEBUG
	strQuery += " and a.xjsjbh='xuser20060301000007'";
#endif
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr pRecordset = NULL;
	xOpenRecordset(pRecordset, m_pConnection, (_bstr_t)strQuery);
	
	for ( ; ! pRecordset->adoEOF && m_bThread ; pRecordset->MoveNext() )
	{
		int nElapse = Settings.Block.UsrCycle;
		
		_variant_t xUserId  = pRecordset->GetCollect( "xjsjbh" );
		if ( xUserId.vt == VT_NULL ) continue;
		
		_variant_t TheValue = pRecordset->GetCollect( "xdate" );
		COleDateTime tSubDate = ( TheValue.vt != VT_NULL ) ? COleDateTime( TheValue ) : COleDateTime();
		
		TheValue = pRecordset->GetCollect( "xfeedate" );
		COleDateTime tFeeDate = ( TheValue.vt != VT_NULL ) ? COleDateTime( TheValue ) : COleDateTime();
		
		if ( nElapse == 365 && ! CheckInYear(tSubDate, tFeeDate ) ) continue;
		if ( nElapse == 31 && ! CheckInMonth(tSubDate, tFeeDate ) ) continue;
		if ( nElapse == 1 && ! CheckInDay(tSubDate, tFeeDate) ) continue;
		
		if ( ! WriteBalance((LPCTSTR)(_bstr_t)xUserId, nExpense) )
		{
			m_wndList.Print( MSG_DEFAULT, (LPCTSTR)(_bstr_t)xUserId, (LPCTSTR)(_bstr_t)xFeeCode, "退订成功[语音订阅]" );
			pRecordset->Delete( adAffectCurrent ); continue;
		}
		
		_variant_t xBookmark= pRecordset->Bookmark;
		
		if ( pRecordset->GetCollect( "xfeedate" ).vt == VT_NULL )
		{
			pRecordset->AddNew();
			
			pRecordset->PutCollect( "b_xjsjbh", xUserId );
			pRecordset->PutCollect( "b_xfeecode", xFeeCode );
		}
		
		pRecordset->PutCollect( "xfeedate", _variant_t(tFeeDate) );
		pRecordset->Update();
		
		pRecordset->Bookmark= xBookmark;
		
		m_wndList.Print( MSG_DEFAULT, (LPCTSTR)(_bstr_t)xUserId, (LPCTSTR)(_bstr_t)xFeeCode, "缴租成功[语音订阅]" );
	}
	
	xCloseRecordset( pRecordset );
	
	return TRUE;
}

BOOL CPluginWnd::WriteBalance(LPCTSTR xUserId, double nExpense)
{
	CString	strQuery;
	
	strQuery.Format( "update xuser_info"
					" set xBalance = xBalance - %.2f"
					" where xjsjbh = '%s' and xBalance + xpreBala >= %.2f", 
					nExpense, (LPCTSTR)(_bstr_t)xUserId, nExpense );
	TRACE( strQuery + "\n" );
	
	_variant_t vRowsAffected(0L);   
	m_pConnection->Execute( (_bstr_t)strQuery, &vRowsAffected, adExecuteNoRecords);
	
	return vRowsAffected.lVal > 0;
}

BOOL CPluginWnd::WriteBalance(_RecordsetPtr pRecordset, double nExpense, double xpreBala)
{
	// 先清赠送
	
	_variant_t TheValue = pRecordset->GetCollect( "xpreBala" );
	double xNewBala = (double)TheValue - min( (double)TheValue, xpreBala );
	pRecordset->PutCollect( "xpreBala", _variant_t(xNewBala + 0.005) );
	
	// 扣余额
	
	TheValue = pRecordset->GetCollect( "xBalance" );
	if ( (double)TheValue + xNewBala < nExpense )
	{
		pRecordset->Update();
		return FALSE;
	}
	
	xNewBala = (double)TheValue - nExpense;
	pRecordset->PutCollect( "xBalance", _variant_t(xNewBala + 0.005) );
	
	// 送话费
	
	TheValue = pRecordset->GetCollect( "xpreBala" );
	pRecordset->PutCollect( "xpreBala", _variant_t((double)TheValue + xpreBala + 0.005) );
	
	pRecordset->Update();
	
	return TRUE;
}

BOOL CPluginWnd::UnsubscibeUser(LPCTSTR xUserId, LPCTSTR xFeeCode)
{
	CString	strQuery;
	
	strQuery.Format( "delete xusefee"
					" where xjsjbh = '%s' and xfeecode = '%s'", xUserId, xFeeCode );
	TRACE( strQuery + "\n" );
	
	xExecute(m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CPluginWnd pay functionality

BOOL CPluginWnd::ReadUserPay()
{
	COleDateTime tNow = COleDateTime::GetCurrentTime();

	DWORD tCurrChk = tNow.GetDay() - 1;
	tCurrChk *= 24; tCurrChk += tNow.GetHour();
	tCurrChk *= 60; tCurrChk += tNow.GetMinute() / 60;
	
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
	
	strQuery.Format( "select a.*, b.xpaydate, b.xuserid from xuser_info a"
					" left join xuserpay_his b on a.xjsjbh = b.xuserid"
					" where xpreBala+xBalance +"
					" (select sum(xfixedfee) from xfeetable where xtalktype='FECE' and xfeetableid in"
					" (select xfeecode from xusefee c where c.xjsjbh = a.xjsjbh)) < %i"
					" and xstatus = 1", Settings.Payer.PayBalan );
#ifdef _DEBUG
	strQuery += " and xjsjbh='xuser20060301000007'";
#endif
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr pRecordset;
	xOpenRecordset(pRecordset, m_pConnection, (_bstr_t)strQuery);
	
	for ( ; ! pRecordset->adoEOF && m_bThread ; pRecordset->MoveNext() )
	{
		_variant_t xUserId  = pRecordset->GetCollect( "xjsjbh" );
		if ( xUserId.vt == VT_NULL || xUserId.vt == VT_EMPTY ) continue;
		
		_variant_t xPhone   = pRecordset->GetCollect( "xphone" );
		_variant_t xBalance = pRecordset->GetCollect( "xBalance" );
		_variant_t xpreBala = pRecordset->GetCollect( "xpreBala" );
#if 1
		if ( (double)xBalance + (double)xpreBala < 0.10 )
		{
			pRecordset->PutCollect( "xstatus", _variant_t("0") );
		}
#endif
		_variant_t xPayDate = pRecordset->GetCollect( "xpaydate" );
		//_variant_t xUserId  = pRecordset->GetCollect( "xjsjbh" );
		
		COleDateTime tPayDate;
		if ( xPayDate.vt != VT_NULL ) tPayDate = COleDateTime( xPayDate );
		else tPayDate = COleDateTime( 1900, 1, 1, 0, 0, 0 );
		
		if ( ! CheckInMonth( tPayDate, tPayDate ) ) continue;
		_variant_t xBookmark= pRecordset->Bookmark;
		
		if ( xPayDate.vt == VT_NULL )
		{
			pRecordset->AddNew();
			pRecordset->PutCollect( "xuserid", xUserId );
		}
		
		tPayDate = COleDateTime::GetCurrentTime();
		pRecordset->PutCollect( "xpaydate", _variant_t(tPayDate) );
		pRecordset->Update();
		
		pRecordset->Bookmark= xBookmark;
		BuildPagerRec( (LPCTSTR)(_bstr_t)xPhone );
		
		CString xContent;
		xContent.Format( "催费: %.2f + %.2f",
				(VT_NULL == xBalance.vt) ? 0.00 : (double)xBalance,
				(VT_NULL == xpreBala.vt) ? 0.00 : (double)xpreBala );
		
		m_wndList.Print( MSG_DEFAULT, (LPCTSTR)(_bstr_t)xPhone, "N:=3", (LPCTSTR)xContent );
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
	
	_RecordsetPtr Recordhis = xExecute(m_pConnection, (_bstr_t)strQuery, adCmdText);
	if ( Recordhis->adoEOF ) return TRUE;
	
	_variant_t xPayDate = Recordhis->GetCollect( "xpaydate" );
	if ( VT_NULL == xPayDate.vt ) return FALSE;
	
	strQuery.Format( "select top 1 xtimestart from xusercost"
					" where xjsjbh = '%s' order by xtimestart desc", xUserId );
	TRACE( strQuery + "\n" );
	
	Recordhis = xExecute(m_pConnection, (_bstr_t)strQuery, adCmdText);
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
	
	xExecute(m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
	
	return TRUE;
}

BOOL CPluginWnd::BuildPagerRec(LPCTSTR xUserId)
{
	CString	strQuery;
	
	strQuery.Format( "insert into xuservoice with (tablock)"
					" (xPCMName,xPhoneNum,xRecDate,xCaller)"
					" values('%s','%s',getdate(),'%s')",
					Settings.Payer.PcmPager, xUserId, "057496031" );
	TRACE( strQuery + "\n" );
	
	xExecute(m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
	
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
	if ( *xFeeType == 'C' )
	{
		*pnElapse  = 365;
		*pnExpense  = (double)_ttoi(xFeeType + 1);
		
		return TRUE;
	}
	
	return FALSE;
}

BOOL CPluginWnd::CheckInMonth(COleDateTime dt, COleDateTime& et) const
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

BOOL CPluginWnd::CheckInDay(COleDateTime dt, COleDateTime& et) const
{
TRACE("--- %s\n",dt.Format("%Y-%m-%d %X"));
TRACE("+++ %s\n",et.Format("%Y-%m-%d %X"));
	COleDateTime tNow = COleDateTime::GetCurrentTime();
	if ( tNow < dt || et > tNow ) return FALSE;
	
	et += COleDateTimeSpan( 1, 0, 0, 0 );
	et = max( dt, et );
TRACE("<<< %s\n",et.Format("%Y-%m-%d %X"));
	return ( et <= tNow );
}

BOOL CPluginWnd::CheckInYear(COleDateTime dt, COleDateTime& et) const
{
TRACE("--- %s\n",dt.Format("%Y-%m-%d %X"));
TRACE("+++ %s\n",et.Format("%Y-%m-%d %X"));
	COleDateTime tNow = COleDateTime::GetCurrentTime();
	if ( tNow < dt || et > tNow ) return FALSE;
	
	dt.SetDate( dt.GetYear(), dt.GetMonth(), dt.GetDay() );
	et.SetDate( et.GetYear(), et.GetMonth(), et.GetDay() );
	//if ( et < dt ) { et = dt; return TRUE;}
	
	INT nYearDays = 365;
	if ( IsLeapYear( et.GetYear() ) ) nYearDays = 366;
	
	et += COleDateTimeSpan( nYearDays, tNow.GetHour(), tNow.GetMinute(), tNow.GetSecond() );
	et = max( dt, et );
TRACE("<<< %s\n",et.Format("%Y-%m-%d %X"));
	return ( et <= tNow );
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
	tCurrChk *= 100; tCurrChk += tNow.GetMonth();
	
	if ( tCurrChk == Settings.SubSm.Cookie ) return FALSE;
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
	
	Settings.SubSm.Cookie = tCurrChk;
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
	
	_RecordsetPtr Recordptr = xExecute(m_pConnection, (_bstr_t)strQuery, adCmdText);
	
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
		xExecute(m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
		
		CString strValue;
		strValue.Format( "xaddBala: %.2f", nGiven );
		m_wndList.Print( MSG_DEFAULT, (LPCTSTR)(_bstr_t)xPhone, "<+SM>", (LPCTSTR)strValue );
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
	xOpenRecordset(pRecordset, m_pConnection, (_bstr_t)strQuery);
	
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
		xExecute(m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
		pRecordset->Delete( adAffectCurrent ); continue;
		
		CString strValue;
		strValue.Format( "xaddBala: %.2f", -nGiven );
		m_wndList.Print( MSG_DEFAULT, (LPCTSTR)xPhoneNum, "<-SM>", (LPCTSTR)strValue );
	}
	
	return TRUE;
}

BOOL CPluginWnd::GetUserId_Of(LPCTSTR xPhone, CString& xUserId)
{
	CString	strQuery;
	
	strQuery.Format( "select xjsjbh from xuser_info"
					" where xPhone = '%s'", xPhone );
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr Recordusr = xExecute(m_pConnection, (_bstr_t)strQuery, adCmdText);
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
	
	_RecordsetPtr Recordusr = xExecute(m_pConnection, (_bstr_t)strQuery, adCmdText);
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
	xOpenRecordset(pRecordset, m_pConnection, (_bstr_t)strQuery);
	
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
	
	_RecordsetPtr Recordsub = xExecute(m_pConnection, (_bstr_t)strQuery, adCmdText);
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
