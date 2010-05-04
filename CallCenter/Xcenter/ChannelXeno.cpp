//
// ChannelXeno.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Network.h"
#include "ChannelXeno.h"

#include "CtrlNetworkChild.h"

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
	rcvMFsig.initiate		= (INITIATE)rcvMFsig_init;
	RcvTalk.initiate		= (INITIATE)RcvTalk_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;

	FwdXenoer.initiate		= (INITIATE)FwdXenoer_init;
	FwdXenoer.process		= (PROCESS)FwdXenoer_cmplt;
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
	EnqMoney.initiate		= (INITIATE)EnqMoney_init;
	EnqMoney.process		= (PROCESS)EnqMoney_cmplt;
	
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

	ZeroMemory( XenoNumber, MAX_PHONE + 1 );
	ZeroMemory( XenoCard, MAX_CARD_LEN + 1 );
	ZeroMemory( XenoWord, MAX_PASSWORD + 1 );
	
	XenoError = XENO_SUCCESS;
	fcnp	  = &FeeLogin;
}

//////////////////////////////////////////////////////////////////////
// CXenoChannel operations

BOOL CXenoChannel::ChkUserState(LPCTSTR xUsrNumber, BOOL bAutoAdd)
{
	CString	strQuery;
	
	strQuery.Format( "select * from xuser_info"
					" where xjsjbh ="
						" (select top 1 xjsjbh from xuser"
						" where xphone = '%s')", xUsrNumber );
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

struct state* CXenoChannel::XenoUserMoney()
{
	CString	strQuery;
	
	strQuery.Format( "select * from Xcostcard"
					" where xcostcode = '%s' and xcostpwd = '%s'",
					XenoCard, XenoWord );
	TRACE( strQuery + "\n" );

	xOpenRecordset(m_pRecordcar, theApp.m_pConnection, (_bstr_t)strQuery);
	if ( m_pRecordcar->adoEOF )
	{
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

	return &PlayXnOk;
}

double CXenoChannel::CalcUserBlance()
{
	m_pRecordusr->Requery( adCmdUnknown );
	
	// Current Balance:= xpreBala + xBalance

	double nBalance = 0;
	_variant_t TheValue;

	TheValue = m_pRecordusr->GetCollect( "xpreBala" );
	if ( VT_NULL != TheValue.vt ) nBalance += (double)TheValue;
	TheValue = m_pRecordusr->GetCollect( "xBalance" );
	if ( VT_NULL != TheValue.vt ) nBalance += (double)TheValue;
	
	return nBalance;
}

struct state* CXenoChannel::FwdCaller()
{
	CTransfer::Disconnect();
	
	int nLength = _tcslen( LocalId );
	CopyMemory(LocalId + 6, LocalId + 5, nLength - 5);
	LocalId[ 5 ] = '+'; LocalId[ ++nLength ] = '#';
	
	CopyMemory( CallType, CA_TALK, MAX_CALLTYPE );

	return fcnp = &FeeLogin;
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

struct state* CXenoChannel::FeeLogin_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_FEE_Login &&
		(evtblk->dwParam & 0xFFFF) == S_FEE_Connected )
	{
		if ( SsmGetChState(nChan) == S_CALL_PICKUPED )
		{
			return &WaitDial;
		}
		if ( SsmGetChState(nChan) == S_CALL_TALKING )
		{
			SsmPutUserEvent( E_CHG_ChState, nChan, S_CALL_RINGING );
		}
#ifdef NPICKUP
		if ( SsmGetChState(nChan) == S_CALL_RINGING )
		{
theApp.Message( MSG_TEMP, "CXenoChannel::NPICKUP" );
SsmStopPlayFile( nChan );
			SsmPutUserEvent( E_CHG_ChState, nChan, S_CALL_RINGING );
		}
#endif
	}
	
	return CChannel::FeeLogin_cmplt(evtblk);
}

int CXenoChannel::rcvMFsig_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( SsmGetChState(nChan) == S_CALL_TALKING )
	return SsmPutUserEvent( E_CHG_ChState, nChan, S_CALL_TALKING );

#ifdef NPICKUP
theApp.Message( MSG_TEMP, "CXenoChannel::NPICKUP" );
	return SsmPutUserEvent( E_CHG_ChState, nChan, S_CALL_TALKING );
#endif
	
	return CChannel::rcvMFsig_init(evtblk);
}

int CXenoChannel::RcvTalk_init(EVTBLK *evtblk)
{
	CChannel::RcvTalk_init(evtblk);
	
	fcnp = &FwdXenoer; Reserved = 0;
	return (this->*fcnp->initiate)(evtblk);
}

