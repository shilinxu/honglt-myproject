//
// Handshake.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Handshake.h"
#include "ChannelUser2.h"
#include "Network.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CHandshake construction

CHandshake::CHandshake(SOCKET hSocket, SOCKADDR_IN* pHost, int nChan)
: CTransfer( hSocket, pHost ), m_nChan( nChan )
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
// CHandshake packet unserialize

CPacket* CHandshake::ReadBuffer(CBuffer* pBuffer)
{
	CString strLine;
	ASSERT( pBuffer != NULL );
	
	if ( ! pBuffer->ReadLine( strLine, TRUE ) ) return NULL;
	
	CPacket* pPacket = CPacketCore::ReadBuffer( pBuffer->m_pBuffer, pBuffer->m_nLength );
	int nLength = strLine.GetLength();
	
	if ( pPacket == NULL )
	{
		pBuffer->Remove( nLength + 1 + (pBuffer->m_pBuffer[nLength] == '\r') ); // + [\r] \n"
		SendPacket( new CErrPacket(ECore_Unknown) ); return NULL;
	}
	else
	if ( pPacket->CommandId != NULL )
	{
		nLength = pPacket->GetBufferSize();
		pBuffer->Remove( nLength + 1 + (pBuffer->m_pBuffer[nLength] == '\r') ); // + [\r] \n"
		return pPacket;
	}
	
	return pPacket;
}

//////////////////////////////////////////////////////////////////////
// CHandshake TCP Transfer func

BOOL CHandshake::OnPacket(CPacket* pPacket)
{
	CChannel* pChannel = Network.FindChannel( m_nChan );
	if ( pChannel ) return pChannel->OnPacket( pPacket );
	
	return CTransfer::OnPacket( pPacket );
}

//////////////////////////////////////////////////////////////////////
// CHandshake send packet

BOOL CHandshake::SendPacket(CPacket* pPacket, BOOL bPeek)
{
	pPacket->EncodeBody( m_pOutput );
	CConnection::OnWrite();
	
	if ( ! bPeek ) pPacket->Release();

	return TRUE;
}
