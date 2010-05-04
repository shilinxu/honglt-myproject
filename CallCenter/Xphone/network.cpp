//
// Network.cpp
//

#include "stdafx.h"
#include "Xphone.h"
#include "Network.h"

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
}

CNetwork::~CNetwork()
{
	Disconnect();
}

//////////////////////////////////////////////////////////////////////
// CNetwork clear

void CNetwork::Clear()
{
	for ( POSITION pos = GetIterator() ; pos ; ) delete GetNext( pos );
}

//////////////////////////////////////////////////////////////////////
// CNetwork connection

BOOL CNetwork::ConnectTo(LPCTSTR pAddress, int nPort)
{
	if ( ! CTransfer::ConnectTo( pAddress, nPort ) ) return FALSE;

	WSAEventSelect( m_hSocket, Network.m_pWakeup, FD_CONNECT|FD_READ|FD_WRITE|FD_CLOSE );
	
	StartThread();
	return TRUE;
}

void CNetwork::Disconnect()
{
	StopThread();
	Clear();
	
	CTransfer::Disconnect();
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
		
		pLock.Lock();
//TRACE("%i -> CNetwork.OnRun\n", GetTickCount());
		RunNeighbours();
//TRACE("%i -> CNetwork.OnRun out...\n", GetTickCount());
		pLock.Unlock();
		if ( ! m_bThread ) break;
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CNetwork check neighbours

void CNetwork::RunNeighbours()
{
	for ( POSITION posNext = GetIterator() ; posNext ; )
	{
		CTransfer* pTransfer = GetNext( posNext );
		
		if ( ! pTransfer->OnRun() ) return;
	}
}

BOOL CNetwork::OnByePacket(CByePacket* pPacket)
{
	CUserChannel::Disconnect(); return FALSE;
}

void CNetwork::OnDropped(int nError)
{
	CUserChannel::OnDropped( nError );
}

//////////////////////////////////////////////////////////////////////
// CNetwork add and remove

void CNetwork::Add(CTransfer* pTransfer)
{
	ASSERT( m_pList.Find( pTransfer ) == NULL );
	m_pList.AddHead( pTransfer );
}

void CNetwork::Remove(CTransfer* pTransfer)
{
	POSITION pos = m_pList.Find( pTransfer );
	ASSERT( pos != NULL );
	m_pList.RemoveAt( pos );
}
