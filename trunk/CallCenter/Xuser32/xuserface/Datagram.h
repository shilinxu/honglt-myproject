//
// Datagram.h
//

#if !defined(AFX_DATAGRAM_H__72C119CA_1D67_4B22_A9DF_51398F17B756__INCLUDED_)
#define AFX_DATAGRAM_H__72C119CA_1D67_4B22_A9DF_51398F17B756__INCLUDED_

#pragma once

struct RTPPayload;

#pragma pack(1)
typedef struct
{
	BYTE	nProtocol;
	BYTE	nType : 7;
	BYTE	nMarker : 1;
	DWORD	nLinkId;
} G2_UDP_HEADER;

typedef struct : G2_UDP_HEADER
{
	WORD	nSequenceNumber;
	DWORD	nTimestamp;
} G3_UDP_HEADER;
#pragma pack()

enum PacketType 
{ 
	BYE,
	APP,
	TEST,
	Unknown
};

#define G2_PROTOCOL	0x5F
#define G3_PAYLOAD	0xF6

#define MAX_DATALST	0x10

class CBuffer;
class CDatagrams;
class CXMLElement;

class CDatagramOut
{
// Construction
public:
	CDatagramOut(SOCKADDR_IN* pHost, CDatagrams* pParent);
	virtual ~CDatagramOut();

// Attributes
public:
	SOCKADDR_IN		m_pHost;
	CDatagrams*		m_pParent;
protected:
	WORD			m_nSequenceNumber;
	DWORD			m_nTimestamp;
private:
	DWORD			m_tActiveTest;

// Operations
public:
	virtual void	Create(SOCKADDR_IN* pHost, CDatagrams* pParent);
	virtual void	Destroy();
public:
	virtual void	IncrementTimestamp(DWORD nTimestampinc);
	virtual BOOL	SendPayload(SOCKADDR_IN* pHost, RTPPayload* pPayload);
	virtual HRESULT RenderBuffer(BYTE* pBuffer, DWORD nLength, BYTE nType, DWORD nTimestampinc,
								HRESULT (CALLBACK* lpfnRender)(BYTE*, DWORD, BYTE, DWORD, LPVOID), LPVOID lv);
protected:
	virtual BOOL	OnRun();
	virtual BOOL	OnSyncTestView(SOCKADDR_IN* pHost);
protected:
	virtual RTPPayload*	LoadBuffer(BYTE* pBuffer, DWORD nBuffer, BYTE nProtocol, BYTE nType);
	
	friend class CDatagrams;
};

class CDatagramIn
{
// Construction
public:
	CDatagramIn(SOCKADDR_IN* pHost, CDatagrams* pParent, DWORD nLinkId);
	virtual ~CDatagramIn();

// Attributes
public:
	SOCKADDR_IN		m_pHost;
	CDatagrams*		m_pParent;
	const DWORD		m_nLinkId;
protected:
	RTPPayload*		m_pDatagram[MAX_DATALST];
	int				m_nDataCookie;
private:
	DWORD			m_tActiveTest;

// Operations
public:
	virtual void	Create(SOCKADDR_IN* pHost, CDatagrams* pParent);
	CXMLElement*	FormatXML(CXMLElement* pParent);
protected:
	virtual BOOL	OnRun(LPCTSTR xUserId);
	virtual BOOL	OnDatagram(SOCKADDR_IN* pHost, G2_UDP_HEADER* pBuffer, DWORD nLength);
	virtual BOOL	OnDatagram(SOCKADDR_IN* pHost, G3_UDP_HEADER* pBuffer, DWORD nLength);
	virtual BOOL	OnSyncTestView(SOCKADDR_IN* pHost, DWORD nLinkId);
	virtual BOOL	OnSyncByeView(DWORD nLinkId);
protected:
	virtual BOOL	DeliverBuffer(G3_UDP_HEADER* pBuffer, DWORD nLength, RTPPayload*& pPayload);
public:
//	virtual HRESULT RenderBuffer(int& nIndex, HRESULT (CALLBACK* lpfnRender)(RTPPayload*, LPVOID), LPVOID lv);
	virtual HRESULT RenderBuffer(int& nIndex, HRESULT (CALLBACK* lpfnRender)(BYTE*, DWORD, BYTE, DWORD, LPVOID), LPVOID lv);
	virtual HRESULT PurgeBuffer();
	
	friend class CDatagrams;
};

#include "Buffer.h"

struct RTPPayload : G3_UDP_HEADER
{
	CBuffer		PayloadBuffer;
	RTPPayload*	PayloadNext;

	RTPPayload(RTPPayload* pPayload = NULL)
	{
		PayloadNext = pPayload;
	}
	
	virtual ~RTPPayload()
	{
		if ( PayloadNext ) delete PayloadNext;
	}
};

#endif // !defined(AFX_DATAGRAM_H__72C119CA_1D67_4B22_A9DF_51398F17B756__INCLUDED_)
