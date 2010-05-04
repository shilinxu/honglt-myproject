//
// Neighbour.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Neighbour.h"
#include "Settings.h"
#include "Profiles.h"
#include "Network.h"

#include "Handshakes.h"
#include "Circuitry.h"
#include "Channel.h"
#include "XML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define TEMP_BUFFER			1024

CNeighbour Neighbour;

//////////////////////////////////////////////////////////////////////
// CNeighbour construction

CNeighbour::CNeighbour()
{
	m_hSocket	= INVALID_SOCKET;
	m_hThread	= NULL;
	m_bThread	= FALSE;
	
	m_hPipe		= INVALID_HANDLE_VALUE;
	m_pPosCookie= NULL;
	m_bConnected= FALSE;
}

CNeighbour::~CNeighbour()
{
	Disconnect();
	
	ASSERT( m_hPipe == INVALID_HANDLE_VALUE );
	ASSERT( m_hSocket == INVALID_SOCKET );
	ASSERT( m_hThread == NULL );
}

//////////////////////////////////////////////////////////////////////
// CNeighbour attributes

BOOL CNeighbour::IsConnected() const
{
	return m_bConnected;
}

//////////////////////////////////////////////////////////////////////
// CNeighbour connect

BOOL CNeighbour::Connect()
{
	if ( IsConnected() ) return TRUE;
	
	CSingleLock pLock( &m_pSection, TRUE );
	
	if ( m_hSocket != INVALID_SOCKET ) return FALSE;
	
	m_hSocket = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
	if ( m_hSocket == INVALID_SOCKET ) return FALSE;
	
	m_bConnected = TRUE;
	
	SOCKADDR_IN pHost;
	
	ZeroMemory( &pHost, sizeof(pHost) );
	DWORD nHost	= inet_addr( (LPCTSTR)Settings.User.Host );
	CopyMemory( &pHost.sin_addr, &nHost, 4 );
	
	pHost.sin_port	= htons( (WORD)Settings.User.Port );
	pHost.sin_family= PF_INET;
	
	if ( bind( m_hSocket, (SOCKADDR*)&pHost, sizeof(pHost) ) ) return FALSE;
	WSAEventSelect( m_hSocket, m_pWakeup, FD_ACCEPT );
	
	listen( m_hSocket, 256 );
	Connect( Settings.Centric.Local );
	
	Handshakes.Setup();
	Handshakes.Connect();
	
	StartThread();
	return TRUE;
}

BOOL CNeighbour::Connect(LPCTSTR lpPipeName, DWORD nMaxInstances, DWORD nTimeOut)
{
	if ( m_hPipe != INVALID_HANDLE_VALUE ) return FALSE;
	
	SECURITY_ATTRIBUTES sa;
	PSECURITY_DESCRIPTOR pSD;
	
	pSD = (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR,SECURITY_DESCRIPTOR_MIN_LENGTH);
	if ( ! InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION) )
	{
		LocalFree( (HLOCAL)pSD ); return FALSE;
	}
	if ( ! SetSecurityDescriptorDacl(pSD, TRUE, (PACL) NULL, FALSE) )
	{
		LocalFree( (HLOCAL)pSD ); return FALSE;
	}
	
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = pSD; 
	sa.bInheritHandle = TRUE;
	
	m_hPipe = CreateNamedPipe( lpPipeName, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE, nMaxInstances, TEMP_BUFFER, TEMP_BUFFER, nTimeOut, &sa );
	
	if ( m_hPipe == INVALID_HANDLE_VALUE ) return FALSE;
	
	ZeroMemory( &m_pOverlapped, sizeof(OVERLAPPED) );
	m_pOverlapped.hEvent = m_pWakeup;
	
	ConnectNamedPipe( m_hPipe, &m_pOverlapped );
TRACE("CreateNamedPipe: %s\n", lpPipeName);
	
	StartThread();
	return TRUE;
}

