//
// Network.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "Network.h"

#include "ChannelUser.h"
#include "RTPSession2.h"

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
	Release();
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

BOOL CNetwork::IsConnected() const
{
	return m_bConnected;
}

//////////////////////////////////////////////////////////////////////
// CNetwork setup

BOOL CNetwork::Setup()
{
	Release();
	Connect();
	
	return TRUE;
}

void CNetwork::Release()
{
	Disconnect();
	Clear();
}

//////////////////////////////////////////////////////////////////////
// CNetwork connection

BOOL CNetwork::Connect()
{
	if ( IsConnected() ) return TRUE;
	
	StartThread();
	
	m_bConnected = TRUE;
	
	return TRUE;
}

void CNetwork::Disconnect()
{
	if ( ! IsConnected() ) return;
	
	m_bConnected = FALSE;
	m_pRTPSession.Destroy();
	
	StopThread();
	Release();
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
	pNetwork->OnRun();
	return 0;
}

BOOL CNetwork::OnRun()
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

CTransfer* CNetwork::Find(CTransfer* pTransfer)
{
	POSITION pos = m_pList.Find( pTransfer );
	if ( pos == NULL ) return NULL;
	
	return (CTransfer*)m_pList.GetAt( pos );
}
