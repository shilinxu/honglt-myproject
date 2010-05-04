//
// Network.cpp
//

#include "stdafx.h"
#include "Xgateway.h"
#include "Network.h"
#include "Settings.h"
#include "Profiles.h"

#include "Datagrams.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CNetwork Network;

//////////////////////////////////////////////////////////////////////
// CNetwork construction

CNetwork::CNetwork()
{
	m_bConnected	= FALSE;
	m_hThread		= NULL;
	m_bThread		= FALSE;
	m_hSocket		= INVALID_SOCKET;
}

CNetwork::~CNetwork()
{
	Disconnect();
	
	ASSERT( m_hSocket == INVALID_SOCKET );
	ASSERT( m_hThread == NULL );
}

//////////////////////////////////////////////////////////////////////
// CNetwork attributes

BOOL CNetwork::IsConnected() const
{
	return m_bConnected;
}

int CNetwork::IsWellConnected() const
{
	if ( ! IsConnected() ) return 0;
	return m_pList.GetCount();
}

//////////////////////////////////////////////////////////////////////
// CNetwork connection

BOOL CNetwork::Connect()
{
	if ( IsConnected() ) return TRUE;
	
	CSingleLock pLock( &m_pSection, TRUE );
	
	if ( m_hSocket != INVALID_SOCKET ) return FALSE;
	
	m_hSocket = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
	if ( m_hSocket == INVALID_SOCKET ) return FALSE;
	
	m_bConnected = TRUE;
	
	SOCKADDR_IN pHost;
	ZeroMemory( &pHost, sizeof(pHost) );
	
	DWORD nHost	= inet_addr( (LPCTSTR)Settings.Network.Host );
	CopyMemory( &pHost.sin_addr, &nHost, 4 );
	
	pHost.sin_port	= htons( (WORD)Settings.Network.Port );
	pHost.sin_family= PF_INET;
	
	if ( bind( m_hSocket, (SOCKADDR*)&pHost, sizeof(pHost) ) ) return FALSE;
	WSAEventSelect( m_hSocket, m_pWakeup, FD_ACCEPT );
	
	listen( m_hSocket, SOMAXCONN );
	Datagrams.Connect( Settings.Network.Port );
	
	StartThread();
	m_bConnected = TRUE;
	
	return TRUE;
}

void CNetwork::Disconnect()
{
	if ( ! IsConnected() ) return;
	
	m_bConnected = FALSE;
	
	closesocket( m_hSocket );
	m_hSocket = INVALID_SOCKET;
	
	StopThread();
	
	for ( POSITION pos = m_pList.GetStartPosition(); pos; )
	{
		CString xUserId;
		CChannel* pChannel;
		
		m_pList.GetNextAssoc( pos, xUserId, (void*&)pChannel );
		
		if ( pChannel ) pChannel->Release();
	}
	
	m_pList.RemoveAll();
	Datagrams.Release();
}

//////////////////////////////////////////////////////////////////////
// CNetwork thread run

void CNetwork::StartThread()
{
	if ( m_hThread != NULL && m_bThread ) return;
	
	m_bThread = TRUE;
	CWinThread* pThread = AfxBeginThread( ThreadStart, this, THREAD_PRIORITY_NORMAL );
	m_hThread = pThread->m_hThread;
}

void CNetwork::StopThread()
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
		theApp.Message( MSG_DEBUG, _T("WARNING: Terminating CNetwork thread.") );
		Sleep( 100 );
	}
	
	m_hThread = NULL;
}

UINT CNetwork::ThreadStart(LPVOID pParam)
{
	CNetwork* pNetwork = (CNetwork*)pParam;
	pNetwork->OnRun();
	return 0;
}

