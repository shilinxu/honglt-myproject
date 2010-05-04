//
// Datagrams.cpp
//

#include "stdafx.h"
#include "Xgateway.h"
#include "Datagrams.h"
#include "Settings.h"

#include "Network.h"
#include "FilterGraph.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define TEMP_BUFFER 4096

CDatagrams Datagrams;

//////////////////////////////////////////////////////////////////////
// CDatagrams construction

CDatagrams::CDatagrams(BYTE nType) : m_nType(nType)
{
	m_hSocket	= INVALID_SOCKET;
	m_pPrev		= m_pNext	= NULL;
	
	CreateRandom();
}

CDatagrams::~CDatagrams()
{
	Disconnect();
}

void CDatagrams::Clear()
{
	for ( POSITION pos = m_pDatagrams.GetStartPosition(); pos; )
	{
		DWORD nLinkId;
		CDatagramIn* pDG;
		
		m_pDatagrams.GetNextAssoc( pos, nLinkId, pDG );
		if ( pDG ) delete pDG;
	}
	
	for ( pos = m_pDatagramOut.GetStartPosition(); pos; )
	{
		CString xUserId;
		CDatagramOut* pOutput;
		
		m_pDatagramOut.GetNextAssoc( pos, xUserId, pOutput );
		delete pOutput;
	}
	
	m_pDatagramOut.RemoveAll();
	m_pDatagrams.RemoveAll();
	m_pList.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// CDatagrams attributes

CDatagrams* CDatagrams::SeekNext(BYTE nType)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	if ( m_nType == nType ) return this;
	
	if ( m_pNext ) return m_pNext->SeekNext(nType);
	return NULL;
}

//////////////////////////////////////////////////////////////////////
// CDatagrams connect

BOOL CDatagrams::Connect(int nPort)
{
	CDatagrams* pDatagrams = this;
	DWORD nAddress = htonl( INADDR_ANY );
	
	pDatagrams->m_nType = Payload_Alaw;
	pDatagrams->Connect( (IN_ADDR*)&nAddress, nPort );
	
	pDatagrams = pDatagrams->Attach( new CDatagrams(Payload_H263) );
	pDatagrams->Connect( (IN_ADDR*)&nAddress, nPort + 1 );
	
	return TRUE;
}

BOOL CDatagrams::Connect(IN_ADDR* pAddress, WORD nPort)
{
	if ( m_hSocket != INVALID_SOCKET ) return FALSE;

	m_hSocket = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if ( m_hSocket == INVALID_SOCKET ) return FALSE;
	
	ZeroMemory( &m_pHost, sizeof(m_pHost) );
	m_pHost.sin_addr	= *pAddress;
	m_pHost.sin_family	= PF_INET;
	m_pHost.sin_port	= htons( nPort );
	
	if ( bind( m_hSocket, (SOCKADDR*)&m_pHost, sizeof(m_pHost) ) ) return FALSE;
	
	WSAEventSelect( m_hSocket, Network.m_pWakeup, FD_READ );
	
	m_nLinkId			= GetRandom((DWORD*)&m_nLinkId);
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CDatagrams disconnect

void CDatagrams::Disconnect()
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	if ( m_hSocket == INVALID_SOCKET ) return;
	
	closesocket( m_hSocket );
	m_hSocket = INVALID_SOCKET;
	
	Detach( this );
	Clear();
	
	pLock.Unlock();
	if ( m_pPrev ) delete this;
}

//////////////////////////////////////////////////////////////////////
// CDatagrams attach to an existing connection

CDatagrams* CDatagrams::Attach(CDatagrams* pConnection)
{
	if ( m_pNext )
	{
		m_pNext->Attach( pConnection );
	}
	else
	{
		ASSERT( pConnection->m_pPrev == NULL );
		
		pConnection->m_pPrev = this;
		m_pNext	= pConnection;
	}
	
	return pConnection;
}

void CDatagrams::Detach(CDatagrams* pConnection)
{
	if ( pConnection->m_pPrev )
	{
		if ( pConnection->m_pNext )
		pConnection->m_pPrev->Attach( pConnection->m_pNext );
		
		pConnection->m_pPrev->m_pNext = NULL;
		pConnection->m_pNext = NULL;
	}
	if ( pConnection->m_pNext )
	{
		pConnection->m_pNext->m_pPrev = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
// CDatagrams socket event handlers

BOOL CDatagrams::BroadNext(LPCTSTR xUserId, BYTE nType)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	CDatagrams* pDatagrams = SeekNext( nType );
	if ( ! pDatagrams ) return FALSE;
	
	CDatagramIn* pDG = pDatagrams->LookupKey( xUserId );
	
	if ( pDG ) pDatagrams->JoinMirror(xUserId, &pDG->m_pHost);
	
	return TRUE;
}

BOOL CDatagrams::UnbroadNext(LPCTSTR xUserId, BYTE nType)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	CDatagrams* pDatagrams = SeekNext( nType );
	if ( ! pDatagrams ) return FALSE;
	
	LeavedMirror( xUserId, 0 );
	return TRUE;
}

