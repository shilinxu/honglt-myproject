//
// ChannelProfit.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Network.h"
#include "Settings.h"
#include "ChannelProfit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CProfitChannel construction

CProfitChannel::CProfitChannel(int nChan, LPCTSTR xFeeCode) : CChannel( nChan, xFeeCode )
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
	
	PlyUlWord.initiate		= (INITIATE)PlyUlWord_init;
	PlyUlWord.process		= (PROCESS)PlyUlWord_cmplt;
	
//	RcvLvWord.initiate		= (INITIATE)RcvLvWord_init;
//	RcvLvWord.process		= (PROCESS)RcvLvWord_cmplt;
	
	BufferPtr = NULL; BufferSize = 0;
	CProfitChannel::Setup();
}

CProfitChannel::CProfitChannel(int nChan, LPCTSTR xFeeCode, BYTE nReserved) : CChannel( nChan, xFeeCode )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	RcvTalk.initiate		= (INITIATE)PlyLvWord_init;
	
	BufferPtr = NULL; BufferSize = 0;
	CProfitChannel::Setup();
}

CProfitChannel::~CProfitChannel()
{
}

//////////////////////////////////////////////////////////////////////
// CProfitChannel attributes

void CProfitChannel::Setup()
{
	CChannel::Setup();
	
	fcnp = &FeeLogin;
}

LPVOID CProfitChannel::NewBuffer(DWORD nBufSize)
{
	FreeBuffer();
	
	BufferPtr  = new BYTE[nBufSize];
	BufferSize = nBufSize;
	
	return BufferPtr;
}

void CProfitChannel::SetBuffer(LPVOID pBuf, DWORD nBufSize)
{
	NewBuffer( nBufSize );
	
	CopyMemory( BufferPtr, pBuf, nBufSize );
}

void CProfitChannel::FreeBuffer()
{
	if ( BufferPtr ) delete [] BufferPtr;
	BufferPtr = NULL; BufferSize = 0;
}

void CProfitChannel::KillLinker()
{
	if ( m_nTaskId != -1 )
	SsmPutUserEvent( E_CHG_ChState, m_nTaskId,
		MAKEVENT(m_nChan & 0xFFFF, S_CALL_PENDING) );
	
	CChannel::KillLinker();
}

struct state* CProfitChannel::FwdCaller(LPCTSTR xLocalId)
{
	CopyMemory( LocalId, xLocalId, MAX_PHONE );
	CopyMemory( CallType, CA_TALK, MAX_CALLTYPE );
	FeeLogin.initiate	= (INITIATE)FeeRebind_init;
	
	return fcnp = &FeeLogin;
}

int CProfitChannel::PlayBlob(int nChan, FieldPtr xPlyBlob, BOOL bTermDtmf)
{
	SsmSetDtmfStopPlay( nChan, bTermDtmf );
	SsmClearRxDtmfBuf( nChan );
	
	LONG lDataSize = xPlyBlob->ActualSize;
	if ( lDataSize <= 0 ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	_variant_t TheValue = xPlyBlob->GetChunk(lDataSize);
	LPTSTR *pBuf = NULL;
	SafeArrayAccessData(TheValue.parray, (void **)&pBuf);
	this->SetBuffer( pBuf, lDataSize );
	SafeArrayUnaccessData( TheValue.parray );
	
	if ( SsmPlayMem( nChan, 6, (LPBYTE)BufferPtr, BufferSize, 0, BufferSize - 1 ) )
	return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	return 0;
}

struct state* CProfitChannel::ShRcvFnOper_cmplt(EVTBLK *evtblk, CHAR* pcDtmfKey)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x04;		// 记录播放完毕标志
		SetTimer( 5000 );
	}
	
	if ( (Reserved & 0x04) && SsmGetRxDtmfLen( nChan ) )
	{
		KillTimer(); SsmGet1stDtmfClr( nChan, pcDtmfKey );
		Reserved &= 0x00; return &RcvFnOper;
	}
	
	if ( evtblk->wEventCode == E_SYS_TIMEOUT )
	{
		Reserved &= 0x00; return &RcvFnOper;
	}
	
	return ChkDTSignal( evtblk, &RcvFnOper );
}

