//
// Channel.cpp
//

#include "stdafx.h"
#include "Xmaster.h"
#include "Channel.h"
#include "Settings.h"
#include "Profiles.h"

#include "WndMain.h"
#include "WndNetwork.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CChannel construction

CChannel::CChannel(SOCKET hSocket, SOCKADDR_IN* pHost) : CTransfer( hSocket, pHost )
{
	m_pOwner		= NULL;
	m_bNavigated	= FALSE;
	m_tNavigated	= 0;
}

CChannel::CChannel(CChannel* pParent)
{
	m_pRecordset = pParent->m_pRecordset->Clone( adLockOptimistic );
}

CChannel::~CChannel()
{
}

//////////////////////////////////////////////////////////////////////
// CChannel attributes

void CChannel::Setup()
{
}

void CChannel::SetOwner(CNetworkChild* pOwner)
{
	m_pOwner		= pOwner;
	ASSERT( pOwner->GetSafeHwnd() );
}

BOOL CChannel::IsNavigated() const
{
	return IsConnected() && m_bNavigated;
}

HRESULT CChannel::GetSeqNumber(DWORD& xSeqNumber) const
{
	if ( m_pRecordset == NULL || m_pRecordset->adoEOF ) return E_FAIL;
	
	_variant_t TheValue = m_pRecordset->GetCollect( "xseqnumber" );
	if ( VT_NULL == TheValue.vt ) TheValue = m_pRecordset->GetCollect( "jsjbh" );
	
	xSeqNumber = TheValue.lVal;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// CChannel disconnect

void CChannel::Disconnect()
{
	if ( IsNavigated() ) CompleteTask();
	
	if ( m_pOwner ) m_pOwner->RemoveItem( this );
	
	CTransfer::Disconnect(); delete this;
}

//////////////////////////////////////////////////////////////////////
// CChannel packet unserialize

CPacket* CChannel::ReadBuffer(CBuffer* pBuffer)
{
	CString strLine;
	ASSERT( pBuffer != NULL );
	
	if ( ! pBuffer->ReadLine( strLine, TRUE ) ) return NULL;
	
	CPacket* pPacket = CPacketCore::ReadBuffer( pBuffer->m_pBuffer, pBuffer->m_nLength );
	int nLength = strLine.GetLength();
	
	if ( pPacket == NULL )
	{
		pBuffer->Remove( nLength + 1 + (pBuffer->m_pBuffer[nLength] == '\r') ); // + [\r] \n"
		SendPacket( new CErrPacket(EError_Unknown) ); return NULL;
	}
	else
	if ( pPacket->CommandId != NULL )
	{
		nLength = pPacket->GetBufferSize();
		pBuffer->Remove( nLength + 1 + (pBuffer->m_pBuffer[nLength] == '\r') ); // + [\r] \n"
		return pPacket;
	}
	
	return pPacket;
}

//////////////////////////////////////////////////////////////////////
// CChannel run function

BOOL CChannel::OnRun()
{
	if ( ! CTransfer::OnRun() ) return FALSE;
	
	DWORD tNow = GetTickCount();
	
	if ( ! IsNavigated() && tNow - m_tConnected >= Settings.General.MaxReadyOut* 1000 )
	{
		OnDropped( WSA_WAIT_TIMEOUT ); return FALSE;
	}
	
	if ( IsNavigated() && ! OnNavigated() )
	{
		OnDropped( ECore_NoPay ); return FALSE;
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CChannel socket event handlers

BOOL CChannel::OnHandshake()
{
	m_bNavigated	= TRUE;
	m_tNavigated	= 0;
	
	return CTransfer::OnConnected();
}

//////////////////////////////////////////////////////////////////////
// CChannel TCP Transfer func

BOOL CChannel::OnPacket(CPacket* pPacket)
{
	if ( pPacket->IsCommandId( ICore_BindLink ) )
	{
		return OnBindPacket( (CBindPacket*)pPacket );
	}
	if ( pPacket->IsCommandId( ICore_TalkBegin ) )
	{
		return OnTalkPacket( (CTalkPacket*)pPacket );
	}
	
	return CTransfer::OnPacket(pPacket);
}

BOOL CChannel::OnBindPacket(CBindPacket* pPacket)
{
	CString xLocalId, xRemoteId, xFeeNumber, xCallType;
	
	xLocalId.ReleaseBuffer( pPacket->GetLocalId( xLocalId.GetBuffer( 21 ) ) );
	xRemoteId.ReleaseBuffer( pPacket->GetRemoteId( xRemoteId.GetBuffer( 21 ) ) );
	
	xFeeNumber.ReleaseBuffer( pPacket->GetFeeNumber( xFeeNumber.GetBuffer( 21 ) ) );
	xCallType.ReleaseBuffer( pPacket->GetCallType( xCallType.GetBuffer( 4 ) ) );
	
	if ( xCallType == "CATK" && ! xFeeNumber.IsEmpty() ) ChkUserState(xRemoteId);
	if ( xFeeNumber.IsEmpty() ) xFeeNumber = (xCallType == "CAHK") ? xLocalId : xRemoteId;
	
	if ( ! ReadFeeEntry(xLocalId, xCallType) || ! ChkUserState(xFeeNumber) )
	{
#if 1
		if ( IsNavigated() ) return TRUE;
		
		if ( xLocalId.ReverseFind( '#' ) == -1 )
		{
			xLocalId += '#'; pPacket->SetLocalId( xLocalId ); pPacket->SetFeeNumber(xFeeNumber);
			return OnBindPacket( pPacket );
		}
#endif
		return OnBindPacketResp( ECore_NoUser );
	}
	
#if 1	// 检查包月，号码类型
	if ( ! ChockFeeMonth( xRemoteId, 0 ) ) ChockFeeRemote( xRemoteId, 0 );
#endif
	
	if ( int nResult = ReadUsrState( (LPCTSTR)xCallType ) )
	{
		return OnBindPacketResp( nResult );
	}
	
	AddExpense( (xCallType == "CAHK") ? (LPCTSTR)xRemoteId : NULL, TRUE );
	if ( AttachTo( xLocalId, xRemoteId, (LPCTSTR)xFeeNumber ) ) return OnBindPacketResp( ECore_Rejected );
	
	_variant_t xFeeCode = m_pRecordfee->GetCollect( "xfeetableid" );
	return OnBindPacketResp( (LPCTSTR)(_bstr_t)xFeeCode );
}

BOOL CChannel::OnBindPacketResp(int nErrCode)
{
	SendFailPacket( nErrCode );
	//OnDropped( nErrCode );
	return FALSE;
}

BOOL CChannel::OnBindPacketResp(LPCTSTR xFeeCode)
{
	CBindPacket* pPacketResp = new CBindPacket;
	pPacketResp->SetFeeCode( (LPCTSTR)xFeeCode );
	
	SendPacket( pPacketResp );
	
	if ( Settings.Record.Enable ) SendPacket( new CRecordPacket(0) );
	
	return OnConnected();
}

BOOL CChannel::OnTalkPacket(CTalkPacket* pPacket)
{
	if ( IsNavigated() ) return TRUE;
	
	m_pOwner->PrintChState( this, Ch_Talking );
	
	return OnHandshake();
}

BOOL CChannel::OnNavigated()
{
	DWORD tNow = GetTickCount();
	DWORD nLength = ( tNow - m_tConnected + 1000 ) / 1000;
	
	if ( m_pOwner ) m_pOwner->PrintLength( this, nLength );
	if ( nLength < Settings.Network.ShortFree ) return TRUE;
#if 1
	if ( nLength >= Settings.Network.MaxLength )
	{
		SendFailPacket( ECore_Busy ); return FALSE;
	}
#endif
	
	double nExpense = 0;
	int nFeeRule = CalcExpense( nLength + 1, m_tNavigated, nExpense );
	
	if ( nFeeRule <= 0 ) return TRUE;
	if ( ! m_tNavigated ) m_tNavigated = m_tConnected - 1000;
	else m_tNavigated += nFeeRule * 1000;
	
#if 0
	if ( nExpense > 0 && CalcUserBlance() <= 0 )
	{
		SendFailPacket( ECore_NoPay ); return FALSE;
	}
#endif
	
	if ( ! WriteExpense( nLength, nExpense, FALSE ) )
	{
		SendFailPacket( ECore_NoPay ); return FALSE;
	}
	
	if ( m_pOwner ) m_pOwner->PrintExpense( this, nExpense );

#if 0
	if ( ! CheckGivenTry() )
	{
		SendFailPacket( ECore_NoPay ); return FALSE;
	}
#endif
	
	return TRUE;
}

BOOL CChannel::CompleteTask()
{
	DWORD tNow = GetTickCount();
	DWORD nLength = ( tNow - m_tConnected + 1000 ) / 1000;
	
	double nExpense = 0;
	if ( ! CalcExpense( nLength, nExpense ) ) return FALSE;
	
	if ( nLength < Settings.Network.ShortFree ) nExpense = 0;
	BOOL bResult = WriteExpense( nLength, nExpense );
	
	m_bNavigated	= FALSE;
	m_tNavigated	= 0;
	
	return bResult;
}

int CChannel::AttachTo(LPCTSTR xLocalId, LPCTSTR xUsrNumber, LPCTSTR xFeeNumber)
{
	if ( Profiles.IsDenied( xUsrNumber ) ) return ECore_Rejected;
	
	CMainWnd* pMainWnd	= (CMainWnd*)theApp.m_pMainWnd;
	ASSERT( pMainWnd->GetSafeHwnd() != NULL );
	
	CNetworkWnd* pNetworkWnd = (CNetworkWnd*)pMainWnd->GetWindow( RUNTIME_CLASS(CNetworkWnd) );
	ASSERT( pNetworkWnd && pNetworkWnd->GetSafeHwnd() );
	
	int nItem = pNetworkWnd->Attach( this );
	ASSERT( m_pOwner && m_pOwner->GetSafeHwnd() );
	
	m_pOwner->PrintOwner( nItem, xLocalId, xUsrNumber,
		xFeeNumber && _tcscmp(xUsrNumber, xFeeNumber) ? xFeeNumber : NULL );
	
	_variant_t xFeeCode = m_pRecordfee->GetCollect( "xfeetableid" );
	DWORD nReason = 0; _stscanf( (_bstr_t)xFeeCode, "%x", &nReason );
	m_pOwner->PrintChState( this, Ch_Ready, nReason );
	
	DWORD xSeqNumber;
	if ( SUCCEEDED(GetSeqNumber( xSeqNumber )) ) m_pOwner->PrintSeqNumber( nItem, xSeqNumber );
	
	return ECore_Success;
}

//////////////////////////////////////////////////////////////////////
// CChannel read feetable

BOOL CChannel::ReadFeeEntry(LPCTSTR xPhonNum, LPCTSTR xTalkType)
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
	
	if ( Recordfee->adoEOF ) return FALSE;
	
	if ( IsNavigated() ) CompleteTask();
	
	m_pRecordfee = Recordfee->Clone( adLockUnspecified );
	
	return ( m_pRecordfee != NULL && ! m_pRecordfee->adoEOF );
}

int CChannel::ReadUsrState(LPCTSTR xTalkType)
{
	_variant_t xCallBack = m_pRecordfee->GetCollect( "xcallback" );
	
	if ( xCallBack.vt != VT_NULL && ! _tcscmp((LPCTSTR)(_bstr_t)xCallBack, "1") )
	{
		if ( ! _tcscmp(xTalkType, "CATK") && ReadCallBack() ) return ECore_CallBack;
	}
	
#if 1	// 检查通话余额
	double nExpense = 0;
	double nBalance	= 0;
	
	CalcExpense( Settings.Network.ShortFree + 1, nExpense );
	if ( nExpense > 0 && nExpense > CalcUserBlance() ) return ECore_NoPay;
#endif
	
	return ECore_Success;
}

#if 0
BOOL CChannel::ReadCallBack(double* pnExpense)
{
#if 1	// 二次流水内不允许回呼
	if ( m_pRecordset != NULL && ! m_pRecordset->adoEOF ) return FALSE;
#endif
	
	CString	strQuery;
	
	strQuery.Format( "select * from xfeetable"
					" where xfeetableid = '%i'", ECore_CallBack );
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr Recordfee = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	if ( Recordfee->adoEOF ) return FALSE;
	
	_variant_t xFeeType	= Recordfee->GetCollect( "xfeetype" );
	if ( xFeeType.vt != VT_NULL && _tcslen( (LPCTSTR)(_bstr_t)xFeeType ) &&
		! ReadUsrType("8888", (LPCTSTR)(_bstr_t)xFeeType, pnExpense) ) return FALSE;
	
	return TRUE;
}
#else
BOOL CChannel::ReadCallBack(double* pnExpense)
{
#if 1	// 二次流水内不允许回呼
	if ( m_pRecordset != NULL && ! m_pRecordset->adoEOF ) return FALSE;
#endif
	
	return ReadUsrType( "8888" );
}
#endif
BOOL CChannel::ReadUsrType(LPCTSTR xFeeCode)
{
	_variant_t xUserId = m_pRecordusr->GetCollect( "xjsjbh" );
	
	CString	strQuery;
	
	strQuery.Format( "select * from xusefee"
					" where xfeecode = '%s' and xjsjbh = '%s'",
					xFeeCode, (LPCTSTR)(_bstr_t)xUserId );
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr Recordfee = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	
	return ( Recordfee->adoEOF == FALSE );
}

BOOL CChannel::ReadFeeType(LPCTSTR xFeeType, int* pnElapse, double* pnExpense) const
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

//////////////////////////////////////////////////////////////////////
// CChannel read user

BOOL CChannel::ChkUserState(LPCTSTR xUsrNumber)
{
	CString	strQuery;
	
	strQuery.Format( "select a.*, b.xuserbindid"
					" from xuser_info a inner join"
					" xuser b on a.xjsjbh = b.xjsjbh"
					" where b.xphone = '%s'", xUsrNumber );
	
	TRACE( strQuery + "\n" );
	
	xOpenRecordset(m_pRecordusr, theApp.m_pConnection, (_bstr_t)strQuery);
	
	if ( m_pRecordusr->adoEOF && Settings.FreeTry.Enable )
	{
		strQuery.Format( "EXECUTE addnewuser '%s'", xUsrNumber );
		TRACE( strQuery + "\n" );
		xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
		m_pRecordusr->Requery( adCmdUnknown );
	}
	
	ASSERT ( m_pRecordusr->RecordCount <= 1 );
	return ( m_pRecordusr->adoEOF == FALSE );
}

void CChannel::AddExpense(LPCTSTR xThirdId, BOOL bUpdate)
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
	
	TheValue = m_pRecordfee->GetCollect( "xtalktype" );
	m_pRecordset->PutCollect( "xtalktype", TheValue );
	
	if ( xThirdId ) m_pRecordset->PutCollect( "xthirdid", _variant_t(xThirdId) );
	
	if ( bUpdate && SUCCEEDED(m_pRecordset->UpdateBatch(adAffectCurrent)) )
	{
		m_pRecordset->Resync( adAffectCurrent, adResyncAllValues );
	}
}

//////////////////////////////////////////////////////////////////////
// CChannel write expenses

BOOL CChannel::WriteExpense(DWORD nLength, double nExpense, BOOL bUpdate)
{
	if ( m_pRecordset == NULL || m_pRecordset->adoEOF ) return FALSE;
	
	double nExpense2 = nExpense;
	_variant_t TheValue = m_pRecordset->GetCollect( "xExpense" );
	if ( VT_NULL != TheValue.vt ) nExpense2 = nExpense - (double)TheValue;
	
	if ( ! bUpdate && nExpense2 > CalcUserBlance() ) return FALSE;
	
	m_pRecordset->PutCollect( "xTimeLength", _variant_t((long)nLength) );
	m_pRecordset->PutCollect( "xExpense", _variant_t(nExpense) );
	
	if ( bUpdate ) m_pRecordset->UpdateBatch(adAffectAll);
	
	if ( nExpense2 ) WriteUserInfo( nExpense2 );
	return TRUE;
}

double CChannel::CalcUserBlance()
{
	double nBalance = 0;
	_variant_t TheValue;
	
	// Current Balance:= xpreBala + xBalance
	m_pRecordusr->Resync( adAffectAll, adResyncAllValues );

	TheValue = m_pRecordusr->GetCollect( "xpreBala" );
	if ( VT_NULL != TheValue.vt ) nBalance += (double)TheValue;
	TheValue = m_pRecordusr->GetCollect( "xBalance" );
	if ( VT_NULL != TheValue.vt ) nBalance += (double)TheValue;

	return nBalance;
}

void CChannel::WriteUserInfo(double nExpense)
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
			m_pRecordusr->PutCollect( "xpreBala", _variant_t((double)nExpense + 0.005) );
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
		m_pRecordusr->PutCollect( "xBalance", _variant_t((double)TheValue - nExpense + 0.005) );
	}
	
	// 更新用户状态
	
	TheValue = m_pRecordusr->GetCollect( "xBalance" );
	double xBala = (TheValue.vt == VT_NULL) ? 0 : (double)TheValue;
	TheValue = m_pRecordusr->GetCollect( "xpreBala" );
	if ( VT_NULL != TheValue.vt ) xBala += (double)TheValue;
