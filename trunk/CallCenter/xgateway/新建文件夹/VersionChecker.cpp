//
// VersionChecker.cpp
//

#include "stdafx.h"
#include "Xgateway.h"
#include "Settings.h"
#include "VersionChecker.h"
#include "VersionUpgrader.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CVersionChecker VersionChecker;

//////////////////////////////////////////////////////////////////////
// CVersionChecker setup

BOOL CVersionChecker::Setup()
{
	return TRUE;
}

void CVersionChecker::Release()
{
	CSingleLock pLock( &m_pSection, TRUE );
}

//////////////////////////////////////////////////////////////////////
// CVersionChecker construction

CVersionChecker::CVersionChecker()
{
	m_hSocket	= INVALID_SOCKET;
	m_hThread	= NULL;
}

CVersionChecker::~CVersionChecker()
{
	Disconnect();

	ASSERT( m_hSocket == INVALID_SOCKET );
	ASSERT( m_hThread == NULL );
}

//////////////////////////////////////////////////////////////////////
// CVersionChecker attributes

BOOL CVersionChecker::IsConnected() const
{
	return m_bConnected;
}

//////////////////////////////////////////////////////////////////////
// CVersionChecker connect

BOOL CVersionChecker::Connect()
{
	if ( IsConnected() ) return TRUE;
	
	CSingleLock pLock( &m_pSection, TRUE );
	
	if ( m_hSocket != INVALID_SOCKET ) return FALSE;
	
	m_hSocket = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
	if ( m_hSocket == INVALID_SOCKET ) return FALSE;
	
	m_bConnected = TRUE;
	
	SOCKADDR_IN pHost;
	
	ZeroMemory( &pHost, sizeof(pHost) );
	DWORD nHost	= inet_addr( (LPCTSTR)Settings.VersionChecker.Host );
	CopyMemory( &pHost.sin_addr, &nHost, 4 );
	
	pHost.sin_port	= htons( (WORD)Settings.VersionChecker.Port );
	pHost.sin_family= PF_INET;
	
	if ( bind( m_hSocket, (SOCKADDR*)&pHost, sizeof(pHost) ) ) return FALSE;
	WSAEventSelect( m_hSocket, m_pWakeup, FD_ACCEPT );
	
	listen( m_hSocket, 256 );
	
	StartThread();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CVersionChecker disconnect

void CVersionChecker::Disconnect()
{
	if ( ! IsConnected() ) return;

	m_bConnected = FALSE;
	
	closesocket( m_hSocket );
	m_hSocket = INVALID_SOCKET;
	
	StopThread();
	Clear();
}

void CVersionChecker::Clear()
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	for ( POSITION posNext = GetIterator() ; posNext ; ) GetNext(posNext)->Disconnect();
	m_pList.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// CVersionChecker thread run

void CVersionChecker::StartThread()
{
	if ( m_hThread != NULL && m_bThread ) return;
	
	m_bThread = TRUE;
	CWinThread* pThread = AfxBeginThread( ThreadStart, this, THREAD_PRIORITY_NORMAL );
	m_hThread = pThread->m_hThread;
}

void CVersionChecker::StopThread()
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
	}
	
	if ( nAttempt == 0 )
	{
		TerminateThread( m_hThread, 0 );
		Sleep( 100 );
	}
	
	m_hThread = NULL;
}

UINT CVersionChecker::ThreadStart(LPVOID pParam)
{
	CVersionChecker* pClass = (CVersionChecker*)pParam;
	pClass->OnRun();
	return 0;
}

void CVersionChecker::OnRun()
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
		RunNeighbours();
		pLock.Unlock();
	}
}

//////////////////////////////////////////////////////////////////////
// CVersionChecker accept a Neighbour

BOOL CVersionChecker::AcceptNeighbour()
{
	SOCKADDR_IN pHost;
	int nHost = sizeof(pHost);

	SOCKET hSocket = WSAAccept(	m_hSocket, (SOCKADDR*)&pHost, &nHost, NULL, (DWORD)this );
	if ( hSocket == INVALID_SOCKET ) return FALSE;
	
	CSingleLock pLock( &m_pSection, TRUE );
	
	CreateNeighbour( hSocket, &pHost );
	
	return AcceptNeighbour();
}

BOOL CVersionChecker::CreateNeighbour(SOCKET hSocket, SOCKADDR_IN* pHost)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	WSAEventSelect( hSocket, m_pWakeup, FD_READ | FD_WRITE | FD_CLOSE );
	m_pList.AddTail( new CVersionUpgrader( hSocket, pHost ) );
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CVersionChecker check neighbours

void CVersionChecker::RunNeighbours()
{
	CSingleLock pLock( &m_pSection, TRUE );

	for ( POSITION posNext = GetIterator() ; posNext ; )
	{
		POSITION posThis = posNext;
		CVersionUpgrader* pVersion = GetNext( posNext );
		
		if ( ! pVersion->OnRun() )
			m_pList.RemoveAt( posThis );
	}
}
