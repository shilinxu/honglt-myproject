//
// Datagram.cpp
//

#include "StdAfx.h"
#include "Xgateway.h"
#include "Datagram.h"
#include "Datagrams.h"
#include "XML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define TEMP_BUFFER		1024

//////////////////////////////////////////////////////////////////////
// CDatagramOut construction

CDatagramOut::CDatagramOut(SOCKADDR_IN* pHost, CDatagrams* pParent) : m_pParent( pParent )
{
	m_tActiveTest	= 0;
	
	Create( pHost, pParent );
}

CDatagramOut::~CDatagramOut()
{
	Destroy();
}

//////////////////////////////////////////////////////////////////////
// CDatagramOut prepare to handle a datagram

void CDatagramOut::Create(SOCKADDR_IN* pHost, CDatagrams* pParent)
{
	m_pParent	= pParent;
	m_tActiveTest= GetTickCount();
	
	CopyMemory( &m_pHost, pHost, sizeof(SOCKADDR_IN) );
	
	m_nSequenceNumber	= pParent->GetRandom(&m_nSequenceNumber);
	m_nTimestamp		= pParent->GetRandom(&m_nTimestamp);
}

void CDatagramOut::Destroy()
{
	ASSERT( m_pParent != NULL );
	
	m_pParent->SendPacket( &m_pHost, NULL, 0, G2_PROTOCOL, BYE );
}

void CDatagramOut::IncrementTimestamp(DWORD nTimestampinc)
{
	m_nTimestamp += nTimestampinc;
}

BOOL CDatagramOut::SendPayload(SOCKADDR_IN* pHost, RTPPayload* pPayload)
{
	ASSERT( m_pParent != NULL );
	if ( ! pPayload ) return TRUE;
	
	if ( ! pHost ) pHost = &m_pHost;
	pPayload->nSequenceNumber= SWAP_SHORT(m_nSequenceNumber++);
	
	DWORD nLength = pPayload->PayloadBuffer.m_nLength;
	BYTE* pBuffer = new BYTE[ sizeof(G3_UDP_HEADER) + nLength ];
	
	CopyMemory( pBuffer, (G3_UDP_HEADER*) pPayload, sizeof(G3_UDP_HEADER) );
	CopyMemory( pBuffer + sizeof(G3_UDP_HEADER), pPayload->PayloadBuffer.m_pBuffer, nLength );
	
	BOOL bSuccess = m_pParent->SendPacket(pHost, pBuffer, sizeof(G3_UDP_HEADER) + nLength);
	delete pBuffer;
	
	return bSuccess ? SendPayload(pHost, pPayload->PayloadNext) : bSuccess;
}

