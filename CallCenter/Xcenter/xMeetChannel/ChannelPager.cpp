//
// ChannelPager.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "ChannelPager.h"

#include "ChannelMeet.h"
#include "Network.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CPagerChannel construction

CPagerChannel::CPagerChannel(int nChan, WORD nRoomId) : CChannel( nChan, "2555" ), RoomNumber( nRoomId )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	bkOffHk.initiate		= (INITIATE)bkOffHk_init;
//	FeeLogin.initiate		= (INITIATE)FeeLogin_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	WaitDial.process		= (PROCESS)WaitDial_cmplt;
	RcvTalk.initiate		= (INITIATE)RcvTalk_init;
	
	MsgMember.initiate		= (INITIATE)MsgMember_init;
	MsgMember.process		= (PROCESS)MsgMember_cmplt;
	JoinRoom.initiate		= (INITIATE)JoinRoom_init;
	JoinRoom.process		= (PROCESS)JoinRoom_cmplt;
	
	RcvTalk					= MsgMember;
	
	CPagerChannel::Setup();
}

CPagerChannel::~CPagerChannel()
{
}

//////////////////////////////////////////////////////////////////////
// CPagerChannel attributes

void CPagerChannel::Setup()
{
	CChannel::Setup();
	fcnp = &FeeLogin;
}
/*
int CPagerChannel::PlayMemberJoined(int nMmbrId)
{
	CHAR xDigital[16];
	char szFileName[MAX_PATH];
	
	SsmClearFileList( m_nChan );
	
	_itot( nMmbrId, xDigital, 10 );
	CompDigital( xDigital, TRUE );
	
	sprintf( szFileName, "%s\\%s", FT_PROMPT, FILE_MEET_MEMBSIT );
	SsmAddToFileList( m_nChan, szFileName, 6, 0, 0xFFFFFFFF );
	MLOG( szFileName );
	
	return SsmPlayFileList( m_nChan );
}

int CPagerChannel::PlayMemberExited(int nMmbrId)
{
	CHAR xDigital[16];
	char szFileName[MAX_PATH];
	
	SsmClearFileList( m_nChan );
	
	_itot( nMmbrId, xDigital, 10 );
	CompDigital( xDigital, TRUE );
	
	sprintf( szFileName, "%s\\%s", FT_PROMPT, FILE_MEET_MEMBEXT );
	SsmAddToFileList( m_nChan, szFileName, 6, 0, 0xFFFFFFFF );
	MLOG( szFileName );
	
	return SsmPlayFileList( m_nChan );
}

int CPagerChannel::PlayMemberSpeaker(int nRoomId, int nChan)
{
	int nMmbrId = -1; DWORD nData = 0;
	SsmGetConfChInfo( nChan, (PINT)&nData, &nMmbrId, (PWORD)&nData, (PWORD)&nData, &nData );
	
	if ( CMeetChannel::GetMemberIndex( nRoomId, 0, (PINT)&nData ) != -1 )
	SsmPutUserEvent( E_TAK_Connect, nData, MAKEVENT(nMmbrId & 0xFFFF, S_TAK_Speaker) );
	
	return 0;
}
*/
//////////////////////////////////////////////////////////////////////
// CPagerChannel operations

CChannel* CPagerChannel::PagerFrom(CChannel* pParent, LPCTSTR xRemoteId, LPCTSTR xFeeNumber, DWORD nRoomId)
{
	CChannel* pChannel = new CPagerChannel( pParent->m_nChan, (WORD)nRoomId );
	pChannel->Clone( pParent ); Network.SetChannel( pChannel );
	
	CDialPacket* pPacket = new CDialPacket;
	pPacket->SetLocalId( "96031555" ); pPacket->SetRemoteId( xRemoteId );
	pPacket->SetCallerId( "96031555" ); pPacket->SetFeeNumber( xFeeNumber );
	
	pChannel->OnDialPacket( pPacket ); pPacket->Release();
	
	return pChannel;
}

//////////////////////////////////////////////////////////////////////
// CPagerChannel TCP Transfer func

