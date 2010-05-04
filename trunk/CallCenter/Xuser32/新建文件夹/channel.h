//
// Channel.h
//

#if !defined(AFX_CHANNEL_H__025B9C73_E4B5_4B21_99BB_5015BB726599__INCLUDED_)
#define AFX_CHANNEL_H__025B9C73_E4B5_4B21_99BB_5015BB726599__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Transfer.h"
#include "PacketBase.h"

class CChannel : public CTransfer  
{
// Construction
public:
	CChannel();
	virtual ~CChannel();

// Attributes
protected:
	
// Operations
public:
	virtual void	Setup();
	inline	void	Release();
	virtual void	Clone(CChannel* pParent);
	virtual void	OnHandshake();
protected:
	virtual void	OnDropped(int nError);
	virtual BOOL	OnPacket(CPacket* pPacket);
	virtual BOOL	OnOkPacket(COkPacket* pPacket);
	virtual BOOL	OnErrPacket(CErrPacket* pPacket);
	virtual CPacket*	ReadBuffer(CBuffer* pBuffer);
};

// inline access

void CChannel::Release()
{
	if ( this == NULL ) return;
	
	delete this;
}

#endif // !defined(AFX_CHANNEL_H__025B9C73_E4B5_4B21_99BB_5015BB726599__INCLUDED_)
