//
// Network.cpp
//

#include "stdafx.h"
#include "Xmonitor.h"
#include "Network.h"

#include "Profiles.h"
#include "Tlhelp32.h"
#include "Psapi.h"
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
	
	m_bConnected	= FALSE;
}

CNetwork::~CNetwork()
{
	Disconnect();
	
	ASSERT( m_hThread == NULL );
}

//////////////////////////////////////////////////////////////////////
// CNetwork clear

void CNetwork::Clear()
{
	for ( POSITION posNext = m_pList.GetStartPosition(); posNext; )
	{
		CString xJobId;
		CChannel* pChannel;
		
		m_pList.GetNextAssoc( posNext, xJobId, (void*&)pChannel );
		
		if ( pChannel ) pChannel->Release();
	}
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
	
	m_bConnected = TRUE;
	
	StartThread();
	return TRUE;
}

void CNetwork::Disconnect()
{
	if ( ! IsConnected() ) return;
	
	m_bConnected = FALSE;
	
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

BOOL CNetwork::RunNeighbours()
{
	for ( POSITION posNext = m_pList.GetStartPosition(); posNext; )
	{
		CString xJobId;
		CChannel* pChannel;
		
		m_pList.GetNextAssoc( posNext, xJobId, (void*&)pChannel );
		ASSERT( pChannel != NULL );
		
		try
		{
			if ( pChannel ) pChannel->OnRun();
		}
		catch( CHAR* sError )
		{
			theApp.Message( MSG_ERROR, "error on channel %s: %s", xJobId, sError );
			return FALSE;
		}
#ifndef _DEBUG
		catch(_com_error e) 
		{ 
			theApp.Message( MSG_ERROR, "error on channel '%s': %s", xJobId, e.ErrorMessage() );
			return FALSE;
		}
		catch(...)
		{
			theApp.Message( MSG_ERROR, "error on channel '%s': %s", xJobId, "a fatal error" );
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
	LPCTSTR xJobId = pChannel->m_xJobId;
	
	CChannel* pChannel2 = FindChannel( xJobId );
	if ( pChannel2 ) pChannel2->Release();
	
	m_pList.SetAt( xJobId, pChannel ); 
	
	return pChannel;
}

CChannel* CNetwork::FindChannel(LPCTSTR xJobId) const
{
	CChannel* pChannel = NULL;
	m_pList.Lookup( xJobId, (void*&)pChannel );
	
	return pChannel;
}

//////////////////////////////////////////////////////////////////////
// CNetwork Job operations

BOOL CNetwork::RestartJob(LPCTSTR xJobId)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	POSITION posNext = NULL;
	if ( CXMLElement* pXML = Profiles.FindProcess( xJobId, posNext ) )
	{
		CString strValue = pXML->GetAttributeValue( "Path" );
		KillJob( (LPCTSTR)strValue );
	}
	
	return TRUE;
}

BOOL CNetwork::KillJob(LPCTSTR xJobId)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if ( hSnapshot == INVALID_HANDLE_VALUE ) return FALSE;
	
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof( PROCESSENTRY32 );
	
	for ( BOOL bRetval = Process32First(hSnapshot,&pe); bRetval; )
	{
		if ( HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | 
			PROCESS_VM_READ | PROCESS_TERMINATE, FALSE, pe.th32ProcessID ) )
		{
			TCHAR szPath[128];
			if ( GetModuleFileNameEx( hProcess, NULL, szPath, MAX_PATH ) )
			{
				if ( ! _tcsicmp( xJobId, szPath ) )
				{
					TerminateProcess( hProcess, 0 );
					
					CloseHandle( hProcess ); return TRUE;
				}
			}
			
			CloseHandle( hProcess );
		}
		
		pe.dwSize = sizeof( PROCESSENTRY32 );
		bRetval = Process32Next( hSnapshot, &pe );
	}
	
	CloseHandle( hSnapshot );
	return FALSE;
}