BOOL CDatagramOut::OnRun()
{
	if ( GetTickCount() - m_tActiveTest > 5* 1000 )
	{
#ifndef _DEBUG
		OnSyncTestView( &m_pHost ); return TRUE;
#endif
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CDatagramOut datagram handler

BOOL CDatagramOut::OnSyncTestView(SOCKADDR_IN* pHost)
{
	ASSERT( m_pParent != NULL );
	m_tActiveTest= GetTickCount();
	
	return m_pParent->SendPacket( pHost, NULL, 0, G2_PROTOCOL, TEST );
}

/////////////////////////////////////////////////////////////////////////////
// CDatagramOut render buffer

HRESULT CDatagramOut::RenderBuffer(BYTE* pBuffer, DWORD nLength, BYTE nType, DWORD nTimestampinc,
								  HRESULT (CALLBACK* lpfnRender)(BYTE*, DWORD, BYTE, DWORD, LPVOID), LPVOID lv)
{
	RTPPayload* pPayload = LoadBuffer(pBuffer, nLength, G3_PAYLOAD, nType);
	
	for ( RTPPayload* pPayload2 = pPayload; pPayload2; pPayload2 = pPayload )
	{
		pPayload = pPayload->PayloadNext; pPayload2->PayloadNext = NULL;
		
		pPayload2->nLinkId = SWAP_LONG(m_pParent->m_nLinkId);
		pPayload2->nTimestamp	= SWAP_LONG(m_nTimestamp);
		
		CBuffer& pBuffer2 = pPayload2->PayloadBuffer;
		
		if ( lpfnRender ) lpfnRender( pBuffer2.m_pBuffer, pBuffer2.m_nLength,
			pPayload2->nType, SWAP_LONG(pPayload2->nTimestamp), lv );
		
		SendPayload( &m_pHost, pPayload2 );
		delete pPayload2;
	}
	
	IncrementTimestamp( nTimestampinc );
	
	return S_OK;
}

HRESULT CDatagramOut::PostBuffer(BYTE* pBuffer, DWORD nLength, BYTE nType, DWORD nTimestamp, LPVOID lv)
{
	RTPPayload* pPayload = LoadBuffer(pBuffer, nLength, G3_PAYLOAD, nType);
	
	CDatagramIn* pDG = (CDatagramIn *)lv;
	pPayload->SetAttribute( pDG->m_nLinkId, nTimestamp );
	
	SendPayload( &m_pHost, pPayload );
	delete pPayload;
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CDatagramOut load buffer

RTPPayload* CDatagramOut::LoadBuffer(BYTE* pBuffer, DWORD nBuffer, BYTE nProtocol, BYTE nType)
{
	RTPPayload* pPayload = NULL;
	RTPPayload** pPayload2 = &pPayload;
	
	for ( int nLength = min( ( nBuffer & 0xFFFFF ), TEMP_BUFFER );
		nLength > 0; nLength = min( ( nBuffer & 0xFFFFF ), TEMP_BUFFER ) )
	{
		(* pPayload2) = new RTPPayload;
		(* pPayload2)->nProtocol= nProtocol;
		
		(* pPayload2)->nType	= nType;
		(* pPayload2)->nMarker	= ( nBuffer - nLength ) > 0 ? FALSE : TRUE;
		
		(* pPayload2)->PayloadBuffer.Add( pBuffer, nLength );
		
		pPayload2 = &(* pPayload2)->PayloadNext;
		pBuffer = (BYTE*)pBuffer + nLength; nBuffer -= nLength;
	}
	
	return pPayload;
}

//////////////////////////////////////////////////////////////////////
// CDatagramIn construction

CDatagramIn::CDatagramIn(SOCKADDR_IN* pHost, CDatagrams* pParent, DWORD nLinkId) : m_nLinkId(nLinkId)
{
	m_pParent		= NULL;
	m_tActiveTest	= 0;
	
	ZeroMemory( m_pDatagram, sizeof(RTPPayload*) * MAX_DATALST );
	m_nDataCookie	= 0;
	
	Create( pHost, pParent );
}

CDatagramIn::~CDatagramIn()
{
	PurgeBuffer();
}

//////////////////////////////////////////////////////////////////////
// CDatagramIn prepare to handle a datagram

void CDatagramIn::Create(SOCKADDR_IN* pHost, CDatagrams* pParent)
{
	m_pParent	= pParent;
	m_tActiveTest= GetTickCount();
	
	CopyMemory( &m_pHost, pHost, sizeof(SOCKADDR_IN) );
	
	ZeroMemory( m_pDatagram, sizeof(RTPPayload*) * MAX_DATALST );
	m_nDataCookie	= 0;
}

BOOL CDatagramIn::OnRun(LPCTSTR xUserId)
{
	if ( GetTickCount() - m_tActiveTest > 15* 1000 )
	{
#ifndef _DEBUG
		OnSyncByeView( m_nLinkId ); return FALSE;
#endif
	}
	
	return TRUE;
}

CXMLElement* CDatagramIn::FormatXML(CXMLElement* pXML)
{
	CXMLElement* pSync = pXML->AddElement( "IP" );
	pSync->SetValue( inet_ntoa(m_pHost.sin_addr) );
	
	CString strValue;
	strValue.Format( "%i", ntohs(m_pHost.sin_port) );
	
	pSync = pXML->AddElement( "Port" );
	pSync->SetValue( strValue );
	
	return pXML;
}

//////////////////////////////////////////////////////////////////////
// CDatagramIn datagram handler

BOOL CDatagramIn::OnDatagram(SOCKADDR_IN* pHost, G2_UDP_HEADER* pBuffer, DWORD nLength)
{
	ASSERT ( pBuffer->nProtocol == G2_PROTOCOL );
	
	if ( nLength < sizeof(G2_UDP_HEADER) ) return FALSE;
	
	if ( pBuffer->nType == TEST )
	{
		return OnSyncTestView(pHost, SWAP_LONG(pBuffer->nLinkId));
	}
	else if ( pBuffer->nType == BYE )
	{
		return OnSyncByeView(SWAP_LONG(pBuffer->nLinkId));
	}
	
	return TRUE;
}

BOOL CDatagramIn::OnSyncTestView(SOCKADDR_IN* pHost, DWORD nLinkId)
{
	ASSERT( nLinkId == m_nLinkId );
	m_tActiveTest= GetTickCount();
	
	return TRUE;
}

BOOL CDatagramIn::OnSyncByeView(DWORD nLinkId)
{
	ASSERT( m_pParent != NULL );
	ASSERT( nLinkId == m_nLinkId );
	
	return m_pParent->LeavedMirror( nLinkId );
}

/////////////////////////////////////////////////////////////////////////////
// CDatagramIn deliver buffer

BOOL CDatagramIn::OnDatagram(SOCKADDR_IN* pHost, G3_UDP_HEADER* pBuffer, DWORD nLength)
{
	RTPPayload* pPayload = m_pDatagram[ m_nDataCookie ];
	
	if ( DeliverBuffer( pBuffer, nLength, pPayload ? pPayload : m_pDatagram[ m_nDataCookie ] ) )
	{
		delete m_pDatagram[ ++m_nDataCookie %= MAX_DATALST ];
		m_pDatagram[ m_nDataCookie % MAX_DATALST ] = pPayload;
	}
	
	return OnSyncTestView(NULL, SWAP_LONG(pBuffer->nLinkId));
}

BOOL CDatagramIn::DeliverBuffer(G3_UDP_HEADER* pBuffer, DWORD nLength, RTPPayload*& pPayload)
{
	BOOL bNext = FALSE;
	for ( ; pPayload && pPayload->PayloadNext ; pPayload = pPayload->PayloadNext );
	
	if ( ! pPayload || pBuffer->nTimestamp != pPayload->nTimestamp )
	{
		if ( pPayload ) bNext = TRUE;
		pPayload = new RTPPayload; pPayload->PayloadNext = NULL;
		
		CopyMemory( (G3_UDP_HEADER*)pPayload, (BYTE*)pBuffer, sizeof(G3_UDP_HEADER) );
	}
	
	pPayload->nMarker = pBuffer->nMarker;
	pPayload->PayloadBuffer.Add( (BYTE*)(pBuffer + 1), nLength - sizeof(G3_UDP_HEADER) );
	
	return bNext;
}

/////////////////////////////////////////////////////////////////////////////
// CDatagramIn render buffer
/*
HRESULT CDatagramIn::RenderBuffer(int& nIndex, HRESULT (CALLBACK* lpfnRender)(RTPPayload*, LPVOID), LPVOID lv)
{
	if ( ( nIndex %= MAX_DATALST ) == m_nDataCookie ) return S_OK;
	
	if ( lpfnRender ) lpfnRender( m_pDatagram[ nIndex ], lv );
	return RenderBuffer( ++nIndex %= MAX_DATALST, lpfnRender, lv );
}
*/
HRESULT CDatagramIn::RenderBuffer(int& nIndex, HRESULT (CALLBACK* lpfnRender)(BYTE*, DWORD, BYTE, DWORD, LPVOID), LPVOID lv)
{
	if ( ( nIndex %= MAX_DATALST ) == m_nDataCookie ) return S_OK;
	
	RTPPayload* pBuffer = m_pDatagram[ nIndex ];
	if ( ! pBuffer ) return S_FALSE;
	
	CBuffer& pBuffer2 = pBuffer->PayloadBuffer;
	
	if ( lpfnRender ) lpfnRender( pBuffer2.m_pBuffer, pBuffer2.m_nLength,
		pBuffer->nType, SWAP_LONG(pBuffer->nTimestamp), lv );
	return RenderBuffer( ++nIndex %= MAX_DATALST, lpfnRender, lv );
}

HRESULT CDatagramIn::PurgeBuffer()
{
	CSingleLock pLock( &m_pParent->m_pSection, TRUE );
	
	for ( int nAt = 0; nAt < MAX_DATALST; nAt++ )
	{
		delete m_pDatagram[ nAt ]; m_pDatagram[ nAt ] = NULL;
	}
	
	m_nDataCookie	= 0;
	return S_OK;
}
