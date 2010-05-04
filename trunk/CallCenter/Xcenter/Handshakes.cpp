//
// Handshakes.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Handshakes.h"
#include "Settings.h"
#include "Profiles.h"
#include "XML.h"

#include "WndMain.h"
#include "WndNetwork.h"
#include "CtrlNetworkChild.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CHandshakes Handshakes;

//////////////////////////////////////////////////////////////////////
// CHandshakes construction

CHandshakes::CHandshakes()
{
	m_hSocket	= INVALID_SOCKET;
	m_hThread	= NULL;
	m_pHomeWnd	= NULL;
}

CHandshakes::~CHandshakes()
{
	Disconnect();

	ASSERT( m_hSocket == INVALID_SOCKET );
	ASSERT( m_hThread == NULL );
}

//////////////////////////////////////////////////////////////////////
// CHandshakes attributes

BOOL CHandshakes::IsConnected() const
{
	return m_bConnected;
}

void CHandshakes::Clear()
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	for ( POSITION posNext = GetIterator() ; posNext ; ) delete GetNext( posNext );
	m_pList.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// CHandshakes setup

BOOL CHandshakes::Setup()
{
	CMainWnd* pMainWnd = (CMainWnd*)theApp.m_pMainWnd;
	
	m_pHomeWnd		   = (CNetworkWnd*)pMainWnd->GetWindow( RUNTIME_CLASS(CNetworkWnd) );
	ASSERT ( m_pHomeWnd->GetSafeHwnd() != NULL );
	
	//StartThread(); 
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CHandshakes connect

BOOL CHandshakes::Connect()
{
	if ( IsConnected() ) return TRUE;
	
	CSingleLock pLock( &m_pSection, TRUE );
	
	if ( m_hSocket != INVALID_SOCKET ) return FALSE;
	
	m_hSocket = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
	if ( m_hSocket == INVALID_SOCKET ) return FALSE;
	
	m_bConnected = TRUE;
	
	SOCKADDR_IN pHost;
	
	ZeroMemory( &pHost, sizeof(pHost) );
	DWORD nHost	= inet_addr( (LPCTSTR)Settings.Monitor.Host );
	CopyMemory( &pHost.sin_addr, &nHost, 4 );

	pHost.sin_port	= htons( (WORD)Settings.Monitor.Port );
	pHost.sin_family= PF_INET;
	
	if ( bind( m_hSocket, (SOCKADDR*)&pHost, sizeof(pHost) ) ) return FALSE;
	WSAEventSelect( m_hSocket, m_pWakeup, FD_ACCEPT );
	
	listen( m_hSocket, 256 );
	
//	StartThread();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CHandshakes disconnect

void CHandshakes::Disconnect()
{
	if ( ! IsConnected() ) return;

	m_bConnected = FALSE;
	
	closesocket( m_hSocket );
	m_hSocket = INVALID_SOCKET;
	
	StopThread();
	Clear();
}

//////////////////////////////////////////////////////////////////////
// CHandshakes thread run

void CHandshakes::StartThread()
{
	if ( m_hThread != NULL && m_bThread ) return;
	
	m_bThread = TRUE;
	CWinThread* pThread = AfxBeginThread( ThreadStart, this, THREAD_PRIORITY_NORMAL );
	m_hThread = pThread->m_hThread;
}

void CHandshakes::StopThread()
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
		theApp.Message( MSG_DEBUG, _T("WARNING: Terminating CHandshakes thread.") );
		Sleep( 100 );
	}
	
	m_hThread = NULL;
}

UINT CHandshakes::ThreadStart(LPVOID pParam)
{
	CHandshakes* pClass = (CHandshakes*)pParam;
	pClass->OnRun();
	return 0;
}
#if 0
BOOL CHandshakes::OnRun()
{
	CSingleLock pLock( &m_pSection );
	
	while ( m_bThread )
	{
		WaitForSingleObject( m_pWakeup, 50 );
		
		pLock.Lock();
		AcceptHandshake();
		pLock.Unlock();
		if ( ! m_bThread ) break;
		
		pLock.Lock();
		RunHandshakes();
		pLock.Unlock();
		if ( ! m_bThread ) break;
	}

	return TRUE;
}
#else
BOOL CHandshakes::OnRun()
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	AcceptHandshake();
	
	RunHandshakes();
	return TRUE;
}
#endif
//////////////////////////////////////////////////////////////////////
// CHandshakes accept a Handshake