BOOL CDatagrams::JoinMirror(LPCTSTR xUserId, SOCKADDR_IN* pHost)
{
	CXMLElement* pXML = new CXMLElement( NULL, "SyncMirrorView" );
	pXML->AddAttribute( "UserId", xUserId );
	
	CString strXML = pXML->ToString( FALSE, TRUE );
	delete pXML;
	
	int nXMLLength = strXML.GetLength();
	SendPacket( pHost, (BYTE*)strXML.GetBuffer(0), nXMLLength, G2_PROTOCOL, APP );
	
	return JoinedMirror( xUserId, pHost );
}

//////////////////////////////////////////////////////////////////////
// CDatagrams run function

BOOL CDatagrams::OnRun()
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	if ( INVALID_SOCKET == m_hSocket ) return FALSE;
	
	{
		if ( ! OnRead() ) return FALSE;
	}
	
	{
		if ( ! TravelMirror() ) return FALSE;
	}
	
	pLock.Unlock();
	return m_pNext ? m_pNext->OnRun() : TRUE;
}

BOOL CDatagrams::TravelMirror()
{
	for ( POSITION pos = m_pList.GetStartPosition(); pos; )
	{
		DWORD nLinkId;
		CString xUserId;
		
		m_pList.GetNextAssoc( pos, xUserId, nLinkId );
		
		CDatagramIn* pDG;
		if ( m_pDatagrams.Lookup(nLinkId, pDG) ) pDG->OnRun( xUserId );
	}
	for ( pos = m_pDatagramOut.GetStartPosition(); pos; )
	{
		CString xUserId;
		CDatagramOut* pDG;
		
		m_pDatagramOut.GetNextAssoc( pos, xUserId, pDG );
		pDG->OnRun();
	}
	
	return TRUE;
}

void CDatagrams::DoPrint(BYTE* pBuffer, DWORD nLength, LPCTSTR lpszHeader)
{
#ifdef _DEBUG
	afxDump << lpszHeader << "[" << nLength << "]" << " (" << m_hSocket << ") "
			<< ToASCII( (BYTE*)pBuffer, min(nLength, 100) ) <<"\n";
#endif
}

//////////////////////////////////////////////////////////////////////
// CDatagrams write event handler

BOOL CDatagrams::SendPacket(SOCKADDR_IN* pHost, BYTE* pBuffer, DWORD nLength)
{
	if ( m_hSocket == INVALID_SOCKET ) return FALSE;
	
	sendto( m_hSocket, (LPCSTR)pBuffer, nLength, 0,
		(SOCKADDR*)pHost, sizeof(SOCKADDR_IN) );
	
//	DoPrint( (BYTE*)pBuffer, nLength, _T("TX:") );
	
	return TRUE;
}

BOOL CDatagrams::SendPacket(SOCKADDR_IN* pHost, BYTE* pBuffer, DWORD nLength, BYTE nProtocol, BYTE nType, BYTE nMarker)
{
	BYTE* pG2Buffer = new BYTE[ sizeof(G2_UDP_HEADER) + nLength ];
	
	G2_UDP_HEADER* pHeader = (G2_UDP_HEADER*)pG2Buffer;
	
	pHeader->nProtocol		= nProtocol;
	pHeader->nLinkId		= SWAP_LONG(m_nLinkId);
	
	pHeader->nType			= nType;
	pHeader->nMarker		= nMarker;
	
	CopyMemory( pG2Buffer + sizeof(G2_UDP_HEADER), pBuffer, nLength );
	
	BOOL bSuccess = SendPacket( pHost, pG2Buffer, sizeof(G2_UDP_HEADER) + nLength );
	
	delete pG2Buffer;
	return bSuccess;
}

//////////////////////////////////////////////////////////////////////
// CDatagrams read event handler