BOOL CNetwork::OnRun()
{
	CSingleLock pLock( &m_pSection );
	
	while ( m_bThread )
	{
		WaitForSingleObject( m_pWakeup, 50 );
		
		if ( ! m_bThread ) break;
		if ( pLock.Lock( 250 ) )
		{
//TRACE("%i-> CNetwork AcceptNeighbour in\n", GetTickCount());
			AcceptNeighbour();
//TRACE("%i-> CNetwork AcceptNeighbour out.......\n", GetTickCount());
			pLock.Unlock();
		}
		
		if ( ! m_bThread ) break;
		if ( pLock.Lock( 250 ) )
		{
//TRACE("%i-> CNetwork in\n", GetTickCount());
			RunNeighbours();
//TRACE("%i-> CNetwork out.......\n", GetTickCount());
			pLock.Unlock();
		}
		
		if ( ! m_bThread ) break;
		{
//TRACE("%i-> CDatagrams in\n", GetTickCount());
			Datagrams.OnRun();
//TRACE("%i-> CDatagrams out.......\n", GetTickCount());
		}
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CNetwork accept a Neighbour

BOOL CNetwork::AcceptNeighbour()
{
	SOCKADDR_IN pHost;
	int nHost = sizeof(pHost);

	SOCKET hSocket = WSAAccept(	m_hSocket, (SOCKADDR*)&pHost, &nHost, AcceptCheck, (DWORD)this );
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

int CALLBACK CNetwork::AcceptCheck(IN LPWSABUF lpCallerId, IN LPWSABUF lpCallerData, IN OUT LPQOS lpSQOS, IN OUT LPQOS lpGQOS, IN LPWSABUF lpCalleeId, OUT LPWSABUF lpCalleeData, OUT GROUP FAR * g, IN DWORD dwCallbackData)
{
	if ( lpCallerId == NULL )                    return CF_REJECT;
	if ( lpCallerId->len < sizeof(SOCKADDR_IN) ) return CF_REJECT;
	
	SOCKADDR_IN* pHost = (SOCKADDR_IN*)lpCallerId->buf;
	
	if ( Profiles.IsDenied( &pHost->sin_addr ) )
	{
		CString strHost = inet_ntoa( pHost->sin_addr );
		theApp.Message( MSG_ERROR, "连接被拒绝，地址:=%s，端口:=%i", (LPCTSTR)strHost, pHost->sin_port );
		return CF_REJECT;
	}
	else
	{
		return CF_ACCEPT;
	}
}

BOOL CNetwork::CreateNeighbour(SOCKET hSocket, SOCKADDR_IN* pHost)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	WSAEventSelect( hSocket, m_pWakeup, FD_READ | FD_WRITE | FD_CLOSE );
	
	SetChannel( new CChannel( hSocket, pHost ) );
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CNetwork check neighbours

BOOL CNetwork::RunNeighbours()
{
	for ( POSITION posNext = m_pList.GetStartPosition(); posNext; )
	{
		CString xUserId;
		CChannel* pChannel;
		
		m_pList.GetNextAssoc( posNext, xUserId, (void*&)pChannel );
		ASSERT( pChannel != NULL );
		
#ifndef _DEBUG
		try
#endif
		{
			if ( ! pChannel->OnRun() ) return FALSE;
		}
#ifndef _DEBUG
		catch( CHAR* sError )
		{
			theApp.Message( MSG_ERROR, "error on channel %s: %s", xUserId, sError );
			return FALSE;
		}
//#ifndef _DEBUG
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
// CNetwork Channel operations

CChannel* CNetwork::SetChannel(CChannel* pChannel)
{
	LPCTSTR xUserId = pChannel->m_xUserId;
	
	CChannel* pChannel2 = FindChannel( xUserId );
	if ( pChannel2 ) pChannel2->Disconnect();
	
	m_pList.SetAt( xUserId, pChannel ); 
	
	return pChannel;
}

CChannel* CNetwork::FindChannel(LPCTSTR xUserId) const
{
	CChannel* pChannel = NULL;
	m_pList.Lookup( xUserId, (void*&)pChannel );
	
	return pChannel;
}
