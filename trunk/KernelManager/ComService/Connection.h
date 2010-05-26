//
// Connection.h
//

#if !defined(AFX_CONNECTION_H__6D0EE5A1_5B4A_457E_A5E3_50A7D22559D5__INCLUDED_)
#define AFX_CONNECTION_H__6D0EE5A1_5B4A_457E_A5E3_50A7D22559D5__INCLUDED_

#pragma once
AKM_BEGIN_VERSIONED_NAMESPACE_DECL

class CBuffer;

class CConnection
{
// Definitions
public:
	enum Blocking
	{
		Inblocking	= 0,
		Nonblocking	= 1,
	};

// Construction
public:
	CConnection();
	CConnection(SOCKET hSocket, SOCKADDR_IN* pHost);
	virtual ~CConnection();

// Attributes
public:
	SOCKADDR_IN		m_pHost;
	BOOL			m_bConnected;
	DWORD			m_tConnected;
public:
	SOCKET			m_hSocket;
	CBuffer*		m_pInput;
	CBuffer*		m_pOutput;

// Operations
public:
	virtual BOOL	Connect(IN_ADDR* pAddress, WORD nPort);
	virtual BOOL	ConnectTo(SOCKADDR_IN* pHost, Blocking bs = Inblocking);
	virtual BOOL	ConnectTo(IN_ADDR* pAddress, WORD nPort, Blocking bs = Inblocking);
	virtual BOOL	ConnectTo(LPCTSTR pszAddress, WORD nPort, Blocking bs = Inblocking);
public:
	virtual void	AcceptFrom(SOCKET hSocket, SOCKADDR_IN* pHost);
	virtual void	AttachTo(CConnection* pConnection);
	virtual void	Close();
public:
	BOOL			IsConnected() const;
	virtual	BOOL	TimerOut(DWORD tDelta) const;
	static	DWORD	TranslateAddr(LPCTSTR pszAddress);
protected:
	virtual BOOL	OnRun();
	virtual BOOL	OnRead();
	virtual BOOL	OnWrite();
	virtual BOOL	OnConnected();
	virtual BOOL	OnAccepted();
	virtual void	OnDropped(int nError);
	virtual void	DoPrint(BYTE* pBuffer, DWORD nLength, LPCTSTR lpszHeader);
};

AKM_END_VERSIONED_NAMESPACE_DECL

#endif // !defined(AFX_CONNECTION_H__6D0EE5A1_5B4A_457E_A5E3_50A7D22559D5__INCLUDED_)
