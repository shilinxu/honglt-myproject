//
// HttpClient.h
//

#if !defined(AFX_HTTPCLIENT_H__2371332D_DDC1_441C_8D08_1AC651D32446__INCLUDED_)
#define AFX_HTTPCLIENT_H__2371332D_DDC1_441C_8D08_1AC651D32446__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Connection.h"

class CPacketBase;
enum HttpState;

class CHttpClient : public CConnection  
{
// Construction
public:
	CHttpClient(SOCKET hSocket, SOCKADDR_IN* pHost);
	virtual ~CHttpClient();
	CHttpClient();
	
// Attributes
protected:
	CMapStringToString	m_pResponseHeaders;
protected:
	HttpState		httpState;
	
// Operations
public:
	BOOL			IsConnected() const;
	virtual BOOL	ConnectTo(LPCTSTR pAddress, int nPort);
	virtual void	Disconnect();
	virtual BOOL	OnRun();
protected:
	virtual BOOL	OnRead();
	virtual BOOL	OnHandshake();
	virtual void	OnDropped(BOOL bError);
	virtual	BOOL	OnPacket(CPacketBase* pPacket);
protected:
	void			SetState(HttpState htState);
	int				ReadBuffer(HttpState& htState);
	virtual BOOL	SendPacket(CPacketBase* pPacket);
};

enum HttpState
{
	htsNull, htsReady,
	htsRequesting,
	htsHeader,
	htsContent,
	htsComplete,
};

#endif // !defined(AFX_HTTPCLIENT_H__2371332D_DDC1_441C_8D08_1AC651D32446__INCLUDED_)