BOOL CDatagrams::OnRead()
{
	if ( m_hSocket == INVALID_SOCKET ) return FALSE;
	
	static BYTE pBuffer[ TEMP_BUFFER ];
	int nFromLen = sizeof( SOCKADDR_IN );
	SOCKADDR_IN pFrom;
	
	DWORD dwRead = 0;
	ioctlsocket(m_hSocket, FIONREAD, &dwRead);
	
	while (dwRead)
	{
		int nLength = min( ( dwRead & 0xFFFFF ), TEMP_BUFFER );
		
		nLength = recvfrom( m_hSocket, (LPSTR)pBuffer, TEMP_BUFFER, 0,
			(SOCKADDR*)&pFrom, &nFromLen );
		if ( nLength <= 0 ) break;
		
		if ( nLength > 0 && nLength <= TEMP_BUFFER )
		{
//			DoPrint( (BYTE*)pBuffer, nLength, _T("RX:") );
			OnDatagram( &pFrom, pBuffer, nLength );
		}
		
		dwRead -= nLength;
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CDatagrams datagram handler

BOOL CDatagrams::OnDatagram(SOCKADDR_IN* pHost, BYTE* pBuffer, DWORD nLength)
{
	if ( nLength < sizeof(G2_UDP_HEADER) ) return FALSE;
	
	if ( pBuffer[0] == G2_PROTOCOL )
	{
		return OnDatagram(pHost, (G2_UDP_HEADER*)pBuffer, nLength);
	}
	else if ( pBuffer[0] == G3_PAYLOAD )
	{
		return OnDatagram(pHost, (G3_UDP_HEADER*)pBuffer, nLength);
	}
	
	return TRUE;
}

BOOL CDatagrams::OnDatagram(SOCKADDR_IN* pHost, G2_UDP_HEADER* pBuffer, DWORD nLength)
{
	DWORD nLinkId = SWAP_LONG(pBuffer->nLinkId);
	
	ASSERT ( pBuffer->nProtocol == G2_PROTOCOL );
	
	if ( nLength < sizeof(G2_UDP_HEADER) ) return FALSE;
	
	CDatagramIn* pDG;
	if ( m_pDatagrams.Lookup(nLinkId, pDG) ) return pDG->OnDatagram(pHost, pBuffer, nLength);
	
	return ReadBuffer(pHost, pBuffer, nLength);
}

BOOL CDatagrams::OnDatagram(SOCKADDR_IN* pHost, G3_UDP_HEADER* pBuffer, DWORD nLength)
{
	DWORD nLinkId = SWAP_LONG(pBuffer->nLinkId);
	
	ASSERT ( pBuffer->nProtocol == G3_PAYLOAD );
	
	if ( nLength < sizeof(G3_UDP_HEADER) ) return FALSE;
	
	CDatagramIn* pDG;
	if ( ! m_pDatagrams.Lookup(nLinkId, pDG) ) return FALSE;
	
	return pDG->OnDatagram(pHost, pBuffer, nLength);
}

BOOL CDatagrams::ReadBuffer(SOCKADDR_IN* pHost, G2_UDP_HEADER* pBuffer, DWORD nLength)
{
	if ( pBuffer->nType != APP ) return FALSE;
	
	CXMLElement* pXML = CXMLElement::FromBytes(
	(BYTE*)(pBuffer + 1), nLength - sizeof(G2_UDP_HEADER), FALSE );
	
	if ( pXML && pXML->IsNamed( "SyncMirrorView" ) )
	{
		OnSyncMirrorView( pXML, SWAP_LONG(pBuffer->nLinkId), pHost );
	}
	
	if ( pXML ) delete pXML;
	return TRUE;
}

BOOL CDatagrams::OnSyncMirrorView(CXMLElement* pXML, DWORD nLinkId, SOCKADDR_IN* pHost)
{
	CString xUserId = pXML->GetAttributeValue( "UserId" );
	
	return JoinedMirror(xUserId, nLinkId) && JoinedMirror(nLinkId, pHost);
}

//////////////////////////////////////////////////////////////////////
// CDatagrams attributes

BOOL CDatagrams::JoinedMirror(LPCTSTR xUserId, DWORD nLinkId)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	DWORD nLinkId2;
	if ( m_pList.Lookup(xUserId, nLinkId2) ) LeavedMirror(xUserId);
	
	m_pList.SetAt( xUserId, nLinkId );
TRACE( "debug[%2i][%s]: SetAt [%u].\n", m_nType, xUserId, nLinkId);
	return TRUE;
}

BOOL CDatagrams::JoinedMirror(DWORD nLinkId, SOCKADDR_IN* pHost)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	CDatagramIn* pDG;
	if ( m_pDatagrams.Lookup(nLinkId, pDG) ) LeavedMirror(nLinkId);
	
	m_pDatagrams.SetAt( nLinkId, pDG = new CDatagramIn(pHost, this, nLinkId) );
TRACE( "debug[%2i][%u]: SetPtr [0x%x].\n", m_nType, nLinkId, pDG);
	return TRUE;
}

