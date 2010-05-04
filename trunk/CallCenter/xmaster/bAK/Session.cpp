//
// Session.cpp
//

#include "stdafx.h"
#include "Xmaster.h"
#include "Session.h"
#include "Settings.h"
#include "Network.h"

#include "WndMain.h"
#include "WndNetwork.h"

#include "CtrlNetworkChild.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CSession construction

CSession::CSession(SOCKET hSocket, SOCKADDR_IN* pHost) : CTransfer( hSocket, pHost )
{
	m_pOwner		= NULL;
	m_bNavigated	= FALSE;
	m_tNavigated	= 0;
	m_nProgress		= EProgressNull;
	m_nSyncMoney	= 0.0;
	m_nSyncRepeat	= 1;
}

CSession::~CSession()
{
}

//////////////////////////////////////////////////////////////////////
// CSession attributes

BOOL CSession::IsOnline() const
{
	return ( IsConnected() && m_nProgress >= EProgressReady );
}

BOOL CSession::IsNavigated() const
{
	return ( IsOnline() && m_nProgress >= EProgressNavigate && m_bNavigated );
}

void CSession::SetOwner(CNetworkChild* pOwner)
{
	m_pOwner = pOwner;
}

//////////////////////////////////////////////////////////////////////
// CSession disconnect

void CSession::Disconnect()
{
	if ( IsNavigated() ) ReckonBill();
	if ( m_pOwner ) m_pOwner->RemoveItem( this );
	
	CTransfer::Disconnect(); delete this;
}

//////////////////////////////////////////////////////////////////////
// CSession run function

BOOL CSession::OnRun()
{
	if ( ! CTransfer::OnRun() ) return FALSE;

	DWORD tNow = GetTickCount();
	
	if ( m_nProgress == EProgressReady && tNow - m_tConnected >= 900* 1000 )
	{
		OnDropped( FALSE ); return FALSE;
	}
	
	if ( m_nProgress == EProgressNavigate && tNow >= m_tConnected )
	{
		OnNavigate( tNow ); m_tConnected = tNow - tNow % 1000 + 1000; return TRUE;
	}
	
	if ( m_nProgress == EProgressComplete )
	{
		OnDropped( TRUE ); return FALSE;
	}

	return TRUE;
}

BOOL CSession::OnPacket(IPacket* pPacket)
{
	if ( pPacket->IsCommandId( ICP_TALK ) )
	{
		return OnTalkPacket( (ITalkPacket*)pPacket );
	}
	else
	if ( pPacket->IsCommandId( ICP_SYNC ) )
	{
		return OnSyncPacket( (ISyncPacket*)pPacket );
	}
	
	return CTransfer::OnPacket(pPacket);
}

BOOL CSession::OnNavigate(DWORD tNow, DWORD* pnLength, double* pnExpense)
{
	DWORD nLength = (tNow - m_tNavigated) / 1000;
	if ( pnLength ) *pnLength = nLength;
	if ( m_pOwner ) m_pOwner->PrintLength( this, nLength );
	
	if ( nLength <= Settings.Network.ShtFree ) return TRUE;
	
	double nUsrBala = CalcUserBlance( );
	double nExpense = CalcExpense( nLength, nUsrBala );
	if ( pnExpense ) * pnExpense = nExpense;
	
	double nExpense2= CalcExpense( nLength + 1, nUsrBala );
	if ( nExpense2 > 0 && nExpense2 > nUsrBala )
	{
		if ( m_nProgress == EProgressNavigate ) SendFailPacket( ICP_NOPAY );
		m_nProgress = EProgressComplete; return FALSE;
	}
	
	m_pOwner->PrintExpense( this, nExpense );
	if ( pnExpense ) *pnExpense = nExpense;
	
	return TRUE;
}