CCircuitry* CNeighbour::ConnectTo(LPCTSTR xUserId, BOOL bPeek, DWORD nTimeOut)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	CCircuitry* pChannel = FindChannel( xUserId );
	if ( pChannel ) return pChannel;
	
	NETRESOURCE ns;
	
	ns.dwScope = RESOURCE_CONNECTED;   
	ns.dwType = RESOURCETYPE_ANY; 
	ns.dwDisplayType = RESOURCEDISPLAYTYPE_GENERIC;
	ns.dwUsage = RESOURCEUSAGE_CONNECTABLE;
	
	ns.lpRemoteName	= Settings.Centric.Remote.GetBuffer(0);
	ns.lpLocalName = NULL;
	ns.lpProvider = NULL;
	
	WNetAddConnection2( &ns, Settings.Centric.Password, Settings.Centric.UserId, 0 );
	
	if ( ! WaitNamedPipe( Settings.Centric.Remote, nTimeOut ) ) return NULL;
	
	pChannel = SetChannel( new CCircuitry( xUserId ) );
	pChannel->ConnectTo( Settings.Centric.Remote, nTimeOut  );
	
	if ( ! bPeek )
	pChannel->SendPacket( new CBindPacket( xUserId, "" ) );
	
	return pChannel;
}

//////////////////////////////////////////////////////////////////////
// CNeighbour disconnect

