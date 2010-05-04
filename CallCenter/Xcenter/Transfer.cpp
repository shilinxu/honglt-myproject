//
// Transfer.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Transfer.h"
#include "Network.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CTransfer construction

CTransfer::CTransfer(SOCKET hSocket, SOCKADDR_IN* pHost) : CConnection( hSocket, pHost )
{
}

CTransfer::CTransfer()
{
}

CTransfer::~CTransfer()
{
	ASSERT( m_hSocket == INVALID_SOCKET );
}

//////////////////////////////////////////////////////////////////////
// CTransfer connect

BOOL CTransfer::ConnectTo(LPCTSTR pAddress, int nPort)
{
	DWORD nAddress = inet_addr( pAddress );
	CConnection::ConnectTo( (IN_ADDR*)&nAddress, nPort );
	
	WSAEventSelect( m_hSocket, Network.m_pWakeup, FD_CONNECT|FD_READ|FD_WRITE|FD_CLOSE );
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CTransfer disconnect

void CTransfer::Disconnect()
{
	if ( IsConnected() ) SendPacket( new CByePacket );

	CConnection::Close();
}

//////////////////////////////////////////////////////////////////////
// CTransfer run function

BOOL CTransfer::OnRun()
{
	if ( ! CConnection::OnRun() ) return FALSE;
	
	DWORD tNow = GetTickCount();
	
	if ( ! IsConnected() && tNow - m_tConnected >= 15* 1000 )
	{
		OnDropped( FALSE ); return FALSE;
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CTransfer socket event handlers

void CTransfer::OnDropped(BOOL bError)
{
	CString strHost = inet_ntoa( m_pHost.sin_addr );
	_RPTF1(_CRT_WARN, _T("WARNING: Dropped Connection From %s\n"), strHost);
	
	Disconnect();
}

void CTransfer::OnHandshake()
{
}

BOOL CTransfer::OnRead()
{
	if ( ! CConnection::OnRead() ) return FALSE;
	
	CPacket* pPacket = ReadBuffer( m_pInput );
	if ( pPacket == NULL ) return TRUE;
	
	BOOL bResult = OnPacket( pPacket );
	pPacket->Release();
		
	// 由于OnPacket可能更新了this指针
	// 由FALSE返回上层操作
	return FALSE;
}

void CTransfer::DoPrint(BYTE* pBuffer, DWORD nLength, LPCTSTR lpszHeader)
{
	CString strLine = lpszHeader;
	strLine += ToASCII( (BYTE*)pBuffer, nLength );
	
	theApp.Message( MSG_DEBUG, (LPCTSTR)strLine );
}

//////////////////////////////////////////////////////////////////////
// CTransfer packet unserialize

BOOL CTransfer::OnPacket(CPacket* pPacket)
{
	if ( pPacket->IsCommandId( "OK" ) )
	{
		return OnOkPacket( (COkPacket*)pPacket );
	}
	else
	if ( pPacket->IsCommandId( "ERR" ) )
	{
		return OnErrPacket( (CErrPacket*)pPacket );
	}
	else
	if ( pPacket->IsCommandId( "BYE" ) )
	{
		return OnByePacket( (CByePacket*)pPacket );
	}
	
	return TRUE;
}

BOOL CTransfer::OnOkPacket(COkPacket* pPacket)
{
	return TRUE;
}

BOOL CTransfer::OnErrPacket(CErrPacket* pPacket)
{
	return FALSE;
}

BOOL CTransfer::OnByePacket(CByePacket* pPacket)
{
	Disconnect(); return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CTransfer send packet

BOOL CTransfer::SendPacket(CPacket* pPacket)
{
	if ( m_hSocket == INVALID_SOCKET ) return FALSE;
	
	pPacket->EncodeBody( m_pOutput );
	CConnection::OnWrite();
	pPacket->Release();

	return TRUE;
}
