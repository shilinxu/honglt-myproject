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
	CChannel();
	virtual ~CChannel();

// Attributes
public:
	CHAR			m_xCallType[MAX_CALLTYPE+1];
protected:
	BOOL			m_bNavigated;
	DWORD			m_tNavigated;
	
// Operations
public:
	virtual void	Setup();
	inline	void	Release();
	virtual void	Clone(CChannel* pParent);
public:
	virtual BOOL	IsNavigated() const;
	inline	BOOL	IsCallType(LPCTSTR xCallType) const;
	virtual void	OnHandshake();
protected:
	virtual void	OnDropped(int nError);
	virtual BOOL	OnPacket(CPacket* pPacket);
	virtual BOOL	OnOkPacket(COkPacket* pPacket);
protected:
	virtual CPacket*	ReadBuffer(CBuffer* pBuffer);
};

// inline access

void CChannel::Release()
{
	if ( this == NULL ) return;
	
	delete this;
}

BOOL CChannel::IsCallType(LPCTSTR xCallType) const
{
	return _tcscmp( m_xCallType, xCallType ) == 0;
}

#endif // !defined(AFX_CHANNEL_H__025B9C73_E4B5_4B21_99BB_5015BB726599__INCLUDED_)
