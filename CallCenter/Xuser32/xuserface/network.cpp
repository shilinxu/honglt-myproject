//
// Network.cpp
//

#include "stdafx.h"
#include "Network.h"

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
	m_hThread		= NULL;
	m_bThread		= FALSE;
}

CNetwork::~CNetwork()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////
// CNetwork clear

void CNetwork::Clear()
{
	for ( POSITION pos = GetIterator() ; pos ; )
	{
		GetNext( pos )->Disconnect();
	}
}

//////////////////////////////////////////////////////////////////////
// CNetwork attributes

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
	
	StartThread();
	return TRUE;
}
#if 0
BOOL CNetwork::Connect(IN_ADDR* pAddress, WORD nPort)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	if ( m_hSocket != INVALID_SOCKET ) return FALSE;
	
	m_hSocket = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
	if ( m_hSocket == INVALID_SOCKET ) return FALSE;
	
	SOCKADDR_IN pHost;
	
	ZeroMemory( &pHost, sizeof(pHost) );
	pHost.sin_addr	= *pAddress;
	
	pHost.sin_port	= htons( nPort );
	pHost.sin_family= PF_INET;
	
	if ( bind( m_hSocket, (SOCKADDR*)&pHost, sizeof(pHost) ) ) return FALSE;
	WSAEventSelect( m_hSocket, m_pWakeup, FD_ACCEPT );
	
	listen( m_hSocket, 256 );
	
	return Connect();
}
#endif
BOOL CNetwork::ConnectTo(LPCTSTR pAddress, int nPort)
{
	CNetwork::Disconnect( 0 );
	CSingleLock pLock( &m_pSection, TRUE );
	
	DWORD nAddress = inet_addr( pAddress );
	CTransfer::ConnectTo( (IN_ADDR*)&nAddress, nPort );
	
	if ( ! m_hThread ) CNetwork::Add( this );
	
	StartThread();
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CNetwork disconnect

void CNetwork::Disconnect()
{
	CTransfer::Disconnect();
	
	DWORD nThreadId = GetCurrentThreadId();
	PostThreadMessage( nThreadId, TM_SHUTDOWN, 0L, 0 );	
}

void CNetwork::Disconnect(DWORD)
{
	if ( ! m_hThread ) return;
	CSingleLock pLock( &m_pSection, TRUE );
	
	CTransfer::Disconnect();
	CNetwork::Remove( this );
	
//	CGraphRender::Release();
	
	m_bThread = FALSE;
	m_pWakeup.SetEvent();
	
	WaitForSingleObject( m_hThread, 500 );
	StopThread();
	
	Datagrams.Release();
	Clear();
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
		_RPTF0(_CRT_WARN, _T("WARNING: Terminating CNetwork thread.\n"));
		Sleep( 100 );
	}
	
	m_hThread = NULL;
}

UINT CNetwork::ThreadStart(LPVOID pParam)
{
	CNetwork* pNetwork = (CNetwork*)pParam;
	pNetwork->OnRun( 0 );
	return 0;
}

BOOL CNetwork::OnRun(DWORD nReserved)
{
	CSingleLock pLock( &m_pSection );
	
	while ( m_bThread )
	{
		WaitForSingleObject( m_pWakeup, 50 );
		
		if ( ! m_bThread ) break;
		if ( pLock.Lock( 250 ) )
		{
			RunNeighbours();
			pLock.Unlock();
		}
		
		if ( ! m_bThread ) break;
		{
			Datagrams.OnRun();
		}
		
		MSG msg;
		while ( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
		{
			if ( GetMessage( &msg, 0, 0, 0 ) ) PreTranslateMessage(&msg);
		}
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CNetwork accept neighbours

BOOL CNetwork::RunNeighbours()
{
	for ( POSITION posNext = GetIterator() ; posNext ; )
	{
		CTransfer* pTransfer = GetNext( posNext );
		
		try
		{
			if ( ! pTransfer->OnRun() ) return FALSE;
		}
		catch( CHAR* sError )
		{
			CString strHost = inet_ntoa( pTransfer->m_pHost.sin_addr );
			XDebugLog( MSG_ERROR, "error on channel '%s': %s", strHost, sError );
			return FALSE;
		}
#ifndef _DEBUG
		catch(...)
		{
			CString strHost = inet_ntoa( pTransfer->m_pHost.sin_addr );
			XDebugLog( MSG_ERROR, "error on channel '%s': %s", strHost, "a fatal error" );
			return FALSE;
		}
#endif
	}
	
	return TRUE;
}

BOOL CNetwork::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == TM_SHUTDOWN )
	{
		m_bThread = FALSE; m_pWakeup.SetEvent();
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CNetwork add and remove

void CNetwork::Add(CTransfer* pTransfer)
{
	if ( m_pList.Find( pTransfer ) ) return;
	m_pList.AddHead( pTransfer );
}

void CNetwork::Remove(CTransfer* pTransfer)
{
	POSITION pos = m_pList.Find( pTransfer );
	if ( pos == NULL ) return;
	
	m_pList.RemoveAt( pos );
}
