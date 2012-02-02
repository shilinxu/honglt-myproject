//
// Transfer.cpp
//

#include "stdafx.h"
#include "KernelManager.h"
#include "Transfer.h"

#define TEMP_BUFFER		1024

//////////////////////////////////////////////////////////////////////
// CTransfer construction

CTransfer::CTransfer() : CConnection()
{
	m_nReference		= 0;
	m_pBufferOut		= NULL;
	m_pReadIoOver		= NULL;
	m_pWriteIoOver		= NULL;
	m_hExitEvent		= NULL;
}

CTransfer::CTransfer(SOCKET hSocket, SOCKADDR_IN* pHost) : CConnection(hSocket, pHost)
{
	m_nReference		= 0;
	m_pBufferOut		= NULL;
	m_pReadIoOver		= NULL;
	m_pWriteIoOver		= NULL;
	m_hExitEvent		= NULL;
}

CTransfer::~CTransfer()
{
// 	Destroy();
// 
// 	ASSERT( m_nReference == 0 );
// 	if ( m_nReference )
// 	TRACE1("CTransfer::~CTransfer() - Unable to detach from IO thread[Ref:%d]\n", m_nReference);
	if ( m_pReadIoOver ) SAFE_DELETE(m_pReadIoOver);
	if ( m_pWriteIoOver ) SAFE_DELETE(m_pWriteIoOver);
	
	if ( m_pBufferOut ) delete [] m_pBufferOut;
}

//////////////////////////////////////////////////////////////////////
// CTransfer disconnect

void CTransfer::Destroy(BOOL bPeek)
{
	if ( m_hSocket == INVALID_SOCKET ) return;
	if ( ! IsConnected()/* && ! bPeek*/ ) { Close(); return; }

	if ( ! m_hExitEvent ) m_hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	HANDLE hExitEvent = m_hExitEvent;

	shutdown( m_hSocket, SD_BOTH );
	closesocket( m_hSocket ); m_hSocket = INVALID_SOCKET;

	if ( ! bPeek ) WaitForSingleObject(hExitEvent, INFINITE);
}

//////////////////////////////////////////////////////////////////////
// CTransfer socket event handlers

void CTransfer::OnDropped(int nError)
{
	CConnection::OnDropped( nError );

	if ( m_hExitEvent ) SetEvent( m_hExitEvent );
}

//////////////////////////////////////////////////////////////////////
// CTransfer read event handler

BOOL CTransfer::OnRead()
{
// 	CSingleLock pLock( &m_pSection, TRUE );
// 	
	if ( ! m_pReadIoOver ) m_pReadIoOver = new CWSAOverlappedRead;
	CWSAOverlappedRead* pWSAOverlapped = (CWSAOverlappedRead *)m_pReadIoOver;
	
	ATLASSERT ( pWSAOverlapped->IsCompleted() );
	
	DWORD nTransferred, dwFlags = 0;
	int nRetCode = WSARecv(m_hSocket, &pWSAOverlapped->m_pBuffer, 1, &nTransferred, &dwFlags, pWSAOverlapped, NULL);
	
	if ( (nRetCode == SOCKET_ERROR) && (WSAGetLastError() != WSA_IO_PENDING) ) return FALSE;
// 
// 	m_nReference++;
	return TRUE;
}

BOOL CTransfer::OnRead(DWORD)
{
	if ( ! CConnection::OnRead() ) return FALSE;
// 	
// 	CPacket* pPacket = ReadBuffer( m_pInput );
// 	if ( pPacket == NULL ) return TRUE;
// 	
// 	BOOL bResult = TRUE; for ( ; pPacket; pPacket = ReadBuffer(m_pInput) )
// 	{
// 		bResult |= OnPacket( pPacket );
// 		pPacket->Release();
// 	}
// 	
// 	return bResult;
}

