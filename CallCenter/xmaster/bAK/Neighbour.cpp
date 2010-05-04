//
// Neighbour.cpp
//

#include "stdafx.h"
#include "Xmaster.h"
#include "Neighbour.h"
#include "Settings.h"
#include "Network.h"
#include "Profiles.h"

#include "WndMain.h"
#include "WndNetwork.h"

#include "WndMain.h"
#include "WndNetwork.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CNeighbour construction

CNeighbour::CNeighbour(SOCKET hSocket, SOCKADDR_IN* pHost) : CTransfer( hSocket, pHost )
{
	m_pHomeWnd		= NULL;
	m_bNavigated	= FALSE;
	m_tNavigated	= 0;
	m_nSyncMoney	= 0.0;
	m_nSyncRepeat	= 1;
	m_tExpensed		= 0;
	
	CNeighbour::Setup();
}

CNeighbour::~CNeighbour()
{
	if ( IsConnected() )
	Disconnect();
}

//////////////////////////////////////////////////////////////////////
// CNeighbour attributes

BOOL CNeighbour::IsNavigated() const
{
	return ( IsConnected() && m_bNavigated );
}

void CNeighbour::Setup()
{
	CMainWnd* pMainWnd	= (CMainWnd*)theApp.m_pMainWnd;
	ASSERT( pMainWnd->GetSafeHwnd() != NULL );

	m_pHomeWnd			= (CNetworkWnd*)pMainWnd->GetWindow( RUNTIME_CLASS(CNetworkWnd) );
	ASSERT( m_pHomeWnd->GetSafeHwnd() != NULL );
}

//////////////////////////////////////////////////////////////////////
// CNeighbour disconnect

void CNeighbour::Disconnect()
{
#if 1
	if ( IsNavigated() ) ReckonBill();
#endif
	CTransfer::Disconnect();
	if ( m_pHomeWnd ) m_pHomeWnd->RemoveSession( this );
	delete this;
}

#if 0
void CNeighbour::OnDropped(BOOL bError)
{
	if ( bError == FALSE )
	{
		CString strHost = inet_ntoa( m_pHost.sin_addr );
		
		try
		{
		
		if ( m_pRecordfee != NULL && ! m_pRecordfee->adoEOF )
		{
			_variant_t xFeeCode = m_pRecordfee->GetCollect( "xfeetableid" );
			strHost += " xfeetableid:= "; strHost += (LPCTSTR)(_bstr_t)xFeeCode;
		}
		
		}
		catch(_com_error e) 
		{ 
		theApp.Message( MSG_ERROR, "CNeighbour::OnDropped: %s", e.ErrorMessage() );
		}
		
		theApp.Message( MSG_ERROR, "WARNING: Dropped Connection From %s", strHost );
	}
	
	Disconnect();
}
#endif
//////////////////////////////////////////////////////////////////////
// CNeighbour run function

BOOL CNeighbour::OnRun()
{
	if ( ! CTransfer::OnRun() ) return FALSE;

	DWORD tNow = GetTickCount();
	
	if ( ! IsNavigated() && tNow - m_tConnected >= Settings.General.MaxReadyOut * 1000 )
	{
		OnDropped( WSA_WAIT_TIMEOUT ); return FALSE;
	}
	
	if ( IsNavigated() && tNow >= m_tConnected )
	{
		m_tConnected = tNow - tNow % 1000 + 1000; 
		if ( ! OnNavigate( tNow ) ) OnDropped( ICP_ERROR );
		return TRUE;
	}

	return TRUE;
}

BOOL CNeighbour::OnPacket(IPacket* pPacket)
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

BOOL CNeighbour::OnNavigate(DWORD tNow)
{
	DWORD nLength = (tNow - m_tNavigated) / 1000;
	if ( m_pHomeWnd ) m_pHomeWnd->PrintLength( this, nLength );
	
//theApp.Message( MSG_TEMP,"%i - %i=%i", nLength, Settings.Network.ShtFree,
//			   nLength-Settings.Network.ShtFree);
	if ( nLength < Settings.Network.ShtFree ) return TRUE;
	
	double nExpense = 0;
	int nFeeRule = CalcExpense( nLength, m_tExpensed, nExpense );
//theApp.Message(MSG_ERROR, "CalcExpense(%i, %i, %.2f) = %i", nLength, m_tExpensed, nExpense, nFeeRule);
	if ( nFeeRule == -1 ) return TRUE;
	
	m_tExpensed = m_tNavigated + nFeeRule * ( nLength / nFeeRule ) * 1000;
	WriteExpense( nLength, nExpense, FALSE );
	
	if ( ! CheckGivenTry() )
	{
		SendFailPacket( ICP_NOPAY ); return FALSE;
	}
	
	m_pHomeWnd->PrintExpense( this, nExpense );
	
	return TRUE;
}

