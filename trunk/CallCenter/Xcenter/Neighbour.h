//
// Neighbour.h
//

#if !defined(AFX_NEIGHBOUR_H__64583D81_A0AD_4988_9E63_35B373122B2A__INCLUDED_)
#define AFX_NEIGHBOUR_H__64583D81_A0AD_4988_9E63_35B373122B2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CChannel;
class CCircuitry;

class AFX_EXT_CLASS CNeighbour
{
// Construction
public:
	CNeighbour();
	virtual ~CNeighbour();

// Attributes
public:
	SOCKET			m_hSocket;
	CEvent			m_pWakeup;
	BOOL			m_bConnected;
	CMutex			m_pSection;
protected:
	HANDLE			m_hThread;
	volatile BOOL	m_bThread;
protected:
	HANDLE			m_hPipe;
	OVERLAPPED		m_pOverlapped;
	CMapStringToPtr	m_pList;
	POSITION		m_pPosCookie;

// Operations
public:
	BOOL			Connect();
	void			Disconnect();
	BOOL			IsConnected() const;
public:
	CCircuitry*		FindChannel(LPCTSTR xUserId) const;
	CCircuitry*		SetChannel(CCircuitry* pChannel);
	CCircuitry*		BroadcastRing(CChannel* pChannel, DWORD nReserved = 0);
	CCircuitry*		ConnectTo(LPCTSTR xUserId, BOOL bPeek = FALSE, DWORD nTimeOut = NMPWAIT_WAIT_FOREVER);
protected:
	virtual	BOOL	Connect(LPCTSTR lpPipeName, DWORD nMaxInstances = PIPE_UNLIMITED_INSTANCES, DWORD nTimeOut = NMPWAIT_WAIT_FOREVER);
protected:
	void			StartThread();
	void			StopThread();
	BOOL			CreateNeighbour(SOCKET hSocket, SOCKADDR_IN* pHost);
	int				ReadUserId(SOCKADDR_IN* pHost);
protected:
	static	UINT	ThreadStart(LPVOID pParam);
	virtual	BOOL	OnRun();
	virtual	BOOL	AcceptNeighbour();
	virtual BOOL	RunNeighbours();
	virtual BOOL	AcceptCircuitry();
	
	friend class CCircuitry;
};

extern AFX_EXT_CLASS CNeighbour Neighbour;

#endif // !defined(AFX_NEIGHBOUR_H__64583D81_A0AD_4988_9E63_35B373122B2A__INCLUDED_)