BOOL CSession::OnBindPacket(IBindPacket* pPacket)
{
	if ( m_nProgress != EProgressNull ) return FALSE;

	CString xLocalId, xRemoteId, xFeeNumber, xCallType;

	int nLength = pPacket->GetRemoteId( xRemoteId.GetBuffer( 21 ) );
	xRemoteId.ReleaseBuffer( nLength );
	
	nLength = pPacket->GetLocalId( xLocalId.GetBuffer( 21 ) );
	xLocalId.ReleaseBuffer( nLength );
	
	nLength = pPacket->GetFeeNumber( xFeeNumber.GetBuffer( 21 ) );
	xFeeNumber.ReleaseBuffer( nLength );
	
	nLength = pPacket->GetCallType( xCallType.GetBuffer( 4 ) );
	xCallType.ReleaseBuffer( nLength );
	
	AddSession( xLocalId, xRemoteId, xFeeNumber );

	if ( Network.IsDenied(xRemoteId) )
	{
		m_nProgress = EProgressComplete;
		m_pOwner->SetFeeEntry( this, "N/A", FALSE );
		SendFailPacket( ICP_ERROR ); return FALSE;
	}
	
	if ( ! ReadFeeEntry((LPCTSTR)xLocalId, (LPCTSTR)xCallType) ||
		 ! ChkUserState(xFeeNumber.IsEmpty() ? xRemoteId : xFeeNumber, xCallType) )
	{
#if 1
		if ( xLocalId.ReverseFind( '#' ) == -1 )
		{
			xLocalId += '#'; pPacket->SetLocalId( xLocalId );
			return OnBindPacket( pPacket );
		}
#endif
		m_nProgress = EProgressComplete;
		m_pOwner->SetFeeEntry( this, "N/A", FALSE );
		SendFailPacket( ICP_NOUSER ); return FALSE;
	}
	
	_variant_t xFeeCode = m_pRecordfee->GetCollect( "xfeetableid" );
	//_variant_t xFeeType	= m_pRecordfee->GetCollect( "xfeetype" );
	m_pOwner->SetFeeEntry( this, (LPCTSTR)(_bstr_t)xFeeCode );
	
	if ( int nResult = ReadUsrState( (LPCTSTR)xCallType ) )
	{
#if 1
		if ( ! xCallType.CompareNoCase( "CATK") && nResult != 8888 )
		if ( xLocalId.ReverseFind( '#' ) == -1 )
		{
			xLocalId.Insert(5,'+'); pPacket->SetLocalId( xLocalId );
			return OnBindPacket( pPacket );
		}
#endif
		m_nProgress = EProgressComplete;
		SendFailPacket( nResult ); return FALSE;
	}
	
	IBindPacket* pPacketResp = new IBindPacket;
	pPacketResp->SetFeeTable( (LPCTSTR)(_bstr_t)xFeeCode );
	SendPacket( pPacketResp );

	AddExpense( xFeeNumber.IsEmpty() ? NULL : (LPCTSTR)xRemoteId );
	
	m_nProgress	= EProgressReady;
	return CTransfer::OnBindPacket(pPacket);
}

BOOL CSession::OnExitPacket(IExitPacket* pPacket)
{
	m_nProgress = EProgressComplete;
	return CTransfer::OnExitPacket(pPacket);
}

BOOL CSession::OnTalkPacket(ITalkPacket* pPacket)
{
	if ( m_nProgress != EProgressReady ) return FALSE;
	
	m_nProgress	= EProgressNavigate; 
	m_tNavigated= GetTickCount();
	m_bNavigated= TRUE;
	
	m_pOwner->SetState( this, DS_NAVIGATE );

	return TRUE;
}

BOOL CSession::OnSyncPacket(ISyncPacket* pPacket)
{
	if ( m_nProgress != EProgressNavigate ) return FALSE;

	m_nSyncMoney += pPacket->SyncMoney;
	m_nSyncRepeat+= pPacket->SyncRepeat;
	OnNavigate( GetTickCount() );
	
	return TRUE;
}

BOOL CSession::ReckonBill(DWORD* pnLength, double* pnExpense)
{
	DWORD nTotalLength = 0;
	double nTotalExpense = 0.00;
	OnNavigate( GetTickCount(), &nTotalLength, &nTotalExpense );
	WriteExpense( nTotalLength, nTotalExpense );
	
	if ( pnLength ) *pnLength = nTotalLength;
	if ( pnExpense ) *pnExpense = nTotalExpense;
	
	return TRUE;
}

