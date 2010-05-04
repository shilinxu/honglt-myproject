//
// Handshakes.h
//

#if !defined(AFX_HANDSHAKES_H__2314A7BE_5C51_4F8E_A4C6_6059A7621AE0__INCLUDED_)
#define AFX_HANDSHAKES_H__2314A7BE_5C51_4F8E_A4C6_6059A7621AE0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PacketCore.h"
#include "Handshake.h"
class CNetworkWnd;

class CHandshakes
{
// Construction
public:
	CHandshakes();
	virtual ~CHandshakes();

// Attributes
public:
	SOCKET			m_hSocket;
	CEvent			m_pWakeup;
	BOOL			m_bConnected;
protected:
	HANDLE			m_hThread;
	volatile BOOL	m_bThread;
	CPtrList		m_pList;
	CCriticalSection m_pSection;
	CNetworkWnd*	m_pHomeWnd;

// Operations
public:
	void			Clear();
	BOOL			Setup();
	BOOL			Connect();
	void			Disconnect();
	BOOL			IsConnected() const;
//	BOOL			IsAccepted(IN_ADDR* pAddress);
	void			SendPacketTo(CPacket* pPacket);
protected:
	void			StartThread();
	void			StopThread();
	BOOL			CreateHandshake(SOCKET hSocket, SOCKADDR_IN* pHost);
	int				ReadUserId(SOCKADDR_IN* pHost);
protected:
	static	UINT	ThreadStart(LPVOID pParam);
	virtual	BOOL	OnRun();
	virtual	BOOL	AcceptHandshake();
	virtual void	RunHandshakes();
//	static int CALLBACK AcceptCheck(IN LPWSABUF lpCallerId, IN LPWSABUF lpCallerData, IN OUT LPQOS lpSQOS, IN OUT LPQOS lpGQOS, IN LPWSABUF lpCalleeId, OUT LPWSABUF lpCalleeData, OUT GROUP FAR * g, IN DWORD dwCallbackData);
	
// List Access
public:
	inline POSITION GetIterator() const
	{
		return m_pList.GetHeadPosition();
	}

	inline CHandshake* GetNext(POSITION& pos) const
	{
		return (CHandshake*)m_pList.GetNext( pos );
	}

	inline int GetCount() const
	{
		return m_pList.GetCount();
	}
	
	friend class CHandshake;
	friend class CNeighbour;
};

extern CHandshakes Handshakes;

#endif // !defined(AFX_HANDSHAKES_H__2314A7BE_5C51_4F8E_A4C6_6059A7621AE0__INCLUDED_)