int CProfitChannel::PlayNavFile(LPCTSTR xRemoteId, LPCTSTR xNavFile, BOOL bTermDtmf)
{
	CString strQuery;
	int nChan = m_nChan;
	
	strQuery.Format( "select top 0 * from xBestowing" );
	TRACE( strQuery + "\n" );
	
	xOpenRecordset(m_pRecordbst, theApp.m_pConnection, (_bstr_t)strQuery);
	m_pRecordbst->AddNew();
	m_pRecordbst->PutCollect( "xphone", _variant_t(xRemoteId) );
	
	COleDateTime tNow = COleDateTime::GetCurrentTime();
	m_pRecordbst->PutCollect( "xdate", _variant_t(tNow) );
	
	m_pRecordbst->Update();
	m_pRecordbst->Resync( adAffectAll, adResyncAllValues );
	
	SsmSetDtmfStopPlay( nChan, bTermDtmf );
	SsmClearRxDtmfBuf( nChan );
	SsmClearFileList( nChan );
	
	int nError = SsmAddToFileList( nChan, (LPTSTR)xNavFile, 6, 0, 0xFFFFFFFF );
	if ( nError != 0 ) return nError;
	
	CSettings::Item* pItem = Settings.GetSetting( "Profit.PlyNumb" );
	if ( *pItem->m_pDword == TRUE)
	{
		SsmClearFileList( nChan );
		DWORD nOffset1 = 0, nOffset2 = 0;
		pItem = Settings.GetSetting( "Profit.SeekNum" );
		_stscanf( *pItem->m_pString, "%i-%i", &nOffset1, &nOffset2 );
		
		SsmAddToFileList( nChan, (LPTSTR)xNavFile, 6, 0, nOffset1 );
		
		_variant_t TheValue = m_pRecordbst->GetCollect( "jsjbh" );
		CompDigital( (LPCTSTR)(_bstr_t)TheValue, TRUE );
		
		SsmAddToFileList( nChan, (LPTSTR)xNavFile, 6, nOffset2, 0xFFFFFFFF );
	}
	
	return SsmPlayFileList( nChan );
}

struct state* CProfitChannel::RedexRecordset(struct state* lpszDefaultState)
{
	if ( m_pRecordset->adoEOF ) return &RcvFnOper;
	
	COleDateTime tNow = COleDateTime::GetCurrentTime();
	_variant_t TheValue = m_pRecordset->GetCollect( "xdateend" );
	if ( VT_NULL != TheValue.vt && (tNow - TheValue).GetTotalDays() > 0 )
	{
		m_pRecordset->MoveNext(); return RedexRecordset( lpszDefaultState );
	}
	
	TheValue = m_pRecordset->GetCollect( "xrule" );
	if ( VT_NULL == TheValue.vt ) return lpszDefaultState;
	
	CString strValue = (LPCTSTR)(_bstr_t)TheValue;
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		WORD nHour1 = -1, nHour2 = -1;
		if ( _stscanf( str, "%i-%i", &nHour1, &nHour2 ) == 2 )
		{
			if ( tNow.GetHour() >= nHour1 && tNow.GetHour() <= nHour2 ) return lpszDefaultState;
		}
		else
		{
			if ( tNow.GetHour() == nHour1 ) return lpszDefaultState;
		}
	}
	
	m_pRecordset->MoveNext();
	return RedexRecordset( lpszDefaultState );
}

struct state* CProfitChannel::ShExitCode_cmplt(EVTBLK *evtblk, int nExitCode)
{
	ExitCode = nExitCode;
	Reserved = 0; return &ExitPlay;
}

//////////////////////////////////////////////////////////////////////
// CProfitChannel OVERLOAD function

int CProfitChannel::bkIdle_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	CChannel* pChannel = new CChannel( nChan );
	pChannel->Clone( this ); Network.SetChannel( pChannel );

	return (pChannel->*(pChannel->fcnp)->initiate)(evtblk);
}

int CProfitChannel::FeeLogin_init(EVTBLK *evtblk)
{
	return 0;
}

