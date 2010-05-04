//
// ChannelUser.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Network.h"
#include "Profiles.h"
#include "Settings.h"
#include "XML.h"

#include "ChannelUser.h"
#include "ChannelUser2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CUserChannel construction

CUserChannel::CUserChannel(int nChan) : CChannel( nChan )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	bkIdle.process			= (PROCESS)bkIdle_cmplt;
	bkOffHk.initiate		= (INITIATE)bkOffHk_init;
	bkOnHk.initiate			= (INITIATE)bkOnHk_init;
	bkOnHk.process			= (PROCESS)bkOnHk_cmplt;
	rcvMFsig.initiate		= (INITIATE)rcvMFsig_init;
	rcvMFsig.process		= (PROCESS)rcvMFsig_cmplt;
	WaitDial.initiate		= (INITIATE)WaitDial_init;
	WaitDial.process		= (PROCESS)WaitDial_cmplt;
	RcvTalk.initiate		= (INITIATE)RcvTalk_init;
	RcvTalk.process			= (PROCESS)RcvTalk_cmplt;

	CUserChannel::Setup();
}

CUserChannel::~CUserChannel()
{
}

//////////////////////////////////////////////////////////////////////
// CUserChannel attributes

void CUserChannel::Setup()
{
	CChannel::Setup();
	
	SsmSetASDT( m_nChan, TRUE );
	WaitDial.initiate		= (INITIATE)WaitDial_init;
}

void CUserChannel::StopChan()
{
	CChannel::StopChan();
	
	int nChan = m_nChan;
	SsmStopRing( nChan );
	SsmStopSendTone( nChan );
}

void CUserChannel::AttachFrom(CChannel* pChannel)
{
	if ( ! _tcsncmp( pChannel->CallType, CA_HOOK, MAX_CALLTYPE ) )
	{
		CopyMemory( this->LocalId, pChannel->RemoteId, MAX_PHONE );
		CopyMemory( this->RemoteId, pChannel->LocalId, MAX_PHONE );
	}		
	else
	{
		CopyMemory( this->LocalId, pChannel->LocalId, MAX_PHONE );
		CopyMemory( this->RemoteId, pChannel->RemoteId, MAX_PHONE );
	}
	
	CopyMemory( this->FeeNumber, pChannel->FeeNumber, MAX_PHONE );
	CopyMemory( this->CallType, CA_TALK, MAX_CALLTYPE );
	
	pChannel->m_nTaskId = m_nChan;
	m_nTaskId = pChannel->m_nChan;
}

void CUserChannel::SetLinker(int nChan)
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

	theApp.Message(MSG_DEBUG,"SetLinker: %i with %i", m_nChan, nChan);
}

struct state* CUserChannel::ChkDTSignal(EVTBLK *evtblk,struct state* lpstDefaultState)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_CHG_ChState &&
		(evtblk->dwParam & 0xFFFF) == S_CALL_STANDBY )
	{
		SsmPutUserEvent( E_CHG_ChState, m_nTaskId,
			MAKEVENT(nChan & 0xFFFF, S_CALL_PENDING) );
		this->StopChan(); return &bkIdle;
	}
	
	return CChannel::ChkDTSignal(evtblk, lpstDefaultState);
}

BOOL CUserChannel::IsRecordChan(int nChan) const
{
	CString strValue = Settings.User.RecTrunk;
	if ( ! strValue.CompareNoCase( _T("任意") ) ) return TRUE;
	
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		INT nCh1 = -1, nCh2 = -1;
		if ( _stscanf( str, "%i-%i", &nCh1, &nCh2 ) == 2 )
		{
			if ( nChan >= nCh1 && nChan <= nCh2 ) return TRUE;
		}
		else
		{
			if ( nChan == nCh1 ) return TRUE;
		}
	}
	
	return FALSE;
}

int CUserChannel::ShExitCode_init(EVTBLK *evtblk, int nExitCode)
{
	ExitCode = nExitCode; fcnp = &ExitPlay;
	Reserved = 0; return (this->*fcnp->initiate)(evtblk);
}

