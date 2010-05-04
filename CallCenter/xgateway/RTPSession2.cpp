//
// RTPSession2.cpp
//

#include "stdafx.h"
#include "Xgateway.h"
#include "Settings.h"
#include "RTPSession2.h"

#include "RTPPacket.h"
#include "RTPIPv4Address.h"
#include "RTPSessionParams.h"
#include "RTPUDPv4Transmitter.h"

#include "Network.h"
#include "FilterGraph.h"
#include "XML.h"

#include "Neighbour.h"
#include "Circuitry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

RTPSession2	RTPSession;

//////////////////////////////////////////////////////////////////////
// RTPSession2 construction

RTPSession2::RTPSession2()
{
}

RTPSession2::~RTPSession2()
{
}

//////////////////////////////////////////////////////////////////////
// RTPSession2 connect

int RTPSession2::Connect()
{
	int nError = 0;
	
	RTPSessionParams sessParams;
	sessParams.SetOwnTimestampUnit( 1.0 / (double)Settings.Network.Timestamp );
	sessParams.SetMaximumPacketSize( Settings.Network.MaxPacket );
	
	RTPUDPv4TransmissionParams transParams;
#ifdef PORT9980
	transParams.SetPortbase( 9980 );
#else
	transParams.SetPortbase( Settings.Network.Port );
#endif
	
	return Create( sessParams, &transParams );
}

int RTPSession2::SendPacket(const void *data,size_t len,
							uint8_t pt,bool mark,uint32_t timestampinc)
{
//	CSingleLock pLock( &Network.m_pSection, TRUE );
	
	return RTPSession::SendPacket( data, len, pt, mark, timestampinc );
}

//////////////////////////////////////////////////////////////////////
// RTPSession2 Transfer func

void RTPSession2::OnRTPPacket(RTPPacket *pack,const RTPTime &receivetime,
							  const RTPAddress *senderaddress)
{
	CSingleLock pLock( &Network.m_pSection, TRUE );
	
	if ( pack->GetPayloadType() == Payload_Control )
	{
		CPacket* pPacket = CPacketBase::ReadBuffer( pack->GetPayloadData(), pack->GetPayloadLength() );
		if ( pPacket && pPacket->CommandId ) OnPacket( pPacket, senderaddress );
		
		if ( pPacket ) pPacket->Release();
		return;
	}
	
	{
		if ( CChannel* pChannel = Network.FindChannel( (RTPAddress*)senderaddress ) )
		{
			pChannel->DeliverBuffer( (RTPPacket*)pack, senderaddress );
		}
	}
/*	
	{
TRACE("%i-> RTPSession2 OnRTPPacket Neighbour lock[333333] in\n", GetTickCount());
		CSingleLock pLock( &Neighbour.m_pSection, TRUE );
		
		CCircuitry* pChannel = Neighbour.FindChannel( (LPCTSTR)xUserId );
		if ( pChannel ) pChannel->DeliverBuffer( pack->GetPayloadData(), pack->GetPayloadLength() );
	}
	*/
}

void RTPSession2::OnRTCPCompoundPacket(RTCPCompoundPacket *pack, const RTPTime &receivetime,
									   const RTPAddress *senderaddress)
{
}

//////////////////////////////////////////////////////////////////////
// RTPSession2 packet event handlers

BOOL RTPSession2::OnPacket(CPacket* pPacket, const RTPAddress *senderaddress)
{
	if ( pPacket->IsCommandId( IBase_SyncOrderRelation ) )
	{
		return OnSyncOrderRelation( (CSyncOrderRelation*)pPacket, senderaddress );
	}
	
	return TRUE;
}

BOOL RTPSession2::OnSyncOrderRelation(CSyncOrderRelation* pPacket, const RTPAddress *senderaddress)
{
	CString sMsgType = pPacket->GetMsgType();
	
	if ( ! sMsgType.CompareNoCase( ISync_Mirror ) )
	{
		return OnSyncOrderRelation_Mirror( pPacket->m_pXML, senderaddress );
	}
	
	return TRUE;
}

BOOL RTPSession2::OnSyncOrderRelation_Mirror(CXMLElement* pXML, const RTPAddress *senderaddress)
{
	ASSERT( pXML && pXML->GetElementByName( _T("MsgType") ) );
	ASSERT( pXML->GetElementByName( _T("MsgType") )->GetValue().CompareNoCase( ISync_Mirror ) == 0 );
	
	CXMLElement* pSync = pXML->GetElementByName( _T("UserId") );
	CString xUserId = pSync ? pSync->GetValue() : _T("");
	
//	CSingleLock pLock( &Network.m_pSection, TRUE );
	
	CChannel* pChannel = Network.FindChannel( xUserId );
	if ( pChannel && ! pChannel->m_xAddress ) return pChannel->Mirror( senderaddress );
	
//	pChannel->m_xAddress = senderaddress->CreateCopy( NULL );
	return FALSE;
}
/*
void RTPSession2::OnPollThreadStep()
{
	BeginDataAccess();
		
	// check incoming packets
	if (GotoFirstSourceWithData())
	{
		do
		{
			RTPPacket *pack;
			RTPSourceData *srcdat;
			
			srcdat = GetCurrentSourceInfo();
			
			while ((pack = GetNextPacket()) != NULL)
			{
				ProcessRTPPacket(*srcdat,*pack);
				DeletePacket(pack);
			}
		} while (GotoNextSourceWithData());
	}
		
	EndDataAccess();
}
#include "rtpsourcedata.h"

void RTPSession2::ProcessRTPPacket(const RTPSourceData &srcdat,const RTPPacket &rtppack)
{
	// You can inspect the packet and the source's info here
	TRACE("Got packet [%i] from SSRC [%i]\n", rtppack.GetExtendedSequenceNumber(), srcdat.GetSSRC());
}
*/