int CProfitChannel::FeeRebind_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	CChannel::OnConnected();
	SetTimer( 5* 1000 );
	
	return 0;
}

struct state* CProfitChannel::FeeLogin_cmplt(EVTBLK *evtblk)
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

int CProfitChannel::rcvMFsig_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( SsmGetChState(nChan) == S_CALL_TALKING ||	// 其他业务转入
		_tcsncmp(CallType, CA_DEBG, 4) == 0 )		// For Debug
	{
		return SsmPutUserEvent( E_CHG_ChState, nChan, S_CALL_TALKING );
	}
	
	return CChannel::rcvMFsig_init(evtblk);
}

int CProfitChannel::RcvTalk_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	CChannel::RcvTalk_init(evtblk);
	
	if ( SsmGetChState(nChan) != S_CALL_TALKING )
		SsmSetPlayDest( nChan, 1 );	// 上总线，用于内线测试
	
	fcnp = &FwdPlay; Reserved = 0;
	return (this->*fcnp->initiate)(evtblk);
}

/////////////////////////////////////////////////////////////////////////////
// CProfitChannel Play function

// 语音接入
int CProfitChannel::FwdPlay_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	if ( PlayFile( FILE_PROT_WELCOME, FT_PROMPT, TRUE ) )
		return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	return 0;
}

struct state* CProfitChannel::FwdPlay_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved = 0; return &RcvFnOper;
	}
	
	return ChkDTSignal( evtblk, &FwdPlay );
}

// 选择操作
int CProfitChannel::RcvFnOper_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	SsmClearRxDtmfBuf(nChan);
	
	Reserved |= 0x08;	// 置状态“正在”
	return PlayFile( FILE_PROT_RCVOPER, FT_PROMPT, TRUE );
}

struct state* CProfitChannel::RcvFnOper_cmplt(EVTBLK *evtblk)
{
	CHAR nDtmfKey = '\0';
	struct state* pState;
	
	pState = ShRcvFnOper_cmplt( evtblk, &nDtmfKey );
	if ( pState != &RcvFnOper || ! nDtmfKey ) return pState;
	
	if ( nDtmfKey == '0' ) return FwdCaller( "96031" );
	
	CString strFile;
	CSettings::Item* pItem = Settings.GetSetting( ".Path" );
	strFile.Format( _T("%s\\%s\\profit.rcvoper%c.pcm"), *pItem->m_pString, FT_PROMPT, nDtmfKey );
	if ( PlayNavFile( RemoteId, strFile, TRUE ) ) return &RcvFnOper;
	
	int nDtmfLen = _tcslen( (LPTSTR)BufferPtr );
	*((LPTSTR)BufferPtr+nDtmfLen++) = nDtmfKey;
	
	SsmClearRxDtmfBuf( evtblk->nReference );
	RcvFnOper.process = (PROCESS)RcvFnOperX_cmplt;
	Reserved |= 0x08; return &RcvFnOper;
}

struct state* CProfitChannel::RcvFnOperX_cmplt(EVTBLK *evtblk)
{
	CHAR nDtmfKey = '\0';
	struct state* pState;
	
	pState = ShRcvFnOper_cmplt( evtblk, &nDtmfKey );
	if ( pState != &RcvFnOper || ! nDtmfKey ) return pState;
	
	if ( nDtmfKey == '0' ) return FwdCaller( "96031" );
	
	int nDtmfLen = _tcslen( (LPTSTR)BufferPtr );
	*((LPTSTR)BufferPtr+nDtmfLen++) = nDtmfKey;
	
	CString strQuery;
	
	strQuery.Format( "select * from xadver_marry"
					" where xkey = '%s'", (LPTSTR)BufferPtr );
	TRACE( strQuery + "\n" );
	
	xOpenRecordset(m_pRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
	if ( m_pRecordset->adoEOF ) return &RcvFnOper;
	
	RcvFnOper.process = (PROCESS)RcvFnOper_cmplt;
	PlyUlWord.initiate = (INITIATE)PlyUlWordX_init;
	PlyUlWord.process = (PROCESS)PlyUlWordX_cmplt;
	
	return RedexRecordset( &PlyUlWord );
}

// 播放节目
int CProfitChannel::PlyUlWord_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	FieldPtr xPlyBlob = m_pRecordset->GetFields()->GetItem("xvoice");
	return PlayBlob( nChan, xPlyBlob, TRUE );
}

