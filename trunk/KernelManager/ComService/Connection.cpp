//
// Connection.cpp
//

#include "stdafx.h"
#include "Buffer.h"
#include "Connection.h"

#define TEMP_BUFFER			1024

//////////////////////////////////////////////////////////////////////
// CConnection construction

CConnection::CConnection()
{
	m_hSocket		= INVALID_SOCKET;
	m_pInput		= NULL;
	m_pOutput		= NULL;
	m_bConnected	= FALSE;
	m_tConnected	= 0;
}

CConnection::CConnection(SOCKET hSocket, SOCKADDR_IN* pHost)
{
	m_hSocket		= INVALID_SOCKET;
	m_pInput		= NULL;
	m_pOutput		= NULL;
	m_bConnected	= FALSE;
	m_tConnected	= 0;
	
	AcceptFrom( hSocket, pHost );
}

CConnection::~CConnection()
{
	CConnection::Close();
}

//////////////////////////////////////////////////////////////////////
// CConnection attributes

BOOL CConnection::IsConnected() const
{
	return m_bConnected;
}

BOOL CConnection::TimerOut(DWORD tDelta) const
{
	return GetTickCount() - m_tConnected > tDelta;
}

DWORD CConnection::TranslateAddr(LPCTSTR pszAddress)
{
	DWORD dwServerIP = inet_addr( pszAddress );
	
	if (dwServerIP == INADDR_NONE)
	{
		LPHOSTENT lpHost = gethostbyname( pszAddress );
		if (lpHost == NULL) return INADDR_NONE;
		
		dwServerIP = ((LPIN_ADDR)lpHost->h_addr)->s_addr;
	}
	
	return dwServerIP;
}

//////////////////////////////////////////////////////////////////////
// CConnection connect

