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
	sessParams.SetOwnTimestampUnit( Settings.Network.MaxRate );
	sessParams.SetMaximumPacketSize( Settings.Network.MaxPacket );
	
	RTPUDPv4TransmissionParams transParams;
	transParams.SetPortbase( Settings.Network.Port );
	
	return Create( sessParams, &transParams );
}

//////////////////////////////////////////////////////////////////////
// RTPSession2 Transfer func

void RTPSession2::OnRTPPacket(RTPPacket *pack,const RTPTime &receivetime,
							  const RTPAddress *senderaddress)
{
	theApp.Message(MSG_TEMP, "[%d]: %s\n", pack->GetPayloadLength(),
		ToHex(pack->GetPacketData(), pack->GetPayloadLength()) );
}

void RTPSession2::OnRTCPCompoundPacket(RTCPCompoundPacket *pack, const RTPTime &receivetime,
									   const RTPAddress *senderaddress)
{
}
