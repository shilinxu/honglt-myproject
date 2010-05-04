//
// ChannelCorpDx.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Network.h"
#include "IPacket.h"
#include "ChannelCorpDx.h"

#include "CtrlNetworkChild.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CCorpChannelDx construction

CCorpChannelDx::CCorpChannelDx(int nChan, LPCTSTR xFeeCode) : CChannel( nChan, xFeeCode )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	WaitDial.initiate		= (INITIATE)WaitDial_init;
	WaitDial.process		= (PROCESS)WaitDial_cmplt;
	RcvTalk.initiate		= (INITIATE)RcvTalk_init;
//	RcvTalk.process			= (PROCESS)RcvTalk_cmplt;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	ExitPlay.initiate		= (INITIATE)ExitPlay_init;
	
	PlyLvWord.initiate		= (INITIATE)PlyLvWord_init;
	PlyLvWord.process		= (PROCESS)PlyLvWord_cmplt;
	
	m_bPending				= FALSE;
	
	CCorpChannelDx::Setup();
}

CCorpChannelDx::~CCorpChannelDx()
{
	if ( m_bPending == TRUE ) UpdateState( "UNDELIV" );
}

//////////////////////////////////////////////////////////////////////
// CCorpChannelDx attributes

void CCorpChannelDx::Setup()
{
	CChannel::Setup();
	
	fcnp = &FeeLogin;
}

void CCorpChannelDx::KillLinker()
{
}

void CCorpChannelDx::UpdateState(LPCTSTR xState)
{
	CString	strQuery;
	
	strQuery.Format( "update xcorppager"
					" set xusrstate = '%s', xsendtime = getdate()"
					" where xjsjbh = %i", xState, m_nTaskId );
	TRACE( strQuery + "\n" );
	
	xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
}

void CCorpChannelDx::RecUsrBack(CHAR xKey)
{
	CString	strQuery;
	
	strQuery.Format( "update xcorppager"
					" set xusrback = '%c'"
					" where xjsjbh = %i", xKey, m_nTaskId );
	TRACE( strQuery + "\n" );
	
	xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
}

void CCorpChannelDx::OnRecdPacket(IRecdPacket* pPacket)
{
	if ( ! _tcscmp(pPacket->PakParam, "STOP") )
	{
		CChannel::OnRecdPacket( pPacket ); return;
	}
		
	//CString xNewFile = Settings.Meeting.RecFile;
	CString xNewFile = "<DATE>\\<REMOTE>-<TIME>.PCM";
	
	if ( xNewFile.Find( "<DATE>" ) >= 0 )
	{
		SYSTEMTIME pTime;
		GetLocalTime( &pTime );
		
		CString strValue;
		strValue.Format( "%04i年%02i月%02i日",
			pTime.wYear, pTime.wMonth, pTime.wDay );
		Replace( xNewFile, "<DATE>", strValue );
	}
	
	if ( xNewFile.Find( "<TIME>" ) >= 0 )
	{
		SYSTEMTIME pTime;
		GetLocalTime( &pTime );
		
		CString strValue;
		strValue.Format( "%02i时%02i分%02i秒%03i",
			pTime.wHour, pTime.wMinute, pTime.wSecond, pTime.wMilliseconds );
		Replace( xNewFile, "<TIME>", strValue );
	}
	
	if ( xNewFile.Find( "<REMOTE>" ) >= 0 )
	{
		CString strValue;
		Replace( xNewFile, "<REMOTE>", RemoteId );
	}
	
	for ( LPCTSTR xPath = xNewFile, xPathExt = NULL;
		  xPathExt = _tcsistr(xPath, "\\"); xPath = xPathExt + 1 )
	{
		CString strValue = xNewFile.Left(xPathExt - (LPCTSTR)xNewFile);
		CreateDirectory( "中国电信\\" + strValue, NULL );
	}
	
	SsmSetRecMixer( m_nChan, TRUE, 0 );
	if ( RecordFile(xNewFile, "中国电信") )
	{
		CHAR sError[1024];
		SsmGetLastErrMsg( sError ); throw sError;
	}
	
	theApp.Message( MSG_TEMP,"Record[%i] -> %s", m_nChan, xNewFile );
}

//////////////////////////////////////////////////////////////////////
// CCorpChannelDx OVERLOAD function