BOOL CConnection::Connect(IN_ADDR* pAddress, WORD nPort)
{
	if ( m_hSocket != INVALID_SOCKET ) return FALSE;
	
	m_hSocket = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
	if ( m_hSocket == INVALID_SOCKET ) return FALSE;
	
	ZeroMemory( &m_pHost, sizeof(m_pHost) );
	m_pHost.sin_addr	= *pAddress;
	m_pHost.sin_family	= PF_INET;
	m_pHost.sin_port	= htons( nPort );
	
	if ( bind( m_hSocket, (SOCKADDR*)&m_pHost, sizeof(m_pHost) ) ) return FALSE;
	
	listen( m_hSocket, SOMAXCONN );
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CConnection connect to

BOOL CConnection::ConnectTo(SOCKADDR_IN* pHost, Blocking bs)
{
	return ConnectTo( &pHost->sin_addr, htons(pHost->sin_port), bs );
}

BOOL CConnection::ConnectTo(LPCTSTR pszAddress, WORD nPort, Blocking bs)
{
	IN_ADDR pAddress;
	pAddress.S_un.S_addr = TranslateAddr( pszAddress );
	
	return ConnectTo( &pAddress, nPort, bs );
}

BOOL CConnection::ConnectTo(IN_ADDR* pAddress, WORD nPort, Blocking bs)
{
	if ( m_hSocket != INVALID_SOCKET ) return FALSE;
	
	if ( pAddress == NULL || nPort == 0 ) return FALSE;
	if ( pAddress->S_un.S_addr == 0 ) return FALSE;
	
	ZeroMemory( &m_pHost, sizeof(m_pHost) );
	m_pHost.sin_addr	= *pAddress;
	
	m_pHost.sin_family	= PF_INET;
	m_pHost.sin_port	= htons( nPort );
	
	m_hSocket = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
	
	ioctlsocket( m_hSocket, FIONBIO, (DWORD*)&bs );
	
	if ( WSAConnect( m_hSocket, (SOCKADDR*)&m_pHost, sizeof(SOCKADDR_IN), NULL, NULL, NULL, NULL ) )
	{
		UINT nError = WSAGetLastError();
		
		if ( nError != WSAEWOULDBLOCK )
		{
			closesocket( m_hSocket );
			m_hSocket = INVALID_SOCKET;
			return FALSE;
		}
	}
	
	if ( m_pInput ) delete m_pInput;
	if ( m_pOutput ) delete m_pOutput;
	
	m_pInput		= new CBuffer;
	m_pOutput		= new CBuffer;
	m_bConnected	= FALSE;
	m_tConnected	= GetTickCount();
	
	if ( bs == Inblocking ) m_bConnected = TRUE;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CConnection accept an incoming connection

void CConnection::AcceptFrom(SOCKET hSocket, SOCKADDR_IN* pHost)
{
//	ASSERT( m_hSocket == INVALID_SOCKET );
	
	m_pInput		= new CBuffer;
	m_pOutput		= new CBuffer;
	m_hSocket		= hSocket;
	m_pHost			= *pHost;
	m_bConnected	= TRUE;
	m_tConnected	= GetTickCount();
	
	DWORD dwValue = 1;
	ioctlsocket( m_hSocket, FIONBIO, &dwValue );
}

//////////////////////////////////////////////////////////////////////
// CConnection attach to an existing connection

void CConnection::AttachTo(CConnection* pConnection)
{
//	ASSERT( m_hSocket == INVALID_SOCKET );
//	ASSERT( pConnection != NULL );
//	ASSERT( pConnection->m_hSocket != INVALID_SOCKET );
	
	m_pOutput		= pConnection->m_pOutput;
	m_pInput		= pConnection->m_pInput;
	m_pHost			= pConnection->m_pHost;
	m_hSocket		= pConnection->m_hSocket;
	m_bConnected	= pConnection->m_bConnected;
	m_tConnected	= pConnection->m_tConnected;
	
	pConnection->m_hSocket	= INVALID_SOCKET;
	pConnection->m_pOutput	= NULL;
	pConnection->m_pInput	= NULL;
	pConnection->m_bConnected = FALSE;
}

//////////////////////////////////////////////////////////////////////
// CConnection close

void CConnection::Close()
{
//	ASSERT( this != NULL );
//	ASSERT( AfxIsValidAddress( this, sizeof(*this) ) );
	
	if ( m_hSocket != INVALID_SOCKET )
	{
		closesocket( m_hSocket );
		m_hSocket = INVALID_SOCKET;
	}
	
	if ( m_pOutput ) delete m_pOutput;
	m_pOutput = NULL;
	
	if ( m_pInput ) delete m_pInput;
	m_pInput = NULL;
	
	m_bConnected = FALSE;
	m_tConnected = GetTickCount();
}

//////////////////////////////////////////////////////////////////////
// CConnection run function

BOOL CConnection::OnRun()
{
	if ( INVALID_SOCKET == m_hSocket ) return FALSE;
	
	WSANETWORKEVENTS pEvents;
	WSAEnumNetworkEvents( m_hSocket, NULL, &pEvents );
	
	if ( pEvents.lNetworkEvents & FD_CONNECT )
	{
		if ( pEvents.iErrorCode[ FD_CONNECT_BIT ] != 0 )
		{
			OnDropped( pEvents.iErrorCode[ FD_CONNECT_BIT ] );
			return FALSE;
		}

		if ( ! OnConnected() ) return FALSE;
	}
	
	if ( pEvents.lNetworkEvents & FD_CLOSE )
	{
		OnDropped( pEvents.iErrorCode[ FD_CLOSE_BIT ] );
		return FALSE;
	}
	
	if ( pEvents.lNetworkEvents & FD_ACCEPT )
	{
		if ( ! OnAccepted() ) return FALSE;
	}
	
	if ( pEvents.lNetworkEvents & FD_WRITE )
	{
		if ( ! OnWrite() ) return FALSE;
	}

	if ( pEvents.lNetworkEvents & FD_READ )
	{
		if ( ! OnRead() ) return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CConnection socket event handlers

BOOL CConnection::OnConnected()
{
	m_bConnected	= TRUE;
	m_tConnected	= GetTickCount();

	return TRUE;
}

void CConnection::OnDropped(int nError)
{
	m_bConnected	= FALSE;
	m_tConnected	= GetTickCount();
}

BOOL CConnection::OnAccepted()
{
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CConnection read event handler

BOOL CConnection::OnRead()
{
	if ( m_hSocket == INVALID_SOCKET ) return FALSE;
	if ( m_pOutput == NULL ) return FALSE;

	DWORD dwRead = 0;
	//check how many bytes we could read
	ioctlsocket(m_hSocket, FIONREAD, &dwRead);
	if (0 == dwRead) return TRUE;

	BYTE pData[TEMP_BUFFER];
	
	while (dwRead)
	{
		int nLength = min( ( dwRead & 0xFFFFF ), TEMP_BUFFER );
		
		nLength = recv( m_hSocket, (char*)pData, nLength, 0 );
		if ( nLength <= 0 ) break;

		if ( nLength > 0 && nLength <= TEMP_BUFFER )
		{
			m_pInput->Add( pData, nLength );
		}
		
		dwRead -= nLength;
		
		DoPrint( (BYTE*)pData, nLength, _T("RX:") );
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CConnection write event handler

BOOL CConnection::OnWrite()
{
	if ( m_hSocket == INVALID_SOCKET ) return FALSE;
	if ( m_pOutput == NULL ) return FALSE;
	
	DWORD tNow		= GetTickCount();
	DWORD nLimit	= 0xFFFFFFFF;
	
	BYTE* pBuffer = m_pOutput->m_pBuffer;
	DWORD nBuffer = m_pOutput->m_nLength;
	
	while ( nLimit && nBuffer )
	{
		int nLength = min( ( nBuffer & 0xFFFFF ), TEMP_BUFFER );
		
		nLength = send( m_hSocket, (char*)pBuffer, nLength, 0 );
		if ( nLength <= 0 ) break;
		
		DoPrint( (BYTE*)pBuffer, nLength, _T("TX:") );
		
		pBuffer += nLength;
		nBuffer -= nLength;
	}
	
	DWORD nTotal = ( m_pOutput->m_nLength - nBuffer );
	if ( nTotal ) m_pOutput->Remove( nTotal );
	
	return TRUE;
}

void CConnection::DoPrint(BYTE* pBuffer, DWORD nLength, LPCTSTR lpszHeader)
{
//#ifdef _DEBUG
//	CString strLine;
//	strLine.Format( "%s[%i] {%s}\n", lpszHeader, nLength, ToHex(pBuffer, min(nLength, 100)) );
	
//	OutputDebugString( (LPCTSTR)strLine );
//#endif
}
