//
// RTPSession2.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "Settings.h"
#include "RTPSession2.h"

#include "RTPIPv4Address.h"
#include "RTPSessionParams.h"
#include "RTPUDPv4Transmitter.h"

#include "Network.h"

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
	sessParams.SetOwnTimestampUnit( 1.0 / 8000.0 );
	sessParams.SetMaximumPacketSize( Settings.Network.MaxPacket );
	
	RTPUDPv4TransmissionParams transParams;
	transParams.SetPortbase( 1960 );
	
	nError = Create( sessParams, &transParams );
	if ( nError < 0 ) return nError;
/*	
	DWORD nAddress = inet_addr( Settings.Network.Host );
	RTPIPv4Address addr( ntohl(nAddress), Settings.Network.Port );
	if ( (nError = AddDestination(addr)) < 0 ) return nError;
	*/
#ifdef _DEBUG
	DWORD nAddress2= inet_addr( "127.0.0.1" );
	RTPIPv4Address addr2( ntohl(nAddress2), 60000 );
	if ( (nError = AddDestination(addr2)) < 0 ) return nError;
#endif
	
	SetDefaultPayloadType(96);
	SetDefaultMark(false);
	SetDefaultTimestampIncrement(160);
	
	return nError;
}

//////////////////////////////////////////////////////////////////////
// RTPSession2 Transfer func

void RTPSession2::OnRTPPacket(RTPPacket *pack,const RTPTime &receivetime,
							  const RTPAddress *senderaddress)
{
	CSingleLock pLock( &Network.m_pSection, TRUE );
	Network.DeliverBuffer( (RTPPacket*)pack, senderaddress );
}

void RTPSession2::OnRTCPCompoundPacket(RTCPCompoundPacket *pack, const RTPTime &receivetime,
									   const RTPAddress *senderaddress)
{
}
