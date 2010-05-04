//
// Handshake.h
//

#if !defined(AFX_HANDSHAKE_H__89BCED1E_9F54_4524_8B5E_6DAC0D6ED007__INCLUDED_)
#define AFX_HANDSHAKE_H__89BCED1E_9F54_4524_8B5E_6DAC0D6ED007__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Transfer.h"
class IPacket;

class CHandshake : public CTransfer  
{
// Construction
public:
	CHandshake(SOCKET hSocket, SOCKADDR_IN* pHost, int nChan);
	virtual ~CHandshake();

// Attributes
public:
	int				m_nChan;
	
// Operations
public:
	virtual BOOL	OnRun();
	virtual BOOL	SendPacket(IPacket* pPacket, BOOL bPeek = FALSE);
protected:
	virtual BOOL	OnConnected();
	virtual void	OnPacket(IPacket* pPacket);
};

#endif // !defined(AFX_HANDSHAKE_H__89BCED1E_9F54_4524_8B5E_6DAC0D6ED007__INCLUDED_)
