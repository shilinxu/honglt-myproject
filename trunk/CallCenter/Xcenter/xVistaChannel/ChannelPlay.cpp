//
// ChannelPlay.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Network.h"
#include "ChannelPlay.h"

#include "Registry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CPlayChannel construction

CPlayChannel::CPlayChannel(int nChan, LPCTSTR xFeeCode) : CChannel( nChan, xFeeCode )
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
	
	RcvVista.initiate		= (INITIATE)RcvVista_init;
	RcvVista.process		= NULL;
	
	CPlayChannel::Setup();
}

CPlayChannel::~CPlayChannel()
{
}

//////////////////////////////////////////////////////////////////////
// CPlayChannel attributes

void CPlayChannel::Setup()
{
	CChannel::Setup();
	fcnp = &FeeLogin;

	SsmSetEvent(E_PROC_PlayFile, m_nChan, FALSE, NULL);
	SsmSetEvent(E_PROC_RecordFile, m_nChan, FALSE, NULL);
	SsmSetEvent(E_SYS_BargeIn, m_nChan, FALSE, NULL);
	SsmSetEvent(E_SYS_NoSound, m_nChan, FALSE, NULL);
}

struct state* CPlayChannel::FwdCaller(LPCTSTR xLocalId, struct state* lpszDefaultState)
{
	CopyMemory( LocalId, xLocalId, MAX_PHONE );
	CopyMemory( CallType, CA_TALK, MAX_CALLTYPE );
	
	CChannel::OnConnected();
	return lpszDefaultState;
}

BOOL CPlayChannel::OpenRecord(LPCTSTR xPhone)
{
	CString strQuery;
	
	strQuery.Format( "select top 1 * from xuservoice"
					" where xCaller = '%s' or xFeeNumber = '%s'"
					" order by xjsjbh desc", xPhone, xPhone );
	TRACE( strQuery + "\n" );
//theApp.Message(MSG_TEMP, strQuery);
	
	xOpenRecordset(m_pRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
	//m_pRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	
	return ! m_pRecordset->adoEOF;
}

struct state* CPlayChannel::KeyNotify(CHAR nDtmfKey)
{
	if ( nDtmfKey == '#' ) return &RcvVista;
	
	return &RcvFnOper;
}

int CPlayChannel::ShRcvTalk_init(EVTBLK *evtblk, struct state* lpszDefaultState)
{
	CChannel::RcvTalk_init(evtblk);
	
	fcnp = lpszDefaultState; Reserved = 0;
	return (this->*fcnp->initiate)(evtblk);
}

int CPlayChannel::ShExitPlay_init(EVTBLK *evtblk, int nExitCode)
{
	ExitCode = nExitCode; fcnp = &ExitPlay;
	Reserved = 0; return (this->*fcnp->initiate)(evtblk);
}

struct state* CPlayChannel::ShExitPlay_cmplt(EVTBLK *evtblk, int nExitCode)
{
	ExitCode = nExitCode;
	Reserved = 0; return &ExitPlay;
}

//////////////////////////////////////////////////////////////////////
// CPlayChannel OVERLOAD function

int CPlayChannel::bkIdle_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	CChannel* pChannel = new CChannel( nChan );
	pChannel->Clone( this ); Network.SetChannel( pChannel );
	
	return (pChannel->*(pChannel->fcnp)->initiate)(evtblk);
}

int CPlayChannel::FeeLogin_init(EVTBLK *evtblk)
{
	return 0;
}

int CPlayChannel::FeeRebind_init(EVTBLK *evtblk)
{
	if ( Reserved & 0x08 ) return 0;
	Reserved |= 0x08;	// 置状态“正在”
	
	CChannel::OnConnected();
	SetTimer( 5* 1000 );
	
	return 0;
}

struct state* CPlayChannel::FeeLogin_cmplt(EVTBLK *evtblk)
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

int CPlayChannel::rcvMFsig_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( SsmGetChState(nChan) == S_CALL_TALKING ||	// 其他业务转入
		_tcsncmp(CallType, CA_DEBG, 4) == 0 )		// For Debug
	{
		return SsmPutUserEvent( E_CHG_ChState, nChan, S_CALL_TALKING );
	}
	
	return CChannel::rcvMFsig_init(evtblk);
}

