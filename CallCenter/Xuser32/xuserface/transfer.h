//
// Transfer.h
//

#if !defined(AFX_TRANSFER_H__866F7CD7_234E_407E_A6CC_F4A73B59980A__INCLUDED_)
#define AFX_TRANSFER_H__866F7CD7_234E_407E_A6CC_F4A73B59980A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Connection.h"
#include "Packet.h"

class CTransfer : public CConnection  
{
// Construction
public:
	CTransfer(SOCKET hSocket, SOCKADDR_IN* pHost);
	virtual ~CTransfer();
	CTransfer(DWORD);
	CTransfer();

// Attributes
public:
	
// Operations
public:
	virtual void	Disconnect();
	virtual BOOL	ConnectTo(IN_ADDR* pAddress, int nPort);
protected:
	virtual BOOL	OnRun();
	virtual BOOL	OnRead();
	virtual void	OnDropped(int nError);
	virtual void	OnHandshake();
protected:
	virtual BOOL	OnPacket(CPacket* pPacket);
	virtual BOOL	OnOkPacket(COkPacket* pPacket);
	virtual BOOL	OnErrPacket(CErrPacket* pPacket);
	virtual BOOL	OnByePacket(CByePacket* pPacket);
protected:
	virtual CPacket*	ReadBuffer(CBuffer* pBuffer) = 0;
//	virtual	BOOL		SendPacket(const void* pData, DWORD nLength);
	virtual BOOL		SendPacket(CPacket* pPacket);
	
	friend class CNetwork;
	friend class CNeighbour;
};

#endif // !defined(AFX_TRANSFER_H__866F7CD7_234E_407E_A6CC_F4A73B59980A__INCLUDED_)
