//
// Network.h
//

#if !defined(AFX_NETWORK_H__3C174927_E261_4F3E_8CFC_274E5B8E40FE__INCLUDED_)
#define AFX_NETWORK_H__3C174927_E261_4F3E_8CFC_274E5B8E40FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTransfer;

class CNetwork
{
	friend class CTransfer;

// Construction
public:
	CNetwork();
	virtual ~CNetwork();

// Attributes
public:
	CEvent			m_pWakeup;
	CMutex			m_pSection;
	BOOL			m_bConnected;
protected:
	HANDLE			m_hThread;
	volatile BOOL	m_bThread;
	SOCKET			m_hSocket;
	CPtrList		m_pList;

// Operations
public:
	BOOL			Connect();
	void			Disconnect();
	BOOL			IsConnected() const;
	int				IsWellConnected() const;
protected:
	void			Add(CTransfer* pTransfer);
	void			Remove(CTransfer* pTransfer);
	void			Clear();
protected:
	void			StartThread();
	void			StopThread();
	BOOL			RunNeighbour();
	CTransfer*		CreateNeighbour(SOCKET hSocket, SOCKADDR_IN* pHost);
	static int CALLBACK AcceptCheck(IN LPWSABUF lpCallerId, IN LPWSABUF lpCallerData, IN OUT LPQOS lpSQOS, IN OUT LPQOS lpGQOS, IN LPWSABUF lpCalleeId, OUT LPWSABUF lpCalleeData, OUT GROUP FAR * g, IN DWORD dwCallbackData);
protected:
	static	UINT	ThreadStart(LPVOID pParam);
	virtual	BOOL	OnRun();
	virtual	BOOL	AcceptNeighbour();

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
};

extern CNetwork Network;

#endif // !defined(AFX_NETWORK_H__3C174927_E261_4F3E_8CFC_274E5B8E40FE__INCLUDED_)
