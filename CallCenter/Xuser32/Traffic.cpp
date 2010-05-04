//
// Traffic.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "Settings.h"
#include "Traffic.h"
#include "Buffer.h"
#include "XML.h"

#include "RTPIPv4Address.h"
#include "RTPSessionParams.h"
#include "RTPUDPv4Transmitter.h"
#include "RTPSourceData.h"
#include "rtcpapppacket.h"
#include "FilterGraph.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define TEMP_BUFFER		1024
#define RTPFILE_PORT	16666

CTraffic Traffic;

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

BOOL RTPSessionLayer::JoinedMirror(LPCTSTR xUserId, UINT ssrc)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	if ( m_pList.Lookup(xUserId, ssrc) ) return TRUE;
	
	m_pList.SetAt( xUserId, ssrc );
TRACE( "debug[%s]: SetAt [%u].\n", xUserId, ssrc);
	
	RTPPayload* pPayload;
	if ( m_pDatagrams.Lookup(ssrc, pPayload) ) return TRUE;
	
	m_pDatagrams.SetAt( ssrc, NULL );
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

BOOL RTPSessionLayer::TravelChild(LPCTSTR xUserId)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	UINT ssrc;
	if ( ! m_pList.Lookup(xUserId, ssrc) ) return FALSE;
	
	if ( GetSourceInfo( ssrc ) == NULL )
	{
		LeavedMirror( xUserId, ssrc ); return FALSE;
	}
	
	return TRUE;
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

int RTPSessionLayer::JoinMirror(LPCTSTR xUserId, const RTPAddress& pReceiver, BOOL bLeave)
{
	if ( ! IsActive() ) return ERR_RTP_SESSION_NOTCREATED;
	
	CXMLElement* pXML = new CXMLElement( NULL, "SyncMirrorView" );
	pXML->AddAttribute( "method", "mirror" );
	
	if ( bLeave ) pXML->AddAttribute( "leave" );
	
	CXMLElement* pSync = pXML->AddElement( "UserId" );
	pSync->SetValue( xUserId );
	
	CString strXML = pXML->ToString( FALSE, TRUE );
	delete pXML;
	
	AddDestination( pReceiver );
	
	int nXMLLength = strXML.GetLength();
	SendRTCPAPPPacket( 0, (BYTE*)"TEST", (LPCTSTR)strXML, (nXMLLength + 3) & 0xFFFC );
	
	return 0;
}

int RTPSessionLayer::JoinMirror(LPCTSTR xUserId, CXMLElement* pParent, BOOL bJoined)
{
	if ( ! IsActive() ) return ERR_RTP_SESSION_NOTCREATED;
	RTPIPv4Address rtpaddr, rtcpaddr;
	
	if ( CXMLElement* pXML = pParent->GetElementByName("RTP") )
	{
		RTPAddressFrom( pXML, &rtpaddr );
	}
	if ( CXMLElement* pXML = pParent->GetElementByName("RTCP") )
	{
		RTPAddressFrom( pXML, &rtcpaddr );
	}
	
	if ( bJoined ) m_pTransmitter->AddDestination( rtpaddr, rtcpaddr );
	else m_pTransmitter->DeleteDestination( rtpaddr, rtcpaddr );
	
	CXMLElement* pXML = new CXMLElement( NULL, "SyncMirrorView" );
	pXML->AddAttribute( "method", "monitor" );
	
	if ( ! bJoined ) pXML->AddAttribute( "leave" );
	
	CXMLElement* pSync = pXML->AddElement( "UserId" );
	pSync->SetValue( xUserId );
	
	CString strXML = pXML->ToString( FALSE, TRUE );
	delete pXML;
	
	int nXMLLength = strXML.GetLength();
	SendRTCPAPPPacket( 0, (BYTE*)"TEST", (LPCTSTR)strXML, (nXMLLength + 3) & 0xFFFC );
#ifdef _DEBUG
	{
		RTPIPv4Address *addr2v4 = (RTPIPv4Address *)&rtcpaddr;
		
		in_addr address;
		address.s_addr = htonl( addr2v4->GetIP() ); 
		
	TRACE( "JoinMirror[%s]: to %s:%i\n", xUserId, inet_ntoa(address),addr2v4->GetPort());
	}
#endif
	
	return 0;
}