BOOL CPagerChannel::OnBindPacket(CBindPacket* pPacket)
{
	CTransfer::OnConnected();
	CString xFeeCode;
	
	int nLength = pPacket->GetFeeCode( xFeeCode.GetBuffer(4) );
	xFeeCode.ReleaseBuffer( nLength );
	
	CChannel* pChannel = this;
	if ( ! xFeeCode.CompareNoCase( this->m_xFeeCode ) )
	{
		if ( _tcslen(pChannel->FeeNumber) )
		theApp.Message( MSG_DEFAULT, "Rcv[%i][%s]: Local=[%s], Remote=[%s], Fee=[%s]",
			pChannel->m_nChan, pChannel->m_xFeeCode, pChannel->LocalId, pChannel->RemoteId, pChannel->FeeNumber );
		else
		theApp.Message( MSG_DEFAULT, "Rcv[%i][%s]: Local=[%s], Remote=[%s]",
			pChannel->m_nChan, pChannel->m_xFeeCode, pChannel->LocalId, pChannel->RemoteId );
		
		SsmPutUserEvent( E_FEE_Login, pChannel->m_nChan, S_FEE_Connected );
	}
	else
	{
		theApp.Message( MSG_ERROR, "N/A[%i][%s]: Local=[%s], Remote=[%s], Fee=[%s]",
			m_nChan, xFeeCode, LocalId, RemoteId, FeeNumber );
		
		SsmPutUserEvent( E_FEE_Login, m_nChan,
			MAKEVENT(ECore_Unknown & 0xFFFF, S_FEE_Disconnected) );
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CPagerChannel OVERLOAD function

int CPagerChannel::bkIdle_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	(this->*bkOffHk.initiate)(evtblk);
	
	CChannel* pChannel = new CChannel( nChan );
	pChannel->Clone( this ); Network.SetChannel( pChannel );

	return (pChannel->*(pChannel->fcnp)->initiate)(evtblk);
}

int CPagerChannel::bkOffHk_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	int nMmbrId = -1; DWORD nData = 0;
	SsmGetConfChInfo( nChan, (PINT)&nData, &nMmbrId, (PWORD)&nData, (PWORD)&nData, &nData );
	
	if ( SsmExitConfGroup( nChan, TRUE ) ) 	;//return CChannel::bkOffHk_init(evtblk);
	
	if ( CMeetChannel::GetMemberIndex( RoomNumber, 0, (PINT)&nData ) != -1 )
	SsmPutUserEvent( E_TAK_Connect, nData, MAKEVENT(nMmbrId & 0xFFFF, S_TAK_Disconnected) );
	
	CString	strQuery;
	
	strQuery.Format( "delete xmeetingroom"
					" where xmeetingroomcode = '%i'"
					" and xmeetingphone = '%s'", RoomNumber, RemoteId );
	TRACE( strQuery + "\n" );
	
	xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
	
	return CChannel::bkOffHk_init(evtblk);
}

int CPagerChannel::FeeLogin_init(EVTBLK *evtblk)
{
	return 0;
}

struct state* CPagerChannel::FeeLogin_cmplt(EVTBLK *evtblk)
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
		if ( SsmGetChState(nChan) == S_CALL_PICKUPED )	// For CallOut
		{
			KillTimer(); Reserved &= 0; return &WaitDial;
		}
	}
	
	return CChannel::FeeLogin_cmplt(evtblk);
}

/////////////////////////////////////////////////////////////////////////////
// CPagerChannel Play function

int CPagerChannel::MsgMember_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;

	ASSERT( RoomNumber != -1 );
	CChannel::RcvTalk_init(evtblk);
	
	CString	strQuery;
	
	strQuery.Format( "update xmeetingroom"
					" set xmeetingphonestate = '2'"
					" where xmeetingroomcode = '%i'"
					" and xmeetingphone = '%s'", RoomNumber, RemoteId );
	TRACE( strQuery + "\n" );
	
	xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
	
	Reserved |= 0x08;	// ÖÃ×´Ì¬¡°ÕýÔÚ¡±
	return PlayFile( FILE_MEET_PLYUTAK, FT_PROMPT, FALSE );
}

struct state* CPagerChannel::MsgMember_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		int nMmbrId = -1;
		DWORD nData = 0;
		
		SsmJoinConfGroup( RoomNumber, nChan, 1,	-1, TRUE, TRUE );
		SsmGetConfChInfo( nChan, (PINT)&nData, &nMmbrId, (PWORD)&nData, (PWORD)&nData, &nData );
		
		SsmPutUserEvent( E_TAK_Connect, nChan, MAKEVENT(nMmbrId & 0xFFFF, S_TAK_Connected) );
	
		Reserved = 0; return &JoinRoom;
	}
	
	return ChkDTSignal( evtblk, &MsgMember );
}

int CPagerChannel::JoinRoom_init(EVTBLK *evtblk)
{
	return 0;
}

struct state* CPagerChannel::JoinRoom_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_TAK_Connect )
	{
		if ( (evtblk->dwParam & 0xFFFF) == S_TAK_Connected )
		{
			CMeetChannel::PlayMemberJoined( this, evtblk->dwParam >> 16 );
			CMeetChannel::PlayMemberSpeaker( RoomNumber, nChan );
		}
		if ( (evtblk->dwParam & 0xFFFF) == S_TAK_Disconnected )
		{
			CMeetChannel::PlayMemberExited( this, evtblk->dwParam >> 16 );
			CMeetChannel::PlayMemberSpeaker( RoomNumber, nChan );
		}
		
		if ( (evtblk->dwParam & 0xFFFF) == S_TAK_Speaker )
		{
			CMeetChannel::PrefixIPSpeaker( FeeNumber, nChan );
		}
	}
	
	return ChkDTSignal( evtblk, &JoinRoom );
}