int CUserChannel::ShDebugOut_init(EVTBLK *evtblk)
{
	CChannel* pChannel = Network.LookOut( Settings.Trunk.LookOut );
	if ( ! pChannel ) return ShExitCode_init( evtblk, EXIT_BUSY );
	
	CopyMemory( pChannel->LocalId, this->RemoteId, MAX_PHONE );
	CopyMemory( pChannel->RemoteId, this->LocalId, MAX_PHONE );
	CopyMemory( pChannel->CallType, CA_DEBG, MAX_CALLTYPE );
	
	SetLinker( pChannel->m_nChan ); return 0;
}

//////////////////////////////////////////////////////////////////////
// CUserChannel run function

BOOL CUserChannel::OnRun()
{
	if ( ! CChannel::OnRun() ) return FALSE;
	
	DWORD tNow = GetTickCount();
	
	if ( ! GetUserId() && tNow - m_tConnected >= 15* 1000 )
	{
		OnDropped( FALSE ); return FALSE;
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CUserChannel TCP Transfer func

BOOL CUserChannel::OnPacket(CPacket* pPacket)
{
#if 1
	if ( pPacket->IsCommandId( "HND" ) )
	{
		return OnBindPacket( (CBindPacket*)pPacket );
	}
#endif
	
	return CChannel::OnPacket( pPacket );
}

BOOL CUserChannel::OnBindPacket(CBindPacket* pPacket)
{
	ASSERT ( ! IsConnected() );
	
	CHAR xUserId[MAX_PHONE + 1];
	xUserId[ pPacket->GetUserId( xUserId ) ] = '\0';
	
	CUserChannel* pChannel = new CUserChannel2( m_nChan, xUserId );
	pChannel->Clone( this ); Network.SetChannel( pChannel );
	
	return pChannel->OnBindPacket( pPacket );
}

BOOL CUserChannel::OnDialPacket(CDialPacket* pPacket)
{
	CHAR xRemoteId[MAX_PHONE + 1];
	xRemoteId[ pPacket->GetRemoteId( xRemoteId ) ] = '\0';
	
	CChannel* pChannel = Network.LookOutPhs( xRemoteId );
	if ( pChannel == NULL ) return FALSE;
	
	LocalId[ pPacket->GetLocalId( LocalId ) ] = '\0';
	RemoteId[ pPacket->GetRemoteId( RemoteId ) ] = '\0';
	FeeNumber[ pPacket->GetFeeNumber( FeeNumber ) ] = '\0';
	
	if ( ! pPacket->GetLocalId(NULL) ) pPacket->SetLocalId( "057496031" );
	pChannel->OnDialPacket( pPacket );
	
	SsmStopSendTone( m_nChan );
	SetLinker( pChannel->m_nChan );
	
	if ( fcnp == &bkOnHk )
	{
		Reserved |= 0x08; fcnp = &WaitDial;
	}
	
	return TRUE;
}

BOOL CUserChannel::OnRecordPacket(CRecordPacket* pPacket)
{
	CChannel* pChannel = Network.FindChannel( pPacket->GetChannel() );
	if ( ! pChannel ) return SendPacket( new CErrPacket(ECore_Unknown) );
	
	pChannel->CChannel::OnRecordPacket( pPacket );
	return SendPacket( new COkPacket );
}

BOOL CUserChannel::OnListenPacket(CListenPacket* pPacket)
{
	CChannel* pChannel = Network.FindChannel( pPacket->GetChannel() );
	if ( ! pChannel ) return SendPacket( new CErrPacket(ECore_Unknown) );
	
	pChannel->OnListenPacket( pPacket );
	return SendPacket( new COkPacket );
}

BOOL CUserChannel::OnHaltPacket(CHaltPacket* pPacket)
{
	CChannel* pChannel = Network.FindChannel( pPacket->LinkId );
	if ( ! pChannel ) return SendPacket( new CErrPacket(ECore_Unknown) );
	
	pChannel->OnHaltPacket( pPacket );
	return SendPacket( new COkPacket );
}

/////////////////////////////////////////////////////////////////////////////
// CUserChannel OVERLOAD function

int CUserChannel::bkIdle_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	CChannel::StopChan(); Setup();
	return 0;
}

struct state* CUserChannel::bkIdle_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_RINGING )
		{
			return &rcvMFsig;
		}
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_PICKUPED )
		{
			return &bkOnHk;
		}
	}
	
	return &bkIdle;
}

