//
// ChannelXeno.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Network.h"
#include "Settings.h"
#include "ChannelXeno.h"
#include "Traffic.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CXenoChannel construction

CXenoChannel::CXenoChannel(int nChan, LPCTSTR xFeeCode) : CChannel( nChan, xFeeCode )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	FeeLogin.initiate		= (INITIATE)FeeLogin_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	rcvMFsig.initiate		= (INITIATE)rcvMFsig_init;
	RcvTalk.initiate		= (INITIATE)RcvTalk_init;

	FwdPlay.initiate		= (INITIATE)FwdPlay_init;
	FwdPlay.process			= (PROCESS)FwdPlay_cmplt;
	RcvFnOper.initiate		= (INITIATE)RcvFnOper_init;
	RcvFnOper.process		= (PROCESS)RcvFnOper_cmplt;
	
	RcvXnTele.initiate		= (INITIATE)RcvXnTele_init;
	RcvXnTele.process		= (PROCESS)RcvXnTele_cmplt;
	RcvXnCard.initiate		= (INITIATE)RcvXnCard_init;
	RcvXnCard.process		= (PROCESS)RcvXnCard_cmplt;
	RcvXnWord.initiate		= (INITIATE)RcvXnWord_init;
	RcvXnWord.process		= (PROCESS)RcvXnWord_cmplt;

	ReputCard.initiate		= (INITIATE)ReputCard_init;
	ReputCard.process		= (PROCESS)ReputCard_cmplt;
	PlayXnOk.initiate		= (INITIATE)PlayXnOk_init;
	PlayXnOk.process		= (PROCESS)PlayXnOk_cmplt;
	ReadBalan.initiate		= (INITIATE)ReadBalan_init;
	ReadBalan.process		= (PROCESS)ReadBalan_cmplt;
	
	RcvXnMony.initiate		= (INITIATE)RcvXnMony_init;
	RcvXnMony.process		= (PROCESS)RcvXnMony_cmplt;
	
	VfXnTele.initiate		= (INITIATE)VfXnTele_init;
	VfXnTele.process		= (PROCESS)VfXnTele_cmplt;
	
	RcvUsrList.initiate		= (INITIATE)RcvUsrList_init;
	RcvUsrList.process		= (PROCESS)RcvUsrList_cmplt;
	
	RcvUsrPass.initiate		= (INITIATE)RcvUsrPass_init;
	RcvUsrPass.process		= (PROCESS)RcvUsrPass_cmplt;
	
	XnTeleNext				= RcvXnCard;
	CXenoChannel::Setup();
}

CXenoChannel::~CXenoChannel()
{
}

//////////////////////////////////////////////////////////////////////
// CXenoChannel attributes

void CXenoChannel::Setup()
{
	CChannel::Setup();
	fcnp = &FeeLogin;

	ZeroMemory( XenoNumber, MAX_BUFFER + 1 );
	ZeroMemory( XenoCard, MAX_CARD_LEN + 1 );
	ZeroMemory( XenoWord, MAX_PASSWORD + 1 );
	
	XenoError = XENO_SUCCESS;
}

struct state* CXenoChannel::FwdCaller(LPCTSTR xLocalId, struct state* lpszDefaultState)
{
	CopyMemory( LocalId, xLocalId, MAX_PHONE );
	CopyMemory( CallType, CA_TALK, MAX_CALLTYPE );
	
	CChannel::OnConnected();
	return lpszDefaultState;
}

