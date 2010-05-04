//
// ChannelFriend.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Network.h"
#include "Settings.h"
#include "ChannelFriend.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MAX_ULW_SIZE	(1000 * 1024)

//////////////////////////////////////////////////////////////////////
// CFriendChannel construction

CFriendChannel::CFriendChannel(int nChan, LPCTSTR xFeeCode) : CChannel( nChan, xFeeCode )
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
	
	RcvUsrComp.initiate		= (INITIATE)RcvUsrComp_init;
	RcvUsrComp.process		= (PROCESS)RcvUsrComp_cmplt;
	PlyUsrComp.initiate		= (INITIATE)PlyUsrComp_init;
	PlyUsrComp.process		= (PROCESS)PlyUsrComp_cmplt;
	
	RcvLvMemb.initiate		= (INITIATE)RcvLvMemb_init;
	RcvLvMemb.process		= (PROCESS)RcvLvMemb_cmplt;
	RcvLvWord.initiate		= (INITIATE)RcvLvWord_init;
	RcvLvWord.process		= (PROCESS)RcvLvWord_cmplt;
	ErrReject.initiate		= (INITIATE)ErrReject_init;
	ErrReject.process		= (PROCESS)ErrReject_cmplt;
	
	RcvLvOper.initiate		= (INITIATE)RcvLvOper_init;
	RcvLvOper.process		= (PROCESS)RcvLvOper_cmplt;
	PlyUlWord.initiate		= (INITIATE)PlyUlWord_init;
	PlyUlWord.process		= (PROCESS)PlyUlWord_cmplt;
	
	RcvUlOper.initiate		= (INITIATE)RcvUlOper_init;
	RcvUlOper.process		= (PROCESS)RcvUlOper_cmplt;
	RcvUlWord.initiate		= (INITIATE)RcvUlWord_init;
	RcvUlWord.process		= (PROCESS)RcvUlWord_cmplt;
	
	LvWordPtr = NULL; LvWordSize = 0;
	CFriendChannel::Setup();
}

CFriendChannel::CFriendChannel(int nChan, LPCTSTR xFeeCode, BYTE nReserved) : CChannel( nChan, xFeeCode )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	RcvTalk.initiate		= (INITIATE)PlyLvWord_init;
	
	LvWordPtr = NULL; LvWordSize = 0;
	CFriendChannel::Setup();
}

CFriendChannel::~CFriendChannel()
{
	FreeBuffer();
}

//////////////////////////////////////////////////////////////////////
// CFriendChannel attributes

void CFriendChannel::Setup()
{
	CChannel::Setup();
	fcnp = &FeeLogin;
}

void CFriendChannel::KillLinker()
{
	if ( m_nTaskId != -1 )
	SsmPutUserEvent( E_CHG_ChState, m_nTaskId,
		MAKEVENT(m_nChan & 0xFFFF, S_CALL_PENDING) );
	
	CChannel::KillLinker();
}

LPVOID CFriendChannel::NewBuffer(DWORD nBufSize)
{
	FreeBuffer();
	
	LvWordPtr  = new BYTE[nBufSize];
	LvWordSize = nBufSize;
	
	return LvWordPtr;
}

void CFriendChannel::SetBuffer(LPVOID pBuf, DWORD nBufSize)
{
//theApp.Message(MSG_TEMP,"SetBuffer(0x%x, %i)", pBuf, nBufSize);
	NewBuffer( nBufSize );
	
	CopyMemory( LvWordPtr, pBuf, nBufSize );
}

void CFriendChannel::FreeBuffer()
{
//theApp.Message(MSG_TEMP,"FreeBuffer(0x%x, %i)", LvWordPtr, LvWordSize);
	if ( LvWordPtr ) delete [] LvWordPtr;
	LvWordPtr = NULL; LvWordSize = 0;
}