struct state* CProfitChannel::PlyUlWord_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		m_pRecordset->MoveNext();
#if 0
		if ( m_pRecordset->adoEOF ) m_pRecordset->MoveFirst();
#endif
		Reserved &= 0x07; return RedexRecordset( &PlyUlWord );
	}
	
	if ( evtblk->wEventCode == E_CHG_RcvDTMF && '#' == (CHAR)(evtblk->dwParam & 0xFFFF) )
	{
		Reserved &= 0x00; return RcvLvWord_cmplt( evtblk );
	}
	
	if ( evtblk->wEventCode == E_SYS_TIMEOUT )
	{
		DWORD tNow = GetTickCount();
		DWORD nLength = (tNow - m_tConnected) / 1000;
		
		if ( (nLength % 60) >= (DWORD)Settings.Profit.MinGivSec )
		{
			long nTimeSpan = nLength / 60 + 1;
			nTimeSpan = min( Settings.Profit.MaxGivLong, nTimeSpan );
			m_pRecordbst->PutCollect( "xaddtime", _variant_t(nTimeSpan) );
			m_pRecordbst->Update();
			
			CSettings::Item* pItem = Settings.GetSetting( "Profit.MinGivSec" );
			SetTimer( *pItem->m_pDword * 1000 );
		}
	}
	
	return ChkDTSignal( evtblk, &PlyUlWord );
}

int CProfitChannel::PlyUlWordX_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	return PlayFile( FILE_PROT_PLYULWORD, FT_PROMPT, FALSE );
}

struct state* CProfitChannel::PlyUlWordX_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		PlyUlWord.initiate = (INITIATE)PlyUlWord_init;
		PlyUlWord.process = (PROCESS)PlyUlWord_cmplt;
		
		m_tConnected = GetTickCount();
		CSettings::Item* pItem = Settings.GetSetting( "Profit.MinGivSec" );
		SetTimer( *pItem->m_pDword * 1000 );
		Reserved &= 0x07; return &PlyUlWord;
	}
	
	return ChkDTSignal( evtblk, &PlyUlWord );
}

/////////////////////////////////////////////////////////////////////////////
// CProfitChannel play lvw's id function

// 会员对话
/*
int CProfitChannel::RcvLvWord_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;

	Reserved |= 0x08;	// 置状态“正在”
	m_pOwner->SetItem( nChan, "会员搜索", DS_PLAY );
	
#if 0
	return PlayFile( FILE_PROMPT_QUERY, FT_PROMPT, FALSE );
#else
	return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
#endif
}
*/
struct state* CProfitChannel::RcvLvWord_cmplt(EVTBLK *evtblk)
{
//	if ( evtblk->wEventCode != E_PROC_PlayEnd )
//	return ChkDTSignal( evtblk, &RcvLvWord );
	
	_variant_t TheValue = m_pRecordset->GetCollect( "xphone" );
	CString xRemoteId = (LPCTSTR)(_bstr_t)TheValue;
	
	CSettings::Item* pItem = Settings.GetSetting( _T("Trunk.TrunkFind") );
	CChannel* pChannel = Network.LookOutPhs( xRemoteId, *pItem->m_pString );
	if ( pChannel == NULL ) return ShExitCode_cmplt( evtblk, EXIT_BUSY );
	
	CDialPacket* pPacket = new CDialPacket; pPacket->SetLocalId( "96031888" );
	pPacket->SetRemoteId( xRemoteId ); pPacket->SetFeeNumber( RemoteId );
	pPacket->SetCallerId( RemoteId );
	
	pChannel->OnDialPacket( pPacket ); pPacket->Release();
	SetLinker( pChannel->m_nChan );
	
	RcvTalk.initiate = (INITIATE)PlyLvWord_init;
	Reserved &= 0x00; return &RcvTalk;
}

// 会员沟通
int CProfitChannel::PlyLvWord_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;

	Reserved |= 0x08; return 0;
}