int CXenoChannel::ShPlyXnBala(int nChan, double nBalance, LPCTSTR xNavFile, DWORD nOffset1, DWORD nOffset2)
{
	SsmSetDtmfStopPlay( nChan, FALSE );
	SsmClearRxDtmfBuf( nChan );
	SsmClearFileList( nChan );
	
	SsmAddToFileList( nChan, (LPTSTR)xNavFile, 6, 0, nOffset1 );
	
	CString strValue;
	strValue.Format( "%.2f", nBalance );
	CompDigital( (LPCTSTR)strValue, TRUE );
	
	SsmAddToFileList( nChan, (LPTSTR)xNavFile, 6, nOffset2, 0xFFFFFFFF );
	
	if ( SsmPlayFileList( nChan ) ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	return 0;
}

int CXenoChannel::ShPlyXnTele(int nChan, LPCTSTR xNavFile)
{
	TCHAR szT[80];
	CSettings::Item* pItem = Settings.GetSetting( _T(".Path") );
	
	::GetPrivateProfileString( "Xeno", "VfXnTele",
		"13784-38816", szT, 80, *pItem->m_pString + "\\MediaCfg.ini" );
	
	SsmSetDtmfStopPlay( nChan, TRUE );
	SsmClearRxDtmfBuf( nChan );
	SsmClearFileList( nChan );
	
	DWORD nOffset1 = 0, nOffset2 = 0;
	_stscanf( szT, "%i-%i", &nOffset1, &nOffset2 );
	
	SsmAddToFileList( nChan, (LPTSTR)xNavFile, 6, 0, nOffset1 );
	
	CompDigital( XenoNumber, FALSE );
	
	SsmAddToFileList( nChan, (LPTSTR)xNavFile, 6, nOffset2, 0xFFFFFFFF );
	
	if ( SsmPlayFileList( nChan ) ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	return 0;
}

int CXenoChannel::ShPlyXnMoney(int nChan, double nBalance)
{
	CSettings::Item* pItem = Settings.GetSetting( _T(".Path") );
	
	CString xNavFile;
	xNavFile.Format( _T("%s\\%s\\%s"), *pItem->m_pString, FT_PROMPT, FILE_XENO_RCVMONY );
	
	TCHAR szT[80];
	::GetPrivateProfileString( "Xeno", "WtXnMoney",
		"18616-22824", szT, 80, *pItem->m_pString + "\\MediaCfg.ini" );
	
	SsmSetDtmfStopPlay( nChan, TRUE );
	SsmClearRxDtmfBuf( nChan );
	SsmClearFileList( nChan );
	
	DWORD nOffset1 = 0, nOffset2 = 0;
	_stscanf( szT, "%i-%i", &nOffset1, &nOffset2 );
	
	SsmAddToFileList( nChan, xNavFile.GetBuffer(0), 6, 0, nOffset1 );
	
	CString strValue;
	strValue.Format( "%.2f", nBalance );
	CompDigital( (LPCTSTR)strValue, TRUE );
	
	SsmAddToFileList( nChan, xNavFile.GetBuffer(0), 6, nOffset2, 0xFFFFFFFF );
	
	if ( SsmPlayFileList( nChan ) ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	return 0;
}

int CXenoChannel::ShExitPlay_init(EVTBLK *evtblk, int nExitCode)
{
	ExitCode = nExitCode; fcnp = &ExitPlay;
	Reserved = 0; return (this->*fcnp->initiate)(evtblk);
}

//////////////////////////////////////////////////////////////////////
// CXenoChannel operations

CChannel* CXenoChannel::ConvertFrom(CChannel* pParent, LPCTSTR xString)
{
	CopyMemory(pParent->LocalId, "96031234", MAX_PHONE);
	CopyMemory(pParent->CallType, CA_TALK, MAX_CALLTYPE );
	
	pParent->OnConnected(); return NULL;
}

BOOL CXenoChannel::ChkUserState(LPCTSTR xUsrNumber, BOOL bAutoAdd)
{
	CString	strQuery;
	
	strQuery.Format( "select a.*,"
					" (select sum(xfixedfee) from xfeetable where xtalktype='FECE' and xfeetableid in"
					" (select xfeecode from xusefee c where c.xjsjbh= a.xjsjbh)) as xmonthfee"
					" from xuser_info a inner join xuser b on a.xjsjbh = b.xjsjbh"
					" where b.xphone = '%s'", xUsrNumber );
	TRACE( strQuery + "\n" );

	xOpenRecordset(m_pRecordusr, theApp.m_pConnection, (_bstr_t)strQuery);
	
	if ( m_pRecordusr->adoEOF && bAutoAdd )
	{
		strQuery.Format( "EXECUTE addnewuser '%s'", xUsrNumber );
		TRACE( strQuery + "\n" );
		xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
		m_pRecordusr->Requery( adCmdUnknown );
	}
	
	return TRUE;
}

double CXenoChannel::CalcUserBlance(BOOL bPeekPre)
{
	m_pRecordusr->Requery( adCmdUnknown );
	
	// Current Balance:= xpreBala + xBalance

	double nBalance = 0;
	_variant_t TheValue;
	
	if ( bPeekPre == FALSE )
	{
	TheValue = m_pRecordusr->GetCollect( "xpreBala" );
	if ( VT_NULL != TheValue.vt ) nBalance += (double)TheValue;
	
	TheValue = m_pRecordusr->GetCollect( "xmonthfee" );
	if ( VT_NULL != TheValue.vt ) nBalance += (double)TheValue;
	}
	
	TheValue = m_pRecordusr->GetCollect( "xBalance" );
	if ( VT_NULL != TheValue.vt ) nBalance += (double)TheValue;
	
	return nBalance;
}

struct state* CXenoChannel::XenoUserMoney(struct state* lpszDefaultState)
{
	CString	strQuery;
	
	strQuery.Format( "select * from Xcostcard"
					" where xcostcode = '%s' and xcostpwd = '%s'",
					XenoCard, XenoWord );
	TRACE( strQuery + "\n" );

	xOpenRecordset(m_pRecordcar, theApp.m_pConnection, (_bstr_t)strQuery);
	if ( m_pRecordcar->adoEOF )
	{
		theApp.Message(MSG_ERROR, "XenoUserMoney[%i]: {%s} {%s} Invalid", m_nChan, XenoCard, XenoWord);
		XenoError = XENO_INVALID; return &ReputCard;
	}
	
	_variant_t TheValue;
	
	TheValue = m_pRecordcar->GetCollect( "xcostuse" );
	if ( VT_NULL == TheValue.vt || _tcscmp( (_bstr_t)TheValue, "0   " ) )
	{
		XenoError = XENO_REJECT; return &ReputCard;
	}
	TheValue = m_pRecordcar->GetCollect( "xcostrun" );
	if ( VT_NULL == TheValue.vt || _tcscmp( (_bstr_t)TheValue, "1   " ) )
	{
		XenoError = XENO_REJECT; return &ReputCard;
	}
	
	TheValue = m_pRecordcar->GetCollect( "xcostdate" );
	if ( VT_NULL != TheValue.vt )
	{
		COleDateTime tt = COleDateTime( TheValue );
		COleDateTimeSpan tSpan = tt - COleDateTime::GetCurrentTime();
		
		if ( tSpan.GetDays() < 0 )
		{
			XenoError = XENO_EXPIRED; return &ReputCard;
		}
	}
	
#if 1
	COleDateTime tt = COleDateTime::GetCurrentTime();
	m_pRecordcar->PutCollect( "xcostuse", _variant_t("1") );
	m_pRecordcar->Update();
#endif

	TheValue = m_pRecordcar->GetCollect( "xcostmoney" );
	double xCostMoney = (double)TheValue;
	
	double xBalance = xCostMoney;
	TheValue = m_pRecordusr->GetCollect( "xBalance" );
	if ( VT_NULL != TheValue.vt ) xBalance += (double)TheValue;
	m_pRecordusr->PutCollect( "xBalance", _variant_t(xBalance) );
	m_pRecordusr->PutCollect( "xStatus", _variant_t("1") );
	m_pRecordusr->Update();
	
	TheValue = m_pRecordusr->GetCollect( "xjsjbh" );
	strQuery.Format( "insert into Xusercost(xjsjbh, xtimestart, xcostcode, Xcode, xpar_value)"
					" values('%s', getdate(), '%s', 99999999, '%i')",
					(LPCTSTR)(_bstr_t)TheValue, XenoCard, (INT)xCostMoney );
	TRACE( strQuery + "\n" );
	xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);

	theApp.Message(MSG_TEMP, "XenoUserMoney[%i]: {%s} XenoCard:=%s xCostMoney:=%i", m_nChan,
		(LPCTSTR)(_bstr_t)m_pRecordusr->GetCollect( "xphone" ), XenoCard,(INT)xCostMoney);
	return lpszDefaultState;
}

struct state* CXenoChannel::MoveUserMoney(double nBalance, LPCTSTR xUsrNumber,
										  struct state* lpszDefaultState)
{
	_variant_t TheValue;
	double xBalance = 0;
	
	// 减自身话费
	TheValue = m_pRecordusr->GetCollect( "xBalance" );
	if ( VT_NULL != TheValue.vt ) xBalance = (double)TheValue;
	m_pRecordusr->PutCollect( "xBalance", _variant_t(xBalance -= nBalance) );
	m_pRecordusr->Update();
	
	// 加对方话费
	
	CString	strQuery;
	
	strQuery.Format( "select * from xuser_info"
					" where xjsjbh ="
						" (select top 1 xjsjbh from xuser"
						" where xphone = '%s')", xUsrNumber );
	TRACE( strQuery + "\n" );

	_RecordsetPtr	xRecordusr;
	xOpenRecordset(xRecordusr, theApp.m_pConnection, (_bstr_t)strQuery);
	if ( xRecordusr->adoEOF ) return lpszDefaultState;
	
	TheValue = xRecordusr->GetCollect( "xBalance" );
	xRecordusr->PutCollect( "xBalance", _variant_t((double)TheValue + nBalance) );
	xRecordusr->PutCollect( "xStatus", _variant_t("1") );
	xRecordusr->Update();
	
	// 增加历史记录
	
	_RecordsetPtr	xRecordtrs;
	CTime pNow = CTime::GetCurrentTime();
	
	strQuery.Format( "select max(cast(right(xtransferid,5) as int))"
					" from xusertransfer where len(xtransferid)=17 and substring(xtransferid,5,8)='%.4i%.2i%.2i'",
					pNow.GetYear(), pNow.GetMonth(), pNow.GetDay() );
	
	xRecordtrs = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	TheValue = xRecordtrs->Fields->GetItem( _variant_t(long(0)) )->Value;
theApp.Message(MSG_TEMP, strQuery);
	
	strQuery.Format( "insert into xusertransfer(xtransferid, xoutphone, xinphone, xmoney, xdate, xmode, xcode)"
					" values('Tran%.4i%.2i%.2i%.5i', '%s', '%s', '%.2f', getdate(), '2222', '9999')",
					pNow.GetYear(), pNow.GetMonth(), pNow.GetDay(),
					( TheValue.vt == VT_NULL ) ? 1 : TheValue.lVal + 1,
					(LPCTSTR)(_bstr_t)m_pRecordusr->GetCollect( "xjsjbh" ),
					(LPCTSTR)(_bstr_t)xRecordusr->GetCollect( "xjsjbh" ),
					nBalance );
	theApp.Message(MSG_DEBUG, strQuery );
	xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);

	theApp.Message(MSG_TEMP, "MoveUserMoney[%i]: {%s} -> {%s} xCostMoney:=%.2f", m_nChan,
		(LPCTSTR)(_bstr_t)m_pRecordusr->GetCollect( "xphone" ),
		(LPCTSTR)(_bstr_t)xRecordusr->GetCollect( "xphone" ), nBalance);
	
	return lpszDefaultState;
}

struct state* CXenoChannel::XenoUserPass(LPCTSTR xNewPass, struct state* lpszDefaultState)
{
	ChkUserState( RemoteId, FALSE );
#if 0
	CHAR xPass[MAX_PASSWORD + 1];
	FillMemory( xPass, MAX_PASSWORD, '0' ); xPass[MAX_PASSWORD] = '\0';
	
	CopyMemory( xPass, xNewPass, _tcslen(xNewPass) );
#else
	if ( _tcslen(xNewPass) == 0 ) xNewPass = "666666";
#endif
	
	m_pRecordusr->PutCollect( "xuserpwd", _variant_t(xNewPass) );
	m_pRecordusr->Update();
	
	return lpszDefaultState;
}

//////////////////////////////////////////////////////////////////////
// CXenoChannel OVERLOAD function

int CXenoChannel::bkIdle_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	CChannel* pChannel = new CChannel( nChan );
	pChannel->Clone( this ); Network.SetChannel( pChannel );

	return (pChannel->*(pChannel->fcnp)->initiate)(evtblk);
}