/////////////////////////////////////////////////////////////////////////////
// CXenoChannel caller function

int CXenoChannel::FwdXenoer_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	m_pOwner->SetItem( nChan, "语音接入", DS_PLAY );
	return PlayFile( FILE_XENO_WELCOME, FT_PROMPT, TRUE );
}

struct state* CXenoChannel::FwdXenoer_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved = 0; return &RcvFnOper;
	}
	
	return ChkDTSignal( evtblk, &FwdXenoer );
}

int CXenoChannel::RcvFnOper_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	SsmClearRxDtmfBuf(nChan);
	
	if ( Reserved++ < 3 )
	{
		Reserved |= 0x08;	// 置状态“正在”
		m_pOwner->SetItem( nChan, "选择操作", DS_PLAY );
		return PlayFile( FILE_XENO_RCVOPER, _T("Media"), TRUE );
	}
	
	ExitCode = EXIT_GOODBYE; fcnp = &ExitPlay;
	return (this->*fcnp->initiate)(evtblk);
}

struct state* CXenoChannel::RcvFnOper_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;

	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x04;	// 记录播放完毕标志
		if ( (Reserved & 0x03) <= 3 ) SetTimer( 5000 );
	}
	
	if ( evtblk->wEventCode == E_SYS_TIMEOUT )
	{
		Reserved &= 0x03;	// 清除播放完毕标志
#ifdef NPICKUP
theApp.Message( MSG_TEMP, "CXenoChannel::NPICKUP" );
KillTimer();
CopyMemory(XenoNumber,"13867877813",11);
CopyMemory(XenoCard,"666666",6);
CopyMemory(XenoWord,"666666",6);
Reserved &= 0x00; return FwdCaller();
//Reserved &= 0x00; return FwdReceive( '2', &GetFxByte );
#endif
	}

	if ( (Reserved & 0x04) && SsmGetRxDtmfLen( nChan ) )
	{
		CHAR nDtmfKey = '\0';
		
		KillTimer(); SsmGet1stDtmf( nChan, &nDtmfKey );
//theApp.Message(MSG_TEMP,"CXenoChannel[%i]::RcvFnOper -> KEY=%c",nChan,nDtmfKey);
		
		switch ( nDtmfKey )
		{
		case	'1':			// 充值
			Reserved &= 0x00; return &RcvXnTele;
		case	'2':			// 查询
			CopyMemory( XenoNumber, RemoteId, MAX_PHONE );
			Reserved &= 0x00; return &EnqMoney;
		case	'0':			// 人工
			Reserved &= 0x00; return FwdCaller();
		default:
			Reserved &= 0x07; return &RcvFnOper;
		}
	}

	return ChkDTSignal( evtblk, &RcvFnOper );
}

int CXenoChannel::RcvXnTele_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;

	Reserved |= 0x08;	// 置状态“正在”
	SsmClearRxDtmfBuf( nChan );
	
	m_pOwner->SetItem( nChan, "输入号码", DS_PLAY );
	return PlayFile( FILE_XENO_WTXNTEL, FT_PROMPT, TRUE );
}

struct state* CXenoChannel::RcvXnTele_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x04;	// 记录播放完毕标志
	}
	
	if ( evtblk->wEventCode == E_CHG_RcvDTMF )
	{
		int nDtmfLen = SsmGetRxDtmfLen( nChan );
		
		if ( '#' == (CHAR)(evtblk->dwParam & 0xFFFF) || nDtmfLen >= MAX_PHONE )
		{
			if ( ! ReadDtmf( XenoNumber, '#' ) ) CopyMemory( XenoNumber, RemoteId, MAX_PHONE );
		}
	}

	if ( (Reserved & 0x04) && _tcslen( XenoNumber ) )
	{
#if 1
		VerifyPhoneNum( XenoNumber );
#endif
theApp.Message(MSG_TEMP,"XenoNumber: %s",XenoNumber);
		ChkUserState( XenoNumber ); Reserved &= 0x00; return &RcvXnCard;
	}
	
	return ChkDTSignal( evtblk, &RcvXnTele );
}

int CXenoChannel::RcvXnCard_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;

	Reserved |= 0x08;	// 置状态“正在”
	SsmClearRxDtmfBuf( nChan );
	
	m_pOwner->SetItem( nChan, "输入卡号", DS_PLAY );
	return PlayFile( FILE_XENO_RCVCARD, FT_PROMPT, TRUE );
}

