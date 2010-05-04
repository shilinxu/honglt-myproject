//
// Traffic.cpp
//

#include "stdafx.h"
#include "Xgateway.h"
#include "Traffic.h"
#include "Neighbour.h"
#include "Circuitry.h"
#include "Network.h"
#include "XML.h"

#include "RTPSessionParams.h"
#include "RTPUDPv4Transmitter.h"
#include "RTPSourceData.h"

#include "rtpipv4address.h"
#include "rtcpapppacket.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define TEMP_BUFFER		1024
#define RTPFILE_PORT	16666

CTraffic Traffic;
RTPSessionLayer RTPAudio;
RTPSessionLayer RTPVideo;

//////////////////////////////////////////////////////////////////////
// CTraffic construction

CTraffic::CTraffic()
{
}

CTraffic::~CTraffic()
{
	Clear();
}

void CTraffic::Clear()
{
	for ( POSITION pos = m_pList.GetStartPosition(); pos; )
	{
		CString xUserId;
		RTPSession* pChannel;
		
		m_pList.GetNextAssoc( pos, xUserId, (void*&)pChannel );
		
		if ( pChannel ) delete pChannel;
	}
	
	m_pList.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
// CTraffic operations

int CTraffic::RenderFile(LPCTSTR pszName, WORD nPort, DWORD nSamplesPer)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	RTPSessionLayer* pRTPSession = FindChannel( pszName ); if ( ! pRTPSession )
	{
		pRTPSession = new RTPSessionLayer; SetChannel( pRTPSession, pszName );
	}
	
	return pRTPSession->RenderFile( nPort, nSamplesPer );
}

void CTraffic::ReleaseFile(LPCTSTR pszName)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	if ( RTPSessionLayer* pRTPSession = FindChannel(pszName) )
	{
		m_pList.RemoveKey( pszName );
		delete pRTPSession;
	}
}

RTPSessionLayer* CTraffic::SetChannel(RTPSessionLayer* pChannel, LPCTSTR xUserId)
{
	RTPSessionLayer* pChannel2 = FindChannel( xUserId );
	if ( pChannel2 ) delete pChannel2;
	
	m_pList.SetAt( xUserId, pChannel ); 
	
	return pChannel;
}

RTPSessionLayer* CTraffic::FindChannel(LPCTSTR xUserId) const
{
	RTPSessionLayer* pChannel = NULL;
	m_pList.Lookup( xUserId, (void*&)pChannel );
	
	return (RTPSessionLayer*)pChannel;
}

//////////////////////////////////////////////////////////////////////
// RTPSessionLayer construction

RTPSessionLayer::RTPSessionLayer()
{
	m_pTransmitter	= NULL;
}

RTPSessionLayer::~RTPSessionLayer()
{
	BYEDestroy( RTPTime(5,0), 0, 0 );
	
	if ( m_pTransmitter )
	RTPDelete(m_pTransmitter,GetMemoryManager());
	
	Clear();
}

void RTPSessionLayer::Clear()
{
	for ( POSITION pos = m_pDatagrams.GetStartPosition(); pos; )
	{
		UINT ssrc;
		RTPPayload* pBuffer;
		
		m_pDatagrams.GetNextAssoc( pos, ssrc, pBuffer );
		if ( pBuffer ) delete pBuffer;
	}
	
	m_pDatagrams.RemoveAll();
	m_pList.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// RTPSessionLayer attributes

CXMLElement* RTPSessionLayer::FormatRTP(LPCTSTR xUserId, CXMLElement* pXML)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	UINT ssrc;
	if ( ! m_pList.Lookup(xUserId, ssrc) ) return NULL;
	
	RTPSourceData* srcdat = GetSourceInfo( ssrc );
	if ( ! srcdat ) return NULL;
	
	if ( ! srcdat->IsRTPAddressSet() || ! srcdat->IsRTCPAddressSet() ) return NULL;
	
	FormatRTP( pXML, srcdat, TRUE );
	FormatRTP( pXML, srcdat, FALSE );
	
	return pXML;
}

CXMLElement* RTPSessionLayer::FormatRTP(CXMLElement* pParent, RTPSourceData* srcdat, BOOL bRTP)
{
	CXMLElement* pXML = pParent->AddElement( bRTP ? _T("RTP") : _T("RTCP") );
	
	const RTPAddress* addr = bRTP ? srcdat->GetRTPDataAddress() :
									srcdat->GetRTCPDataAddress();
	if ( addr )
	{
		ASSERT( addr->GetAddressType() == RTPAddress::IPv4Address );
		RTPIPv4Address *addr2 = (RTPIPv4Address *)addr;
		
		in_addr address;
		address.s_addr = htonl( addr2->GetIP() ); 
		
		pXML->AddAttribute( "IP", inet_ntoa(address) );
		
		CString strValue;
		strValue.Format( "%i", addr2->GetPort() );
		
		pXML->AddAttribute( "Port", strValue );
	}
	
	return pXML;
}