int CXenoChannel::FeeLogin_init(EVTBLK *evtblk)
{
	return 0;
}

struct state* CXenoChannel::FeeLogin_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_RINGING )
		{
			KillTimer(); Reserved &= 0; return &rcvMFsig;
		}
	}
	if ( evtblk->wEventCode == E_FEE_Login &&
		(evtblk->dwParam & 0xFFFF) == S_FEE_Connected )
	{
		if ( SsmGetChState(nChan) == S_CALL_TALKING ||	// 其他业务转入
			 SsmGetChState(nChan) == S_CALL_RINGING ||	// 确保收到RINGING消息
			_tcsncmp(CallType, CA_DEBG, 4) == 0 )		// For Debug
		{
			SsmPutUserEvent( E_CHG_ChState, nChan, S_CALL_RINGING );
		}
		else
		if ( SsmGetChState(nChan) == S_CALL_PICKUPED )	// For CallOut
		{
			KillTimer(); Reserved &= 0; return &WaitDial;
		}
	}
	
	return CChannel::FeeLogin_cmplt(evtblk);
}

int CXenoChannel::rcvMFsig_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( SsmGetChState(nChan) == S_CALL_TALKING ||	// 其他业务转入
		_tcsncmp(CallType, CA_DEBG, 4) == 0 )		// For Debug
	{
		return SsmPutUserEvent( E_CHG_ChState, nChan, S_CALL_TALKING );
	}
	
	return CChannel::rcvMFsig_init(evtblk);
}

