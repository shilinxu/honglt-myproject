//
// ChannelCaller.cpp
//

#include "stdafx.h"
#include "Network.h"
#include "Settings.h"
#include "ChannelCaller.h"

#include "Xcenter.h"
#include "WndMain.h"
#include "WndUser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CCallerChannel construction

CCallerChannel::CCallerChannel(int nChan, LPCTSTR xFeeCode) : CChannel( nChan, xFeeCode )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	FeeLogin.initiate		= (INITIATE)FeeLogin_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	rcvMFsig.initiate		= (INITIATE)rcvMFsig_init;
	WaitDial.process		= (PROCESS)WaitDial_cmplt;
	RcvTalk.initiate		= (INITIATE)RcvTalk_init;
	RcvTalk.process			= (PROCESS)RcvTalk_cmplt;
	
	FwdCaller.initiate		= (INITIATE)FwdCaller_init;
	FwdCaller.process		= (PROCESS)FwdCaller_cmplt;
	FindUser.initiate		= (INITIATE)FindUser_init;
	FindUser.process		= (PROCESS)FindUser_cmplt;
	
	ExitPlay.initiate		= (INITIATE)ExitPlay_init;
	
	m_pUserWnd		= NULL;
	CCallerChannel::Setup();
}

CCallerChannel::~CCallerChannel()
{
}

//////////////////////////////////////////////////////////////////////
// CCallerChannel attributes

void CCallerChannel::Setup()
{
	CChannel::Setup();
	fcnp = &FeeLogin;
	
	CMainWnd* pMainWnd	= (CMainWnd*)theApp.m_pMainWnd;
	ASSERT( pMainWnd->GetSafeHwnd() != NULL );

	m_pUserWnd			= (CUserWnd*)pMainWnd->GetWindow( RUNTIME_CLASS(CUserWnd) );
	ASSERT( m_pUserWnd->GetSafeHwnd() != NULL );
}

void CCallerChannel::KillLinker()
{
	if ( m_nTaskId != -1 )
	SsmPutUserEvent( E_CHG_ChState, m_nTaskId,
		MAKEVENT(m_nChan & 0xFFFF, S_CALL_PENDING) );
	
	CChannel::KillLinker();
}

void CCallerChannel::SetLinker(int nChan)
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

BOOL CCallerChannel::IsNotUserTime()
{
	CSettings::Item* pItem = Settings.GetSetting( _T("User.NoUser") );
	if ( *pItem->m_pDword == FALSE ) return FALSE;
	
	pItem = Settings.GetSetting( _T("User.UserTime") );
	CString strValue = * pItem->m_pString;
	
	CString str	= strValue.SpanExcluding( _T("-") );
	WORD nLTime = _ttoi( str );
	
	strValue	= strValue.Mid( str.GetLength() + 1 );
	WORD nHTime = _ttoi( strValue );
	
	CTime tNow = CTime::GetCurrentTime();
	WORD nNowTime = tNow.GetHour()* 100 + tNow.GetMinute();
	
	return nNowTime < nLTime || nNowTime > nHTime;
}

//////////////////////////////////////////////////////////////////////
// CCallerChannel OVERLOAD function

int CCallerChannel::bkIdle_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	SsmSetPlayDest( nChan, 0 );	// 取消上总线
	
	CChannel* pChannel = new CChannel( nChan );
	pChannel->Clone( this ); Network.SetChannel( pChannel );
	
	return (pChannel->*(pChannel->fcnp)->initiate)(evtblk);
}

int CCallerChannel::FeeLogin_init(EVTBLK *evtblk)
{
	return 0;
}

struct state* CCallerChannel::FeeLogin_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_TALKING )
		{
			KillTimer(); Reserved &= 0; return &RcvTalk;
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
		
#if 1	// 非坐席服务时段
		if ( IsNotUserTime() ) 
		{
			Reserved &= 0;
	#if 0
			return &bkOffHk;
	#else
			SsmPickup( nChan ); Sleep( 250 );
			ExitCode = EXIT_NOTUSER; return &ExitPlay;
	#endif
		}
#endif
	}
	
	return CChannel::FeeLogin_cmplt(evtblk);
}

int CCallerChannel::rcvMFsig_init(EVTBLK *evtblk)
{
	Reserved &= 0; fcnp = &FwdCaller;
	return (this->*fcnp->initiate)(evtblk);
}

struct state* CCallerChannel::WaitDial_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_TALKING )
		{
			RcvTalk_init( evtblk );
			return &rcvMFsig;
		}
	}
	
	return CChannel::WaitDial_cmplt(evtblk);
}

int CCallerChannel::ExitPlay_init(EVTBLK *evtblk)
{
	if ( ! Reserved && ExitCode == EXIT_NOTUSER )	// 不在坐席服务时间段
	{
		Reserved |= 0x08;	// 置状态“正在”
		return PlayFile( FILE_KINGB_NOTUSER, FT_PROMPT, FALSE );
	}
		
	return CChannel::ExitPlay_init(evtblk);	
}

/////////////////////////////////////////////////////////////////////////////
// CCallerChannel caller function

int CCallerChannel::FwdCaller_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	SsmPickup( nChan ); Sleep( 250 );
	
	return PlayFile( FILE_KINGB_WELCOME, FT_PROMPT, TRUE );
}

struct state* CCallerChannel::FwdCaller_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved &= 0; return &FindUser;
	}
	
	return ChkDTSignal( evtblk, &FwdCaller );
}

int CCallerChannel::FindUser_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	SsmPutUserEvent( E_SYS_TIMEOUT, nChan, 0 );
	SsmSetPlayDest( nChan, 1 );	// 上总线，阻止被SsmTalkWith中断放音
	
	return PlayFile( FILE_KINGB_FINDUSR, FT_PROMPT, FALSE );
}

struct state* CCallerChannel::FindUser_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;

	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved &= 0; return &FindUser; // 继续等待
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
	
	if ( evtblk->wEventCode == E_SYS_TIMEOUT )
	{
		if ( m_nTaskId != -1 ) KillLinker();
		
		if ( CChannel* pChannel = m_pUserWnd->ReadItem(LocalId, S_CALL_STANDBY) )
		{
			m_nTaskId = pChannel->m_nChan; pChannel->m_nTaskId = m_nChan;
			
			SsmPutUserEvent( E_CHG_ChState, pChannel->m_nChan,
				MAKEVENT(nChan & 0xFFFF, S_CALL_RINGING) ); return &FindUser;
		}
#if 0	// 坐席占线置忙
		else
		{
			CSettings::Item* pItem = Settings.GetSetting( _T("User.UserBusy") );
			if ( * pItem->m_pDword == TRUE )
			{
				SsmPickup( nChan ); Sleep( 250 );
				
				CChannel::StopChan();
				return &bkOffHk;
			}
		}
#endif
		
		SetTimer( 5 * 1000 ); return &FindUser;
	}
	
	return ChkDTSignal( evtblk, &FindUser );
}

int CCallerChannel::RcvTalk_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	CChannel::RcvTalk_init(evtblk);
	
	Reserved |= 0x08;	// 置状态“正在”
	return SsmStopPlayFile( nChan );
}

struct state* CCallerChannel::RcvTalk_cmplt(EVTBLK *evtblk)
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
