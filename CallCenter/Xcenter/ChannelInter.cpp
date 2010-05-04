//
// ChannelInter.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Settings.h"
#include "Network.h"
#include "profiles.h"

#include "ChannelInter.h"

#include "CtrlNetworkChild.h"

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
	bkOffHk.initiate		= (INITIATE)bkOffHk_init;
	rcvMFsig.initiate		= (INITIATE)rcvMFsig_init;
	RcvTalk.initiate		= (INITIATE)RcvTalk_init;
	RcvTalk.process			= (PROCESS)RcvTalk_cmplt;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	WaitDial.process		= (PROCESS)WaitDial_cmplt;
	
	FindUser.initiate		= (INITIATE)FindUser_init;
	FindUser.process		= (PROCESS)FindUser_cmplt;
	
	CInterChannel::Setup();
}

CInterChannel::~CInterChannel()
{
	KillLinker();
}

//////////////////////////////////////////////////////////////////////
// CInterChannel attributes

void CInterChannel::Setup()
{
	CChannel::Setup();
	fcnp = &FeeLogin;
}

void CInterChannel::SetLinker(int nChan)
{
	KillLinker();

	if ( SsmTalkWithEx(m_nChan, 1, nChan, 1) )
	{
		CHAR sError[1024];
		SsmGetLastErrMsg( sError );
		theApp.Message( MSG_ERROR, "SsmTalkWith:%i,%i ->%s", m_nChan,nChan,sError );
	}

	CChannel* pChannel = Network.FindChannel( nChan );
	if ( ! pChannel ) throw "Invalid channel";
	pChannel->m_nTaskId = m_nChan;
	m_nTaskId = nChan;

//#ifdef _DEBUG
	theApp.Message(MSG_DEBUG,"SetLinker: %i with %i", m_nChan, nChan);
//#endif
}

//////////////////////////////////////////////////////////////////////
// CInterChannel OVERLOAD function

int CInterChannel::bkIdle_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	SsmSetPlayDest( nChan, 0 );	// 取消上总线
	Disconnect(); StopChan();
	
	CChannel* pChannel = new CChannel( nChan );
	pChannel->Clone( this ); Network.SetChannel( pChannel );

	return (pChannel->*(pChannel->fcnp)->initiate)(evtblk);
}

int CInterChannel::bkOffHk_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	SsmPutUserEvent( E_CHG_ChState, m_nTaskId,
		MAKEVENT(nChan & 0xFFFF, S_CALL_PENDING) );
	
	KillLinker();
	
	return CChannel::bkOffHk_init(evtblk);
}

struct state* CInterChannel::FeeLogin_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_FEE_Login &&
		(evtblk->dwParam & 0xFFFF) == S_FEE_Connected )
	{
		if ( SsmGetChState(nChan) == S_CALL_PICKUPED )
		{
			Reserved &= 0; return &WaitDial;
		}
		if ( SsmGetChState(nChan) == S_CALL_TALKING )
		{
			SsmPutUserEvent( E_CHG_ChState, nChan, S_CALL_RINGING );
		}
#ifdef NPICKUP
		if ( SsmGetChState(nChan) == S_CALL_RINGING )
		{
theApp.Message( MSG_TEMP, "CInterChannel::NPICKUP" );
			SsmPutUserEvent( E_CHG_ChState, nChan, S_CALL_RINGING );
		}
#endif
	}
	
	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_PICKUPED )
		{
			return &bkOnHk;
		}
	}
	
	return CChannel::FeeLogin_cmplt(evtblk);
}

int CInterChannel::rcvMFsig_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( Reserved == 0 )
	{
		fcnp = &FindUser; Reserved = 0;
		return (this->*fcnp->initiate)(evtblk);
	}
	
	if ( SsmGetChState(nChan) == S_CALL_TALKING )
	return SsmPutUserEvent( E_CHG_ChState, nChan, S_CALL_TALKING );
	
#ifdef NPICKUP
theApp.Message( MSG_TEMP, "CInterChannel::NPICKUP" );
	return SsmPutUserEvent( E_CHG_ChState, nChan, S_CALL_TALKING );
#endif
	
	return CChannel::rcvMFsig_init(evtblk);
}

struct state* CInterChannel::WaitDial_cmplt(EVTBLK *evtblk)
{
	struct state* pState = CChannel::WaitDial_cmplt(evtblk);
	if ( pState == &RcvTalk ) return &FindUser;

	return pState;
}

/////////////////////////////////////////////////////////////////////////////
// CInterChannel caller function

int CInterChannel::FindUser_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	m_pOwner->SetItem( nChan, "呼叫坐席", DS_PLAY );
	
	VerifyPhoneNum( LocalId );
	
	int nChan2 = Profiles.FindInter( LocalId );
	if ( SsmGetChState(nChan2) != S_CALL_STANDBY )
	{
		fcnp = &bkOffHk; Reserved = 0;
		return (this->*fcnp->initiate)(evtblk);
	}
	
	SsmSetPlayDest( nChan, 1 );	// 上总线，阻止被SsmTalkWith中断放音
	int nError = PlayFile( FILE_CALLER_FINDUSR, FT_PROMPT, FALSE );
	if ( ! nError ) nError = SsmSetPlayTime( nChan, 10 * 1000 );
	
	m_nTaskId = nChan2;
	SsmPutUserEvent( E_CHG_ChState, nChan2,
			MAKEVENT(nChan & 0xFFFF, S_CALL_RINGING) );
	
	return nError;
}

struct state* CInterChannel::FindUser_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;

	if ( evtblk->wEventCode == E_PROC_PlayEnd ||
		 evtblk->wEventCode == E_SYS_TIMEOUT )
	{
		Reserved &= 0; return &bkOffHk;
	}
	
	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		// 消息来自TalkWith
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_TALKING )
		{
			SetLinker( evtblk->dwParam >> 16 );
			Reserved = 0x01; return &rcvMFsig;
			//Reserved = 0; return &RcvTalk;
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
	m_pOwner->SetItem( nChan, "正在对话", DS_PLAY );
	
	return SsmStopPlayFile( nChan );
}

struct state* CInterChannel::RcvTalk_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_PENDING )
		{
			SsmPutUserEvent( E_CHG_ChState, m_nTaskId,
					MAKEVENT(nChan & 0xFFFF, S_CALL_PENDING) );
			KillLinker(); return &bkOffHk;
		}
	}
	
	return CChannel::RcvTalk_cmplt(evtblk);
}