int CXenoChannel::RcvTalk_init(EVTBLK *evtblk)
{
	CChannel::RcvTalk_init(evtblk);
	
	fcnp = &FwdPlay; Reserved = 0;
	return (this->*fcnp->initiate)(evtblk);
}

/////////////////////////////////////////////////////////////////////////////
// CXenoChannel caller function

// 语音接入
int CXenoChannel::FwdPlay_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
#if 0// 内部清理，强制拆线
	if (! _tcscmp(RemoteId, "13867877813") )
	{
	HINSTANCE hInstance = LoadLibrary( "d:\\callcenter\\temp\\xNotifyChannel.dll" );
	theApp.Message(MSG_TEMP, "hhhhh->[0x%x]", hInstance);
	FreeLibrary( hInstance );
		for (int nn=32; nn<=91; nn++)
		{
			CChannel* pChannel = Network.FindChannel(nn);
			if (!pChannel) continue;
//			if ( SsmGetChState(nn)!=0) continue;
			if ( !pChannel->m_xFeeCode || !_tcslen(pChannel->m_xFeeCode) ) continue;
	if (pChannel->m_hInstance!=hInstance) continue;
			theApp.Message(MSG_TEMP, "debug[%i]-> [%i][%s]", m_nChan, nn, pChannel->m_xFeeCode);
	CChannel* pChannel22 = new CChannel( nn );
	pChannel22->Clone( pChannel ); Network.SetChannel( pChannel22 );

	EVTBLK aaa;
	aaa.dwParam=S_CALL_STANDBY;
	aaa.nReference=nn;
	aaa.wEventCode=E_CHG_ChState;
	(pChannel22->*(pChannel22->fcnp)->initiate)(&aaa);
	
		}
		
	}