int CPlayChannel::RcvTalk_init(EVTBLK *evtblk)
{
	return ShRcvTalk_init( evtblk, &FwdPlay );
}

/////////////////////////////////////////////////////////////////////////////
// CPlayChannel Pager function

int CPlayChannel::FwdPlay_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	CString strFile = FILE_VIST_WELCOME;
	strFile.Insert( strFile.GetLength() - 4, '.' );
	strFile.Insert( strFile.GetLength() - 4, LocalId );
	
	if ( PlayFile( FILE_VIST_WELCOME, FT_PROMPT, TRUE ) )
		return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	return 0;
}

struct state* CPlayChannel::FwdPlay_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved = 0; return &RcvFnOper;
	}
	
	return ChkDTSignal( evtblk, &FwdPlay );
}

int CPlayChannel::RcvFnOper_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	SsmSetDtmfStopPlay( nChan, TRUE );
	SsmClearRxDtmfBuf( nChan );
	SsmClearFileList( nChan );
	
	if ( Reserved++ >= 3 )
	{
		ExitCode = EXIT_GOODBYE; fcnp = &ExitPlay;
		return (this->*fcnp->initiate)(evtblk);
	}
	
#if 0
	CRegistry pRegistry;
	CString strHost = pRegistry.GetString( "Vista", "Host", "057496031" );
#else
	CHAR xLocalId[MAX_PHONE + 1];
	CopyMemory( xLocalId, LocalId, MAX_PHONE + 1 );
	VerifyPhoneNum( xLocalId );
#endif
	
	if ( ! OpenRecord( xLocalId ) ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	_variant_t TheValue = m_pRecordset->GetCollect( "xPCMName" );
	if ( VT_NULL == TheValue.vt ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	SsmAddToFileList( nChan, (LPTSTR)(_bstr_t)TheValue, 6, 0, 0xFFFFFFFF );
	
	CString strFile;
	strFile.Format( "%s\\%s", FT_PROMPT, FILE_VIST_RCVOPER );
	
	strFile.Insert( strFile.GetLength() - 4, '.' );
	strFile.Insert( strFile.GetLength() - 4, LocalId );
	
	SsmAddToFileList( nChan, strFile.GetBuffer(0), 6, 0, 0xFFFFFFFF );
	
	Reserved |= 0x08;	// 置状态“正在”
	if ( SsmPlayFileList( nChan ) ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	return 0;
}

struct state* CPlayChannel::RcvFnOper_cmplt(EVTBLK *evtblk)
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
				
		if ( nDtmfKey == '0' ) return FwdCaller( "96031", &RcvFnOper );
		Reserved &= 0x00; return KeyNotify(nDtmfKey); 
	}
	
	return ChkDTSignal( evtblk, &RcvFnOper );
}

int CPlayChannel::RcvVista_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	_variant_t xPCMName = m_pRecordset->GetCollect( "xPCMName" );
	
	m_pRecordset->AddNew();
	m_pRecordset->PutCollect( "xPCMName", xPCMName );
	m_pRecordset->PutCollect( "xPhoneNum", RemoteId );
	m_pRecordset->PutCollect( "xCaller", _variant_t("p") );
	
#if 0
	CRegistry pRegistry;
	CString strHost = pRegistry.GetString( "Vista", "Host", "057496031" );
#else
	CHAR xLocalId[MAX_PHONE + 1];
	CopyMemory( xLocalId, LocalId, MAX_PHONE + 1 );
	VerifyPhoneNum( xLocalId );
#endif

	_variant_t TheValue = m_pRecordset->GetCollect( "xFeeNumber" );
	m_pRecordset->PutCollect( "xFeeNumber", TheValue );
	
	COleDateTime tNow = COleDateTime::GetCurrentTime();
	m_pRecordset->PutCollect( "xRecDate", _variant_t(tNow) );
	
	m_pRecordset->PutCollect( "xTheState", _variant_t("DELIVRD") );
	m_pRecordset->PutCollect( "xUsrBack", _variant_t("#") );
	m_pRecordset->Update();
	
	return ShExitPlay_init( evtblk, EXIT_GOODBYE );;
}