int CCorpChannelDx::bkIdle_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	CChannel* pChannel = new CChannel( nChan );
	pChannel->Clone( this ); Network.SetChannel( pChannel );

	return (pChannel->*(pChannel->fcnp)->initiate)(evtblk);
}

struct state* CCorpChannelDx::FeeLogin_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_FEE_Login &&
		(evtblk->dwParam & 0xFFFF) == S_FEE_Connected )
	{
		KillTimer(); Reserved &= 0; return &WaitDial;
	}
	
	if ( evtblk->wEventCode == E_FEE_Login &&
		(evtblk->dwParam & 0xFFFF) == S_FEE_Disconnected )
	{
		KillTimer(); Reserved &= 0; return &bkOffHk;
	}
	
	return CChannel::FeeLogin_cmplt(evtblk);
}

int CCorpChannelDx::WaitDial_init(EVTBLK *evtblk)
{
	if ( ! CChannel::WaitDial_init(evtblk) ) return 0;
	
	int nChan = evtblk->nReference;
	CHAR sError[1024]; SsmGetLastErrMsg( sError );
	theApp.Message( MSG_ERROR, "error on channel %d: %s", nChan, sError );
	
	//UpdateState( "UNKNOWN" ); m_bPending = FALSE;
	fcnp = &bkOffHk; Reserved &= 0;
	
	return (this->*fcnp->initiate)(evtblk);
}

struct state* CCorpChannelDx::WaitDial_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;

	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_TALKING )
		{
			UpdateState( "DELIVRD" ); m_bPending = FALSE;
		}
	}
	if ( evtblk->wEventCode == E_PROC_AutoDial )
	{
		switch (evtblk->dwParam & 0xFFFF)
		{
		case	DIAL_DIALING:
			UpdateState( "PENDING" ); m_bPending = TRUE;
			break;

		case	DIAL_VOICE:
			UpdateState( "DELIVRD" ); m_bPending = FALSE;
			break;

		case	DIAL_INVALID_PHONUM:
			UpdateState( "REJECTD" ); m_bPending = FALSE;
			break;

		case	DIAL_NO_DIALTONE:
		case	DIAL_FAILURE:
		//	UpdateState( "UNKNOWN" ); m_bPending = FALSE;
		//	break;
			
		case	DIAL_BUSYTONE:
		case	DIAL_NOANSWER:
			UpdateState( "UNDELIV" ); m_bPending = FALSE;
		}
	}
/*	
	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_TALKING )
		{
			KillTimer(); Reserved &= 0; return &RcvTalk;
		}
	}
*/
	return CChannel::WaitDial_cmplt(evtblk);
}

int CCorpChannelDx::RcvTalk_init(EVTBLK *evtblk)
{
	CChannel::RcvTalk_init(evtblk);
	
	IRecdPacket* pPacket = new IRecdPacket(m_nChan);
	OnRecdPacket( pPacket ); pPacket->Release();
	
	fcnp = &PlyLvWord; Reserved = 0;
	return (this->*fcnp->initiate)(evtblk);
}

/////////////////////////////////////////////////////////////////////////////
// CCorpChannelDx pager function

int CCorpChannelDx::PlyLvWord_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	m_pOwner->SetItem( nChan, "正在送信", DS_PLAY );
	
	SsmEnableRxDtmf( nChan, FALSE );
	return PlayFile( FILE_PAGE_PLYWORD, "中国电信", FALSE );
}

struct state* CCorpChannelDx::PlyLvWord_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		SsmEnableRxDtmf( nChan, TRUE );
		SetTimer( 10000 );
	}
	
	if ( evtblk->wEventCode == E_SYS_TIMEOUT )
	{
		return &bkOffHk;
	}
	
	if ( evtblk->wEventCode == E_CHG_RcvDTMF &&
		'1' == (CHAR)(evtblk->dwParam & 0xFFFF) )
	{
	RecUsrBack('1');
		KillTimer(); Reserved=0; return &ExitPlay;
	}
	
	return ChkDTSignal( evtblk, &PlyLvWord );
}

int CCorpChannelDx::ExitPlay_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	m_pOwner->SetItem( nChan, "谢谢使用", DS_PLAY );
	
	return PlayFile( FILE_PAGE_GOODBYE, "中国电信", FALSE );
}
