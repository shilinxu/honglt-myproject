//
// Datagrams.h
//

#if !defined(AFX_DATAGRAMS_H__289BAC8C_A04C_4CB6_A185_ED043CA21CDA__INCLUDED_)
#define AFX_DATAGRAMS_H__289BAC8C_A04C_4CB6_A185_ED043CA21CDA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Datagram.h"

class CXMLElement;

class CDatagrams  
{
// Construction
public:
	CDatagrams(BYTE nType = 0);
	virtual ~CDatagrams();

// Attributes
public:
	SOCKET			m_hSocket;
	SOCKADDR_IN		m_pHost;
	CDatagrams*		m_pPrev;
	CDatagrams*		m_pNext;
public:
	volatile BYTE	m_nType;
	volatile DWORD	m_nLinkId;
	CCriticalSection m_pSection;
protected:
	CMap<CString, LPCTSTR, DWORD, DWORD>	m_pList;
	CMap<DWORD, DWORD, CDatagramIn*, CDatagramIn*>	m_pDatagrams;
	CMap<CString, LPCTSTR, CDatagramOut*, CDatagramOut*> m_pDatagramOut;

// Operations
public:
	virtual BOOL	Connect(IN_ADDR* pAddress, WORD nPort);
	virtual void	Disconnect();
	virtual BOOL	Connect(int nPort);
public:
	virtual BOOL	JoinedMirror(LPCTSTR xUserId, DWORD nLinkId);
	virtual BOOL	JoinedMirror(DWORD nLinkId, SOCKADDR_IN* pHost);
	virtual BOOL	JoinedMirror(LPCTSTR xUserId, SOCKADDR_IN* pHost);
	virtual BOOL	LeavedMirror(LPCTSTR xUserId, DWORD);
	virtual BOOL	LeavedMirror(LPCTSTR xUserId);
	virtual BOOL	LeavedMirror(DWORD nLinkId);
public:
	CDatagrams*		SeekNext(BYTE nType);
	virtual BOOL	BroadNext(LPCTSTR xUserId, BYTE nType);
	virtual BOOL	UnbroadNext(LPCTSTR xUserId, BYTE nType);
	virtual CDatagrams*	Attach(CDatagrams* pConnection);
	virtual void	Detach(CDatagrams* pConnection);
	void			Clear();
public:
	virtual BOOL	SendPacket(SOCKADDR_IN* pHost, BYTE* pBuffer, DWORD nLength);
	virtual BOOL	SendPacket(SOCKADDR_IN* pHost, BYTE* pBuffer, DWORD nLength, BYTE nProtocol, BYTE nType = Unknown, BYTE nMarker = 0);
	virtual BOOL	JoinMirror(LPCTSTR xUserId, SOCKADDR_IN* pHost);
	CXMLElement*	FormatXML(CXMLElement* pXML, LPCTSTR xUserId);
	CDatagramOut*	LookupKey(LPCTSTR xUserId, CDatagramOut*& pDG);
	CDatagramIn*	LookupKey(LPCTSTR xUserId);
protected:
	virtual BOOL	OnRun();
	virtual BOOL	OnRead();
	virtual BOOL	TravelMirror();
	virtual BOOL	OnDatagram(SOCKADDR_IN* pHost, BYTE* pBuffer, DWORD nLength);
	virtual BOOL	OnDatagram(SOCKADDR_IN* pHost, G2_UDP_HEADER* pBuffer, DWORD nLength);
	virtual BOOL	OnDatagram(SOCKADDR_IN* pHost, G3_UDP_HEADER* pBuffer, DWORD nLength);
	virtual BOOL	ReadBuffer(SOCKADDR_IN* pHost, G2_UDP_HEADER* pBuffer, DWORD nLength);
	virtual	BOOL	OnSyncMirrorView(CXMLElement* pXML, DWORD nLinkId, SOCKADDR_IN* pHost);
	virtual void	DoPrint(BYTE* pBuffer, DWORD nLength, LPCTSTR lpszHeader);
protected:
	void			CreateRandom();
	WORD			GetRandom(WORD *pnId = NULL) const;
	DWORD			GetRandom(DWORD *pnId = NULL) const;

// List Access
public:
	inline	void	Release()
	{
		if ( this == NULL ) return;
		m_pNext->Release();
		Disconnect();
	}
	
	friend class CNetwork;
	friend class CDatagramIn;
	friend class CDatagramOut;
};

extern CDatagrams Datagrams;

// Reverses the order of 2 bytes, like "12" to "21"
#define SWAP_SHORT(x) ( ( ( (x) & 0xFF00 ) >> 8 ) + ( ( (x) & 0x00FF ) << 8 ) )

// Reverses the order of 4 bytes, like "1234" to "4321"
#define SWAP_LONG(x) ( ( ( (x) & 0xFF000000 ) >> 24 ) + ( ( (x) & 0x00FF0000 ) >> 8 ) + ( ( (x) & 0x0000FF00 ) << 8 ) + ( ( (x) & 0x000000FF ) << 24 ) )

#endif // !defined(AFX_DATAGRAMS_H__289BAC8C_A04C_4CB6_A185_ED043CA21CDA__INCLUDED_)