struct state* CFriendChannel::FwdCaller(LPCTSTR xLocalId)
{
	CopyMemory( LocalId, xLocalId, MAX_PHONE );
	CopyMemory( CallType, CA_TALK, MAX_CALLTYPE );
	FeeLogin.initiate	= (INITIATE)FeeRebind_init;
	
	return fcnp = &FeeLogin;
}

BOOL CFriendChannel::ChkUserState(LPCTSTR xUsrNumber)
{
	/*
	CHAR xRemoteId[MAX_PHONE + 1];
	ZeroMemory( xRemoteId, MAX_PHONE + 1 );
	CopyMemory( xRemoteId, xUsrNumber, MAX_PHONE );
#if 1
	// 移除主叫方0574
	if ( ! _tcsncmp( xRemoteId, "0574", 4 ) )
	{
		MoveMemory( xRemoteId, xRemoteId + 4, MAX_PHONE - 4 );
	}
#endif
	
	 */
	CString strQuery;
	
	m_pRecordset->MoveFirst();
	strQuery.Format( "xphone = '%s'", xUsrNumber );
	m_pRecordset->Find( (_bstr_t)strQuery, 0, adSearchForward, "" );
	
	if ( ! m_pRecordset->adoEOF ) return TRUE;
	
	m_pRecordset->MoveFirst();
	strQuery.Format( "xphone_tmp = '%s'", xUsrNumber );
	m_pRecordset->Find( (_bstr_t)strQuery, 0, adSearchForward, "" );
	
	return ( ! m_pRecordset->adoEOF );
}

BOOL CFriendChannel::FindUserId(LPCTSTR xUserId)
{
	CString strQuery;
	
	m_pRecordset->MoveFirst();
	strQuery.Format( "xbh = '%s'", xUserId );
	m_pRecordset->Find( (_bstr_t)strQuery, 0, adSearchForward );
	
	if ( ! m_pRecordset->adoEOF ) return TRUE;
	
	return FALSE;
}

BOOL CFriendChannel::IsRejectTime()
{
	CTime tNow = CTime::GetCurrentTime();
	WORD nHour = tNow.GetHour();
	
	_variant_t TheValue = m_pRecordset->GetCollect( "xtime" );
	if ( VT_NULL == TheValue.vt ) return FALSE;
	
	CString strValue = (LPCTSTR)(_bstr_t)TheValue;
	//
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		INT nCh1 = -1, nCh2 = -1;
		if ( _stscanf( str, "%i-%i", &nCh1, &nCh2 ) == 2 )
		{
			if ( nHour >= nCh1 && nHour <= nCh2 ) return TRUE;
		}
		else
		{
			if ( nHour == nCh1 ) return TRUE;
		}
	}
	//
	
	return FALSE;
}