#endif
	if ( PlayFile( FILE_XENO_WELCOME, FT_PROMPT, TRUE ) )
		return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	return 0;
}

struct state* CXenoChannel::FwdPlay_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved = 0; return &RcvFnOper;
	}
	
	return ChkDTSignal( evtblk, &FwdPlay );
}

int CXenoChannel::RcvFnOper_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	SsmClearRxDtmfBuf(nChan);
	
	if ( Reserved++ < 3 )
	{
		Reserved |= 0x08;	// 置状态“正在”
		return PlayFile( FILE_XENO_RCVOPER, FT_PROMPT, TRUE );
	}
	
	return ShExitPlay_init( evtblk, EXIT_GOODBYE );
}

struct state* CXenoChannel::RcvFnOper_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x04;	// 记录播放完毕标志
		SetTimer( 5000 ); //return &RcvFnOper;
	}
	
	if ( evtblk->wEventCode == E_SYS_TIMEOUT )
	{
		Reserved &= 0x03;	// 清除播放完毕标志
		return &RcvFnOper;
	}
	
	if ( (Reserved & 0x04) && SsmGetRxDtmfLen( nChan ) )
	{
		CHAR nDtmfKey = '\0';
		KillTimer();
		
		SsmGet1stDtmf( nChan, &nDtmfKey );
		
		switch ( nDtmfKey )
		{
		case	'1':			// 充值
			XnTeleNext	= RcvXnCard;
			XnTelePrev	= RcvXnTele;
			Reserved &= 0x00; return &RcvXnTele;
			
		case	'2':			// 查询
			CopyMemory( XenoNumber, RemoteId, MAX_PHONE );
			Reserved &= 0x00; return &ReadBalan;
			
		case	'3':			// 转充
			XnTeleNext	= RcvXnMony;
			XnTelePrev	= RcvXnTele;
			Reserved &= 0x00; return &RcvXnTele;
			
		case	'4':			// 通信录
			Reserved &= 0x00; return &RcvUsrList;
		
		case	'5':			// 服务密码
			Reserved &= 0x00; return &RcvUsrPass;
		
		case	'0':	// 人工
			//Reserved &= 0x00;
			return FwdCaller( "96031", &RcvFnOper );
		
		default:
			Reserved &= 0x07; return &RcvFnOper;
		}
	}
	
	return ChkDTSignal( evtblk, &RcvFnOper );
}