BOOL CNeighbour::OnNavigate(DWORD tNow, DWORD* pnLength, double* pnExpense)
{
	DWORD nLength = (tNow - m_tNavigated) / 1000;
	if ( ! IsNavigated() ) nLength = 0;
	if ( m_pHomeWnd ) m_pHomeWnd->PrintLength( this, nLength );
	
	double nExpense = 0;
	if ( ! CalcExpense( nLength, nExpense ) ) return TRUE;
	
	m_tExpensed = tNow;
	WriteExpense( nLength, nExpense );
	
	m_pHomeWnd->PrintExpense( this, nExpense );
	
	return TRUE;
}

BOOL CNeighbour::OnBindPacket(IBindPacket* pPacket)
{
	//if ( IsConnected() ) return FALSE;
	
	CString xLocalId, xRemoteId, xFeeNumber, xCallType;
	
	int nLength = pPacket->GetRemoteId( xRemoteId.GetBuffer( 21 ) );
	xRemoteId.ReleaseBuffer( nLength );
	
	nLength = pPacket->GetLocalId( xLocalId.GetBuffer( 21 ) );
	xLocalId.ReleaseBuffer( nLength );
	
	nLength = pPacket->GetFeeNumber( xFeeNumber.GetBuffer( 21 ) );
	xFeeNumber.ReleaseBuffer( nLength );
	
	nLength = pPacket->GetCallType( xCallType.GetBuffer( 4 ) );
	xCallType.ReleaseBuffer( nLength );
	
	AddSession( xLocalId, xRemoteId, xFeeNumber.IsEmpty() ? NULL : (LPCTSTR)xFeeNumber );
	if ( xFeeNumber.IsEmpty() ) xFeeNumber = (xCallType == "CAHK") ? xLocalId : xRemoteId;
	
	if ( Profiles.IsDenied(xRemoteId) )
	{
		m_pHomeWnd->SetFeeEntry( this, "N/A", FALSE );
		SendFailPacket( ICP_ERROR ); return FALSE;
	}
	
	if ( ! ReadFeeEntry(xLocalId, xCallType) || ! ChkUserState(xFeeNumber, xCallType) )
	{
#if 1
		if ( xLocalId.ReverseFind( '#' ) == -1 )
		{
			xLocalId += '#'; pPacket->SetLocalId( xLocalId );
			return OnBindPacket( pPacket );
		}
#endif
		m_pHomeWnd->SetFeeEntry( this, "N/A", FALSE );
		SendFailPacket( ICP_NOUSER ); return FALSE;
	}
	
	_variant_t xFeeCode = m_pRecordfee->GetCollect( "xfeetableid" );
	//_variant_t xFeeType	= m_pRecordfee->GetCollect( "xfeetype" );
	m_pHomeWnd->SetFeeEntry( this, (LPCTSTR)(_bstr_t)xFeeCode );
	
#if 1	// 外呼: 区分本地/长途
	ChockTalkRemote( xRemoteId, xCallType, (LPCTSTR)(_bstr_t)xFeeCode );
#endif
	
	if ( int nResult = ReadUsrState( (LPCTSTR)xCallType ) )
	{
#if 0
		if ( ! xCallType.CompareNoCase( "CATK") && nResult != 8888 )
		if ( xLocalId.ReverseFind( '#' ) == -1 )
		{
			xLocalId.Insert(5,'+'); pPacket->SetLocalId( xLocalId );
			return OnBindPacket( pPacket );
		}
#endif
		SendFailPacket( nResult );
		OnDropped( nResult ); return FALSE;
	}
	
	IBindPacket* pPacketResp = new IBindPacket;
	pPacketResp->SetFeeTable( (LPCTSTR)(_bstr_t)xFeeCode );
	SendPacket( pPacketResp );
	
	if ( IsConnected() ) OnNavigate( GetTickCount(), NULL, NULL );
	AddExpense( (xCallType == "CAHK") ? (LPCTSTR)xRemoteId : NULL, xCallType, FALSE );
	m_bNavigated	= FALSE;
	m_tNavigated	= 0;
	
	return CTransfer::OnBindPacket(pPacket);
}

BOOL CNeighbour::OnExitPacket(IExitPacket* pPacket)
{
	CTransfer::OnExitPacket(pPacket);
	return FALSE;
}

