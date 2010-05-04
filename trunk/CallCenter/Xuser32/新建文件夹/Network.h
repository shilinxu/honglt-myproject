//
// Network.h
//

#if !defined(AFX_NETWORK_H__0C616F1D_3108_424C_AA95_7FD3135A3AD7__INCLUDED_)
#define AFX_NETWORK_H__0C616F1D_3108_424C_AA95_7FD3135A3AD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTransfer;
#include "RTPSession2.h"

class CNetwork  
{
// Construction
public:
	CNetwork();
	virtual ~CNetwork();

// Attributes
public:
	CEvent			m_pWakeup;
	CMutex			m_pSection;
	BOOL			m_bConnected;
	RTPSession2		m_pRTPSession;
protected:
	HANDLE			m_hThread;
	volatile BOOL	m_bThread;
	CPtrList		m_pList;

// Operations
public:
	BOOL			Setup();
	void			Release();
	BOOL			Connect();
	void			Disconnect();
	BOOL			IsConnected() const;
protected:
	void			Add(CTransfer* pTransfer);
	void			Remove(CTransfer* pTransfer);
public:
	CTransfer*		Find(CTransfer* pTransfer);
	void			Clear();
protected:
	void			StartThread();
	void			StopThread();
protected:
	static	UINT	ThreadStart(LPVOID pParam);
	virtual	BOOL	OnRun();
	virtual void	RunNeighbours();

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