#if 0	
	if ( xBala <= 0 ) m_pRecordusr->PutCollect( "xStatus", _variant_t("0") );
#endif

	m_pRecordusr->UpdateBatch(adAffectCurrent);
	
	ChockRedPay( xBala );
}

double CChannel::CalcExpense(DWORD nLength, int nFeeRule)
{
	int nFeePoint = nLength / nFeeRule + ( (nLength % nFeeRule) > 0 );
	
	_variant_t TheValue = m_pRecordfee->GetCollect( "xFeeValue" );
	double nExpense = nFeePoint * (double)TheValue;
	
#if 1
	// 检查封顶费用
	nExpense = InfFixedFee( nExpense );
#endif
	
#if 0
	// 同步扣费
	nExpense += m_nSyncMoney;
	nExpense *= m_nSyncRepeat;
#endif
	
	return nExpense;
}

BOOL CChannel::CalcExpense(DWORD nLength, double& nExpense)
{
	if ( m_pRecordfee == NULL || m_pRecordfee->adoEOF ) return FALSE;
	
	_variant_t TheValue = m_pRecordfee->GetCollect( "xFeeRule" );
	int nFeeRule = (TheValue.vt != VT_NULL) ? TheValue.lVal : 0x7FFFFF;
	
	nExpense = CalcExpense( nLength, nFeeRule );
	return TRUE;
}