BOOL RTPSessionLayer::JoinedMirror(LPCTSTR xUserId, UINT ssrc)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	if ( m_pList.Lookup(xUserId, ssrc) ) return TRUE;
	
	m_pList.SetAt( xUserId, ssrc );
TRACE( "debug[%s]: SetAt [%u].\n", xUserId, ssrc);
	
	RTPPayload* pPayload;
	if ( m_pDatagrams.Lookup(ssrc, pPayload) ) return TRUE;
	
	m_pDatagrams.SetAt( ssrc, NULL );
#ifdef _DEBUG
	if ( RTPSourceData* srcdat = GetSourceInfo( ssrc ) )
	{
		const RTPAddress* addr = srcdat->GetRTCPDataAddress();
		if ( addr )
		{
			ASSERT( addr->GetAddressType() == RTPAddress::IPv4Address );
			RTPIPv4Address *addr2 = (RTPIPv4Address *)addr;
			
			in_addr address;
			address.s_addr = htonl( addr2->GetIP() ); 
			
	theApp.Message( MSG_TEMP, "JoinedMirror[%s]: From %s:%i", xUserId, inet_ntoa(address),addr2->GetPort());
		}
	}
#endif
	return TRUE;
}

BOOL RTPSessionLayer::LeavedMirror(LPCTSTR xUserId, UINT ssrc)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
//	ASSERT( m_pList.Lookup(xUserId, ssrc) );
	if ( ! ssrc ) m_pList.Lookup( xUserId, ssrc );
	
	m_pList.RemoveKey( xUserId );
TRACE( "debug[%s]: RemoveKey [%u].\n", xUserId, ssrc);
		
	RTPPayload* pPayload;
	if ( m_pDatagrams.Lookup(ssrc, pPayload) )
	{
		m_pDatagrams.RemoveKey( ssrc );
		delete pPayload;
	}
	
	return TRUE;
}

BOOL RTPSessionLayer::LookupKey(UINT ssrc, CString& xUserId)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	for ( POSITION pos  = m_pList.GetStartPosition(); pos; )
	{
		UINT ssrc2;
		m_pList.GetNextAssoc( pos, xUserId, ssrc2 );
		
		if ( ssrc == ssrc2 ) return TRUE;
	}
	
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// RTPSessionLayer operations

int RTPSessionLayer::RenderFile(WORD nPort, DWORD nSamplesPer)
{
	RTPSessionParams SessParams;
	
	SessParams.SetOwnTimestampUnit( 1.0 / (double)nSamplesPer );
	SessParams.SetAcceptOwnPackets( true );
	
	RTPUDPv4TransmissionParams TransParams;
	TransParams.SetPortbase( nPort );
	
#if 0
	return Create( SessParams, &TransParams );
#else
	RTPUDPv4Transmitter* pTransmitter = new RTPUDPv4Transmitter(GetMemoryManager());
	
	int status;
	if ((status = pTransmitter->Init(SessParams.IsUsingPollThread())) < 0)
	{
		RTPDelete(pTransmitter,GetMemoryManager());
		return status;
	}
	if ((status = pTransmitter->Create(SessParams.GetMaximumPacketSize(),&TransParams)) < 0)
	{
		RTPDelete(pTransmitter,GetMemoryManager());
		return status;
	}
	
	return Create( SessParams, m_pTransmitter = pTransmitter );
#endif
}

int RTPSessionLayer::SendPayload(const void *pBuffer, size_t nBuffer, BYTE nPayloadType, DWORD nTimestampinc)
{
	BeginDataAccess();
	
	for ( int nLength = min( ( nBuffer & 0xFFFFF ), TEMP_BUFFER );
		nLength > 0; nLength = min( ( nBuffer & 0xFFFFF ), TEMP_BUFFER ) )
	{
		bool cMark = ( nBuffer - nLength ) > 0 ? false : true;
		RTPSession::SendPacket( pBuffer, nLength, nPayloadType, cMark, 0 );
		
		pBuffer = (BYTE*)pBuffer + nLength; nBuffer -= nLength;
	}
	
	IncrementTimestamp( nTimestampinc );
	
	EndDataAccess();
	return 0;
}

