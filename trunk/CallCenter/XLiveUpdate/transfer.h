//
// Transfer.h
//

#if !defined(AFX_TRANSFER_H__866F7CD7_234E_407E_A6CC_F4A73B59980A__INCLUDED_)
#define AFX_TRANSFER_H__866F7CD7_234E_407E_A6CC_F4A73B59980A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\COMMON\connection.h"
#include "..\COMMON\IPacket.h"

class CTransfer : public CConnection  
{
// Construction
public:
	CTransfer(SOCKET hSocket, SOCKADDR_IN* pHost);
	virtual ~CTransfer();
	CTransfer();

// Attributes
public:
	
// Operations
public:
	virtual BOOL	ConnectTo(LPCTSTR pAddress, int nPort);
	virtual void	Disconnect();
	virtual BOOL	OnRun();
	virtual BOOL	SendPacket(IPacket* pPacket);
	virtual BOOL	SendPacket(const void * pData, DWORD nLength);
protected:
	virtual BOOL	OnRead();
	virtual BOOL	OnConnected();
	virtual void	OnDropped(BOOL bError);
	virtual void	OnHandshake();
};

#endif // !defined(AFX_TRANSFER_H__866F7CD7_234E_407E_A6CC_F4A73B59980A__INCLUDED_)
