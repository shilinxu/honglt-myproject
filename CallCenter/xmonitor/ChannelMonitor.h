//
// ChannelMonitor.h
//

#if !defined(AFX_CHANNELMONITOR_H__6E08DC92_B772_4D10_836B_7DDC9C66323B__INCLUDED_)
#define AFX_CHANNELMONITOR_H__6E08DC92_B772_4D10_836B_7DDC9C66323B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"
class CNetworkChild;

class CMonitorChannel : public CChannel  
{
// Construction
public:
	CMonitorChannel();
	virtual ~CMonitorChannel();

// Attributes
protected:
	CNetworkChild*	m_pOwner;
	
// Operations
public:
	CNetworkChild*	SetOwner(CNetworkChild* pOwner);
protected:
	virtual void	Disconnect();
protected:
	virtual BOOL	OnRun();
	virtual void	OnDropped(int nError);
protected:
	virtual BOOL	OnPacket(CPacket* pPacket);
	virtual BOOL	OnAchnPacket(CAchnPacket* pPacket);
	virtual BOOL	OnEchnPacket(CEchnPacket* pPacket);
};

#endif // !defined(AFX_CHANNELMONITOR_H__6E08DC92_B772_4D10_836B_7DDC9C66323B__INCLUDED_)