struct state* CXenoChannel::RcvXnCard_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x04;	// 记录播放完毕标志
	}
	
	if ( evtblk->wEventCode == E_CHG_RcvDTMF )
	{
		int nDtmfLen = SsmGetRxDtmfLen( nChan );
		
		if ( '#' == (CHAR)(evtblk->dwParam & 0xFFFF) || nDtmfLen >= MAX_CARD_LEN )
		{
			ReadDtmf( XenoCard, '#' );
		}
	}

	if ( (Reserved & 0x04) && _tcslen( XenoCard ) )
	{
theApp.Message(MSG_TEMP,"XenoCard: %s",XenoCard);
		Reserved &= 0x00; return &RcvXnWord;
	}
	
	return ChkDTSignal( evtblk, &RcvXnCard );
}

int CXenoChannel::RcvXnWord_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;

	Reserved |= 0x08;	// 置状态“正在”
	SsmClearRxDtmfBuf( nChan );

	m_pOwner->SetItem( nChan, "输入密码", DS_PLAY );
	return PlayFile( FILE_XENO_RCVWORD, FT_PROMPT, TRUE );
}

struct state* CXenoChannel::RcvXnWord_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x04;	// 记录播放完毕标志
	}
	
	if ( evtblk->wEventCode == E_CHG_RcvDTMF )
	{
		int nDtmfLen = SsmGetRxDtmfLen( nChan );
		
		if ( '#' == (CHAR)(evtblk->dwParam & 0xFFFF) || nDtmfLen >= MAX_PASSWORD )
		{
			ReadDtmf( XenoWord, '#' );
		}
	}

	if ( (Reserved & 0x04) && _tcslen( XenoWord ) )
	{
theApp.Message(MSG_TEMP,"XenoWord: %s",XenoWord);
		Reserved &= 0x00; return XenoUserMoney();
	}
	
	return ChkDTSignal( evtblk, &RcvXnWord );
}

int CXenoChannel::ReputCard_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;

	Reserved |= 0x08;	// 置状态“正在”
	m_pOwner->SetItem( nChan, "充值失败", DS_FAIL );
	
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
	m_pOwner->SetItem( nChan, "充值成功", DS_PLAY );
	return PlayFile( FILE_XENO_XENOOK, FT_PROMPT, FALSE );
}

struct state* CXenoChannel::PlayXnOk_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved &= 0x00; ExitCode = EXIT_GOODBYE; return &EnqMoney;
	}
	
	return ChkDTSignal( evtblk, &PlayXnOk );
}

int CXenoChannel::EnqMoney_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	ChkUserState( XenoNumber, FALSE );
	if ( m_pRecordusr == NULL || m_pRecordusr->adoEOF )
	{
		ExitCode = EXIT_NOTUSER; fcnp = &ExitPlay;
		return (this->*fcnp->initiate)(evtblk);
	}

	Reserved |= 0x08;	// 置状态“正在”
	m_pOwner->SetItem( nChan, "帐户查询", DS_PLAY );
	
	char szFileName[MAX_PATH];
	SsmClearFileList( nChan );
	
	double nBalance = CalcUserBlance();
	
	if ( nBalance >= 0 )
	{
	sprintf(szFileName, "%s\\%s", FT_PROMPT, FILE_XENO_AQUERY1);
	SsmAddToFileList( nChan, szFileName, 6, 0, 0xFFFFFFFF );
theApp.Message( MSG_DEBUG, szFileName );
	}
	else
	{
		sprintf(szFileName, "%s\\%s", FT_PROMPT, FILE_XENO_NOPAY);
		SsmAddToFileList( nChan, szFileName, 6, 0, 0xFFFFFFFF );
theApp.Message( MSG_DEBUG, szFileName );
	}
	
	sprintf( szFileName, "%.2f", nBalance );
	CompDigital( szFileName, TRUE );
	
	sprintf(szFileName, "%s\\%s", FT_PROMPT, "digital.yuan.pcm" );
	SsmAddToFileList( nChan, szFileName, 6, 0, 0xFFFFFFFF );
theApp.Message( MSG_DEBUG, szFileName );
	
	sprintf(szFileName, "%s\\%s", FT_PROMPT, FILE_XENO_AQUERY2);
	SsmAddToFileList( nChan, szFileName, 6, 0, 0xFFFFFFFF );
theApp.Message( MSG_DEBUG, szFileName );
	
	return SsmPlayFileList( nChan );
}

struct state* CXenoChannel::EnqMoney_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved &= 0x00;
		ExitCode = EXIT_GOODBYE; return &ExitPlay;
	}
	
	return ChkDTSignal( evtblk, &EnqMoney );
}
