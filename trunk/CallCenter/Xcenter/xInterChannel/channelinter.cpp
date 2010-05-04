//
// ChannelInter.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Network.h"
#include "Profiles.h"
#include "ChannelInter.h"

#include "Neighbour.h"
#include "Circuitry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CInterChannel construction

CInterChannel::CInterChannel(int nChan, LPCTSTR xFeeCode) : CChannel( nChan, xFeeCode )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	FeeLogin.initiate		= (INITIATE)FeeLogin_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	rcvMFsig.initiate		= (INITIATE)rcvMFsig_init;
	RcvTalk.initiate		= (INITIATE)RcvTalk_init;
	RcvTalk.process			= (PROCESS)RcvTalk_cmplt;

	FwdPlay.initiate		= (INITIATE)FwdPlay_init;
	FwdPlay.process			= (PROCESS)FwdPlay_cmplt;
	FindUser.initiate		= (INITIATE)FindUser_init;
	FindUser.process		= (PROCESS)FindUser_cmplt;
	
	m_fnKiller		= NULL;
	CInterChannel::Setup();
}

CInterChannel::~CInterChannel()
{
}

//////////////////////////////////////////////////////////////////////
// CInterChannel attributes

void CInterChannel::Setup()
{
	CChannel::Setup();
	fcnp = &FeeLogin;
}

void CInterChannel::KillLinker()
{
	if ( m_fnKiller ) {(this->*m_fnKiller)(); return;}
	
	if ( m_nTaskId == -1 ) return;
	
	SsmPutUserEvent( E_CHG_ChState, m_nTaskId,
		MAKEVENT(m_nChan & 0xFFFF, S_CALL_PENDING) );
	
	CChannel::KillLinker();
}

CChannel* CInterChannel::ConvertFrom(CChannel* pParent, LPCTSTR xString)
{
	CopyMemory(pParent->LocalId, xString, MAX_PHONE);
	CopyMemory(pParent->CallType, CA_TALK, MAX_CALLTYPE );
	
	pParent->OnConnected(); return NULL;
}

BOOL CInterChannel::PutUserRing(LPCTSTR xLocalId, LPCTSTR xRemoteId)
{
	int nChan = Profiles.FindInter( xLocalId );
	
	if ( nChan == -1 || SsmGetChState(nChan) != S_CALL_STANDBY ) return FALSE;
	
	SsmSetPlayDest( nChan, 1 );	// 上总线，阻止被SsmTalkWith中断放音
	
	SsmPutUserEvent( E_CHG_ChState, m_nTaskId = nChan,
		MAKEVENT(m_nChan & 0xFFFF, S_CALL_RINGING) );
	
	return TRUE;
}

BOOL CInterChannel::PutNetworkRing(LPCTSTR xLocalId, LPCTSTR xRemoteId)
{
	CString strQuery;
	
	strQuery.Format( "select * from xoperator"
					" where xentercode in (select xentercode from xenterprise"
					" where left('%s',len(xphone)) = xphone)", xLocalId );
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr xRecordset;
	xRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	
	for ( ; ! xRecordset->adoEOF; xRecordset->MoveNext() )
	{
		CSingleLock pLock( &Neighbour.m_pSection, TRUE );
		
		_variant_t TheValue = xRecordset->GetCollect( "xcode" );
		
		CCircuitry* pChannel = Neighbour.FindChannel( (LPCTSTR)(_bstr_t)TheValue );
		if ( ! pChannel || ! pChannel->IsConnected() ) continue;
		
		CPacket* pPacket = new CRingPacket(xLocalId, xRemoteId, m_nChan);
		pChannel->OnPacket( pPacket ); pPacket->Release();
		
		m_nTaskId = (DWORD)pChannel;
		m_fnKiller= KillNetwork;
		return TRUE;
	}
	
	return FALSE;
}

void CInterChannel::KillNetwork()
{
	ASSERT( m_fnKiller && m_nTaskId );
	
	CSingleLock pLock( &Neighbour.m_pSection, TRUE );
	
	CCircuitry* pChannel = (CCircuitry*)m_nTaskId;
	pChannel->m_nTaskId = -1 ; pChannel->Reset();
	
	m_fnKiller	= NULL;
	m_nTaskId	= -1;
}

//////////////////////////////////////////////////////////////////////
// CInterChannel OVERLOAD function

int CInterChannel::bkIdle_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	KillLinker();
	SsmSetPlayDest( nChan, 0 );	// 取消上总线
	
	CChannel* pChannel = new CChannel( nChan );
	pChannel->Clone( this ); Network.SetChannel( pChannel );

	return (pChannel->*(pChannel->fcnp)->initiate)(evtblk);
}

int CInterChannel::FeeLogin_init(EVTBLK *evtblk)
{
	return 0;
}

struct state* CInterChannel::FeeLogin_cmplt(EVTBLK *evtblk)
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
	}
	
	return CChannel::FeeLogin_cmplt(evtblk);
}

int CInterChannel::rcvMFsig_init(EVTBLK *evtblk)
{
	Reserved &= 0; fcnp = &FwdPlay;
	return (this->*fcnp->initiate)(evtblk);
}

/////////////////////////////////////////////////////////////////////////////
// CInterChannel caller function

// 语音接入
int CInterChannel::FwdPlay_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	CString strFile = FILE_INTR_WELCOME;
	strFile += '.'; strFile += m_xFeeCode;
	
	if ( PlayFile( strFile, FT_PROMPT, TRUE ) && PlayFile( FILE_INTR_WELCOME, FT_PROMPT, TRUE ) )
		return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	return 0;
}

struct state* CInterChannel::FwdPlay_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved = 0; return &FindUser;
	}
	
	return ChkDTSignal( evtblk, &FwdPlay );
}

int CInterChannel::FindUser_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	if ( ! PutUserRing(LocalId, RemoteId) && ! PutNetworkRing(LocalId, RemoteId) )
	{
		fcnp = &bkOffHk; Reserved = 0;
		return (this->*fcnp->initiate)(evtblk);
	}
	
	return PlayFile( FILE_INTR_FINDUSR, FT_PROMPT, FALSE );
}

struct state* CInterChannel::FindUser_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;

	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
	//	Reserved &= 0; return &bkOffHk;
	}
	
	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		// 消息来自TalkWith
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_TALKING )
		{
			SsmPickup( nChan ); m_nTaskId = -1;	// 跳过KillLinker
			SetLinker( evtblk->dwParam >> 16 );
			Reserved &= 0; return &RcvTalk;
		}
	}
	
	return ChkDTSignal( evtblk, &FindUser );
}

int CInterChannel::RcvTalk_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	CChannel::RcvTalk_init(evtblk);
	
	Reserved |= 0x08;	// 置状态“正在”
	return SsmStopPlayFile( nChan );
}

struct state* CInterChannel::RcvTalk_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_PENDING )
		{
			KillLinker(); return &bkOffHk;
		}
	}
	
	return CChannel::RcvTalk_cmplt(evtblk);
}
