//
// Channel.h
//

#if !defined(AFX_CHANNEL_H__BC4D8D18_12C5_4D14_93EC_BFCC26D1EBD3__INCLUDED_)
#define AFX_CHANNEL_H__BC4D8D18_12C5_4D14_93EC_BFCC26D1EBD3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//
// Configuration
//

#define MAX_BUFFER			256

#include "Transfer.h"

class CChannel : public CTransfer
{
// Construction
public:
	CChannel(LPCTSTR xUserId);
	CChannel(SOCKET hSocket, SOCKADDR_IN* pHost);
	virtual ~CChannel();

// Attributes
public:
	LPTSTR			m_xUserId;

// Operations
public:
	virtual void	Setup();
	virtual void	Release();
	virtual void	Clone(CChannel* pParent);
	virtual void	SetUserId(LPCTSTR xUserId);
public:
	virtual void	OnHandshake();
	virtual void	OnDropped(BOOL bError);
protected:
	virtual BOOL	OnPacket(IPacket* pPacket);
	virtual BOOL	OnExitPacket(IExitPacket* pPacket);
	virtual int		OnUserPacket(IUserPacket* pPacket);
protected:
	int				OnReadUser(LPCTSTR xUserId, LPTSTR xUserPwd);
};

#endif // !defined(AFX_CHANNEL_H__BC4D8D18_12C5_4D14_93EC_BFCC26D1EBD3__INCLUDED_)