int CFriendChannel::PlayBlob(int nChan, FieldPtr xPlyBlob, BOOL bTermDtmf)
{
	SsmSetDtmfStopPlay( nChan, bTermDtmf );
	SsmClearRxDtmfBuf( nChan );
	
	LONG lDataSize = xPlyBlob->ActualSize;
//theApp.Message(MSG_ERROR,"PlayBlob-> lDataSize=%i...",lDataSize);
	if ( lDataSize <= 0 ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	_variant_t TheValue = xPlyBlob->GetChunk(lDataSize);
if ( TheValue.vt !=(VT_ARRAY|VT_UI1) )
{
	theApp.Message(MSG_ERROR,"TheValue.vt !=(VT_ARRAY|VT_UI1)");
	return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
}
	LPTSTR pBuf = NULL;
//theApp.Message(MSG_ERROR,"PlayBlob-> SafeArrayAccessData...");
	SafeArrayAccessData(TheValue.parray, (void **)&pBuf);
//theApp.Message(MSG_ERROR,"PlayBlob-> SetBuffer...");
	this->SetBuffer( pBuf, lDataSize );
//theApp.Message(MSG_ERROR,"PlayBlob-> SafeArrayUnaccessData...");
	SafeArrayUnaccessData( TheValue.parray );
	
	if ( SsmPlayMem( nChan, 6, (LPBYTE)LvWordPtr, LvWordSize, 0, LvWordSize - 1 ) )
	return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
//theApp.Message(MSG_ERROR,"debug[%i], playmem:=%x, size=%i",
//			   nChan,LvWordPtr,LvWordSize);
	return 0;
}

struct state* CFriendChannel::ShRcvFdWord_cmplt(EVTBLK *evtblk, LPTSTR lpstBuffer,
												struct state* lpstDefaultState,
												struct state* lpstEntryState)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x04;		// 记录播放完毕标志
	}
	
	if ( evtblk->wEventCode == E_CHG_RcvDTMF )
	{
		int nDtmfLen = _tcslen( lpstBuffer );
		CHAR cDtmfKey= (CHAR)(evtblk->dwParam & 0xFFFF);
		if ( '#' != cDtmfKey ) *(lpstBuffer+nDtmfLen++) = cDtmfKey;
		
		if ( '#' == cDtmfKey || nDtmfLen >= MAX_BUFFER )
		{
			Reserved |= 0x01;	// 标记DTMF#接收成功
		}
	}
	
	if ( (Reserved & 0x04) && (Reserved & 0x01) )
	{
		Reserved &= 0x00; return lpstEntryState;
	}
	
	return ChkDTSignal( evtblk, lpstDefaultState );
}

struct state* CFriendChannel::ShRcvLvMemb_cmplt(EVTBLK *evtblk, struct state* lpstEntryState)
{
	struct state* pState = ShRcvFdWord_cmplt(evtblk, (LPTSTR)LvWordPtr, &RcvLvMemb, lpstEntryState);
	if ( pState != lpstEntryState ) return pState;	
	
	if ( ! FindUserId( (LPTSTR)LvWordPtr ) ) return &RcvLvMemb;
	if ( IsRejectTime() ) {Reserved &= 0x00; return &ErrReject;}
	
	return lpstEntryState;
}

int CFriendChannel::ShPlyUlWord_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
if ( m_pRecordset->adoEOF )
{
	theApp.Message(MSG_ERROR,"ShPlyUlWord_init-> adoEOF...");
}
	
_variant_t TheValue = m_pRecordset->GetCollect( "xphone" );
if ( VT_NULL != TheValue.vt)
{
	CString xRemoteId = (LPCTSTR)(_bstr_t)TheValue;
	theApp.Message(MSG_ERROR,"XPHONE:=%s", (LPCTSTR)xRemoteId);
}
	FieldPtr xPlyBlob = m_pRecordset->GetFields()->GetItem("xfile");
//theApp.Message(MSG_ERROR,"ShPlyUlWord_init-> GetFields...");
	return PlayBlob( nChan, xPlyBlob, TRUE );
}

struct state* CFriendChannel::ShPlyUlWord_cmplt(EVTBLK *evtblk, struct state* lpstEntryState)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		if ( Reserved &= 0x07 ) 
		{
theApp.Message( MSG_ERROR,"[%i] new add 0 resvv=[%i]", nChan, Reserved );
			return lpstEntryState;
		}
	}
	
	if ( evtblk->wEventCode == E_CHG_RcvDTMF && '1' == (CHAR)(evtblk->dwParam & 0xFFFF) )
	{
		if ( IsRejectTime() ) {Reserved &= 0x00; return &ErrReject;}
		
		Reserved &= 0x00; return &RcvLvWord;
	}
	
	return ChkDTSignal( evtblk, &PlyUlWord );
}

struct state* CFriendChannel::ShPlyUlWordX_cmplt(EVTBLK *evtblk, CHAR xSex)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		CString strQuery;
		strQuery.Format( "xsex = '%c'", xSex );
		