BOOL CTransfer::OnReadCompleted(COverlapped* pOverlapped, DWORD nLength)
{
	CAutoLock<CThreadGuard> pLock( m_pSection );
// 	
// 	ASSERT ( pOverlapped->IsCompleted() );
	m_nReference--;
	
	ioctlsocket( m_hSocket, FIONREAD, &nLength );
	if ( nLength && OnRead(0) && OnRead() ) return TRUE;
	
	if ( m_nReference ) return FALSE;
	m_bConnected = FALSE;
// 	pLock.Unlock();
// 	
// 	OnDropped( WSAGetLastError() );
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CTransfer write event handler

BOOL CTransfer::OnWrite()
{
	if ( ! m_pOutput->m_nLength ) return TRUE;
	if ( m_hSocket == INVALID_SOCKET ) return FALSE;
// 
// 	CSingleLock pLock( &m_pSection, TRUE );
	
	if ( ! m_pWriteIoOver ) m_pWriteIoOver = new CWSAOverlappedWrite;
	CWSAOverlappedWrite* pWSAOverlapped = (CWSAOverlappedWrite *)m_pWriteIoOver;
// 
// 	if ( pWSAOverlapped && pWSAOverlapped->m_pBuffer.len ) return TRUE;
// 	
// 	ASSERT ( pWSAOverlapped->IsCompleted() );
// 	
// 	if ( ! m_pBufferOut ) m_pBufferOut = new BYTE [TEMP_BUFFER];
// 	pWSAOverlapped->m_pBuffer.buf = (CHAR*)m_pBufferOut;
// 	
// 	pWSAOverlapped->m_pBuffer.len = min(m_pOutput->m_nLength, TEMP_BUFFER);
// 	CopyMemory( m_pBufferOut, m_pOutput->m_pBuffer, pWSAOverlapped->m_pBuffer.len );
	
	DWORD nTransferred;
	int nRetCode = WSASend(m_hSocket, &pWSAOverlapped->m_pBuffer, 1, &nTransferred, 0, pWSAOverlapped, NULL);
	
	if ( (nRetCode == SOCKET_ERROR) && (WSAGetLastError() != WSA_IO_PENDING) ) return FALSE;
	
	m_nReference++;
	return TRUE;
}

BOOL CTransfer::OnWriteCompleted(COverlapped* pOverlapped, DWORD nLength)
{
// 	CSingleLock pLock( &m_pSection, TRUE );
// 	
// 	ASSERT ( pOverlapped->IsCompleted() );
// 	m_nReference--;
// 	
// 	DoPrint( m_pBufferOut, nLength, _T("TX:") );
// 	m_pOutput->Remove( nLength );
// 	
// 	CWSAOverlappedWrite* pWSAOverlapped = (CWSAOverlappedWrite *)pOverlapped;
// 
// 	pWSAOverlapped->m_pBuffer.len = 0;
// 	if ( m_hSocket != INVALID_SOCKET ) return OnWrite();
// // 	if ( ! m_pOutput->m_nLength ) return TRUE;
// 
// 	if ( m_nReference ) return FALSE;
// 	pLock.Unlock();
// 	
// 	OnDropped( WSAGetLastError() );
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CTransfer packet unserialize

BOOL CTransfer::ReadBuffer(CBuffer* pBuffer, DWORD)
{
	return TRUE;
}
#if 0
BOOL CTransfer::SendPacket(const void* pData, DWORD nLength)
{
	if ( ! m_bConnected ) return FALSE;
	CSingleLock pLock( &m_pSection, TRUE );
	
	m_pOutput->Add( pData, nLength );
	if ( m_bConnected ) OnWrite();
	
	return TRUE;
}

BOOL CTransfer::SendPacket(CPacket* pPacket, BOOL bPeek)
{
	if ( ! m_bConnected ) return FALSE;
	CSingleLock pLock( &m_pSection, TRUE );
	
	pPacket->EncodeBody( m_pOutput );
	if ( m_bConnected ) OnWrite();
	
	if ( ! bPeek ) pPacket->Release();

	return TRUE;
}
#endif