BOOL CDatagrams::JoinedMirror(LPCTSTR xUserId, SOCKADDR_IN* pHost)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	CDatagramOut* pDG;
	if ( m_pDatagramOut.Lookup(xUserId, pDG) ) delete pDG;
	
	m_pDatagramOut.SetAt( xUserId, new CDatagramOut(pHost, this) );
	return TRUE;
}

BOOL CDatagrams::LeavedMirror(LPCTSTR xUserId)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	DWORD nLinkId;
	if ( ! m_pList.Lookup(xUserId, nLinkId) ) return TRUE;
	
	CDatagramIn* pDG;
	if ( m_pDatagrams.Lookup(nLinkId, pDG) ) delete pDG;
	
	m_pDatagrams.RemoveKey( nLinkId );
TRACE( "debug[%2i][%u]: RemovePtr [0x%x].\n", m_nType, nLinkId, pDG);
	m_pList.RemoveKey( xUserId );
TRACE( "debug[%2i][%s]: RemoveKey [%u].\n", m_nType, xUserId, nLinkId);

	return TRUE;
}

BOOL CDatagrams::LeavedMirror(LPCTSTR xUserId, DWORD)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	CDatagramOut* pDG;
	if ( m_pDatagramOut.Lookup(xUserId, pDG) ) delete pDG;
	
	m_pDatagramOut.RemoveKey( xUserId );
	return TRUE;
}

BOOL CDatagrams::LeavedMirror(DWORD nLinkId)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	CDatagramIn* pDG;
	if ( m_pDatagrams.Lookup(nLinkId, pDG) ) delete pDG;
	
	m_pDatagrams.RemoveKey( nLinkId );
TRACE( "debug[%2i][%s]: RemoveKey [%u].\n", m_nType, NULL, nLinkId);
	return TRUE;
}

CDatagramIn* CDatagrams::LookupKey(LPCTSTR xUserId)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	DWORD nLinkId;
	if ( ! m_pList.Lookup(xUserId, nLinkId) ) return NULL;
	
	CDatagramIn* pDG = NULL;
	m_pDatagrams.Lookup( nLinkId, pDG );
	
	return pDG;
}

CDatagramOut* CDatagrams::LookupKey(LPCTSTR xUserId, CDatagramOut*& pDG)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	if ( ! m_pDatagramOut.Lookup(xUserId, pDG) ) pDG = NULL;
	return pDG;
}

CXMLElement* CDatagrams::FormatXML(CXMLElement* pParent, LPCTSTR xUserId)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	if ( CDatagramIn* pDG = LookupKey( xUserId ) )
	{
		CXMLElement* pXML = pParent->AddElement( "mirror" );
		
		CString strValue;
		strValue.Format( "%i", m_nType );
		pXML->AddAttribute( "Type", strValue );
		
		strValue.Format( "%u", pDG->m_nLinkId );
		pXML->AddAttribute( "ID", strValue );
		
		pDG->FormatXML( pXML );
	}
	
	if ( m_pNext ) m_pNext->FormatXML(pParent, xUserId);
	
	return pParent;
}

//////////////////////////////////////////////////////////////////////
// CDatagrams random handler

void CDatagrams::CreateRandom()
{
	DWORD nRandId = GetCurrentProcessId();
	
	FILETIME pFileTime;
	SYSTEMTIME pSystemTime;
	
	GetSystemTime( &pSystemTime );
	SystemTimeToFileTime( &pSystemTime, &pFileTime );
	
	nRandId += pFileTime.dwLowDateTime;
	
	nRandId ^= (DWORD)((BYTE *)this - (BYTE *)0);
	srand( (unsigned int)nRandId );
}

WORD CDatagrams::GetRandom(WORD *pnId) const
{
	WORD x;
	if ( ! pnId ) pnId = &x;
	
	*pnId = (WORD)(65536.0*((double)rand())/((double)RAND_MAX+1.0));
	return (*pnId);
}

DWORD CDatagrams::GetRandom(DWORD *pnId) const
{
	DWORD x,y;
	
	x = (DWORD)(65536.0*((double)rand())/((double)RAND_MAX+1.0));
	y = x;
	x = (DWORD)(65536.0*((double)rand())/((double)RAND_MAX+1.0));
	y ^= (x<<8);
	x = (DWORD)(65536.0*((double)rand())/((double)RAND_MAX+1.0));
	y ^= (x<<16);

	return y;
}