//theApp.Message( MSG_ERROR,"[%i] 11111 [%i]", nChan, Reserved );
		if ( (Reserved &= 0x07) == 1 )
		{
			m_pRecordset->MoveFirst();
			m_pRecordset->Find( (_bstr_t)strQuery, 0, adSearchForward, "" );
		}
		else
		{
			m_pRecordset->Find( (_bstr_t)strQuery, 1, adSearchForward, m_pRecordset->Bookmark );
		}
		
		if ( m_pRecordset->adoEOF )
		{
//theApp.Message( MSG_ERROR,"[%i] m_pRecordset->adoEOF", nChan );
			Reserved &= 0;
			return &RcvLvOper;
		}
		
//theApp.Message( MSG_ERROR,"[%i] aaa [%i]", nChan, Reserved );
		return &PlyUlWord;
	}
	
	return ShPlyUlWord_cmplt( evtblk, &RcvLvOper );
}

struct state* CFriendChannel::ShExitCode_cmplt(EVTBLK *evtblk, int nExitCode)
{
	ExitCode = nExitCode;
	Reserved = 0; return &ExitPlay;
}

struct state* CFriendChannel::ShPlayEnd_cmplt(EVTBLK *evtblk, struct state* lpstDefaultState, struct state* lpstEntryState)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved &= 0x07; return lpstEntryState;
	}
	
	return ChkDTSignal( evtblk, lpstDefaultState );
}

//////////////////////////////////////////////////////////////////////
// CFriendChannel OVERLOAD function

int CFriendChannel::bkIdle_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	SsmStopRecToMem( nChan );
	
	CChannel* pChannel = new CChannel( nChan );
	pChannel->Clone( this ); Network.SetChannel( pChannel );

	return (pChannel->*(pChannel->fcnp)->initiate)(evtblk);
}

int CFriendChannel::FeeLogin_init(EVTBLK *evtblk)
{
	return 0;
}

int CFriendChannel::FeeRebind_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	CChannel::OnConnected();
	SetTimer( 5* 1000 );
	
	return 0;
}

struct state* CFriendChannel::FeeLogin_cmplt(EVTBLK *evtblk)
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

int CFriendChannel::rcvMFsig_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( SsmGetChState(nChan) == S_CALL_TALKING ||	// 其他业务转入
		_tcsncmp(CallType, CA_DEBG, 4) == 0 )		// For Debug
	{
		return SsmPutUserEvent( E_CHG_ChState, nChan, S_CALL_TALKING );
	}
	
	return CChannel::rcvMFsig_init( evtblk );
}

int CFriendChannel::RcvTalk_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	CChannel::RcvTalk_init(evtblk);
	
	if ( SsmGetChState(nChan) != S_CALL_TALKING )
		SsmSetPlayDest( nChan, 1 );	// 上总线，用于内线测试
	
	fcnp = &FwdPlay; Reserved = 0;
	return (this->*fcnp->initiate)(evtblk);
}

/////////////////////////////////////////////////////////////////////////////
// CFriendChannel Play function

int CFriendChannel::FwdPlay_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	if ( PlayFile( FILE_FRND_WELCOME, FT_PROMPT, TRUE ) )
		return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	return 0;
}

struct state* CFriendChannel::FwdPlay_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved = 0; return &RcvUsrComp;
	}
	
	return ChkDTSignal( evtblk, &FwdPlay );
}

int CFriendChannel::RcvFnOper_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	SsmClearRxDtmfBuf(nChan);
	
	Reserved |= 0x08;	// 置状态“正在”
	return PlayFile( FILE_FRND_RCVOPER, FT_PROMPT, TRUE );
}

struct state* CFriendChannel::RcvFnOper_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x04;	// 记录播放完毕标志
		SetTimer( 5000 ); return &RcvFnOper;
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
		case	'1':		// 会员转接
			Reserved &= 0x00; return &RcvLvMemb;
		
		case	'2':		// 收听语音
			Reserved &= 0x00; return &RcvLvOper;
		
		case	'3':		// 机主留言
			m_pRecordset->Bookmark = m_pHostmark;
			Reserved &= 0x00; return &RcvUlOper;

		case	'0':	// 人工
			Reserved &= 0x00; return FwdCaller( "96031" );
		
		default:
			Reserved &= 0x07; return &RcvFnOper;
		}
	}
	
	return ChkDTSignal( evtblk, &RcvFnOper );
}

