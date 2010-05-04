//
// Network.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "Settings.h"
#include "Network.h"

#include "FilterGraph.h"
#include "Traffic.h"
#include "XML.h"

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
	
//	m_bConnected = TRUE;
//	CNetwork::Add( this );
	
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
	CSingleLock pLock( &m_pSection, TRUE );
	
	CNetwork::Disconnect();
	CTransfer::ConnectTo( pAddress, nPort );
	
	if ( ! m_hThread ) CNetwork::Add( this );
	
	StartThread();
	return TRUE;
}

void CNetwork::Disconnect()
{
	if ( ! m_hThread ) return;
	CSingleLock pLock( &m_pSection, TRUE );
	
	CTransfer::Disconnect();
	CNetwork::Remove( this );
	
	m_bThread = FALSE;
	m_pWakeup.SetEvent();
	
	WaitForSingleObject( m_hThread, 500 );
	StopThread();
	
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
		
		if ( ! pLock.Lock(100) ) continue;
//TRACE("%i -> CNetwork.OnRun\n", GetTickCount());
		RunNeighbours();
//TRACE("%i -> CNetwork.OnRun out...\n", GetTickCount());
		pLock.Unlock();
		if ( ! m_bThread ) break;
		
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
			theApp.Message( MSG_ERROR, "error on channel '%s': %s", strHost, sError );
			return FALSE;
		}
#ifndef _DEBUG
		catch(_com_error e) 
		{ 
			CString strHost = inet_ntoa( pTransfer->m_pHost.sin_addr );
			theApp.Message( MSG_ERROR, "error on channel '%s': %s", strHost, e.ErrorMessage() );
			return FALSE;
		}
		catch(...)
		{
			CString strHost = inet_ntoa( pTransfer->m_pHost.sin_addr );
			theApp.Message( MSG_ERROR, "error on channel '%s': %s", strHost, "a fatal error" );
			return FALSE;
		}
#endif
	}
	
	return TRUE;
}

BOOL CNetwork::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_SYNCNOTIFY )
	{
		if ( pMsg->wParam == NM_SHUTDOWN )
		{
			m_bThread = FALSE;
			m_pWakeup.SetEvent();
		}
	}
	
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
// CNetwork socket event handlers
/*
#include "Psapi.h"
#pragma comment ( lib, "psapi.lib" )

void CNetwork::OnDropped(int nError)
{
	CString str;
	str.Format( IDS_NETWORK_CONNECT_ERROR, nError );
	
	AfxMessageBox( str );
	
#ifndef _DEBUG
//	CUserChannel::OnDropped( nError );
//#else	
	if ( HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | 
			PROCESS_VM_READ | PROCESS_TERMINATE, FALSE, GetCurrentProcessId() ) )
	{
		TCHAR szPath[128];
		if ( GetModuleFileNameEx( hProcess, NULL, szPath, MAX_PATH ) )
			ShellExecute( NULL, _T("open"), szPath, "", NULL, SW_SHOWNORMAL );
		
		TerminateProcess( hProcess, 0 );
		CloseHandle( hProcess );
	}
#endif
}
*/
void CNetwork::OnHandshake()
{
	CUserChannel::OnHandshake();
	
	if ( RTPSessionLayer* pChannel = Traffic.FindChannel( RTP_Audio ) )
	{
		DWORD nIPv4Address = inet_addr( Settings.Network.Host );
		RTPIPv4Address pIPv4Address( ntohl(nIPv4Address), Settings.Audio.Port );
		
		pChannel->JoinMirror( m_xUserId, pIPv4Address );
	}
	if ( RTPSessionLayer* pChannel = Traffic.FindChannel( RTP_Video ) )
	{
		DWORD nIPv4Address = inet_addr( Settings.Network.Host );
		RTPIPv4Address pIPv4Address( ntohl(nIPv4Address), Settings.Video.Port );
		
		pChannel->JoinMirror( m_xUserId, pIPv4Address );
	}
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
