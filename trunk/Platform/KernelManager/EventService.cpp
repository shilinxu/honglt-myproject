//
// EventService.cpp
//

#include "stdafx.h"
#include "KernelManager.h"
#include "EventService.h"

TCHAR CEventService::szMessageBuffer[16384];

//////////////////////////////////////////////////////////////////////
// CEventService construction

CEventService::CEventService() : m_pApplication( NULL )
{
}

CEventService::~CEventService()
{
	CleanEvent();
}

//////////////////////////////////////////////////////////////////////
// CEventService attributes

CEventService* CEventService::Default()
{
	static CEventService pEventService;
	return &pEventService;
}

/////////////////////////////////////////////////////////////////////////////
// CEventService message handlers

BOOL CEventService::StartService(IApplication* pApplication)
{
	Default()->m_pApplication	= pApplication;

	return Default()->StartThread();
}

BOOL CEventService::StopService()
{
	Default()->StopThread();
	Default()->CleanEvent();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CEventService message operations

void CEventService::Message(int nType, LPCTSTR pszFormat, ...)
{
	va_list pArgs;
	va_start( pArgs, pszFormat );

	Default()->Print( nType, pszFormat, pArgs );
	va_end( pArgs );
}

void CEventService::Print(int nType, LPCTSTR pszFormat, ...)
{
	CAutoLock<CThreadGuard> pLock( m_pSection );
	va_list pArgs;

	va_start( pArgs, pszFormat );
	Print( nType, pszFormat, pArgs );
	va_end( pArgs );
}

void CEventService::Print(int nType, LPCTSTR pszFormat, va_list pArgs)
{
	CAutoLock<CThreadGuard> pLock( m_pSection );
	_vsntprintf( szMessageBuffer, 16380, pszFormat, pArgs );

	PrintMessage( nType, szMessageBuffer );
}

BOOL CEventService::PrintMessage(int nType, LPCTSTR pszMsg)
{
	if ( ! m_pApplication ) return FALSE;
	return m_pApplication->PrintMessage(nType, pszMsg);
}

//////////////////////////////////////////////////////////////////////
// CEventService event operations

void CEventService::CreateEvent(WORD wEventCode, DWORD dwReference, LPONEVENTBLOCK fnModuleProc, DWORD dwTimestamp, DWORD dwParam)
{
	Default()->_AddEvent_(wEventCode, dwReference, fnModuleProc, dwTimestamp, dwParam);
}

void CEventService::ResetEvent(WORD wEventCode, DWORD dwReference, DWORD dwEndReason, DWORD dwParam)
{
	Default()->_ResetEvent_(wEventCode, dwReference, dwEndReason, dwParam);
}

void CEventService::ResetEvent(WORD wEventCode, DWORD dwReference/*, DWORD dwEndReason*/)
{
	Default()->_RemoveEvent_(wEventCode, dwReference);
}

WORD CEventService::_AddEvent_(WORD wEventCode, DWORD dwReference, LPONEVENTBLOCK fnModuleProc, DWORD dwTimestamp, DWORD dwParam)
{
	CAutoLock<CThreadGuard> pLock( m_pSection );

	try
	{
		LPEVENTBLOCK pEventBlock = NULL;
		INT_PTR nStroeCount = m_pEventBlockPtrRelease.GetCount(); if (nStroeCount>0)
		{
			pEventBlock = m_pEventBlockPtrRelease[nStroeCount-1];
			m_pEventBlockPtrRelease.RemoveAt(nStroeCount-1);
		}
		else
		{
			pEventBlock = new EVENTBLOCK;
		}

		ZeroMemory( pEventBlock, sizeof(EVENTBLOCK) );

		pEventBlock->wEventCode		= wEventCode;
		pEventBlock->dwReference	= dwReference;
		pEventBlock->fnModuleProc	= fnModuleProc;
		pEventBlock->dwParam		= dwParam;

// 		if ( dwTimestamp )
		{
			pEventBlock->dwTimestamp= dwTimestamp + GetTickCount();
			m_pEventBlockPtrActive.Add( pEventBlock );
		}
// 		else
// 		{
// 			m_pEventBlockPtrSet.Add( pEventBlock );
// 		}

// 		m_pWakeup.SetEvent();
	}
	catch (...)
	{
		return 0;
	}
}

BOOL CEventService::_ResetEvent_(WORD wEventCode, DWORD dwReference, DWORD dwEndReason, DWORD dwParam)
{
	CAutoLock<CThreadGuard> pLock( m_pSection );

	INT_PTR nIndex = _FindEvent_( wEventCode, dwReference );
	if ( nIndex == -1 ) return FALSE;

	LPEVENTBLOCK pEventBlock = m_pEventBlockPtrActive[ nIndex ];
// 	pEventBlock->dwEndReason = dwEndReason;

	m_pEventBlockPtrActive.RemoveAt( nIndex );
#if 0
	m_pEventBlockPtrSet.Add( pEventBlock );

	m_pWakeup.SetEvent();
#else
	m_pSection.Unlock();
	OnEventBlock( pEventBlock, dwEndReason, dwParam );

// 	pLock.Lock();
// 	m_pEventBlockPtrRelease.Add( pEventBlock );
#endif
	return TRUE;
}

BOOL CEventService::_RemoveEvent_(WORD wEventCode, DWORD dwReference)
{
	CAutoLock<CThreadGuard> pLock( m_pSection );

	INT_PTR nIndex = _FindEvent_( wEventCode, dwReference );
	if ( nIndex == -1 ) return FALSE;

	LPEVENTBLOCK pEventBlock = m_pEventBlockPtrActive[ nIndex ];

	m_pEventBlockPtrRelease.Add( pEventBlock );
	m_pEventBlockPtrActive.RemoveAt( nIndex );

	return TRUE;
}

INT_PTR CEventService::_FindEvent_(WORD wEventCode, DWORD dwReference)
{
	CAutoLock<CThreadGuard> pLock( m_pSection );

	LPEVENTBLOCK pEventBlock;
	for( INT_PTR nIndex = 0; nIndex < m_pEventBlockPtrActive.GetCount(); nIndex++ )
	{
		pEventBlock = m_pEventBlockPtrActive[ nIndex ];

		if ( pEventBlock->wEventCode != wEventCode ) continue;
		if ( pEventBlock->dwReference == dwReference ) return nIndex;
	}
	return -1;
}

void CEventService::CleanEvent()
{
// 	for( INT_PTR nIndex = 0; nIndex < m_pEventBlockPtrSet.GetCount(); nIndex++ )
// 	{
// 		SAFE_DELETE( m_pEventBlockPtrSet[ nIndex ] );
// 	}
	for( INT_PTR nIndex = 0; nIndex < m_pEventBlockPtrActive.GetCount(); nIndex++ )
	{
		SAFE_DELETE( m_pEventBlockPtrActive[ nIndex ] );
	}
	for( /*INT_PTR*/ nIndex = 0; nIndex < m_pEventBlockPtrRelease.GetCount(); nIndex++ )
	{
		SAFE_DELETE( m_pEventBlockPtrRelease[ nIndex ] );
	}

// 	m_pEventBlockPtrSet.RemoveAll();
	m_pEventBlockPtrActive.RemoveAll();
	m_pEventBlockPtrRelease.RemoveAll();
}

BOOL CEventService::DoRun()
{
#if 0
	PostEventRun( NULL );

	PostEventBlock( E_SYS_NoError );
#else
	LPEVENTBLOCK pEventBlock;
	if ( GetEventBlock( pEventBlock ) ) OnEventBlock( pEventBlock, E_SYS_TimeOut, 0 );
#endif
// 	WaitForSingleObject(m_pWakeup, 50);
	return TRUE;
}

BOOL CEventService::GetEventBlock(LPEVENTBLOCK& pEventBlock)
{
	CAutoLock<CThreadGuard> pLock( m_pSection );

	for( INT_PTR nIndex = 0; nIndex < m_pEventBlockPtrActive.GetCount(); nIndex++ )
	{
		/*LPEVENTBLOCK */pEventBlock = m_pEventBlockPtrActive[ nIndex ];

		if ( GetTickCount() < pEventBlock->dwTimestamp ) continue;
		pEventBlock->dwEndReason	= E_SYS_TimeOut;

		m_pEventBlockPtrActive.RemoveAt( nIndex );
		return TRUE;
	}
	return FALSE;
}
// 
// INT_PTR CEventService::PostEventRun(LPEVENTBLOCK* ppEventBlock)
// {
// 	CAutoLock<CThreadGuard> pLock( m_pSection );
// 
// 	for( INT_PTR nIndex = 0; nIndex < m_pEventBlockPtrActive.GetCount(); nIndex++ )
// 	{
// 		LPEVENTBLOCK pEventBlock = m_pEventBlockPtrActive[ nIndex ];
// 
// 		if ( GetTickCount() < pEventBlock->dwTimestamp ) continue;
// 
// 		if ( ppEventBlock ) ( * ppEventBlock ) = pEventBlock;
// 		pEventBlock->dwEndReason	= E_SYS_TimeOut;
// 
// 		m_pEventBlockPtrActive.RemoveAt( nIndex );
// 		m_pEventBlockPtrSet.Add( pEventBlock );
// 
// // 		m_pWakeup.SetEvent(); return nIndex;
// 	}
// 	return -1;
// }
// 
// void CEventService::PostEventBlock(DWORD dwEndReason/* = E_SYS_NoError*/)
// {
// 	for( INT_PTR nIndex = 0; nIndex < m_pEventBlockPtrSet.GetCount(); nIndex++ )
// 	{
// 		LPEVENTBLOCK pEventBlock = m_pEventBlockPtrSet[ nIndex ];
// 
// 		OnEventBlock( pEventBlock, pEventBlock->dwEndReason );
// 
// 		m_pEventBlockPtrRelease.Add( pEventBlock );
// 		m_pEventBlockPtrSet.RemoveAt( nIndex );
// 	}
// }

void CEventService::OnEventBlock(LPEVENTBLOCK pEventBlock, DWORD dwEndReason, DWORD dwParam)
{
	if ( ! pEventBlock->fnModuleProc ) return;
	(*pEventBlock->fnModuleProc)(pEventBlock->wEventCode, pEventBlock->dwReference, dwEndReason, dwParam, (LPVOID)pEventBlock->dwParam);

	CAutoLock<CThreadGuard> pLock( m_pSection );
	m_pEventBlockPtrRelease.Add( pEventBlock );
}