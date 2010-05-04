//
// Neighbour.cpp
//

#include "stdafx.h"
#include "Xgateway.h"
#include "Neighbour.h"
#include "Settings.h"

#include "Circuitry.h"
#include "Datagram.h"

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
	m_hThread	= NULL;
	m_bThread	= FALSE;
	
	m_hPipe		= INVALID_HANDLE_VALUE;
	m_bConnected= FALSE;
}

CNeighbour::~CNeighbour()
{
	Disconnect();
	
	ASSERT( m_hPipe == INVALID_HANDLE_VALUE );
	ASSERT( m_hThread == NULL );
}

//////////////////////////////////////////////////////////////////////
// CNeighbour attributes

BOOL CNeighbour::IsConnected() const
{
	return m_bConnected;
}

//////////////////////////////////////////////////////////////////////
// CNeighbour connection

BOOL CNeighbour::Connect()
{
	if ( IsConnected() ) return TRUE;
	
	CSingleLock pLock( &m_pSection, TRUE );
	
	m_bConnected = TRUE;
	
	StartThread(); return TRUE;
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

CCircuitry* CNeighbour::ConnectTo(LPCTSTR xUserId, DWORD nTimeOut)
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
	
	pChannel->SendPacket( new CBindPacket( xUserId, "" ) );
	
	Connect(); return pChannel;
}

//////////////////////////////////////////////////////////////////////
// CNeighbour disconnect

void CNeighbour::Disconnect()
{
	if ( ! IsConnected() ) return;

	m_bConnected = FALSE;
	
	StopThread();
	
	DisconnectNamedPipe( m_hPipe );
	m_hPipe		= INVALID_HANDLE_VALUE;
	
	for ( POSITION pos = m_pList.GetStartPosition(); pos; )
	{
		CString xUserId;
		CCircuitry* pChannel;
		
		m_pList.GetNextAssoc( pos, xUserId, (void*&)pChannel );
		
		if ( pChannel ) pChannel->Release();
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
		Sleep( 100 );
	}
	
	m_hThread = NULL;
}

UINT CNeighbour::ThreadStart(LPVOID pParam)
{
	CNeighbour* pNeighbour = (CNeighbour*)pParam;
	pNeighbour->OnRun();
	return 0;
}

BOOL CNeighbour::OnRun()
{
	CSingleLock pLock( &m_pSection );
	
	while ( m_bThread )
	{
		WaitForSingleObject( m_pWakeup, 50 );
		
		if ( ! m_bThread ) break;
		if ( pLock.Lock( 250 ) )
		{
//TRACE("%i->>>>>>>>> CNeighbour AcceptCircuitry in\n", GetTickCount());
			AcceptCircuitry();
//TRACE("%i->>>>>>>>> CNeighbour AcceptCircuitry out.......\n", GetTickCount());
			pLock.Unlock();
		}
		
		if ( ! m_bThread ) break;
		if ( pLock.Lock( 250 ) )
		{
//TRACE("%i->>>>>>>>> CNeighbour in\n", GetTickCount());
			RunNeighbours();
//TRACE("%i->>>>>>>>> CNeighbour out.......\n", GetTickCount());
			pLock.Unlock();
		}
	}
	
	return TRUE;
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
	if ( pChannel2 ) pChannel2->Release();
	
	m_pList.SetAt( xUserId, pChannel ); 
	
	return pChannel;
}

CCircuitry* CNeighbour::FindChannel(LPCTSTR xUserId) const
{
	CCircuitry* pChannel = NULL;
	m_pList.Lookup( xUserId, (void*&)pChannel );
	
	return pChannel;
}