int RTPSessionLayer::JoinMirror(RTPSourceData* srcdat, LPCTSTR xUserId)
{
	const RTPAddress* addr1 = srcdat->GetRTPDataAddress();
	const RTPAddress* addr2 = srcdat->GetRTCPDataAddress();
	
	CXMLElement* pXML = new CXMLElement( NULL, "SyncMirrorView" );
	pXML->AddAttribute( "method", "mirror" );
	
	CXMLElement* pSync = pXML->AddElement( "UserId" );
	pSync->SetValue( xUserId );
	
	CString strXML = pXML->ToString( FALSE, TRUE );
	delete pXML;
	
	m_pTransmitter->AddDestination( *addr1, *addr2 );
	
	int nXMLLength = strXML.GetLength();
	SendRTCPAPPPacket( 0, (BYTE*)"TEST", (LPCTSTR)strXML, (nXMLLength + 3) & 0xFFFC );
	
	m_pTransmitter->DeleteDestination( *addr1, *addr2 );
#ifdef _DEBUG
	{
		ASSERT( addr2->GetAddressType() == RTPAddress::IPv4Address );
		RTPIPv4Address *addr2v4 = (RTPIPv4Address *)addr2;
		
		in_addr address;
		address.s_addr = htonl( addr2v4->GetIP() ); 
		
	theApp.Message( MSG_TEMP, "JoinMirror[%s]: to %s:%i", xUserId, inet_ntoa(address),addr2v4->GetPort());
	}
#endif
	return 0;
}

int RTPSessionLayer::JoinMirrorWith(LPCTSTR xUserId)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	UINT ssrc;
	if ( ! m_pList.Lookup(xUserId, ssrc) ) return -1;
	
	RTPSourceData* srcdat = GetSourceInfo( ssrc );
	if ( ! srcdat || ! srcdat->IsRTCPAddressSet() ) return -1;
	
	if ( ! srcdat->IsRTPAddressSet() ) return -1;
	
	return JoinMirror( srcdat, _T("Xgateway") );
}

//////////////////////////////////////////////////////////////////////
// RTPSessionLayer thread run

void RTPSessionLayer::OnPollThreadStep()
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	for ( POSITION pos = m_pDatagrams.GetStartPosition(); pos; )
	{
		UINT ssrc;
		RTPPayload* pPayload;
		
		m_pDatagrams.GetNextAssoc( pos, ssrc, pPayload );
		
		CString xUserId;
		if ( LookupKey(ssrc, xUserId) )
		{
			if ( pPayload )	RenderBuffer( m_pDatagrams[ ssrc ], xUserId );
		}
	}
}

void RTPSessionLayer::OnNewSource(RTPSourceData *srcdat)
{
//TRACE("%u-> On New: %u\n", GetTickCount(),srcdat->GetSSRC() );
	if ( srcdat->IsOwnSSRC() ) return;
	
	CString xUserId;
	if ( ! LookupKey(srcdat->GetSSRC(), xUserId) ) return;
}

void RTPSessionLayer::OnRemoveSource(RTPSourceData *srcdat)
{
//TRACE("%u-> On Remove: %u\n", GetTickCount(),srcdat->GetSSRC() );
	if ( srcdat->IsOwnSSRC() ) return;
	
	CString xUserId;
	if ( LookupKey(srcdat->GetSSRC(), xUserId) )
	{
		LeavedMirror( xUserId, srcdat->GetSSRC() );
	}
}

/////////////////////////////////////////////////////////////////////////////
// RTPSessionLayer deliver buffer

void RTPSessionLayer::OnRTPPacket(RTPPacket *pack,const RTPTime &receivetime, const RTPAddress *senderaddress)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	RTPPayload* pPayload;
	if ( m_pDatagrams.Lookup( pack->GetSSRC(), pPayload ) )
	{
		RTPPayload*& pPayload2 = m_pDatagrams[ pack->GetSSRC() ];
		DeliverBuffer( pack, pPayload ? pPayload : pPayload2 );
	}
}

HRESULT RTPSessionLayer::DeliverBuffer(RTPPacket *pack, RTPPayload*& pPayload)
{
	CSingleLock pLock( &m_pSection, TRUE );
//TRACE("%u->(%i) RTP::DeliverBuffer, {%u} {%u}\n", GetTickCount(), pack->GetPayloadType(),
//	  pack->GetTimestamp(), pack->GetSequenceNumber());
	
	for ( int nItem = 0 ; pPayload && pPayload->PayloadNext ; pPayload = pPayload->PayloadNext );
	
	if ( ! pPayload || pack->GetTimestamp() != ntohl(pPayload->timestamp) )
	{
		RTPPayload*& pPayload2 = pPayload ? pPayload->PayloadNext : pPayload;
		
		pPayload = pPayload2 = new RTPPayload; pPayload2->PayloadNext = NULL;
		
		CopyMemory( (RTPHeader*)pPayload, pack->GetPacketData(), sizeof(RTPHeader) );
	}
	
	pPayload->marker = pack->HasMarker();
	pPayload->PayloadBuffer.Add( pack->GetPayloadData(), pack->GetPayloadLength() );
	
	return S_OK;
}