void CNeighbour::Disconnect()
{
	if ( ! IsConnected() ) return;

	m_bConnected = FALSE;
	
	StopThread();
	
	Handshakes.Disconnect();
	
	closesocket( m_hSocket );
	m_hSocket	= INVALID_SOCKET;
	
	DisconnectNamedPipe( m_hPipe );
	m_hPipe		= INVALID_HANDLE_VALUE;
	
	for ( POSITION pos = m_pList.GetStartPosition(); pos; )
	{
		CString xUserId;
		CCircuitry* pChannel;
		
		m_pList.GetNextAssoc( pos, xUserId, (void*&)pChannel );
		
		if ( pChannel ) pChannel->Disconnect();
	}
	
	m_pList.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// CNeighbour thread run

void CNeighbour::StartThread()
{
	if ( m_hThread != NULL && m_bThread ) return;
	
	m_bThread = TRUE;
	CWinThread* pThread = AfxBeginThread( ThreadStart, this, THREAD_PRIORITY_NORMAL );
	m_hThread = pThread->m_hThread;
}

void CNeighbour::StopThread()
{
	if ( m_hThread == NULL ) return;
	
	m_bThread = FALSE;
	m_pWakeup.SetEvent();
	
	for ( int nAttempt = 50 ; nAttempt > 0 ; nAttempt-- )
	{
		DWORD nCode;
		if ( ! GetExitCodeThread( m_hThread, &nCode ) ) break;
		if ( nCode != STILL_ACTIVE ) break;
		Sleep( 100 );

		MSG msg;
		while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}
	
	if ( nAttempt == 0 )
	{
		TerminateThread( m_hThread, 0 );
		theApp.Message( MSG_DEBUG, _T("WARNING: Terminating CNeighbour thread.") );
		Sleep( 100 );
	}
	
	m_hThread = NULL;
}

UINT CNeighbour::ThreadStart(LPVOID pParam)
{
	CNeighbour* pClass = (CNeighbour*)pParam;
	pClass->OnRun();
	return 0;
}

BOOL CNeighbour::OnRun()
{
	CSingleLock pLock( &m_pSection );
	
	while ( m_bThread )
	{
		WaitForSingleObject( m_pWakeup, 50 );
		
		pLock.Lock();
		AcceptNeighbour();
		pLock.Unlock();
		if ( ! m_bThread ) break;
		
		pLock.Lock();
		Handshakes.OnRun();
		pLock.Unlock();
		if ( ! m_bThread ) break;
		
		if ( ! m_bThread ) break;
		pLock.Lock();
		AcceptCircuitry();
		pLock.Unlock();
		
		pLock.Lock();
		RunNeighbours();
		pLock.Unlock();
		if ( ! m_bThread ) break;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CNeighbour accept a Neighbour

BOOL CNeighbour::AcceptNeighbour()
{
	SOCKADDR_IN pHost;
	int nHost = sizeof(pHost);

	SOCKET hSocket = WSAAccept(	m_hSocket, (SOCKADDR*)&pHost, &nHost, NULL, (DWORD)this );
	if ( hSocket == INVALID_SOCKET ) return FALSE;
	
	CSingleLock pLock( &m_pSection, TRUE );
	
	if ( ! CreateNeighbour( hSocket, &pHost ) )
	{
		closesocket( hSocket );
		
		CString strHost = inet_ntoa( pHost.sin_addr );
		theApp.Message( MSG_ERROR, "连接被拒绝，地址:=%s，端口:=%i", (LPCTSTR)strHost, pHost.sin_port );
	}
	
	return AcceptNeighbour();
}

BOOL CNeighbour::CreateNeighbour(SOCKET hSocket, SOCKADDR_IN* pHost)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	WSAEventSelect( hSocket, m_pWakeup, FD_READ | FD_WRITE | FD_CLOSE );
	CChannel* pChannel = Network.FindChannel( ReadUserId(pHost) );
	
	if ( pChannel != NULL && pChannel->m_hSocket == INVALID_SOCKET )
	{
		pChannel->AcceptFrom( hSocket, pHost ); return TRUE;
	}
	
	return FALSE;
}

int CNeighbour::ReadUserId(SOCKADDR_IN* pHost)
{
	CXMLElement* pParent = Profiles[ "User" ];
	
	if ( CXMLElement* pXML = Profiles.LookupKey(pParent, inet_ntoa(pHost->sin_addr), "Host") )
	{
		CString strValue = pXML->GetAttributeValue( "ID" );
		return _ttoi( (LPCTSTR)strValue );
	}

	return -1;
}

//////////////////////////////////////////////////////////////////////
// CNeighbour accept neighbours

BOOL CNeighbour::AcceptCircuitry()
{
	if ( m_hPipe == INVALID_HANDLE_VALUE ) return Connect( Settings.Centric.Local );
	if ( ! HasOverlappedIoCompleted( &m_pOverlapped ) ) return TRUE;
	
	SetChannel( new CCircuitry( m_hPipe ) );
	
	m_hPipe	= INVALID_HANDLE_VALUE;
	
	return TRUE;
}

BOOL CNeighbour::RunNeighbours()
{
	for ( POSITION posNext = m_pList.GetStartPosition(); posNext; )
	{
		CString xUserId;
		CCircuitry* pChannel;
		
		m_pList.GetNextAssoc( posNext, xUserId, (void*&)pChannel );
		ASSERT( pChannel != NULL );
		
		try
		{
			if ( pChannel ) pChannel->OnRun();
		}
		catch( CHAR* sError )
		{
			theApp.Message( MSG_ERROR, "error on channel '%s': %s", xUserId, sError );
			return FALSE;
		}
#ifndef _DEBUG
		catch(_com_error e) 
		{ 
			theApp.Message( MSG_ERROR, "error on channel '%s': %s", xUserId, e.ErrorMessage() );
			return FALSE;
		}
		catch(...)
		{
			theApp.Message( MSG_ERROR, "error on channel '%s': %s", xUserId, "a fatal error" );
			return FALSE;
		}
#endif
	}
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CNeighbour CCircuitry operations

CCircuitry* CNeighbour::SetChannel(CCircuitry* pChannel)
{
	LPCTSTR xUserId = pChannel->m_xUserId;
	
	CCircuitry* pChannel2 = FindChannel( xUserId );
	if ( pChannel2 ) pChannel2->Disconnect();
	
	m_pList.SetAt( xUserId, pChannel ); 
	
	return pChannel;
}

CCircuitry* CNeighbour::FindChannel(LPCTSTR xUserId) const
{
	CCircuitry* pChannel = NULL;
	m_pList.Lookup( xUserId, (void*&)pChannel );
	
	return pChannel;
}

CCircuitry* CNeighbour::BroadcastRing(CChannel* pChannel, DWORD nReserved)
{
	CSingleLock pLock( &Neighbour.m_pSection, TRUE );
	
	CString xUserId;
	CCircuitry* pChain;
	
	if ( ! m_pPosCookie && nReserved ) return NULL;
	
	for ( ; m_pPosCookie; )
	{
		m_pList.GetNextAssoc( m_pPosCookie, xUserId, (void*&)pChain );
		ASSERT( pChain != NULL );
		
		CRingPacket* pPacket = new CRingPacket(
			pChannel->LocalId, pChannel->RemoteId, pChannel->m_nChan );
		
		pChain->SendPacket( pPacket );
	}
	
	m_pPosCookie = m_pList.GetStartPosition();
	return BroadcastRing( pChannel, nReserved++ );
}