BOOL CHandshakes::AcceptHandshake()
{
	SOCKADDR_IN pHost;
	int nHost = sizeof(pHost);
	
	SOCKET hSocket = WSAAccept(	m_hSocket, (SOCKADDR*)&pHost, &nHost, NULL, (DWORD)this );
	if ( hSocket == INVALID_SOCKET ) return FALSE;
	
	CSingleLock pLock( &m_pSection, TRUE );
	
	CreateHandshake( hSocket, &pHost );
	
	return AcceptHandshake();
}

void CHandshakes::RunHandshakes()
{
	CSingleLock pLock( &m_pSection, TRUE );

	for ( POSITION posNext = GetIterator() ; posNext ; )
	{
		POSITION posThis = posNext;
		CHandshake* pHandshake = GetNext( posNext );
		
		if ( ! pHandshake->OnRun() && m_pList.GetAt( posThis ) == pHandshake )
		{
			delete pHandshake;
			m_pList.RemoveAt( posThis );
		}
	}
}
/*
int CALLBACK CHandshakes::AcceptCheck(IN LPWSABUF lpCallerId, IN LPWSABUF lpCallerData, IN OUT LPQOS lpSQOS, IN OUT LPQOS lpGQOS, IN LPWSABUF lpCalleeId, OUT LPWSABUF lpCalleeData, OUT GROUP FAR * g, IN DWORD dwCallbackData)
{
	if ( lpCallerId == NULL )                    return CF_REJECT;
	if ( lpCallerId->len < sizeof(SOCKADDR_IN) ) return CF_REJECT;
	
	SOCKADDR_IN* pHost = (SOCKADDR_IN*)lpCallerId->buf;
	
	if ( Handshakes.IsAccepted( &pHost->sin_addr ) )
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

BOOL CHandshakes::IsAccepted(IN_ADDR* pAddress)
{
	CSingleLock pLock( &Network.m_pSection );
	if ( ! pLock.Lock( 50 ) ) return FALSE;
	
	CString strValue = Settings.Monitor.IpAddr;
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		str.TrimLeft(); str.TrimRight();
		if ( str.IsEmpty() ) continue;
		
		if ( str.CompareNoCase( _T("*") ) == 0 ) str.Empty();
		if ( inet_addr((LPCTSTR)str) == *(DWORD*)pAddress ) return TRUE;
	}
	
	return FALSE;
}
*/
BOOL CHandshakes::CreateHandshake(SOCKET hSocket, SOCKADDR_IN* pHost)
{
	ASSERT( m_pHomeWnd->GetSafeHwnd() );
	CSingleLock pLock( &m_pSection, TRUE );
	
	int nChan = ReadUserId( pHost );
	if ( nChan == -1 ) return FALSE;
	
	WSAEventSelect( hSocket, m_pWakeup, FD_READ | FD_WRITE | FD_CLOSE );
	m_pList.AddTail( new CHandshake( hSocket, pHost, nChan ) );
	
	HTREEITEM pItem = NULL;
	CNetworkChild* pCtrlChild = NULL;
	
	for ( pCtrlChild = m_pHomeWnd->FindChild(-1, &pItem); pCtrlChild;
		  pCtrlChild = m_pHomeWnd->FindChild(-1, &pItem) )
	{
		CAchnPacket* pPacket = new CAchnPacket;
		pPacket->ChType		 = pCtrlChild->m_nChType;
		pPacket->ChFirst	 = pCtrlChild->m_nChFirst;
		pPacket->ChLength	 = pCtrlChild->GetChCount();
		SendPacketTo( pPacket );
	}

	return TRUE;
}

int CHandshakes::ReadUserId(SOCKADDR_IN* pHost)
{
	CXMLElement* pParent = Profiles[ "User" ];
	
	if ( CXMLElement* pXML = Profiles.LookupKey(pParent, inet_ntoa(pHost->sin_addr), "Host") )
	{
		CString strValue = pXML->GetAttributeValue( "ID" );
		return _ttoi( (LPCTSTR)strValue );
	}

	return -1;
}

void CHandshakes::SendPacketTo(CPacket* pPacket)
{
	for ( POSITION posNext = GetIterator() ; posNext ; )
	{
		GetNext( posNext )->SendPacket( pPacket, TRUE );
	}
	
	pPacket->Release();
}
