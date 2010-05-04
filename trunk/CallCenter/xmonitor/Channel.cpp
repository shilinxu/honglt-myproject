//
// Channel.cpp
//

#include "stdafx.h"
#include "Xmonitor.h"
#include "Network.h"
#include "Channel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CChannel construction

CChannel::CChannel(SOCKET hSocket, SOCKADDR_IN* pHost) : CTransfer( hSocket, pHost )
{
	m_xJobId	= NULL;
	
	CChannel::Setup();
}

CChannel::CChannel(LPCTSTR xJobId)
{
	m_xJobId	= _tcsdup( xJobId );
	
	CChannel::Setup();
}

CChannel::~CChannel()
{
	free( m_xJobId ); m_xJobId = NULL;
}

//////////////////////////////////////////////////////////////////////
// CChannel attributes

void CChannel::Setup()
{
}

void CChannel::Clone(CChannel* pParent)
{
	if ( pParent->m_hSocket != INVALID_SOCKET )
	{
		this->AttachTo( pParent );
	}
}

BOOL CChannel::TimeOut(DWORD nExpire) const
{
	return GetTickCount() - m_tConnected > nExpire;
}

//////////////////////////////////////////////////////////////////////
// CChannel disconnect

void CChannel::Disconnect()
{
	CSingleLock pLock( &Network.m_pSection, TRUE );
	Network.m_pList.RemoveKey( m_xJobId );
	
	CTransfer::Disconnect(); delete this;
}

//////////////////////////////////////////////////////////////////////
// CChannel socket event handlers

void CChannel::OnHandshake()
{
	SendPacket( new COkPacket );
	
	CTransfer::OnHandshake();
}

void CChannel::AlarmHotlink(int nError)
{
	theApp.Message( MSG_ERROR, IDS_NETWORK_CONNECT_ERROR, nError );
	OnAlarm( ALARM_HOTLINK_FAIL );
}

BOOL CChannel::LinkRestart()
{
	CConnection::ConnectTo( (SOCKADDR_IN*)&m_pHost );
	WSAEventSelect( m_hSocket, Network.m_pWakeup, FD_CONNECT|FD_READ|FD_WRITE|FD_CLOSE );
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CChannel packet unserialize

CPacket* CChannel::ReadBuffer(CBuffer* pBuffer)
{
	CString strLine;
	ASSERT( pBuffer != NULL );
	
	if ( ! pBuffer->ReadLine( strLine, TRUE ) ) return NULL;
	
	CPacket* pPacket = CPacketCore::ReadBuffer( pBuffer->m_pBuffer, pBuffer->m_nLength );
	int nLength = strLine.GetLength();
	
	if ( pPacket == NULL )
	{
		pBuffer->Remove( nLength + 1 + (pBuffer->m_pBuffer[nLength] == '\r') ); // + [\r] \n"
		SendPacket( new CErrPacket(EError_Unknown) );
	}
	else
	if ( pPacket && pPacket->CommandId )
	{
		nLength = pPacket->GetBufferSize();
		pBuffer->Remove( nLength + 1 + (pBuffer->m_pBuffer[nLength] == '\r') ); // + [\r] \n"
	}
	
	return pPacket;
}

//////////////////////////////////////////////////////////////////////
// CChannel alarm helper

void CChannel::OnAlarm(int nAlarm)
{
	switch ( nAlarm )
	{
	case ALARM_HOTLINK_FAIL :
		Beep( 3500, 1000 );
		break;
	
	case ALARM_GENERAL:
		Beep( 500, 500 );
		//Beep(800,200);
		break ;
	
	case ALARM_NO_RESPONSE:
		Beep( 1500, 100 );
		break;
	
	default :
		Beep( 2500, 500 );
	}
}