int CUserChannel::bkOffHk_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	StopChan(); SsmStopSendTone( nChan);
	
	return SsmSendTone( nChan, 3 );
}

int CUserChannel::rcvMFsig_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	int nChFrom = evtblk->dwParam >> 16;
	CChannel* pChannel = Network.FindChannel( nChFrom );
	if ( ! pChannel ) throw "Invalid channel";
	
//	theApp.Message(MSG_TEMP, "debug(1): rcvMFsig_init [%i]:{%s}{%s} --> [%i]:{%s}{%s}", 
//		m_nChan, LocalId, RemoteId,
//		pChannel->m_nChan,pChannel->LocalId, pChannel->RemoteId);
	AttachFrom( pChannel );
//	theApp.Message(MSG_TEMP, "debug(2): rcvMFsig_init [%i]:{%s}{%s} --> [%i]:{%s}{%s}", 
//		m_nChan, LocalId, RemoteId,
//		pChannel->m_nChan,pChannel->LocalId, pChannel->RemoteId);
	
	SetTimer( Settings.User.MaxRing * 1000 );
	return SsmStartRingWithCIDStr(nChan, RemoteId, _tcslen(RemoteId), -1);
}

struct state* CUserChannel::rcvMFsig_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;

	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_PICKUPED )
		{
			SsmPutUserEvent( E_CHG_ChState, nChan, S_CALL_TALKING );
			KillTimer(); SsmStopRing( nChan ); return &rcvMFsig;
		}
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_PENDING )
		{
			KillTimer(); SsmStopRing( nChan );
			Reserved = 0; return &bkIdle;
		}
	}
	
	if ( evtblk->wEventCode == E_SYS_TIMEOUT )
	{
		SsmPutUserEvent( E_SYS_TIMEOUT, m_nTaskId, MAKEVENT(nChan & 0xFFFF, 0) );
		SsmStopRing( nChan ); return &bkIdle;
	}
	
	return CChannel::rcvMFsig_cmplt(evtblk);
}

// 摘机
int CUserChannel::bkOnHk_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	return 0;
}

struct state* CUserChannel::bkOnHk_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_CHG_RcvDTMF )
	{
		SsmStopSendTone( nChan ); KillTimer();
		return ShRcvFdTele_cmplt( evtblk, &WaitDial );
	}
	
	if ( evtblk->wEventCode == E_SYS_TIMEOUT )
	{
		SsmStopSendTone( nChan ); KillTimer();
		Reserved &= 0; return &WaitDial;
	}
	
	return ChkDTSignal( evtblk, &bkOnHk );
}

struct state* CUserChannel::ShRcvFdTele_cmplt(EVTBLK *evtblk, struct state* lpstEntryState)
{
	int nChan = evtblk->nReference;
	
	int nDtmfLen = _tcslen( RemoteId );
	CHAR cDtmfKey= (CHAR)(evtblk->dwParam & 0xFFFF);
	if ( '#' != cDtmfKey ) *(RemoteId+nDtmfLen++) = cDtmfKey;
	
	if ( '#' == cDtmfKey || nDtmfLen >= MAX_PHONE )
	{
		Reserved &= 0; return lpstEntryState;
	}
	
	if ( (Reserved & 0x01) == 0 &&
		 Profiles.FindCorpId(RemoteId, nChan) )
	{
		WaitDial.initiate = (INITIATE)WaitDialX_init;
		SsmClearRxDtmfBuf( nChan ); SsmSendTone(nChan, 0);
		CopyMemory( FeeNumber, RemoteId, MAX_PHONE );
		ZeroMemory( RemoteId, MAX_PHONE + 1);
		Reserved |= 0x01; return &bkOnHk;
	}
	
	SetTimer( 5 * 1000 ); return &bkOnHk;
}