HRESULT RTPSessionLayer::DeliverBuffer(BYTE* pBuffer, DWORD nLength, BYTE nPayloadType, DWORD nTimestamp, LPCTSTR xUserId)
{
#if 0
	CSingleLock pLock( &Network.m_pSection, TRUE );
#else
	CSingleLock pLock( &Network.m_pSection );
	if ( !pLock.Lock(5000))
		nLength=nLength;
#endif
	
	if ( CChannel* pChannel = Network.FindChannel( xUserId ) )
	{
		pChannel->DeliverBuffer( pBuffer, nLength, nPayloadType, nTimestamp );
	}
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// RTPSessionLayer render buffer

HRESULT RTPSessionLayer::RenderBuffer(RTPPayload*& pPayload, LPCTSTR xUserId)
{
	if ( ! pPayload->PayloadNext && ! pPayload->marker ) return S_FALSE;
//	TRACE("%u-> RTP::RenderBuffer, {%u} ok\n", GetTickCount(), ntohl(pPayload->timestamp));
	
//TRACE("%u-> RTP::RenderBuffer, {DeliverBuffer} in.\n", GetTickCount());
	DeliverBuffer( pPayload->PayloadBuffer.m_pBuffer, pPayload->PayloadBuffer.m_nLength,
		pPayload->payloadtype, ntohl(pPayload->timestamp), xUserId );
//TRACE("%u-> RTP::RenderBuffer, {DeliverBuffer} outtttt.\n", GetTickCount());
	
//TRACE("%u-> RTP::RenderBuffer, {Neighbour} in.\n", GetTickCount());
#if 0
	CSingleLock pLock( &Neighbour.m_pSection, TRUE );
#else
	CSingleLock pLock( &Neighbour.m_pSection );
	if ( !pLock.Lock(5000))
		xUserId=xUserId;
#endif
	
	if ( CCircuitry* pChannel = Neighbour.FindChannel( xUserId ) )
	{
		pChannel->RenderBuffer( pPayload->PayloadBuffer.m_pBuffer,
			pPayload->PayloadBuffer.m_nLength, pPayload->payloadtype );
	}
	
	RTPPayload* pPayload2 = pPayload; pPayload = pPayload->PayloadNext;
	pPayload2->PayloadNext = NULL; delete pPayload2;
	
	pLock.Unlock();
//TRACE("%u-> RTP::RenderBuffer, {Neighbour} outtttt.\n", GetTickCount());
	return pPayload ? RenderBuffer(pPayload, xUserId) : S_OK;
}

HRESULT RTPSessionLayer::RenderBuffer(BYTE* pBuffer, DWORD nLength, BYTE nPayloadType, LPCTSTR xUserId)
{
	CSingleLock pLock( &m_pSection );
	
	if ( ! pLock.Lock( 50 ) ) return S_FALSE;
	
	UINT ssrc;
	if ( ! m_pList.Lookup(xUserId, ssrc) ) return S_FALSE;
	
	RTPSourceData* srcdat = GetSourceInfo( ssrc );
	if ( ! srcdat ) throw "Invalid RTPSource";
	
	if ( ! srcdat->IsRTPAddressSet() ) throw "Invalid RTPSource";
	const RTPAddress* addr = srcdat->GetRTPDataAddress();
	
	pLock.Unlock();
	AddDestination( *addr );
	
	SendPayload( pBuffer, nLength, nPayloadType, 160 );

	DeleteDestination( *addr );
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// RTPSessionLayer rtcp render

void RTPSessionLayer::OnAPPPacket(RTCPAPPPacket *apppacket, const RTPTime &receivetime, const RTPAddress *senderaddress)
{
	if ( RTPSourceData* srcdat = GetSourceInfo( apppacket->GetSSRC() ) )
	{
		CXMLElement* pXML = CXMLElement::FromBytes(
			apppacket->GetAPPData(), apppacket->GetAPPDataLength(), FALSE );
		
		if ( pXML ) { ParseXML( pXML, srcdat ); delete pXML; }
	}
}

BOOL RTPSessionLayer::ParseXML(CXMLElement* pXML, RTPSourceData *srcdat)
{
	if ( pXML->IsNamed( "SyncMirrorView" ) )
	{
		OnSyncMirrorView( pXML, srcdat->GetSSRC() );
	}
	
	return TRUE;
}

BOOL RTPSessionLayer::OnSyncMirrorView(CXMLElement* pXML, UINT ssrc)
{
	CString strMethod = pXML->GetAttributeValue( "method" );
	
	BOOL bJoined = pXML->GetAttribute("leave") == NULL;
	
	if ( CXMLElement* pSync = pXML->GetElementByName( "UserId" ) )
	{
		if ( bJoined ) JoinedMirror( pSync->GetValue(), ssrc );
		else LeavedMirror( pSync->GetValue(), ssrc );
	}
	
	return TRUE;
}
