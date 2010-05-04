//
// Channel.h
//

#if !defined(AFX_CHANNEL_H__025B9C73_E4B5_4B21_99BB_5015BB726599__INCLUDED_)
#define AFX_CHANNEL_H__025B9C73_E4B5_4B21_99BB_5015BB726599__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//
// Configuration
//

#define MAX_PHONE			21
#define MAX_BUFFER			256
#define MAX_FILENAME		128
#define MAX_PASSWORD		10
#define MAX_CALLTYPE		4

//
// definitions of call types
//

#define CA_IDLE				"IDLE"	// 空闲
#define CA_TALK				"CATK"	// 主叫
#define CA_HOOK				"CAHK"	// 被叫
#define CA_BACK				"CABK"	// 回呼
#define CA_DEBG				"DEBG"	// 内部测试(保留)

#include "Transfer.h"
#include "PacketBase.h"

class CChannel : public CTransfer  
{
// Construction
public:
	CChannel(SOCKET hSocket, SOCKADDR_IN* pHost);
	CChannel(LPCTSTR xUserId = NULL);
	virtual ~CChannel();

// Attributes
public:
	BOOL			m_bNavigated;
	DWORD			m_tNavigated;
	CHAR			m_xCallType[MAX_CALLTYPE+1];
protected:
	QWORD			m_nTransactionId;
	CXMLElement*	m_pXMLCookie;
	HANDLE			m_hEventResp;
public:
	LPTSTR			m_xUserId;
	DWORD			m_nLastError;
	
// Operations
public:
	virtual void	Setup();
	virtual void	Clone(CChannel* pParent);
	virtual void	Disconnect();
public:
	virtual BOOL	IsNavigated() const;
	virtual void	SetUserId(LPCTSTR xUserId);
	virtual DWORD	CallWorker(CXMLElement** ppXML, DWORD nTimeOut = 6 * 1000);
public:
	virtual BOOL	DialupWith(CXMLElement* pXML);
	virtual BOOL	RingWith(CXMLElement* pXML);
	virtual void	StartTalking();
	virtual BOOL	LeaveTalk();
	virtual BOOL	OnRecordFile();
	virtual BOOL	SendPacket(CPacket* pPacket);
protected:
	virtual BOOL	OnConnected();
	virtual void	OnDropped(int nError);
	virtual void	OnPacketed(CXMLElement* pXML);
protected:
	virtual BOOL	OnPacket(CPacket* pPacket);
	virtual BOOL	OnFaultResp(CXMLElement* pXML);
	virtual BOOL	OnSyncHostFile(CXMLElement* pXML);
protected:
	virtual CPacket*	ReadBuffer(CBuffer* pBuffer);

// inline access
public:
	inline void CChannel::Release()
	{
		if ( this == NULL ) return;
		
		delete this;
	}
	
	inline BOOL CChannel::IsCallType(LPCTSTR xCallType) const
	{
		return _tcscmp( m_xCallType, xCallType ) == 0;
	}
};

#endif // !defined(AFX_CHANNEL_H__025B9C73_E4B5_4B21_99BB_5015BB726599__INCLUDED_)
