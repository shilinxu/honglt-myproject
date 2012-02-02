//
// Transfer.h
//

#if !defined(AFX_TRANSFER_H__F1169289_448E_4118_8B34_2F579FE06E26__INCLUDED_)
#define AFX_TRANSFER_H__F1169289_448E_4118_8B34_2F579FE06E26__INCLUDED_

#pragma once
#include "Connection.h"

#define TR_OFF				0
#define TR_ON       		1

class KERNEL_MANAGER_CLASS CTransfer : public CConnection
{
// Construction
public:
	CTransfer();
	CTransfer(SOCKET hSocket, SOCKADDR_IN* pHost);
	virtual ~CTransfer();

// Attributes
public:
	volatile DWORD		m_nTransferId;
protected:
	COverlapped*		m_pReadIoOver;
	COverlapped*		m_pWriteIoOver;
protected:
	CThreadGuard		m_pSection;
private:
	BYTE*				m_pBufferOut;
	LONG				m_nReference;
	HANDLE				m_hExitEvent;

// Operations
public:
	virtual void	Destroy(BOOL bPeek = FALSE);
// 	virtual	void	Release() { delete this; }
// public:
// 	virtual BOOL	SendPacket(const void* pData, DWORD nLength);
// 	virtual BOOL	SendPacket(CPacket* pPacket, BOOL bPeek = FALSE);
protected:
	virtual BOOL	OnRead();
	virtual BOOL	OnWrite();
	virtual BOOL	OnRead(DWORD);
protected:
	virtual void	OnDropped(int nError);
	virtual BOOL	OnReadCompleted(COverlapped*, DWORD nLength);
	virtual BOOL	OnWriteCompleted(COverlapped*, DWORD nLength);
protected:
// 	virtual CPacket*ReadBuffer(CBuffer* pBuffer) = NULL;
	virtual BOOL	ReadBuffer(CBuffer* pBuffer, DWORD);
// 	virtual BOOL	OnPacket(CPacket* pPacket) = NULL;
// 	virtual BOOL	OnRun() = NULL;
// 
// 	friend class	CTransferThread;
// 	friend class	CTCPServer;
// 	friend class	CNeighbour;
// 	friend class	CNetwork;
};

#endif // !defined(AFX_TRANSFER_H__F1169289_448E_4118_8B34_2F579FE06E26__INCLUDED_)
