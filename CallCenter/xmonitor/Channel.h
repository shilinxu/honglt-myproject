//
// Channel.h
//

#if !defined(AFX_CHANNEL_H__BC4D8D18_12C5_4D14_93EC_BFCC26D1EBD3__INCLUDED_)
#define AFX_CHANNEL_H__BC4D8D18_12C5_4D14_93EC_BFCC26D1EBD3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Transfer.h"
#include "PacketCore.h"

class CChannel : public CTransfer
{
// Construction
public:
	CChannel(SOCKET hSocket, SOCKADDR_IN* pHost);
	CChannel(LPCTSTR xJobId = NULL);
	virtual ~CChannel();

// Attributes
public:
	LPTSTR			m_xJobId;

// Operations
public:
	virtual void	Setup();
	inline	void	Release();
	virtual void	Disconnect();
public:
	BOOL			TimeOut(DWORD nExpire) const;
	BOOL			LinkRestart();
protected:
	virtual void	AlarmHotlink(int nError);
	virtual void	Clone(CChannel* pParent);
protected:
	virtual void	OnHandshake();
	virtual void	OnAlarm(int nAlarm);
protected:
	virtual CPacket*	ReadBuffer(CBuffer* pBuffer);
	
	friend class CNetwork;
};

// inline access

void CChannel::Release()
{
	if ( this == NULL ) return;
	
	Disconnect();
}

typedef enum
{
	ALARM_GENERAL		= 1,
	ALARM_HOTLINK_FAIL	= 2,
	ALARM_NO_RESPONSE	= 3,
} ALARM_TYPE;

#endif // !defined(AFX_CHANNEL_H__BC4D8D18_12C5_4D14_93EC_BFCC26D1EBD3__INCLUDED_)