BOOL CNeighbour::OnTalkPacket(ITalkPacket* pPacket)
{
//	if ( ! IsConnected() || IsNavigated() ) return FALSE;
	
	m_tNavigated	= GetTickCount();
	m_bNavigated	= TRUE;
	m_tExpensed		= 0;
	
	m_pHomeWnd->SetState( this, DS_NAVIGATE );
	
	return TRUE;
}

BOOL CNeighbour::OnSyncPacket(ISyncPacket* pPacket)
{
	if ( ! IsNavigated() ) return FALSE;
	
	m_nSyncMoney += pPacket->SyncMoney;
	m_nSyncRepeat+= pPacket->SyncRepeat;
	OnNavigate( GetTickCount() );
	
	return TRUE;
}
#if 1
BOOL CNeighbour::ReckonBill(DWORD* pnLength, double* pnExpense)
{
	DWORD nTotalLength = 0;
	double nTotalExpense = 0.00;
	
	OnNavigate( GetTickCount(), &nTotalLength, &nTotalExpense );
	
	if ( pnLength ) *pnLength = nTotalLength;
	if ( pnExpense ) *pnExpense = nTotalExpense;
	
	return TRUE;
}
#endif
BOOL CNeighbour::ReadFeeEntry(LPCTSTR xPhonNum, LPCTSTR xTalkType)
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
#if 0
	if ( ! _tcscmp(xTalkType, "CABK") )	// 回呼
	{
		strFilter.Format( "xtalktype = 'CATK' and xcallback = '1'" );
	}
	else
#endif
	{
		strFilter.Format( "xtalktype = '%s'", xTalkType );
	}
	
	strQuery += _T(" and ") + strFilter;
	strQuery += " order by len(xphone) desc";
#ifdef _DEBUG
	afxDump << (LPCTSTR)strQuery << "\n";
#endif
	
	_RecordsetPtr Recordfee = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	//xOpenRecordset(m_pRecordfee, theApp.m_pConnection, (_bstr_t)strQuery);
	m_pRecordfee = Recordfee->Clone( adLockUnspecified );
	xCloseRecordset( Recordfee);
#ifdef _DEBUG
	if ( ! m_pRecordfee->adoEOF ) ASSERT( m_pRecordfee->GetRecordCount() == 1 );
#endif
	
	return ( m_pRecordfee->adoEOF == FALSE );
}

int CNeighbour::ReadUsrState(LPCTSTR xTalkType)
{
#if 0
	_variant_t xFeeCode = m_pRecordfee->GetCollect( "xfeetableid" );
	_variant_t xFeeType	= m_pRecordfee->GetCollect( "xfeetype" );
	
	if ( xFeeType.vt != VT_NULL && _tcslen( (LPCTSTR)(_bstr_t)xFeeType ) &&
		! ReadUsrType((LPCTSTR)(_bstr_t)xFeeCode, (LPCTSTR)(_bstr_t)xFeeType) ) return ICP_NOPAY;
#endif
	_variant_t xCallBack = m_pRecordfee->GetCollect( "xcallback" );
	if ( xCallBack.vt != VT_NULL && ! _tcscmp((LPCTSTR)(_bstr_t)xCallBack, "1") )
	if ( ! _tcscmp(xTalkType, "CATK") && IsUsrRegisted() && ReadCallBack() ) return ICP_CALLBACK;
	
#if 1	// 检查通话余额
	double nExpense = 0;
	double nBalance	= 0;
	
	CalcExpense( Settings.Network.ShtFree + 1, nExpense );
	if ( nExpense > 0 && nExpense > CalcUserBlance() ) return ICP_NOPAY;
#endif
	
	return ICP_SUCCESS;
}