// 拨内线
int CUserChannel::WaitDial_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	CXMLElement* pXML = Profiles.FindInter( nChan );
	if ( ! pXML ) return ShExitCode_init( evtblk, EXIT_BUSY );
	
	CString strValue;
	strValue = pXML->GetAttributeValue( "VNET" );
	ZeroMemory( LocalId, MAX_PHONE + 1);
	CopyMemory( LocalId, strValue.GetBuffer(0), strValue.GetLength() );
	
	int nOutCh = Profiles.FindInter(RemoteId, "VNET");
#if 0
	if ( nOutCh == -1 ) return ShExitCode_init( evtblk, EXIT_NOTUSER );
#else
	if ( nOutCh == -1 ) return ShDebugOut_init( evtblk );
#endif
	
	if ( SsmGetChState(nOutCh) != S_CALL_STANDBY )
	return SsmSendTone( nChan, 1 );	// 送忙
	
	SsmPutUserEvent( E_CHG_ChState, nOutCh, 
		MAKEVENT( nChan & 0xFFFF, S_CALL_RINGING) );
	
	// AttachFrom所需，置呼出属性
	CopyMemory( CallType, CA_HOOK, MAX_CALLTYPE );
	return SsmSendTone( nChan, 2 );	// 回铃
}

// 拨外线
int CUserChannel::WaitDialX_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	CXMLElement* pXML = Profiles.FindCorpId( FeeNumber, nChan );
	if ( ! pXML ) return ShExitCode_init( evtblk, EXIT_BUSY );
	
	if ( Profiles.IsDenied( pXML, RemoteId ) ) return ShExitCode_init( evtblk, EXIT_NOTUSER );
	
	CString strValue = pXML->GetAttributeValue( "LinkId" );
	
	CChannel* pChannel = Network.LookOutPcm( (LPCTSTR)strValue );
	if ( ! pChannel ) return ShExitCode_init( evtblk, EXIT_BUSY );
	
	strValue = pXML->GetAttributeValue( "Host" );
	CopyMemory( LocalId, strValue.GetBuffer(0), MAX_PHONE );
	
	CDialPacket* pPacket = new CDialPacket;
	pPacket->SetLocalId( LocalId );	pPacket->SetRemoteId( RemoteId );
	
	pChannel->OnDialPacket(pPacket); pPacket->Release();
	SetLinker( pChannel->m_nChan ); return 0;
}

struct state* CUserChannel::WaitDial_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_CHG_ChState &&
		(evtblk->dwParam & 0xFFFF) == S_CALL_STANDBY )
	{
		SsmPutUserEvent( E_CHG_ChState, m_nTaskId,
			MAKEVENT(nChan & 0xFFFF, S_CALL_PENDING) );
		CChannel::StopChan(); return &bkIdle;
	}
	
	return CChannel::WaitDial_cmplt( evtblk );
}

int CUserChannel::RcvTalk_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	SsmStopSendTone( nChan );
	this->SetLinker( m_nTaskId );
	//CChannel::RcvTalk_init(evtblk);
	
	CTransfer::OnConnected();
	Reserved |= 0x08;	// 置状态“正在”
	
	if ( Settings.User.RecType && IsRecordChan( nChan ) )
	{
		CString xRecFile = Settings.Trunk.RecFile;
		Settings.Trunk.RecFile = Settings.User.RecFile;
		
		CRecordPacket* pPacket = new CRecordPacket(m_nChan);
		OnRecordPacket( pPacket ); pPacket->Release();
		
		Settings.Trunk.RecFile = xRecFile;
	}
	
	return SsmPutUserEvent( E_CHG_ChState, m_nTaskId,
		MAKEVENT(nChan & 0xFFFF, S_CALL_TALKING) );
}

struct state* CUserChannel::RcvTalk_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;

	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_STANDBY )
		{
			SsmPutUserEvent( E_CHG_ChState, m_nTaskId,
				MAKEVENT(nChan & 0xFFFF, S_CALL_PENDING) );
			StopChan(); KillLinker(); return &bkIdle;
		}
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_PENDING )
		{
			KillLinker(); return &bkOffHk;
		}
	}

	return CChannel::RcvTalk_cmplt(evtblk);
}