// 充值号码
int CXenoChannel::RcvXnTele_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;
	SsmClearRxDtmfBuf( nChan );
	ZeroMemory( XenoNumber, MAX_PHONE + 1 );
	
	if ( IsTestNumber( RemoteId ) || XnTeleNext.initiate == RcvXnMony.initiate )
	{
		return PlayFile( FILE_XENO_WTXNTEL, FT_PROMPT, TRUE );
	}
	else
	{
		Reserved |= 0x01;	// 记录播放完毕标志
		return SsmPutUserEvent( E_CHG_RcvDTMF, nChan, '#' );
	}
}

struct state* CXenoChannel::RcvXnTele_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x01;	// 记录播放完毕标志
	}
	
	if ( evtblk->wEventCode == E_CHG_RcvDTMF &&
		('#' == (CHAR)(evtblk->dwParam & 0xFFFF) || SsmGetRxDtmfLen(nChan) >= MAX_PHONE) )
	{
		Reserved |= 0x02;	// 记录按键完毕标志
	}

	if ( (Reserved & 0x03) == 0x03 )
	{
		if ( ! ReadDtmf( XenoNumber, '#' ) ) CopyMemory( XenoNumber, RemoteId, MAX_PHONE );
		
		Reserved &= 0x00; return &VfXnTele;
//		VerifyPhoneNum( XenoNumber ); ChkUserState( XenoNumber );
		
//		Reserved &= 0x00; return &XnTeleNext;
	}
	
	return ChkDTSignal( evtblk, &RcvXnTele );
}

// 号码确认
int CXenoChannel::VfXnTele_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;

	Reserved |= 0x08;
	SsmClearRxDtmfBuf( nChan );
	
	CSettings::Item* pItem = Settings.GetSetting( _T(".Path") );
	if ( ! pItem ) return ShExitPlay_init( evtblk, EXIT_BUSY );
	
	CString strFile;
	strFile.Format( _T("%s\\%s\\%s"), *pItem->m_pString, FT_PROMPT, FILE_XENO_VFXNTEL );
	
	return ShPlyXnTele( nChan, (LPCTSTR)strFile );
}

struct state* CXenoChannel::VfXnTele_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x04;	// 记录播放完毕标志
	}
	
	if ( (Reserved & 0x04) && SsmGetRxDtmfLen( nChan ) )
	{
		CHAR nDtmfKey = '\0';
		KillTimer();
		
		SsmGet1stDtmf( nChan, &nDtmfKey );
		
		if ( nDtmfKey == '1' )
		{
			VerifyPhoneNum( XenoNumber );
			ChkUserState( XenoNumber );
			
			Reserved &= 0x00; return &XnTeleNext;
		}
		else
		{
			Reserved &= 0x00; return &XnTelePrev;
		}
	}
	
	return ChkDTSignal( evtblk, &VfXnTele );
}

// 充值卡号
int CXenoChannel::RcvXnCard_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;

	Reserved |= 0x08;	// 置状态“正在”
	SsmClearRxDtmfBuf( nChan );
	
	return PlayFile( FILE_XENO_RCVCARD, FT_PROMPT, TRUE );
}

struct state* CXenoChannel::RcvXnCard_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x01;	// 记录播放完毕标志
	}
	
	if ( evtblk->wEventCode == E_CHG_RcvDTMF &&
		('#' == (CHAR)(evtblk->dwParam & 0xFFFF) || SsmGetRxDtmfLen(nChan) >= MAX_CARD_LEN) )
	{
		Reserved |= 0x02;	// 记录按键完毕标志
	}

	if ( (Reserved & 0x03) == 0x03 )
	{
		ReadDtmf( XenoCard, '#' );
		
		Reserved &= 0x00; return &RcvXnWord;
	}
	
	return ChkDTSignal( evtblk, &RcvXnCard );
}

// 充值密码
int CXenoChannel::RcvXnWord_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;

	Reserved |= 0x08;	// 置状态“正在”
	SsmClearRxDtmfBuf( nChan );
	
	return PlayFile( FILE_XENO_RCVWORD, FT_PROMPT, TRUE );
}

struct state* CXenoChannel::RcvXnWord_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x01;	// 记录播放完毕标志
	}
	
	if ( evtblk->wEventCode == E_CHG_RcvDTMF &&
		('#' == (CHAR)(evtblk->dwParam & 0xFFFF) || SsmGetRxDtmfLen(nChan) >= MAX_PASSWORD) )
	{
		Reserved |= 0x02;	// 记录按键完毕标志
	}

	if ( (Reserved & 0x03) == 0x03 )
	{
		ReadDtmf( XenoWord, '#' );
		
		Reserved &= 0x00; return XenoUserMoney( &PlayXnOk );
	}
	
	return ChkDTSignal( evtblk, &RcvXnWord );
}

