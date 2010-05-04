//
// Network.h
//

#if !defined(AFX_NETWORK_H__0C616F1D_3108_424C_AA95_7FD3135A3AD7__INCLUDED_)
#define AFX_NETWORK_H__0C616F1D_3108_424C_AA95_7FD3135A3AD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"

class CNetwork  
{
// Construction
public:
	CNetwork();
	virtual ~CNetwork();

// Attributes
public:
	SOCKET			m_hSocket;
	CEvent			m_pWakeup;
	CMutex			m_pSection;
	BOOL			m_bConnected;
protected:
	HANDLE			m_hThread;
	volatile BOOL	m_bThread;
public:
	CMapStringToPtr	m_pList;

// Operations
public:
	BOOL			Connect();
	void			Disconnect();
	BOOL			IsConnected() const;
	int				IsWellConnected() const;
public:
	CChannel*		FindChannel(LPCTSTR xUserId) const;
	CChannel*		SetChannel(CChannel* pChannel);
protected:
	void			StartThread();
	void			StopThread();
	BOOL			CreateNeighbour(SOCKET hSocket, SOCKADDR_IN* pHost);
protected:
	static	UINT	ThreadStart(LPVOID pParam);
	virtual	BOOL	OnRun();
	virtual	BOOL	AcceptNeighbour();
	virtual BOOL	RunNeighbours();
	static int CALLBACK AcceptCheck(IN LPWSABUF lpCallerId, IN LPWSABUF lpCallerData, IN OUT LPQOS lpSQOS, IN OUT LPQOS lpGQOS, IN LPWSABUF lpCalleeId, OUT LPWSABUF lpCalleeData, OUT GROUP FAR * g, IN DWORD dwCallbackData);
	
	friend class CChannel;
};

extern CNetwork Network;

#endif // !defined(AFX_NETWORK_H__0C616F1D_3108_424C_AA95_7FD3135A3AD7__INCLUDED_)
