//
// Network.h
//

#if !defined(AFX_NETWORK_H__0C616F1D_3108_424C_AA95_7FD3135A3AD7__INCLUDED_)
#define AFX_NETWORK_H__0C616F1D_3108_424C_AA95_7FD3135A3AD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChannelUser.h"

class CNetwork : public CUserChannel
{
// Construction
public:
	CNetwork();
	virtual ~CNetwork();

// Attributes
public:
	CEvent			m_pWakeup;
	CMutex			m_pSection;
protected:
	HANDLE			m_hThread;
	volatile BOOL	m_bThread;
	CPtrList		m_pList;

// Operations
public:
	virtual BOOL	ConnectTo(LPCTSTR pAddress, int nPort);
	virtual void	Disconnect();
	BOOL			Connect();
protected:
	void			Add(CTransfer* pTransfer);
	void			Remove(CTransfer* pTransfer);
	void			Clear();
protected:
	void			StartThread();
	void			StopThread();
protected:
	virtual	BOOL	OnRun(DWORD nReserved);
	virtual void	RunNeighbours();
	static	UINT	ThreadStart(LPVOID pParam);
protected:
	virtual void	OnDropped(int nError);
	virtual BOOL	OnByePacket(CByePacket* pPacket);

// List Access
public:
	inline POSITION GetIterator() const
	{
		return m_pList.GetHeadPosition();
	}

	inline CTransfer* GetNext(POSITION& pos) const
	{
		return (CTransfer*)m_pList.GetNext( pos );
	}

	inline int GetCount() const
	{
		return m_pList.GetCount();
	}
	
	friend class CTransfer;
};

extern CNetwork Network;

#endif // !defined(AFX_NETWORK_H__0C616F1D_3108_424C_AA95_7FD3135A3AD7__INCLUDED_)
