//
// ChannelUser.cpp
//

#include "stdafx.h"
#include "Xphone.h"
#include "ChannelUser.h"
#include "PacketCore.h"

//#include "RenderGraph.h"
#include "RTPPacket.h"

//////////////////////////////////////////////////////////////////////
// CUserChannel construction

CUserChannel::CUserChannel(LPCTSTR xUserId)
{
	m_xUserId	 = _tcsdup( xUserId );
}

CUserChannel::~CUserChannel()
{
	free( m_xUserId ); m_xUserId = NULL;
}

//////////////////////////////////////////////////////////////////////
// CUserChannel attributes

void CUserChannel::Setup()
{
	CChannel::Setup();
}

void CUserChannel::SetUserId(LPCTSTR xUserId)
{
	free( m_xUserId );
	
	m_xUserId = _tcsdup( xUserId );
}

BOOL CUserChannel::IsNavigated(LPCTSTR xCallType) const
{
	return CChannel::IsNavigated() && IsCallType( xCallType );
}

BOOL CUserChannel::ConnectTo(LPCTSTR xUserId, LPCTSTR xPassword)
{
	CLoginRemoteReq* pPacket = new CLoginRemoteReq( xUserId, xPassword );
	SendPacket( pPacket );
	
	SetUserId( xUserId );
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CUserChannel send packet

BOOL CUserChannel::SendPacket(CPacket* pPacket)
{
	return CChannel::SendPacket( pPacket );
}

BOOL CUserChannel::CallRemote(LPCTSTR xRemoteId, LPCTSTR xLocalId)
{
	ASSERT( IsConnected() );
	CDialPacket* pPacket = new CDialPacket;
	
	if ( xLocalId && _tcslen(xLocalId) )
		pPacket->SetLocalId( xLocalId );
	else
		pPacket->SetLocalId( "057496031" );
	
	pPacket->SetRemoteId( xRemoteId );
	
	SendPacket( pPacket );
	CopyMemory( m_xCallType, CA_HOOK, MAX_CALLTYPE );
	
	m_bNavigated = TRUE;
	m_tNavigated = GetTickCount();
	
	return TRUE;
}

BOOL CUserChannel::PickupRemote()
{
	ASSERT( IsConnected() );
	ASSERT( IsCallType( CA_TALK ) );
	
	SendPacket( new CTalkPacket );
	CopyMemory( m_xCallType, CA_TALK, MAX_CALLTYPE );
	
	m_bNavigated = TRUE;
	m_tNavigated = GetTickCount();
	
	return TRUE;
}

void CUserChannel::HangRemote()
{
	if ( ! CChannel::IsNavigated() ) return;
	SendPacket( new CByePacket );
	
	m_bNavigated = FALSE;
	m_tNavigated = GetTickCount();
}

/////////////////////////////////////////////////////////////////////////////
// CUserChannel deliver buffer

BOOL CUserChannel::DeliverBuffer(RTPPacket *pack, const RTPTime* receivetime)
{
	BYTE nPayload = pack->GetPayloadType();
	BYTE* pBuffer = pack->GetPayloadData();
	DWORD nBuffer = pack->GetPayloadLength();
	
//	RenderGraph.RenderBuffer( nPayload, pBuffer, nBuffer, pack->GetTimestamp() );
//	if ( nPayload != Payload_Audio ) return TRUE;
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CUserChannel socket event handlers

void CUserChannel::OnHandshake()
{
	SendPacket( new CSyncOrderRelation );
	CChannel::OnHandshake();
}

//////////////////////////////////////////////////////////////////////
// CUserChannel TCP Transfer func