BOOL RTPSessionLayer::RTPAddressFrom(CXMLElement* pXML, RTPAddress* addr)
{
	ASSERT( addr->GetAddressType() == RTPAddress::IPv4Address );
	
	CString strValue = pXML->GetAttributeValue( "IP" );
	
	DWORD nIPv4Address = inet_addr( strValue );
	
	strValue = pXML->GetAttributeValue( "Port" );
	
	RTPIPv4Address* addr2 = (RTPIPv4Address*)addr;
	addr2->SetIP( ntohl(nIPv4Address) );
	addr2->SetPort( _ttoi(strValue) );
	
	return TRUE;
}

int RTPSessionLayer::JoinRecord(BOOL bJoined)
{
	CXMLElement* pXML = new CXMLElement( NULL, "SyncRecordView" );
	
	if ( ! bJoined ) pXML->AddAttribute( "method", "stop" );
	
	CString strXML = pXML->ToString( FALSE, TRUE );
	delete pXML;
	
	int nXMLLength = strXML.GetLength();
	SendRTCPAPPPacket( 0, (BYTE*)"TEST", (LPCTSTR)strXML, (nXMLLength + 3) & 0xFFFC );
	
	return 0;
}

BOOL RTPSessionLayer::PostMessageToOwner(CXMLElement* pXML, UINT ssrc)
{
	CSingleLock pLock( &theApp.m_pSection, TRUE );
	
	if ( theApp.m_pMainWnd->GetSafeHwnd() )
	theApp.m_pMainWnd->PostMessage( WM_SYNCDELIVER, ssrc, (LPARAM)pXML );
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// RTPSessionLayer thread run

void RTPSessionLayer::OnPollThreadStep()
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	for ( POSITION pos = m_pList.GetStartPosition(); pos; )
	{
		UINT ssrc;
		CString xUserId;
		
		m_pList.GetNextAssoc( pos, xUserId, ssrc );
		TravelChild( xUserId );
	}
}

/////////////////////////////////////////////////////////////////////////////
// RTPSessionLayer delive buffer

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

/////////////////////////////////////////////////////////////////////////////
// RTPSessionLayer render buffer

HRESULT RTPSessionLayer::RenderBuffer(LPCTSTR xUserId, HRESULT (CALLBACK* lpfnRender)(LPVOID, RTPPayload*), LPVOID lpHandle)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	UINT ssrc;
	if ( ! m_pList.Lookup( xUserId, ssrc ) ) return S_FALSE;
	
	RTPPayload*& pPayload = m_pDatagrams[ ssrc ];
	if ( ! pPayload ) return S_OK;
	
	if ( ! pPayload->PayloadNext && ! pPayload->marker ) return S_OK;
	if ( lpfnRender ) lpfnRender( lpHandle, pPayload );
	
	RTPPayload* pPayload2 = pPayload; pPayload = pPayload->PayloadNext;
	pPayload2->PayloadNext = NULL; delete pPayload2;
	
	pLock.Unlock();
	return RenderBuffer(xUserId, lpfnRender, lpHandle);
}

HRESULT RTPSessionLayer::PurgeBuffer(LPCTSTR xUserId)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	UINT ssrc;
	if ( ! m_pList.Lookup( xUserId, ssrc ) ) return S_FALSE;
	
	RTPPayload* pPayload;
	if ( m_pDatagrams.Lookup(ssrc, pPayload) )
	{
		m_pDatagrams.RemoveKey( ssrc );
		delete pPayload;
	}
	
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
		
		if ( pXML ) { ParseXML( pXML, srcdat ); /*delete pXML;*/ }
	}
}

BOOL RTPSessionLayer::ParseXML(CXMLElement* pXML, RTPSourceData *srcdat)
{
#ifdef _DEBUG
	CString strXML;
	strXML = pXML->ToString(FALSE,TRUE);
	
	TRACE("%u-> RTP->RX: %s\n", GetTickCount(), strXML );
#endif
	
	return PostMessageToOwner( pXML, srcdat->GetSSRC() );
}

BOOL RTPSessionLayer::OnSyncMirrorView(CXMLElement* pXML, UINT ssrc)
{
	CString strMethod = pXML->GetAttributeValue( "method" );
	
	if ( ! strMethod.CompareNoCase( "mirror") )
	{
		CXMLElement* pSync = pXML->GetElementByName("UserId");
		if ( pSync ) JoinedMirror( pSync->GetValue(), ssrc );
	}
	
	return TRUE;
}