BOOL CSession::ReadFeeEntry(LPCTSTR xPhonNum, LPCTSTR xTalkType)
{
	CString strQuery;

	int nLength = _tcslen( xPhonNum );

	if ( nLength && *( xPhonNum + _tcslen(xPhonNum) - 1 ) == '#' )
	{
		strQuery.Format( "select * from xfeetable"
						" where left('%s',len(xphone)) = xphone", xPhonNum );
//						" and xtalktype like '%%%s%%'", xPhonNum, xTalkType );
	}
	else
	{
		strQuery.Format( "select * from xfeetable"
						" where xphone = '%s'", xPhonNum );
	}
	
	CString strFilter;
	if ( ! _tcscmp(xTalkType, "CABK") )	// 回呼
	{
		strFilter.Format( "xtalktype = 'CATK' and xcallback = '1'" );
	}
	else
	{
		strFilter.Format( "xtalktype = '%s'", xTalkType );
	}
	
	strQuery += _T(" and ") + strFilter;
	strQuery += " order by len(xphone) desc";
#ifdef _DEBUG
	afxDump << (LPCTSTR)strQuery << "\n";
#endif

	xOpenRecordset(m_pRecordfee, theApp.m_pConnection, (_bstr_t)strQuery);
#ifdef _DEBUG
	if ( ! m_pRecordfee->adoEOF ) ASSERT( m_pRecordfee->GetRecordCount() == 1 );
#endif
	
	return ( m_pRecordfee->adoEOF == FALSE );
}

int CSession::ReadUsrState(LPCTSTR xTalkType)
{
	_variant_t xFeeCode = m_pRecordfee->GetCollect( "xfeetableid" );
	_variant_t xFeeType	= m_pRecordfee->GetCollect( "xfeetype" );
	
	if ( xFeeType.vt != VT_NULL && _tcslen( (LPCTSTR)(_bstr_t)xFeeType ) &&
		! ReadUsrType((LPCTSTR)(_bstr_t)xFeeCode, (LPCTSTR)(_bstr_t)xFeeType) ) return ICP_NOPAY;
	
	_variant_t xCallBack = m_pRecordfee->GetCollect( "xcallback" );
	if ( xCallBack.vt != VT_NULL && ! _tcscmp((LPCTSTR)(_bstr_t)xCallBack, "1") )
	if ( ! _tcscmp(xTalkType, "CATK") && IsUsrRegisted() && ReadCallBack() ) return 8888;
	
	double nUsrBala = CalcUserBlance();
	double nExpense = CalcExpense(1, nUsrBala);
	
	if ( nExpense > 0 && nExpense > nUsrBala ) return ICP_NOPAY;
	
	return ICP_SUCCESS;
}

BOOL CSession::ReadCallBack(double* pnExpense)
{
	CString	strQuery;
	
	strQuery.Format( "select * from xfeetable"
					" where xfeetableid = '8888'" );
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr Recordfee = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	if ( Recordfee->adoEOF ) return FALSE;
	
	_variant_t xFeeType	= Recordfee->GetCollect( "xfeetype" );
	if ( xFeeType.vt != VT_NULL && _tcslen( (LPCTSTR)(_bstr_t)xFeeType ) &&
		! ReadUsrType("8888", (LPCTSTR)(_bstr_t)xFeeType, pnExpense) ) return FALSE;
	
#if 0
	// 检查回呼保证金
	if ( CalcUserBlance() < 10.00 ) 
	{
#if 1
		_variant_t xRemoteId = m_pRecordusr->GetCollect( "xphone" );
		_variant_t xLocalId = m_pRecordfee->GetCollect( "xphone" );
		
		theApp.Message(MSG_ERROR, "%s -> %s: CalcUserBlance[%.2f] < 10.00",
					(LPCTSTR)(_bstr_t)xRemoteId,
					(LPCTSTR)(_bstr_t)xLocalId,
					CalcUserBlance() );
#endif
		return FALSE;
	}
#endif
	
	return TRUE;
}

BOOL CSession::ReadUsrType(LPCTSTR xFeeCode, LPCTSTR xFeeType, double* pnExpense)
{
	if ( ! ReadFeeType(xFeeType, NULL, pnExpense) ) return FALSE;
	_variant_t xUserId = m_pRecordusr->GetCollect( "xjsjbh" );
	
	CString	strQuery;
	
	strQuery.Format( "select * from xusefee"
					" where xfeecode = '%s' and xjsjbh = '%s'",
					xFeeCode, (LPCTSTR)(_bstr_t)xUserId );
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr Recordfee = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	
	return ( Recordfee->adoEOF == FALSE );
}

BOOL CSession::ReadFeeType(LPCTSTR xFeeType, int* pnElapse, double* pnExpense) const
{
	if ( *xFeeType == 'A' )
	{
		if ( pnElapse ) *pnElapse = 31;
		if ( pnExpense ) *pnExpense = (double)_ttoi(xFeeType + 1);
		
		return TRUE;
	}
	if ( *xFeeType == 'B' )
	{
		if ( pnElapse ) *pnElapse = 1;
		if ( pnExpense ) *pnExpense = (double)_ttoi(xFeeType + 1) / 100;
		
		return TRUE;
	}
	
	return FALSE;
}