/////////////////////////////////////////////////////////////////////////////
// CFriendChannel rcv usr's company function

int CFriendChannel::RcvUsrComp_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	SsmClearRxDtmfBuf(nChan);
	LvWordPtr = NewBuffer( MAX_BUFFER + 1 );
	ZeroMemory( LvWordPtr, MAX_BUFFER + 1 );
	
	Reserved |= 0x08;	// 置状态“正在”
	return PlayFile( FILE_FRND_RCVCOMP, FT_PROMPT, TRUE );
}

struct state* CFriendChannel::RcvUsrComp_cmplt(EVTBLK *evtblk)
{
	return ShRcvFdWord_cmplt(evtblk, (LPTSTR)LvWordPtr, &RcvUsrComp, &PlyUsrComp);
}

int CFriendChannel::PlyUsrComp_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	Reserved |= 0x08;	// 置状态“正在”
	
	CString strQuery;
	
	strQuery.Format( "select * from xfriendmarry"
					" where xcombh = '%s'", (LPTSTR)LvWordPtr );
	TRACE( strQuery + "\n" );
theApp.Message(MSG_ERROR,strQuery);
	
	xOpenRecordset(m_pRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
	if ( m_pRecordset->adoEOF || (ChkUserState(RemoteId) ?\
 FALSE : ChkUserState(CString("0") + RemoteId) == FALSE) )
	{
		ExitCode = EXIT_NOTUSER;
		return PlayFile( FILE_FRND_NOTUSER, FT_PROMPT, FALSE );
	}
	
	m_pHostmark = m_pRecordset->Bookmark;
	
	_variant_t TheValue = m_pRecordset->GetCollect( "xcombh" );
	if ( VT_NULL == TheValue.vt )
	{
		fcnp = &RcvUsrComp; Reserved = 0;
		return (this->*fcnp->initiate)(evtblk);
	}
	
	strQuery.Format( "select * from xmarrycompany"
					" where xcombh = '%s'", (LPTSTR)(_bstr_t)TheValue );
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr xRecordset;
	xRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	
	FieldPtr xPlyBlob = xRecordset->GetFields()->GetItem("xvoice");
	return PlayBlob( nChan, xPlyBlob, FALSE );
}

struct state* CFriendChannel::PlyUsrComp_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;

	if ( !SsmGetPlayType( nChan ) && SsmGetRxDtmfLen( nChan ) )
	{
		CHAR nDtmfKey = '\0';
		SsmGet1stDtmf( nChan, &nDtmfKey );
		
		if ( '0' == nDtmfKey )
		{
			Reserved &= 0x00; return FwdCaller( "96031" );
		}
	}
	
	if ( evtblk->wEventCode != E_PROC_PlayEnd )
	return ChkDTSignal( evtblk, &PlyUsrComp );
	
	Reserved = 0;
	if ( ExitCode == EXIT_NOTUSER ) return &RcvUsrComp;
	return &RcvFnOper;
}

/////////////////////////////////////////////////////////////////////////////
// CFriendChannel rcv lv's id function

int CFriendChannel::RcvLvMemb_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	SsmClearRxDtmfBuf( nChan );
	LvWordPtr = NewBuffer( MAX_BUFFER + 1 );
	ZeroMemory( LvWordPtr, MAX_BUFFER + 1 );

	Reserved |= 0x08;	// 置状态“正在”
	return PlayFile( FILE_FRND_WTLVMEMB, FT_PROMPT, TRUE );
}

struct state* CFriendChannel::RcvLvMemb_cmplt(EVTBLK *evtblk)
{
	return ShRcvLvMemb_cmplt( evtblk, &RcvLvWord );
}

int CFriendChannel::RcvLvWord_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;

	Reserved |= 0x08;	// 置状态“正在”
	
