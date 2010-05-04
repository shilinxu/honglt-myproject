//
// Channel.h
//

#if !defined(AFX_CHANNEL_H__BC4D8D18_12C5_4D14_93EC_BFCC26D1EBD3__INCLUDED_)
#define AFX_CHANNEL_H__BC4D8D18_12C5_4D14_93EC_BFCC26D1EBD3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//
// Configuration
//

#define MAX_PHONE			21
#define MAX_BUFFER			256

#include "Transfer.h"
#include "PacketBase.h"

class CChannel : public CTransfer
{
// Construction
public:
	CChannel(LPCTSTR xUserId);
	CChannel(SOCKET hSocket, SOCKADDR_IN* pHost);
	virtual ~CChannel();

// Attributes
public:
	LPTSTR			m_xUserId;
protected:
	LPTSTR			m_xRemoteId;

// Operations
public:
	virtual void	Setup();
	inline	void	Release();
	virtual void	Disconnect();
protected:
	virtual void	Disconnect(DWORD);
	CString			GetRecordFile(LPCTSTR fMask) const;
public:
	virtual HRESULT	RenderBuffer(BYTE* pBuffer, DWORD nLength, BYTE nType);
	virtual HRESULT	DeliverBuffer(BYTE* pBuffer, DWORD nLength, BYTE nType, DWORD);
	static	HRESULT CALLBACK DeliverBuffer(BYTE* pBuffer, DWORD nBuffer, BYTE nType, DWORD nTimestamp, LPVOID lv);
	static	HRESULT CALLBACK RenderBuffer(BYTE* pBuffer, DWORD nBuffer, BYTE nType, DWORD, LPVOID lv);
protected:
	virtual void	OnHandshake();
	virtual void	Clone(CChannel* pParent);
protected:
	virtual BOOL	OnPacket(CPacket* pPacket);
	virtual BOOL	OnSyncOrderRelation(CXMLElement* pXML);
	virtual BOOL	OnSyncHostView(CXMLElement* pXML);
protected:
	virtual BOOL	OnFaultResp(int nFaultCode);
	virtual BOOL	OnSyncHostView(int nResult);
protected:
	virtual CPacket*	ReadBuffer(CBuffer* pBuffer);
	int					OnReadUser(LPCTSTR sUserId, CString& sUserPwd);
	int					OnReadGroup(LPCTSTR sUserId, CString& sUserPwd);
	
	friend class CNetwork;
	friend class CCircuitry;
};

// inline access

void CChannel::Release()
{
	if ( this == NULL ) return;
	
	Disconnect();
}

#endif // !defined(AFX_CHANNEL_H__BC4D8D18_12C5_4D14_93EC_BFCC26D1EBD3__INCLUDED_)