BOOL CSession::ChkUserState(LPCTSTR xUsrNumber, LPCTSTR xCallType)
{
	CString	strQuery;
	
	strQuery.Format( "select a.*, a.xphone as PhoneNum, b.xuserbindid"
					" from xuser_info a inner join"
					" xuser b on a.xjsjbh = b.xjsjbh"
					" where b.xphone = '%s'", xUsrNumber );
	
	TRACE( strQuery + "\n" );
	
	xOpenRecordset(m_pRecordusr, theApp.m_pConnection, (_bstr_t)strQuery);
	
	if ( m_pRecordusr->adoEOF && Settings.FreeTry.Enable && ! _tcscmp(xCallType, "CATK") )
	{
		strQuery.Format( "EXECUTE addnewuser '%s'", xUsrNumber );
		TRACE( strQuery + "\n" );
		xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
		m_pRecordusr->Requery( adCmdUnknown );
	}
#if 1
	else
	if ( m_pRecordusr->RecordCount > 1 )
	{
		CString strFilter;
		strFilter.Format( "PhoneNum <> '%s'", xUsrNumber );
		m_pRecordusr->Filter = (_bstr_t)strFilter;
	}
#endif
	
	return ( m_pRecordusr->adoEOF == FALSE );
}

BOOL CSession::IsUsrRegisted()
{
	_variant_t TheValue = m_pRecordusr->GetCollect( "xstatus" );
	if ( VT_NULL == TheValue.vt ) return FALSE;
	
	CString xStatus = (LPCTSTR)(_bstr_t)TheValue;
	xStatus.TrimLeft(); xStatus.TrimRight();
	
	return xStatus.CompareNoCase("1") == 0;
}

BOOL CSession::AddSession(LPCTSTR xLocaldId, LPCTSTR xUsrNumber, LPCTSTR xFeeNumber)
{
	CMainWnd* pMainWnd = (CMainWnd*)theApp.m_pMainWnd;
	if ( ! pMainWnd || ! pMainWnd->GetSafeHwnd() ) return FALSE;
	
	CNetworkWnd* pNetworkWnd = (CNetworkWnd*)pMainWnd->GetWindow( RUNTIME_CLASS(CNetworkWnd) );
	pNetworkWnd->SendMessage( WM_ADDSESSION, (WPARAM)this );
	ASSERT( m_pOwner && m_pOwner->IsKindOf( RUNTIME_CLASS(CNetworkChild) ) );

	m_pOwner->SetPrimary( this, xUsrNumber, xLocaldId, xFeeNumber );
	
	return TRUE;
}

