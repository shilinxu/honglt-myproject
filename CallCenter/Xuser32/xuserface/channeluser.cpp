//
// ChannelUser.cpp
//

#include "stdafx.h"
#include "ChannelUser.h"
#include "XML.h"

//////////////////////////////////////////////////////////////////////
// CUserChannel construction

CUserChannel::CUserChannel(SOCKET hSocket, SOCKADDR_IN* pHost) : CChannel(hSocket, pHost)
{
}

CUserChannel::CUserChannel(LPCTSTR xUserId) : CChannel(xUserId)
{
}

CUserChannel::~CUserChannel()
{
}

//////////////////////////////////////////////////////////////////////
// CUserChannel attributes

void CUserChannel::Setup()
{
	CChannel::Setup();
}

//////////////////////////////////////////////////////////////////////
// CUserChannel operations

void CUserChannel::JoinMonitor(BOOL bJoined)
{
	CXMLElement* pXML = new CXMLElement( NULL, ISyncOrderRelation );
	pXML->AddAttribute( "method", "monitor" );
	
	if ( ! bJoined ) pXML->AddAttribute( "leave" );
	
	SendPacket( new CSyncOrderRelation(pXML) );
	delete pXML;
}

//////////////////////////////////////////////////////////////////////
// CUserChannel TCP Transfer func

BOOL CUserChannel::OnPacket(CPacket* pPacket)
{
	if ( pPacket->IsCommandId(IBase_SyncOrderRelation) )
	{
		return OnSyncOrderRelation( ((CPacketBase*)pPacket)->m_pXML );
	}
	if ( pPacket->IsCommandId(IBase_SyncDataView) )
	{
		return OnSyncDataView( ((CPacketBase*)pPacket)->m_pXML );
	}
	
	return CChannel::OnPacket(pPacket);
}

BOOL CUserChannel::OnSyncOrderRelation(CXMLElement* pXML)
{
	OnPacketed( pXML ); return TRUE;
}

BOOL CUserChannel::OnSyncDataView(CXMLElement* pXML)
{
	OnPacketed( pXML ); return TRUE;
}
