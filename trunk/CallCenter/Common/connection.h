//
// Connection.h
//

#if !defined(AFX_CONNECTION_H__6D0EE5A1_5B4A_457E_A5E3_50A7D22559D5__INCLUDED_)
#define AFX_CONNECTION_H__6D0EE5A1_5B4A_457E_A5E3_50A7D22559D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Buffer.h"

class CConnection
{
// Construction
public:
	CConnection();
	CConnection(SOCKET hSocket, SOCKADDR_IN* pHost);
	virtual ~CConnection();

// Attributes
public:
	SOCKADDR_IN	m_pHost;
	CString		m_sAddress;
	BOOL		m_bConnected;
	DWORD		m_tConnected;
public:
	SOCKET		m_hSocket;
	CBuffer*	m_pInput;
	CBuffer*	m_pOutput;

// Operations
public:
	virtual BOOL	Connect(IN_ADDR* pAddress, WORD nPort);
	virtual BOOL	ConnectTo(SOCKADDR_IN* pHost);
	virtual BOOL	ConnectTo(IN_ADDR* pAddress, WORD nPort);
	virtual void	AcceptFrom(SOCKET hSocket, SOCKADDR_IN* pHost);
	virtual void	AttachTo(CConnection* pConnection);
	virtual void	Close();
public:
	BOOL			IsConnected() const;
	CString			ToHex(BYTE* pBuffer, DWORD nLength);
	CString			ToASCII(BYTE* pBuffer, DWORD nLength);
protected:
	virtual BOOL	OnRun();
	virtual BOOL	OnRead();
	virtual BOOL	OnWrite();
	virtual BOOL	OnConnected();
	virtual BOOL	OnAccepted();
	virtual void	OnDropped(int nError);
	virtual void	DoPrint(BYTE* pBuffer, DWORD nLength, LPCTSTR lpszHeader);
	
	friend class CNetwork;
};

#endif // !defined(AFX_CONNECTION_H__6D0EE5A1_5B4A_457E_A5E3_50A7D22559D5__INCLUDED_)