BOOL CNeighbour::ReadCallBack(double* pnExpense)
{
#if 1	// 二次流水内不允许回呼
//	_variant_t TheValue = m_pRecordset->GetCollect( "xseqnumber" );
//	if ( VT_NULL != TheValue.vt ) return FALSE;
	if ( m_pRecordset != NULL && ! m_pRecordset->adoEOF ) return FALSE;
#endif
	
	CString	strQuery;
	
	strQuery.Format( "select * from xfeetable"
					" where xfeetableid = '%i'", ICP_CALLBACK );
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

BOOL CNeighbour::ReadUsrType(LPCTSTR xFeeCode, LPCTSTR xFeeType, double* pnExpense)
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

BOOL CNeighbour::ReadFeeType(LPCTSTR xFeeType, int* pnElapse, double* pnExpense) const
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

BOOL CNeighbour::ChkUserState(LPCTSTR xUsrNumber, LPCTSTR xCallType)
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

BOOL CNeighbour::IsUsrRegisted()
{
	_variant_t TheValue = m_pRecordusr->GetCollect( "xstatus" );
	if ( VT_NULL == TheValue.vt ) return FALSE;
	
	CString xStatus = (LPCTSTR)(_bstr_t)TheValue;
	xStatus.TrimLeft(); xStatus.TrimRight();
	
	return xStatus.CompareNoCase("1") == 0;
}

BOOL CNeighbour::AddSession(LPCTSTR xLocaldId, LPCTSTR xUsrNumber, LPCTSTR xFeeNumber)
{
	ASSERT( m_pHomeWnd && m_pHomeWnd->GetSafeHwnd() );
	
	m_pHomeWnd->RemoveSession( this, FALSE );
	m_pHomeWnd->AddSession( this, xUsrNumber, xLocaldId, xFeeNumber );
	
	return TRUE;
}

void CNeighbour::AddExpense(LPCTSTR xThirdId, LPCTSTR xTalkType, BOOL bUpdate)
{
	_variant_t TheValue;
	
	if ( m_pRecordset == NULL )
	{
		CString	strQuery;
		strQuery.Format( "select top 0 * from XuserDetail");
		TRACE( strQuery + "\n" );
		
		xOpenRecordset(m_pRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
		m_pRecordset->AddNew();
	}
	else
	{
		TheValue = m_pRecordset->GetCollect( "xseqnumber" );
		if ( VT_NULL == TheValue.vt ) TheValue = m_pRecordset->GetCollect( "jsjbh" );
		
		m_pRecordset->AddNew();
		m_pRecordset->PutCollect( "xseqnumber", TheValue );
	}
	
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
	
	//TheValue = m_pRecordfee->GetCollect( "xtalktype" );
	m_pRecordset->PutCollect( "xtalktype", _variant_t(xTalkType) );
	
	if ( xThirdId ) m_pRecordset->PutCollect( "xthirdid", _variant_t(xThirdId) );
	
	if ( bUpdate ) m_pRecordset->Update();
	//m_pRecordset->Resync( adAffectAll, adResyncAllValues );
}

void CNeighbour::WriteExpense(DWORD nLength, double nExpense, BOOL bUpdate)
{
	double nExpense2 = nExpense;
	_variant_t TheValue = m_pRecordset->GetCollect( "xExpense" );
	if ( VT_NULL != TheValue.vt ) nExpense2 = nExpense - (double)TheValue;
	
	m_pRecordset->PutCollect( "xTimeLength", _variant_t((long)nLength) );
	m_pRecordset->PutCollect( "xExpense", _variant_t(nExpense) );
	
	if ( bUpdate ) m_pRecordset->Update();
	
	WriteUserInfo( nExpense2 );
}

double CNeighbour::CalcUserBlance()
{
	double nBalance = 0;
	_variant_t TheValue;
	
	// Current Balance:= xpreBala + xBalance
//	m_pRecordusr->Resync( adAffectAll, adResyncAllValues );

	TheValue = m_pRecordusr->GetCollect( "xpreBala" );
	if ( VT_NULL != TheValue.vt ) nBalance += (double)TheValue;
	TheValue = m_pRecordusr->GetCollect( "xBalance" );
	if ( VT_NULL != TheValue.vt ) nBalance += (double)TheValue;

	return nBalance;
}

void CNeighbour::WriteUserInfo(double nExpense)
{
	_variant_t TheValue;
	
	m_pRecordusr->Resync( adAffectAll, adResyncAllValues );
	
	// 先扣优惠余额
	
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
	
	ChockRedPay( xBala );
}

double CNeighbour::CalcExpense(DWORD nLength, int nFeeRule)
{
	int nFeePoint = nLength / nFeeRule + 1;
	
	_variant_t TheValue = m_pRecordfee->GetCollect( "xFeeValue" );
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

BOOL CNeighbour::CalcExpense(DWORD nLength, double& nExpense)
{
	_variant_t TheValue = m_pRecordfee->GetCollect( "xFeeRule" );
	int nFeeRule = (TheValue.vt != VT_NULL) ? TheValue.lVal : 0x7FFFFF;
	
	nExpense = CalcExpense( nLength, nFeeRule );
	return TRUE;
}

int CNeighbour::CalcExpense(DWORD nLength, DWORD tExpense, double& nExpense)
{
	_variant_t TheValue = m_pRecordfee->GetCollect( "xFeeRule" );
	int nFeeRule = (TheValue.vt != VT_NULL) ? TheValue.lVal : 0x7FFFFF;
	
	DWORD tNow = GetTickCount();
//theApp.Message(MSG_ERROR, "(%lu - %lu) / 1000 < %i ==> %i", tNow, tExpense, nFeeRule, (tNow - tExpense) / 1000 < (DWORD)nFeeRule);
	if ( (tNow - tExpense) / 1000 < (DWORD)nFeeRule ) return -1;
	
	nExpense = CalcExpense( nLength, nFeeRule );
	return nFeeRule;
}

double CNeighbour::InfFixedFee(double nExpense)
{
	_variant_t TheValue;

	TheValue = m_pRecordfee->GetCollect( "xFixedFee" );
	if ( TheValue.vt == VT_NULL || (double)TheValue == 0 ) return nExpense;

	if ( (double)TheValue < nExpense )
	{
#if 0	// 96031666本地手机封顶业务
		if ( m_pRecordfee->GetCollect( "xjsjbh" ).lVal == 1666 )
		{
			_variant_t xThirdId = m_pRecordset->GetCollect( "xthirdid" );
			CString sThirdId = xThirdId.vt != VT_NULL ? (LPCTSTR)(_bstr_t)xThirdId : "";
			
			if ( _tcsncmp(sThirdId, "13", 2) && _tcsncmp(sThirdId, "15", 2) ) return nExpense;
		}
#endif
		return double(TheValue);
	}

	return nExpense;
}

BOOL CNeighbour::CheckGivenTry()
{
	if ( CalcUserBlance() > 0 ) return TRUE;
	
	if ( ! Settings.GivenTry.Enable ) return FALSE;
	
	_variant_t TheValue = m_pRecordfee->GetCollect( "xtalktype" );
	if ( _tcscmp((LPCTSTR)(_bstr_t)TheValue, "CATK") ) return FALSE;
	
	// 绿色通道服务
	
	//if ( (INT)nPayLength > Settings.GivenTry.MaxLength ) return nExpense;
	
	TheValue = m_pRecordfee->GetCollect( "xphone" );
	
	CString strValue;
	
	strValue = Settings.GivenTry.LocalId;
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		int nPos = str.ReverseFind( '*' );
		if ( nPos != -1 && ! _tcsncmp( (LPCTSTR)(_bstr_t)TheValue, str, nPos ) ) return TRUE;
		if ( ! str.CompareNoCase( (LPCTSTR)(_bstr_t)TheValue ) ) return TRUE;
	}
	
	return FALSE;
}

BOOL CNeighbour::ChockRedPay(double xBala)
{
	if ( ! Settings.Payer.RedEnable ) return FALSE;
	if ( Settings.Payer.RedBalan < xBala ) return FALSE;
	
	_variant_t TheValue = m_pRecordfee->GetCollect( "xtalktype" );
	if ( _tcscmp((LPCTSTR)(_bstr_t)TheValue, "CATK") ) return FALSE;
	
	TheValue = m_pRecordfee->GetCollect( "xFeeRule" );
	int nFeeRule = (TheValue.vt != VT_NULL) ? TheValue.lVal : 0x7FFFFF;
	if ( nFeeRule != 60 ) return FALSE;
	
	TheValue = m_pRecordfee->GetCollect( "xFeeValue" );
	int nFeePoint = int(xBala / (double)TheValue + 0.5);
	
	SendPacket( new IRedPacket( nFeePoint ) );
	return TRUE;
}

// 外呼: 区分本地/长途

BOOL CNeighbour::ChockTalkRemote(LPCTSTR xPhone, LPCTSTR xCallType, LPCTSTR xFeeCode)
{
	if ( _tcsicmp( xCallType, "CAHK" ) ) return FALSE;
//	if ( _tcsicmp( xFeeCode, "2666" ) && _tcsicmp( xFeeCode, "2222") ) return FALSE;
	
	if ( *xPhone != '0' ) return FALSE;
	_variant_t pBookmark = m_pRecordfee->Bookmark;
	
	CString strQuery;
	strQuery.Format( "xfeetype = '%c'", '0' );
	
	m_pRecordfee->Find( (_bstr_t)strQuery, 1, adSearchForward, m_pRecordfee->Bookmark );
	if ( ! m_pRecordfee->adoEOF ) return TRUE;
	
	m_pRecordfee->Bookmark = pBookmark;
	return FALSE;
}