int CChannel::CalcExpense(DWORD nLength, DWORD tExpense, double& nExpense)
{
	_variant_t TheValue = m_pRecordfee->GetCollect( "xFeeRule" );
	int nFeeRule = (TheValue.vt != VT_NULL) ? TheValue.lVal : 0x7FFFFF;
	
	if ( GetTickCount() < tExpense + nFeeRule * 1000 ) return 0;
	
	nExpense = CalcExpense( nLength, nFeeRule );
	return nFeeRule;
}

double CChannel::InfFixedFee(double nExpense)
{
	_variant_t TheValue;

	TheValue = m_pRecordfee->GetCollect( "xFixedFee" );
	if ( TheValue.vt == VT_NULL ) return nExpense;

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

BOOL CChannel::CheckGivenTry()
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

BOOL CChannel::ChockRedPay(double xBala)
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
	
	SendPacket( new CRedPacket( "MIN", nFeePoint ) );
	return TRUE;
}

// 检查包月

BOOL CChannel::ChockFeeMonth(LPCTSTR xPhone, int nSkip)
{
#ifdef _DEBUG
	{
		_variant_t xjsjbh	= m_pRecordfee->GetCollect( "xjsjbh" );
		afxDump << "ChockFeeMonth-> xjsjbh:=" << xjsjbh.lVal << "\n";
	}
#endif
	_variant_t pBookmark = m_pRecordfee->Bookmark;
	
	CString strQuery;
	strQuery.Format( "xfeetype <> null" );
#ifdef _DEBUG
	afxDump << (LPCTSTR)strQuery << "\n";
#endif
	
	m_pRecordfee->Find( (_bstr_t)strQuery, nSkip, adSearchForward, m_pRecordfee->Bookmark );
	
	if ( m_pRecordfee->adoEOF )
	{
		m_pRecordfee->Bookmark = pBookmark;
#ifdef _DEBUG
		{
			_variant_t xjsjbh	= m_pRecordfee->GetCollect( "xjsjbh" );
			afxDump << "             <- xjsjbh:=" << xjsjbh.lVal << "\n";
		}
#endif
		return FALSE;
	}
	
#ifdef _DEBUG
	{
		_variant_t xjsjbh	= m_pRecordfee->GetCollect( "xjsjbh" );
		afxDump << "             -> xjsjbh:=" << xjsjbh.lVal << "\n";
	}
#endif
	
	_variant_t xFeeType	= m_pRecordfee->GetCollect( "xfeetype" );
	if ( ReadUsrType( (LPCTSTR)(_bstr_t)xFeeType ) && ChockFeeRemote( xPhone, 0 ) ) return TRUE;
	
	if ( ChockFeeMonth( xPhone, 1 ) ) return TRUE;
	m_pRecordfee->Bookmark = pBookmark;
#ifdef _DEBUG
	{
		_variant_t xjsjbh	= m_pRecordfee->GetCollect( "xjsjbh" );
		afxDump << "             <- xjsjbh:=" << xjsjbh.lVal << "\n";
	}
#endif
	return FALSE;
}

