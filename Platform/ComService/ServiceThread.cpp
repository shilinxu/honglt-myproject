//
// ServiceThread.cpp
//

#include "stdafx.h"
#include <Process.h>
#include "ServiceThread.h"

//////////////////////////////////////////////////////////////////////
// CServiceThread construction

CServiceThread::CServiceThread()
{
	m_hThread		= NULL;
	m_bThread		= FALSE;
	m_nThreadId		= 0;
}

CServiceThread::~CServiceThread()
{
	StopThread();
}

//////////////////////////////////////////////////////////////////////
// CServiceThread attributes

BOOL CServiceThread::IsRuning() const
{
	if ( m_hThread == NULL ) return FALSE;
	
	return WaitForSingleObject(m_hThread, 0) == WAIT_TIMEOUT;
}

//////////////////////////////////////////////////////////////////////
// CServiceThread thread run

BOOL CServiceThread::StartThread()
{
	if ( IsRuning() ) return FALSE;
	
	m_bThread = TRUE;
	m_hThread = (HANDLE)::_beginthreadex(NULL,0,ThreadStart,this,0,&m_nThreadId);
	
	return TRUE;
}

BOOL CServiceThread::StopThread(DWORD dwTimeOut)
{
	if ( ! IsRuning() ) return TRUE;
	
	m_bThread = FALSE;
	if ( WaitForSingleObject(m_hThread, dwTimeOut) == WAIT_TIMEOUT ) return FALSE;

	CloseHandle( m_hThread );
	m_hThread = NULL;
	
	return TRUE;
}

BOOL CServiceThread::TerminateThread(DWORD dwExitCode)
{
	if ( m_hThread == NULL ) return TRUE;
	
	::TerminateThread( m_hThread, dwExitCode );
	
	CloseHandle( m_hThread );
	m_hThread = NULL;
	
	return TRUE;
}

UINT __stdcall CServiceThread::ThreadStart(LPVOID pParam)
{
	CServiceThread* pServiceThread = (CServiceThread*)pParam;

	pServiceThread->OnThreadStart();

	pServiceThread->OnRun();
	
	pServiceThread->OnThreadConclude();

	_endthreadex(0);
	return 0;
}

BOOL CServiceThread::OnRun()
{
	while( m_bThread )
	{
#ifndef _DEBUG
		try
#endif
		{
			if ( ! DoRun() ) 
				break;
		}
#ifndef _DEBUG
		catch (...)
		{
		}
#endif
	}
	
	return TRUE;
}