void CSession::AddExpense(LPCTSTR xThirdId, BOOL bUpdate)
{
	CString	strQuery;
	
	strQuery.Format( "select top 0 * from XuserDetail");
	TRACE( strQuery + "\n" );
	
	xOpenRecordset(m_pRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
	m_pRecordset->AddNew();

	_variant_t TheValue;
	
	TheValue = m_pRecordusr->GetCollect( "xjsjbh" );
	m_pRecordset->PutCollect( "xjsjbh", TheValue );
	TheValue = m_pRecordusr->GetCollect( "xuserbindid" );
	m_pRecordset->PutCollect( "xuserbindid", TheValue );
	
	TheValue = m_pRecordfee->GetCollect( "xphone" );
	m_pRecordset->PutCollect( "xFwdPhone", TheValue );
	
	TheValue = m_pRecordfee->GetCollect( "xjsjbh" );
	m_pRecordset->PutCollect( "xfeetableId", TheValue );
	
	COleDateTime tt = COleDateTime::GetCurrentTime();
	m_pRecordset->PutCollect( "xTimeStart", _variant_t(tt) );
	
	if ( xThirdId ) m_pRecordset->PutCollect( "xthirdid", _variant_t(xThirdId) );
	
	if ( bUpdate ) m_pRecordset->Update();
	m_pRecordset->Resync( adAffectAll, adResyncAllValues );
}

double CSession::WriteExpense(DWORD nLength, double nExpense)
{
	m_pRecordset->Resync( adAffectAll, adResyncAllValues );
	
	m_pRecordset->PutCollect( "xTimeLength", _variant_t((long)nLength) );
	m_pRecordset->PutCollect( "xExpense", _variant_t(nExpense) );
	
	m_pRecordset->Update();
	
	m_pRecordusr->Resync( adAffectAll, adResyncAllValues );
//	m_pRecordusr->Requery( adCmdUnknown );

	// 先扣优惠余额
	_variant_t TheValue;
	
	TheValue = m_pRecordusr->GetCollect( "xpreBala" );
	if ( VT_NULL != TheValue.vt && (double)TheValue > 0 )
	{
		if ( (double)TheValue >= nExpense )
		{
			nExpense = (double)TheValue - nExpense;
			m_pRecordusr->PutCollect( "xpreBala", _variant_t((double)nExpense) );
			nExpense = 0.00;
		}
		else
		{
			m_pRecordusr->PutCollect( "xpreBala", _variant_t((double)0.00) );
			nExpense = nExpense - (double)TheValue;
		}
	}
	
	// 后扣实际余额
	if ( nExpense > 0 )
	{
		TheValue = m_pRecordusr->GetCollect( "xBalance" );
		m_pRecordusr->PutCollect( "xBalance", _variant_t((double)TheValue - nExpense) );
	}
	
	// 更新用户状态
	TheValue = m_pRecordusr->GetCollect( "xBalance" );
	double xBala = (TheValue.vt == VT_NULL) ? 0 : (double)TheValue;
	TheValue = m_pRecordusr->GetCollect( "xpreBala" );
	if ( VT_NULL != TheValue.vt ) xBala += (double)TheValue;
	
	if ( xBala <= 0 ) m_pRecordusr->PutCollect( "xStatus", _variant_t("0") );

	m_pRecordusr->Update();
	
	TheValue = m_pRecordusr->GetCollect( "xBalance" );
	if ( VT_NULL == TheValue.vt ) return 0;
	
	return double(TheValue);
}

double CSession::CalcUserBlance()
{
	// Current Balance:= xpreBala + xBalance

	double nBalance = 0;
	_variant_t TheValue;

	TheValue = m_pRecordusr->GetCollect( "xpreBala" );
	if ( VT_NULL != TheValue.vt ) nBalance += (double)TheValue;
	TheValue = m_pRecordusr->GetCollect( "xBalance" );
	if ( VT_NULL != TheValue.vt ) nBalance += (double)TheValue;

	return nBalance;
}

double CSession::CalcExpense(DWORD nLength)
{
	_variant_t TheValue = m_pRecordfee->GetCollect( "xFeeRule" );
	int nFeeRule = (TheValue.vt != VT_NULL) ? TheValue.lVal : 0x7FFFFF;
	
	int nFeePoint = nLength / nFeeRule + 1;
	
	TheValue = m_pRecordfee->GetCollect( "xFeeValue" );
	double nExpense = nFeePoint * (double)TheValue;
	
#if 1
	// 检查封顶费用
	nExpense = InfFixedFee( nExpense );
#endif
	
#if 1
	// 同步扣费
	nExpense += m_nSyncMoney;
	nExpense *= m_nSyncRepeat;
#endif
	
	return nExpense;
}

double CSession::CalcExpense(DWORD nLength, double nUpFlow)
{
	double nExpense = CalcExpense( nLength );
	if ( nExpense <= nUpFlow ) return nExpense;
	
	if ( ! Settings.GivenTry.Enable ) return nExpense;
	
	_variant_t TheValue = m_pRecordfee->GetCollect( "xtalktype" );
	if ( _tcscmp((LPCTSTR)(_bstr_t)TheValue, "CATK") ) return nExpense;
	
	// 绿色通道服务
	
	if ( (INT)nLength > Settings.GivenTry.MaxLength ) return nExpense;
	
	TheValue = m_pRecordfee->GetCollect( "xphone" );
	
	if ( ! Settings.GivenTry.LocalId.CompareNoCase( (LPCTSTR)(_bstr_t)TheValue ) )
	{
		nExpense = max( nUpFlow, 0 );
	}
	
	return nExpense;
}

double CSession::InfFixedFee(double nExpense)
{
	_variant_t TheValue;

	TheValue = m_pRecordfee->GetCollect( "xFixedFee" );
	if ( TheValue.vt == VT_NULL || (double)TheValue == 0 ) return nExpense;

	if ( (double)TheValue < nExpense ) return double(TheValue);

	return nExpense;
}
