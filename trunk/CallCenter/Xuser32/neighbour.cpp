//
// Neighbour.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "Neighbour.h"

#include "ChannelUser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CNeighbour Neighbour;

//////////////////////////////////////////////////////////////////////
// CNeighbour construction

CNeighbour::CNeighbour()
{
	m_hSocket	= INVALID_SOCKET;
	m_hThread	= NULL;
	m_bThread	= FALSE;
	
	m_bConnected= FALSE;
}

CNeighbour::~CNeighbour()
{
	Disconnect();
	
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
	pHost.sin_addr.s_addr	= htonl( INADDR_ANY );
	
	pHost.sin_port	= htons( 1960 );
	pHost.sin_family= PF_INET;
	
	if ( bind( m_hSocket, (SOCKADDR*)&pHost, sizeof(pHost) ) ) return FALSE;
	WSAEventSelect( m_hSocket, m_pWakeup, FD_ACCEPT );
	
	listen( m_hSocket, 256 );
	
	StartThread();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CNeighbour disconnect

void CNeighbour::Disconnect()
{
	if ( ! IsConnected() ) return;

	m_bConnected = FALSE;
	
	StopThread();
	
	closesocket( m_hSocket );
	m_hSocket	= INVALID_SOCKET;
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
	
	if ( CreateNeighbour( hSocket, &pHost ) )
	{
		closesocket( m_hSocket );
		m_hSocket	= INVALID_SOCKET;
		
		m_bThread = FALSE;
		return TRUE;
	}
	
	closesocket( hSocket );
	return AcceptNeighbour();
}

BOOL CNeighbour::CreateNeighbour(SOCKET hSocket, SOCKADDR_IN* pHost)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	WSAEventSelect( hSocket, m_pWakeup, FD_READ | FD_WRITE | FD_CLOSE );
	CChannel* pChannel = new CUserChannel( hSocket, pHost );
	
	return TRUE;
}