int CXenoChannel::ReputCard_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;

	Reserved |= 0x08;	// 置状态“正在”
	
	if ( XenoError == XENO_REJECT || XenoError == XENO_EXPIRED )
		return PlayFile( FILE_XENO_NOTCARD, FT_PROMPT, FALSE );
	else
		return PlayFile( FILE_XENO_NOCARD, FT_PROMPT, FALSE );
}

struct state* CXenoChannel::ReputCard_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved = 0; return &RcvXnCard;
	}
	
	return ChkDTSignal( evtblk, &ReputCard );
}

int CXenoChannel::PlayXnOk_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;

	Reserved |= 0x08;	// 置状态“正在”
	return PlayFile( FILE_XENO_XENOOK, FT_PROMPT, FALSE );
}

struct state* CXenoChannel::PlayXnOk_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved &= 0x00; return &ReadBalan;
	}
	
	return ChkDTSignal( evtblk, &PlayXnOk );
}

int CXenoChannel::ReadBalan_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	ChkUserState( XenoNumber, FALSE );
	if ( m_pRecordusr == NULL || m_pRecordusr->adoEOF ) return ShExitPlay_init( evtblk, EXIT_NOTUSER );
	
	double nBalance = CalcUserBlance();
	theApp.Message(MSG_TEMP, "CalcUserBlance[%i]: {%s} nBalance:=%.2f", nChan,
		(LPCTSTR)(_bstr_t)m_pRecordusr->GetCollect( "xphone" ), nBalance);
	
	CSettings::Item* pItem = Settings.GetSetting( _T(".Path") );
	if ( ! pItem ) return ShExitPlay_init( evtblk, EXIT_BUSY );
	
	Reserved |= 0x08;	// 置状态“正在”
	
	if ( nBalance >= 0 )
	{
		TCHAR szT[80];
		::GetPrivateProfileString( "Xeno", "PlyXnBala",
			"13912-17096", szT, 80, *pItem->m_pString + "\\MediaCfg.ini" );
		
		DWORD nOffset1 = 0, nOffset2 = 0;
		_stscanf( szT, "%i-%i", &nOffset1, &nOffset2 );
		
		CString strFile;
		strFile.Format( _T("%s\\%s\\%s"), *pItem->m_pString, FT_PROMPT, FILE_XENO_AQUERY1 );
		
		return ShPlyXnBala( nChan, nBalance, (LPCTSTR)strFile, nOffset1, nOffset2 );
	}
	else
	{
		TCHAR szT[80];
		::GetPrivateProfileString( "Xeno", "PlyNoPay",
			"12000-15312", szT, 80, *pItem->m_pString + "\\MediaCfg.ini" );
		
		DWORD nOffset1 = 0, nOffset2 = 0;
		_stscanf( szT, "%i-%i", &nOffset1, &nOffset2 );
		
		CString strFile;
		strFile.Format( _T("%s\\%s\\%s"), *pItem->m_pString, FT_PROMPT, FILE_XENO_NOPAY );
		
		return ShPlyXnBala( nChan, nBalance, (LPCTSTR)strFile, nOffset1, nOffset2 );
	}
}

struct state* CXenoChannel::ReadBalan_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved = 0; return &RcvFnOper;
	}
	
	return ChkDTSignal( evtblk, &ReadBalan );
}

// 转充金额
int CXenoChannel::RcvXnMony_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	ChkUserState( RemoteId, FALSE );
	ASSERT( m_pRecordusr != NULL && ! m_pRecordusr->adoEOF );

	double nBalance = CalcUserBlance(TRUE);
	if ( nBalance > 0 ) return ShPlyXnMoney(nChan, nBalance);
	
	return ShExitPlay_init( evtblk, EXIT_GOODBYE );
}

struct state* CXenoChannel::RcvXnMony_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x01;	// 记录播放完毕标志
	}
	
	if ( evtblk->wEventCode == E_CHG_RcvDTMF &&
		('#' == (CHAR)(evtblk->dwParam & 0xFFFF) || SsmGetRxDtmfLen(nChan) >= 10) )
	{
		Reserved |= 0x02;	// 记录按键完毕标志
	}

	if ( (Reserved & 0x03) == 0x03 )
	{
		CHAR xBalan[10 + 1];
		ZeroMemory( xBalan, 11 );
		
		double nBalance = CalcUserBlance(TRUE);
		
		if ( ReadDtmf( xBalan, '#' ) )
		{
			nBalance = max( 0, min(nBalance, atof(xBalan)) );
		}
		
		Reserved &= 0x00;
		MoveUserMoney( nBalance, XenoNumber, &PlayXnOk );

		Traffic.AddUsrBack( LocalId, XenoNumber );
		CopyMemory( XenoNumber, RemoteId, MAX_PHONE );
			
		return &PlayXnOk;
	}
	
	return ChkDTSignal( evtblk, &RcvXnMony );
}

