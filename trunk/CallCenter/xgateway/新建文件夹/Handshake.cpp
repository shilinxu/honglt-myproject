//
// Handshake.cpp
//

#include "stdafx.h"
#include "Xgateway.h"
#include "Handshake.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CHandshake construction

CHandshake::CHandshake(SOCKET hSocket, SOCKADDR_IN* pHost) : CTransfer( hSocket, pHost )
{
}

CHandshake::~CHandshake()
{
	Disconnect();
}

//////////////////////////////////////////////////////////////////////
// CHandshake run function

BOOL CHandshake::OnRun()
{
	return CConnection::OnRun();
}

//////////////////////////////////////////////////////////////////////
// CHandshake socket event handlers

BOOL CHandshake::OnConnected()
{
	return CConnection::OnConnected();
}

//////////////////////////////////////////////////////////////////////
// CHandshake TCP Transfer func

void CHandshake::OnPacket(IPacket* pPacket)
{
	CTransfer::OnPacket( pPacket );
	
	if ( pPacket->IsCommandId( ICP_LISN ) )
	{
		CChannel* pChannel = Network.FindChannel( m_nChan );
		if ( pChannel ) pChannel->OnLisnPacket( (ILisnPacket*)pPacket );
	}
	else
	if ( pPacket->IsCommandId( ICP_HALT ) )
	{
		CChannel* pChannel = Network.FindChannel( m_nChan );
		if ( pChannel ) pChannel->OnHaltPacket( (IHaltPacket*)pPacket );
	}
}

//////////////////////////////////////////////////////////////////////
// CHandshake send packet

BOOL CHandshake::SendPacket(IPacket* pPacket, BOOL bPeek)
{
	pPacket->EncodeBody( m_pOutput );
	CConnection::OnWrite();
	
	if ( ! bPeek ) pPacket->Release();

	return TRUE;
}
