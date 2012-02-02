//
// TransferManager.cpp
//

#include "stdafx.h"
#include "KernelManager.h"
#include "TransferManager.h"
// #include "Randomize.h"
// 
// CTransferManager TransferManager;

//////////////////////////////////////////////////////////////////////
// CTransferManager construction

CTransferManager::CTransferManager() : m_hWakeup( NULL )
{
	m_hCompletionPort	= NULL;
	m_nReference		= 0;

	m_hWakeup			= CreateEvent(NULL, FALSE, FALSE, NULL);
}

CTransferManager::~CTransferManager()
{
	StopService();

	CloseHandle( m_hWakeup ); m_hWakeup	= NULL;
}

//////////////////////////////////////////////////////////////////////
// CTransferManager attributes
// 
// DWORD CTransferManager::GetIdentifier() const
// {
// 	GUID Guid;
// 	CoCreateGuid(&Guid);
// 
// 	DWORD dwRandom = CRandomize::GetRandom( (WORD*)NULL );
// 	/*DWORD*/ dwRandom   *= CRandomize::GetRandom( (DWORD*)NULL );
// 
// 	dwRandom   ^= Guid.Data1;	dwRandom   ^= Guid.Data2;
// 	dwRandom   ^= Guid.Data3;	dwRandom   ^= *((DWORD *)Guid.Data4);
// 
// 	TRACE("[%u] %i-> GetIdentifier:=%u\n", GetCurrentProcessId(),GetCurrentThreadId(), dwRandom);
// 	return dwRandom;
// }

//////////////////////////////////////////////////////////////////////
// CTransferManager OVERLOAD function

BOOL CTransferManager::Attach(ITransferable* pTransfer, HANDLE hHandle, LONG lEvent)
{
	CAutoLock<CThreadGuard> pLock( m_pSection );
	
	CreateIoCompletionPort(hHandle, m_hCompletionPort, (ULONG_PTR)pTransfer, 0);
	m_pTransferArray.Add( pTransfer );

	WSAEventSelect( (SOCKET)hHandle, m_hWakeup, lEvent );
	SetEvent( m_hWakeup );
	return TRUE;
}

BOOL CTransferManager::Detach(ITransferable* pTransfer)
{
	CAutoLock<CThreadGuard> pLock( m_pSection );

	for ( INT_PTR i = 0; i < m_pTransferArray.GetCount(); i++ )
	{
		if ( m_pTransferArray[ i ] != pTransfer ) continue;

		m_pTransferArray.RemoveAt( i ); return TRUE;
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CTransferManager operations

BOOL CTransferManager::StartService()
{
	if ( (ULONG)InterlockedIncrement( &m_nReference ) > 1 ) return TRUE;
	
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);
	
	DWORD dwThreadCount = SystemInfo.dwNumberOfProcessors;
	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, SystemInfo.dwNumberOfProcessors);
#ifdef _DEBUG
	dwThreadCount = 1;
#endif
	for ( DWORD i = 0 ; i < dwThreadCount; i++ )
	{
		CTransferThread* pTransferThread = new CTransferThread( m_hCompletionPort );
		
		m_pTransferThreadArray.Add( pTransferThread );
		pTransferThread->StartThread();
	}

	return StartThread();
}

BOOL CTransferManager::StopService()
{
	if ( ! IsRuning() ) return TRUE;
	if ( (ULONG)InterlockedDecrement(&m_nReference) ) return TRUE;

	m_bThread = FALSE; SetEvent( m_hWakeup );

	StopThread(); PostCleanup();

	for ( INT_PTR i = 0; i < m_pTransferThreadArray.GetCount(); i++ )
	{
		if ( m_hCompletionPort ) PostQueuedCompletionStatus(m_hCompletionPort,0,NULL,NULL);
	}

	for ( i = 0; i < m_pTransferThreadArray.GetCount(); i++ )
	{
		CTransferThread* pTransferThread = m_pTransferThreadArray[ i ];
		
		if ( pTransferThread ) pTransferThread->StopThread();
		SAFE_DELETE( pTransferThread );
	}
	m_pTransferThreadArray.RemoveAll();

	for ( i = 0; i < m_pTransferArray.GetCount(); i++ )
	{
		m_pTransferArray[ i ]->Release();
	}
	m_pTransferArray.RemoveAll();
	
	if ( m_hCompletionPort != NULL )
	{
		CloseHandle( m_hCompletionPort );
		m_hCompletionPort = NULL;
	}
	
	return TRUE;
}

void CTransferManager::PostCleanup()
{
	CAutoLock<CThreadGuard> pLock( m_pSection );

	for ( int i = 0; i < m_pTransferArray.GetCount(); i++ )
	{
		m_pTransferArray[ i ]->Destroy( TRUE );
	}
}

//////////////////////////////////////////////////////////////////////
// CTransferManager thread run

BOOL CTransferManager::DoRun()
{
	CAutoLock<CThreadGuard> pLock( m_pSection );

	for ( INT_PTR i = 0; i < m_pTransferArray.GetCount(); i++ )
	{
		ITransferable* pTransfer = m_pTransferArray[ i ];

		if ( pTransfer ) pTransfer->OnRun();
	}

// 	pLock.Unlock();
// 	WaitForSingleObject( m_pWakeup, 6000 ); //WaitForSingleObject函数用来检测hHandle事件的信号状态，在某一线程中调用该函数时，线程暂时挂起，如果在挂起的dwMilliseconds毫秒内，线程所等待的对象变为有信号状态，则该函数立即返回；如果超时时间已经到达dwMilliseconds毫秒，但hHandle所指向的对象还没有变成有信号状态，函数照样返回。参数dwMilliseconds有两个具有特殊意义的值：0和INFINITE。若为0，则该函数立即返回；若为INFINITE，则线程一直被挂起，直到hHandle所指向的对象变为有信号状态时为止。
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CTransferThread construction

CTransferThread::CTransferThread()
{
	m_hCompletionPort	= NULL;
}

CTransferThread::CTransferThread(HANDLE hCompletionPort)
{
	m_hCompletionPort	= hCompletionPort;
}

CTransferThread::~CTransferThread()
{
}

//////////////////////////////////////////////////////////////////////
// CTransferThread OVERLOAD function

BOOL CTransferThread::OnThreadStart()
{
// 	CRandomize::CreateRandom();

	return CServiceThread::OnThreadStart();
}

//////////////////////////////////////////////////////////////////////
// CTransferThread thread run

BOOL CTransferThread::DoRun()
{
	DWORD nTransferred = 0;
	
	OVERLAPPED* pOverlapped = NULL;
	ITransferable* pTransfer = NULL;
	
	BOOL bSuccess = GetQueuedCompletionStatus(m_hCompletionPort, &nTransferred, (PULONG_PTR)&pTransfer, &pOverlapped, INFINITE);

	if ( GetLastError() == ERROR_PORT_UNREACHABLE || GetLastError() == ERROR_MORE_DATA ) return TRUE;
	if ( ! bSuccess && GetLastError() != ERROR_NETNAME_DELETED && GetLastError() != ERROR_OPERATION_ABORTED ) return FALSE;
	if ( ! pTransfer && ! pOverlapped ) return FALSE;
	
	COverlapped* pOperationIoOver = (COverlapped *)pOverlapped;
	
	if ( pOperationIoOver->OperationType == OperationType_Read )
	{
		pTransfer->OnReadCompleted( pOperationIoOver, nTransferred );
	}
	else if ( pOperationIoOver->OperationType == OperationType_Write )
	{
		pTransfer->OnWriteCompleted( pOperationIoOver, nTransferred );
	}
	
	return TRUE;
}