#if 0
	return PlayFile( FILE_PROMPT_QUERY, FT_PROMPT, FALSE );
#else
	return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
#endif
}

struct state* CFriendChannel::RcvLvWord_cmplt(EVTBLK *evtblk)
{
//	if ( evtblk->wEventCode != E_PROC_PlayEnd )
//	return ChkDTSignal( evtblk, &RcvLvWord );
	
	_variant_t TheValue = m_pRecordset->GetCollect( "xphone" );
	CString xRemoteId = (LPCTSTR)(_bstr_t)TheValue;
	
	CSettings::Item* pItem = Settings.GetSetting( _T("Trunk.TrunkFind") );
	CChannel* pChannel = Network.LookOutPhs( xRemoteId, *pItem->m_pString );
	if ( pChannel == NULL ) return ShExitCode_cmplt( evtblk, EXIT_BUSY );
	
	CDialPacket* pPacket = new CDialPacket; pPacket->SetLocalId( "96031999" );
	pPacket->SetRemoteId( xRemoteId ); pPacket->SetFeeNumber( RemoteId );
	
	m_pRecordset->Bookmark = m_pHostmark;
	TheValue = m_pRecordset->GetCollect( "xbh" );
	
	CString strValue;
	strValue  = _T("96031999");
	strValue += (LPCTSTR)(_bstr_t)TheValue;
	pPacket->SetCallerId( (LPCTSTR)strValue );
	
	pChannel->OnDialPacket( pPacket ); pPacket->Release();
	SetLinker( pChannel->m_nChan );
	
	RcvTalk.initiate = (INITIATE)PlyLvWord_init;
	Reserved &= 0x00; return &RcvTalk;
}

// 会员沟通
int CFriendChannel::PlyLvWord_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08; return 0;
}

int CFriendChannel::ErrReject_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	CString strValue;
	Reserved |= 0x08;	// 置状态“正在”
	
	_variant_t TheValue = m_pRecordset->GetCollect( "xcombh" );
	strValue.Format( "frnd.reject%s.pcm", (LPCTSTR)(_bstr_t)TheValue );
	
	if ( PlayFile( strValue, FT_PROMPT, FALSE ) )
		return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	return 0;
}

struct state* CFriendChannel::ErrReject_cmplt(EVTBLK* evtblk)
{
	return ShPlayEnd_cmplt( evtblk, &ErrReject, &RcvFnOper );
}

/////////////////////////////////////////////////////////////////////////////
// CFriendChannel rcv lvw's id function

// 选择会员
int CFriendChannel::RcvLvOper_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;
	SsmClearRxDtmfBuf( nChan );
	return PlayFile( FILE_FRND_RCVLVOPER, FT_PROMPT, TRUE );
}

struct state* CFriendChannel::RcvLvOper_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;

	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x04;	// 记录播放完毕标志
	}
	
	if ( ! (Reserved & 0x04) || ! SsmGetRxDtmfLen( nChan ) )
	return ChkDTSignal( evtblk, &RcvLvOper );
	
	CHAR nDtmfKey = '\0';
	SsmGet1stDtmf( nChan, &nDtmfKey );
	
	if ( '1' == nDtmfKey )	// 男会员
	{
		PlyUlWord.process = (PROCESS)PlyUlWord1_cmplt;
		Reserved = 0x00; return &PlyUlWord;
	}
	else
	if ( '2' == nDtmfKey )	// 女会员
	{
		PlyUlWord.process = (PROCESS)PlyUlWord2_cmplt;
		Reserved = 0x00; return &PlyUlWord;
	}
	else
	if ( '3' == nDtmfKey )	// 找会员
	{
		RcvLvMemb.process = (PROCESS)RcvUlMemb_cmplt;
		Reserved &= 0x00; return &RcvLvMemb;
	}
	else
	if ( '*' == nDtmfKey )	// GoBack
	{
		Reserved &= 0x00; return &RcvFnOper;
	}
	
	Reserved &= 0x00; return &RcvLvOper;
}

