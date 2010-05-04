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
	CTransfer();

// Attributes
public:
	
// Operations
public:
	virtual void	Disconnect();
	virtual BOOL	ConnectTo(LPCTSTR pAddress, int nPort);
	virtual BOOL	SendPacket(CPacket* pPacket);
protected:
	virtual BOOL	OnRun();
	virtual BOOL	OnRead();
	virtual void	OnDropped(BOOL bError);
	virtual void	OnHandshake();
	virtual void	DoPrint(BYTE* pBuffer, DWORD nLength, LPCTSTR lpszHeader);
protected:
	virtual BOOL	OnPacket(CPacket* pPacket);
	virtual BOOL	OnOkPacket(COkPacket* pPacket);
	virtual BOOL	OnErrPacket(CErrPacket* pPacket);
	virtual BOOL	OnByePacket(CByePacket* pPacket);
protected:
	virtual CPacket*	ReadBuffer(CBuffer* pBuffer) = 0;
};

#endif // !defined(AFX_TRANSFER_H__866F7CD7_234E_407E_A6CC_F4A73B59980A__INCLUDED_)
