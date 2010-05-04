//
// ChannelCaller.cpp
//

#include "stdafx.h"
#include "Xphone.h"
#include "ChannelCaller.h"
#include "PacketCore.h"

//////////////////////////////////////////////////////////////////////
// CCallerChannel construction

CCallerChannel::CCallerChannel(LPCTSTR xUserId)
{
	m_xUserId	 = _tcsdup( xUserId );
}

CCallerChannel::~CCallerChannel()
{
	free( m_xUserId ); m_xUserId = NULL;
}

//////////////////////////////////////////////////////////////////////
// CCallerChannel attributes

void CCallerChannel::Setup()
{
	CChannel::Setup();
}

void CCallerChannel::SetUserId(LPCTSTR xUserId)
{
	free( m_xUserId );
	
	m_xUserId = _tcsdup( xUserId );
}

BOOL CCallerChannel::IsNavigated(LPCTSTR xCallType) const
{
	return CChannel::IsNavigated() && IsCallType( xCallType );
}

BOOL CCallerChannel::ConnectTo(LPCTSTR xUserId, LPCTSTR xPassword)
{
	CLoginRemoteReq* pPacket = new CLoginRemoteReq( xUserId, xPassword );
	SendPacket( pPacket );
	
	SetUserId( xUserId );
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CCallerChannel socket event handlers

void CCallerChannel::OnHandshake()
{
	CChannel::OnHandshake();
}

//////////////////////////////////////////////////////////////////////
// CCallerChannel TCP Transfer func