struct state* CFriendChannel::RcvUlMemb_cmplt(EVTBLK *evtblk)
{
	struct state* pState = ShRcvFdWord_cmplt(evtblk, (LPTSTR)LvWordPtr, &RcvLvMemb, &PlyUlWord);
	if ( pState != &PlyUlWord ) return pState;
	
	if ( ! FindUserId( (LPTSTR)LvWordPtr ) ) return &RcvLvOper;
	
	PlyUlWord.process = (PROCESS)PlyUlWord_cmplt;
	return &PlyUlWord;
}

int CFriendChannel::PlyUlWord_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	if ( Reserved++ == 0 )
	{
		Reserved |= 0x08;
		SsmClearRxDtmfBuf( nChan );
		return PlayFile( FILE_FRND_PLYULWORD, FT_PROMPT, FALSE );
	}
	
	/*if ( Reserved < 5)
	{
		Reserved ++; Reserved &= 0x07;
	}*/
	Reserved = min(3, Reserved);
	return ShPlyUlWord_init( evtblk );
}

struct state* CFriendChannel::PlyUlWord_cmplt(EVTBLK *evtblk)
{
	return ShPlyUlWord_cmplt( evtblk, &RcvLvOper );
}

struct state* CFriendChannel::PlyUlWord1_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		if ( (Reserved &= 0x07) > 1 )
		Reserved |= 0x04;	// 记录播放完毕标志
	}
	
	if ( (Reserved & 0x04) && SsmGetRxDtmfLen( nChan ) )
	{
		CHAR nDtmfKey = '\0';
		SsmGet1stDtmf( nChan, &nDtmfKey );
		
		Reserved = 0;
		if ( nDtmfKey == '*' ) return &RcvLvOper;
	}
	
	return ShPlyUlWordX_cmplt( evtblk, '2' );
}

struct state* CFriendChannel::PlyUlWord2_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		if ( (Reserved &= 0x07) > 1 )
		Reserved |= 0x04;	// 记录播放完毕标志
	}
	
	if ( (Reserved & 0x04) && SsmGetRxDtmfLen( nChan ) )
	{
		CHAR nDtmfKey = '\0';
		SsmGet1stDtmf( nChan, &nDtmfKey );
		
		Reserved = 0;
		if ( nDtmfKey == '*' ) return &RcvLvOper;
	}
	
	return ShPlyUlWordX_cmplt( evtblk, '1' );
}

/////////////////////////////////////////////////////////////////////////////
// CFriendChannel rcv ulw's id function

// 机主选择
int CFriendChannel::RcvUlOper_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;
	SsmClearRxDtmfBuf( nChan );
	return PlayFile( FILE_FRND_RCVULOPER, FT_PROMPT, TRUE );
}

struct state* CFriendChannel::RcvUlOper_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;

	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x04;	// 记录播放完毕标志
	}
	
	if ( ! (Reserved & 0x04) || ! SsmGetRxDtmfLen( nChan ) )
	return ChkDTSignal( evtblk, &RcvUlOper );
	
	CString strQuery;
	CHAR nDtmfKey = '\0';
	SsmGet1stDtmf( nChan, &nDtmfKey );
	
	if ( '1' == nDtmfKey )	// 录音
	{
		Reserved &= 0x00; return &RcvUlWord;
	}
	else
	if ( '2' == nDtmfKey )	// 自听
	{
//theApp.Message(MSG_ERROR,"%i-> 自听", nChan);
		PlyUlWord.initiate = (INITIATE)ShPlyUlWord_init;
		PlyUlWord.process = (PROCESS)xPlyUlWord_cmplt;
		Reserved &= 0x00; return &PlyUlWord;
	}
	else
	if ( '3' == nDtmfKey )	// 删除
	{
		Reserved &= 0x00; return DelUlWord_cmplt( &RcvUlOper );
	}
	else
	if ( '*' == nDtmfKey )	// GoBack
	{
		Reserved &= 0x00; return &RcvFnOper;
	}
	
	Reserved &= 0x00; return &RcvUlOper;
}

