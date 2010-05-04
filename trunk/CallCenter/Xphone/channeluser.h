//
// ChannelUser.h
//

#if !defined(AFX_CHANNELUSER_H__B55F8BDC_4162_4DDB_8596_153F57A34582__INCLUDED_)
#define AFX_CHANNELUSER_H__B55F8BDC_4162_4DDB_8596_153F57A34582__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"
class RTPPacket;
class RTPTime;

class CUserChannel	: public CChannel
{
// Construction
public:
	CUserChannel(LPCTSTR xUserId = NULL);
	virtual ~CUserChannel();

// Attributes
public:
	LPTSTR			m_xUserId;
	
// Operations
public:
	virtual void	Setup();
	virtual void	OnHandshake();
	virtual BOOL	IsNavigated(LPCTSTR xCallType) const;
public:
	virtual void	SetUserId(LPCTSTR xUserId);
	virtual BOOL	ConnectTo(LPCTSTR xUserId, LPCTSTR xPassword);
public:
	virtual BOOL	CallRemote(LPCTSTR xRemoteId, LPCTSTR xLocalId = NULL);
	virtual BOOL	PickupRemote();
	virtual void	HangRemote();
	virtual BOOL	DeliverBuffer(RTPPacket *pack, const RTPTime* receivetime);
	virtual BOOL	SendPacket(CPacket* pPacket);
};

#endif // !defined(AFX_CHANNELUSER_H__B55F8BDC_4162_4DDB_8596_153F57A34582__INCLUDED_)