// 通信录
int CXenoChannel::RcvUsrList_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;
	SsmClearRxDtmfBuf( nChan );
	ZeroMemory( XenoNumber, MAX_BUFFER + 1 );
	
	return PlayFile( FILE_XENO_WTUSRCM, FT_PROMPT, TRUE );
}

struct state* CXenoChannel::RcvUsrList_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x01;	// 记录播放完毕标志
	}
	
	if ( evtblk->wEventCode == E_CHG_RcvDTMF )
	{
		int nKeyPos = min( evtblk->dwParam >> 16, MAX_BUFFER );
		XenoNumber[ max(nKeyPos - 1, 0) ] = (CHAR)(evtblk->dwParam & 0xFFFF);
		
		if ( '#' == (CHAR)(evtblk->dwParam & 0xFFFF) || nKeyPos >= MAX_BUFFER )
		{
			if ( '#' == (CHAR)(evtblk->dwParam & 0xFFFF) ) XenoNumber[ max(nKeyPos - 1, 0) ] = '\0';
			Reserved |= 0x02;	// 记录按键完毕标志
		}
	}

	if ( (Reserved & 0x03) == 0x03 )
	{
		PutUsrRecord( (_bstr_t)"", XenoNumber );
		
		Reserved &= 0x00;
		ExitCode = EXIT_GOODBYE; return &ExitPlay;
	}
	
	return ChkDTSignal( evtblk, &RcvUsrList );
}

BOOL CXenoChannel::PutUsrRecord(_bstr_t xjsjbh, LPCTSTR xUsrList)
{
	CString	strQuery;
	
	if ( xjsjbh == (_bstr_t)"" )
	{
		strQuery.Format( "select xjsjbh from xuser"
			" where xphone = '%s'", RemoteId );
		TRACE( strQuery + "\n" );
		
		_RecordsetPtr xRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
		_variant_t TheValue = xRecordset->GetCollect( "xjsjbh" );
		
		xjsjbh = (_bstr_t)TheValue;
	}
	
	CHAR* pptr;
	CHAR szFdTel[MAX_PHONE + 1];
	int nLength = GetNextBuffer( xUsrList, pptr );
	
	nLength = min( nLength, MAX_PHONE );
	CopyMemory( szFdTel, xUsrList, nLength ); szFdTel[nLength] = '\0';
	
	if ( _tcslen( szFdTel ) )
	{
		strQuery.Format( "insert into XusrCommList(xjsjbh, xfmobile)"
			" values('%s', '%s')", (LPCTSTR)xjsjbh, szFdTel );
		TRACE( strQuery + "\n" );
		
		xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
	}
	if ( *pptr != '\0' ) return PutUsrRecord( (LPCTSTR)xjsjbh, pptr + 1);
	
	return TRUE;
}

// 服务密码
int CXenoChannel::RcvUsrPass_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;

	Reserved |= 0x08;	// 置状态“正在”
	SsmClearRxDtmfBuf( nChan );
	
	return PlayFile( FILE_XENO_RCVWORD, FT_PROMPT, TRUE );
}

struct state* CXenoChannel::RcvUsrPass_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x01;	// 记录播放完毕标志
	}
	
	if ( evtblk->wEventCode == E_CHG_RcvDTMF &&
		('#' == (CHAR)(evtblk->dwParam & 0xFFFF) || SsmGetRxDtmfLen(nChan) >= MAX_PASSWORD) )
	{
		Reserved |= 0x02;	// 记录按键完毕标志
	}

	if ( (Reserved & 0x03) == 0x03 )
	{
		CHAR xPass[MAX_PASSWORD + 1];
		ZeroMemory( xPass, MAX_PASSWORD + 1 );
		
		ReadDtmf( xPass, '#' );
		ExitCode = EXIT_GOODBYE;
		
		Reserved &= 0x00; return XenoUserPass( xPass, &ExitPlay );
	}
	
	return ChkDTSignal( evtblk, &RcvUsrPass );
}
