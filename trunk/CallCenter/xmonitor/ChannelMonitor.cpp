//
// ChannelMonitor.cpp
//

#include "stdafx.h"
#include "Xmonitor.h"
#include "ChannelMonitor.h"
#include "Settings.h"

#include "CtrlNetworkChild.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CMonitorChannel construction

CMonitorChannel::CMonitorChannel() : CChannel( "Xmonitor" )
{
	ZeroMemory( &m_pHost, sizeof(m_pHost) );
	
	m_pHost.sin_family	= PF_INET;
	m_pHost.sin_addr.s_addr	= inet_addr( Settings.Network.Host );
	m_pHost.sin_port	= htons( Settings.Network.Port );
	
	m_pOwner	= NULL;
}

CMonitorChannel::~CMonitorChannel()
{
}

//////////////////////////////////////////////////////////////////////
// CMonitorChannel attributes

CNetworkChild* CMonitorChannel::SetOwner(CNetworkChild* pOwner)
{
	return m_pOwner = pOwner;
}

//////////////////////////////////////////////////////////////////////
// CMonitorChannel disconnect

void CMonitorChannel::Disconnect()
{
	CTransfer::Disconnect();
	
	m_pOwner->ResetMap();
}

//////////////////////////////////////////////////////////////////////
// CMonitorChannel run function

BOOL CMonitorChannel::OnRun()
{
	if ( ! CChannel::OnRun() ) return FALSE;
	
	DWORD tNow = GetTickCount();
	
	if ( IsConnected() && tNow - m_tConnected >= 180* 1000 )
	{
		OnDropped( WSA_WAIT_TIMEOUT ); return FALSE;
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CMonitorChannel socket event handlers

void CMonitorChannel::OnDropped(int nError)
{
	AlarmHotlink( nError );
	
	Disconnect();
}

//////////////////////////////////////////////////////////////////////
// CMonitorChannel packet event handlers

BOOL CMonitorChannel::OnPacket(CPacket* pPacket)
{
	CTransfer::OnConnected();
	
	if ( pPacket->IsCommandId( ICore_AddLink ) )
	{
		return OnAchnPacket( (CAchnPacket*)pPacket );
	}
	if ( pPacket->IsCommandId( ICore_SyncLink ) )
	{
		return OnEchnPacket( (CEchnPacket*)pPacket );
	}
	
	return CChannel::OnPacket( pPacket );
}

BOOL CMonitorChannel::OnAchnPacket(CAchnPacket* pPacket)
{
	m_pOwner->MapClassToXML( pPacket->ChType, pPacket->ChFirst, pPacket->ChLength );
	return TRUE;
}

BOOL CMonitorChannel::OnEchnPacket(CEchnPacket* pPacket)
{
	m_pOwner->UpdateChState( pPacket->Channel, pPacket->ChState );
	return TRUE;
}
