//
// Neighbour.h
//

#if !defined(AFX_NEIGHBOUR_H__C6467955_2EF7_412B_BE2E_B7804BBA04A6__INCLUDED_)
#define AFX_NEIGHBOUR_H__C6467955_2EF7_412B_BE2E_B7804BBA04A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCircuitry;
class CNeighbour
{
// Construction
public:
	CNeighbour();
	virtual ~CNeighbour();

// Attributes
public:
	CEvent			m_pWakeup;
	CMutex			m_pSection;
	BOOL			m_bConnected;
protected:
	HANDLE			m_hThread;
	volatile BOOL	m_bThread;
protected:
	HANDLE			m_hPipe;
	OVERLAPPED		m_pOverlapped;
	CMapStringToPtr	m_pList;
	
// Operations
public:
	BOOL			Connect();
	void			Disconnect();
	BOOL			IsConnected() const;
public:
	CCircuitry*		FindChannel(LPCTSTR xUserId) const;
	CCircuitry*		SetChannel(CCircuitry* pChannel);
	CCircuitry*		ConnectTo(LPCTSTR xUserId, DWORD nTimeOut = NMPWAIT_WAIT_FOREVER);
protected:
	virtual	BOOL	Connect(LPCTSTR lpPipeName, DWORD nMaxInstances = PIPE_UNLIMITED_INSTANCES, DWORD nTimeOut = NMPWAIT_WAIT_FOREVER);
protected:
	void			StartThread();
	void			StopThread();
protected:
	virtual	BOOL	OnRun();
	virtual BOOL	RunNeighbours();
	static	UINT	ThreadStart(LPVOID pParam);
	virtual BOOL	AcceptCircuitry();
	
	friend class CCircuitry;
};

extern CNeighbour Neighbour;

#endif // !defined(AFX_NEIGHBOUR_H__C6467955_2EF7_412B_BE2E_B7804BBA04A6__INCLUDED_)