// 检查号码类型：区分网内/本地/长途

BOOL CChannel::ChockFeeRemote(LPCTSTR xPhone, int nSkip)
{
#ifdef _DEBUG
	{
		_variant_t xjsjbh	= m_pRecordfee->GetCollect( "xjsjbh" );
		afxDump << "ChockFeeRemote-> xjsjbh:=" << xjsjbh.lVal << "\n";
	}
#endif
	
	_variant_t pBookmark = m_pRecordfee->Bookmark;
	
	CString strQuery;
	strQuery.Format( "xnet like '%%%c,%%'", *xPhone );
#ifdef _DEBUG
	afxDump << (LPCTSTR)strQuery << "\n";
#endif
	
	m_pRecordfee->Find( (_bstr_t)strQuery, nSkip, adSearchForward, m_pRecordfee->Bookmark );
	
	if ( m_pRecordfee->adoEOF )
	{
		m_pRecordfee->Bookmark = pBookmark;
#ifdef _DEBUG
		{
			_variant_t xjsjbh	= m_pRecordfee->GetCollect( "xjsjbh" );
			afxDump << "             <- xjsjbh:=" << xjsjbh.lVal << "\n";
		}
#endif
		return FALSE;
	}
#ifdef _DEBUG
	{
		_variant_t xjsjbh	= m_pRecordfee->GetCollect( "xjsjbh" );
		afxDump << "             -> xjsjbh:=" << xjsjbh.lVal << "\n";
	}
#endif
	
	return TRUE;
}
