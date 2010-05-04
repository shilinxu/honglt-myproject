//
// Connection.cpp
//

#include "stdafx.h"
#include "Connection.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

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
	
	m_sAddress		= inet_ntoa( m_pHost.sin_addr );
	
	if ( bind( m_hSocket, (SOCKADDR*)&m_pHost, sizeof(m_pHost) ) ) return FALSE;
	
	listen( m_hSocket, 256 );
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CConnection connect to

BOOL CConnection::ConnectTo(SOCKADDR_IN* pHost)
{
	return ConnectTo( &pHost->sin_addr, htons( pHost->sin_port ) );
}

BOOL CConnection::ConnectTo(IN_ADDR* pAddress, WORD nPort)
{
	if ( m_hSocket != INVALID_SOCKET ) return FALSE;
	
	if ( pAddress == NULL || nPort == 0 ) return FALSE;
	if ( pAddress->S_un.S_addr == 0 ) return FALSE;
	
	if ( pAddress != &m_pHost.sin_addr )
	{
		ZeroMemory( &m_pHost, sizeof(m_pHost) );
		m_pHost.sin_addr	= *pAddress;
	}
	
	m_pHost.sin_family	= PF_INET;
	m_pHost.sin_port	= htons( nPort );
	
	m_sAddress		= inet_ntoa( m_pHost.sin_addr );
	m_hSocket		= socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
	// m_hSocket		= WSASocket( PF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED );
	
	DWORD dwValue = 1;
	ioctlsocket( m_hSocket, FIONBIO, &dwValue );
	
	// if ( connect( m_hSocket, (SOCKADDR*)&m_pHost, sizeof(SOCKADDR_IN) ) ) 
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
	
	if ( m_pInput ) delete m_pInput;	// Safe
	if ( m_pOutput ) delete m_pOutput;
	
	m_pInput		= new CBuffer;
	m_pOutput		= new CBuffer;
	m_bConnected	= FALSE;
	m_tConnected	= GetTickCount();
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CConnection accept an incoming connection

void CConnection::AcceptFrom(SOCKET hSocket, SOCKADDR_IN* pHost)
{
	ASSERT( m_hSocket == INVALID_SOCKET );
	
	m_pInput		= new CBuffer;
	m_pOutput		= new CBuffer;
	m_hSocket		= hSocket;
	m_pHost			= *pHost;
	m_sAddress		= inet_ntoa( m_pHost.sin_addr );
	m_bConnected	= TRUE;
	m_tConnected	= GetTickCount();
	
	DWORD dwValue = 1;
	ioctlsocket( m_hSocket, FIONBIO, &dwValue );
}

//////////////////////////////////////////////////////////////////////
// CConnection attach to an existing connection

void CConnection::AttachTo(CConnection* pConnection)
{
	ASSERT( m_hSocket == INVALID_SOCKET );
	ASSERT( pConnection != NULL );
	ASSERT( pConnection->m_hSocket != INVALID_SOCKET );
	
	m_pOutput		= pConnection->m_pOutput;
	m_pInput		= pConnection->m_pInput;
	m_pHost			= pConnection->m_pHost;
	m_sAddress		= pConnection->m_sAddress;
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
	ASSERT( this != NULL );
	ASSERT( AfxIsValidAddress( this, sizeof(*this) ) );
	
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
	
	// if ( pEvents.lNetworkEvents & FD_WRITE )
	{
		if ( ! OnWrite() ) return FALSE;
	}

	// if ( pEvents.lNetworkEvents & FD_READ )
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
#ifdef _DEBUG
	afxDump << lpszHeader << "[" << nLength << "]" << " (" << m_hSocket << ") "
			<< ToASCII( (BYTE*)pBuffer, nLength ) <<"\n";
#endif
}

//////////////////////////////////////////////////////////////////////
// String conversion

CString CConnection::ToHex(BYTE* pBuffer, DWORD nLength)
{
	LPCTSTR pszHex = _T("0123456789ABCDEF");
	CString strDump;
	
	LPTSTR pszDump = strDump.GetBuffer( nLength * 3 );
	
	for ( DWORD i = 0 ; i < nLength ; i++ )
	{
		int nChar = pBuffer[i];
		if ( i ) *pszDump++ = ' ';
		*pszDump++ = pszHex[ nChar >> 4 ];
		*pszDump++ = pszHex[ nChar & 0x0F ];
	}
	
	*pszDump = 0;
	strDump.ReleaseBuffer();
	
	return strDump;
}

CString CConnection::ToASCII(BYTE* pBuffer, DWORD nLength)
{
	CString strDump;
	
	LPTSTR pszDump = strDump.GetBuffer( nLength + 1 );
	
	for ( DWORD i = 0 ; i < nLength ; i++ )
	{
		int nChar = pBuffer[i];
		*pszDump++ = ( nChar >= 32 ? nChar : '.' );
	}
	
	*pszDump = 0;
	strDump.ReleaseBuffer();
	
	return strDump;
}