int CFriendChannel::RcvUlWord_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	if ( Reserved++ == 0 )
	{
		// 接收留言
		Reserved |= 0x08;
		return PlayFile( FILE_FRND_RCVULWORD, FT_PROMPT, FALSE );
	}
	
	// 播放背景音乐
	Reserved |= 0x08;
	return PlayFile( FILE_FRND_BJMUSIC, FT_PROMPT, FALSE );
}

struct state* CFriendChannel::RcvUlWord_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		if ( (Reserved &= 0x07) == 0x01 )	// 提示语播放完毕
		{
			LvWordPtr = NewBuffer( MAX_ULW_SIZE );
			SsmRecToMem( nChan, 6, (LPBYTE)LvWordPtr, LvWordSize, 0 );
		}
		else
		if ( (Reserved & 0x04) == 0x04 )	// 短信录制完毕
		{
			Reserved &= 0x00;
			return PutUlWord_cmplt( &RcvUlOper );
		}
		
		Reserved = 0x01; // 循环播放背景音乐
	}
	if ( evtblk->wEventCode == E_PROC_RecordEnd )
	{
		Reserved |= 0x04;	// 记录录制完毕标志
		SsmStopPlayFile( nChan );
	}
	if ( evtblk->wEventCode == E_CHG_RcvDTMF )
	{
		if ( CHAR(evtblk->dwParam & 0xFFFF) == '#' )
		{
			LvWordSize = SsmGetRecOffset( nChan );
			SsmStopRecToMem( nChan );
		}
	}

	return ChkDTSignal( evtblk, &RcvUlWord );
}

struct state* CFriendChannel::PutUlWord_cmplt(struct state* lpstDefaultState)
{
	VARIANT varBLOB;
	SAFEARRAY *psa;
	SAFEARRAYBOUND rgsabound[1];
	
	rgsabound[0].lLbound = 0;
	rgsabound[0].cElements = LvWordSize;

	LPTSTR pBuf = (LPTSTR)LvWordPtr;
	psa = SafeArrayCreate( VT_UI1, 1, rgsabound );
	for (long i = 0; i < (long)LvWordSize; i++) SafeArrayPutElement (psa, &i, pBuf++);
	
	varBLOB.vt = VT_ARRAY | VT_UI1;                                   ///将varBLOB的类型设置为BYTE类型的数组
	varBLOB.parray = psa;                                             ///为varBLOB变量赋值
	
	FieldPtr xPlyBlob = m_pRecordset->GetFields()->GetItem("xfile");
	xPlyBlob->AppendChunk( varBLOB );
	
	m_pRecordset->Update();
	
	FreeBuffer();
	return lpstDefaultState;
}

struct state* CFriendChannel::DelUlWord_cmplt(struct state* lpstDefaultState)
{
if ( ! m_pRecordset->adoEOF )
{
_variant_t TheValue = m_pRecordset->GetCollect( "xphone" );
if ( VT_NULL != TheValue.vt)
{
	CString xRemoteId = (LPCTSTR)(_bstr_t)TheValue;
	theApp.Message(MSG_ERROR,"DelUlWord_cmplt-> XPHONE:=%s", (LPCTSTR)xRemoteId);
}
}
else
{
	theApp.Message(MSG_ERROR,"DelUlWord_cmplt-> adoEOF...");
}
//	m_pRecordset->PutCollect( "xfile", _variant_t() );
//	m_pRecordset->Update();
	FreeBuffer();
	return PutUlWord_cmplt( lpstDefaultState );
	
	//return lpstDefaultState;
}

struct state* CFriendChannel::xPlyUlWord_cmplt(EVTBLK *evtblk)
{
	return ShPlayEnd_cmplt( evtblk, &PlyUlWord, &RcvUlOper );
}
