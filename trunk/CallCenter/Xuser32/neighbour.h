//
// Neighbour.h
//

#if !defined(AFX_NEIGHBOUR_H__64583D81_A0AD_4988_9E63_35B373122B2A__INCLUDED_)
#define AFX_NEIGHBOUR_H__64583D81_A0AD_4988_9E63_35B373122B2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNeighbour
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

// Operations
public:
	BOOL			Connect();
	void			Disconnect();
	BOOL			IsConnected() const;
protected:
	void			StartThread();
	void			StopThread();
	BOOL			CreateNeighbour(SOCKET hSocket, SOCKADDR_IN* pHost);
protected:
	static	UINT	ThreadStart(LPVOID pParam);
	virtual	BOOL	OnRun();
	virtual	BOOL	AcceptNeighbour();
};

extern CNeighbour Neighbour;

#endif // !defined(AFX_NEIGHBOUR_H__64583D81_A0AD_4988_9E63_35B373122B2A__INCLUDED_)
