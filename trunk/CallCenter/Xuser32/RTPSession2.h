//
// RTPSession2.h
//

#if !defined(AFX_RTPSESSION2_H__3D7EB65C_FBD0_49FB_997A_299261797D26__INCLUDED_)
#define AFX_RTPSESSION2_H__3D7EB65C_FBD0_49FB_997A_299261797D26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RTPSession.h"

class RTPSession2 : public RTPSession  
{
// Construction
public:
	RTPSession2();
	virtual ~RTPSession2();

// Attributes
public:

// Operations
public:
	virtual int		Connect();

protected:
	virtual void	OnRTPPacket(RTPPacket *pack, const RTPTime &receivetime,
							const RTPAddress *senderaddress);
	virtual void	OnRTCPCompoundPacket(RTCPCompoundPacket *pack, const RTPTime &receivetime,
							const RTPAddress *senderaddress);
};

extern RTPSession2	RTPSession;

#endif // !defined(AFX_RTPSESSION2_H__3D7EB65C_FBD0_49FB_997A_299261797D26__INCLUDED_)
