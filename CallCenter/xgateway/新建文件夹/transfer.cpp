//
// Transfer.cpp
//

#include "stdafx.h"
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
	m_bConnected	= FALSE;
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
	if ( IsConnected() ) SendPacket( new IExitPacket );
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

BOOL CTransfer::OnConnected()
{
	//m_bConnected	= TRUE;
	m_tConnected	= GetTickCount();
	
	return TRUE;
}

void CTransfer::OnDropped(BOOL bError)
{
	CString strHost = inet_ntoa( m_pHost.sin_addr );
	_RPTF1(_CRT_WARN, _T("WARNING: Dropped Connection From %s\n"), strHost);
	
	Disconnect();
}

void CTransfer::OnHandshake()
{
	m_bConnected	= TRUE;
	m_tConnected	= GetTickCount();
}

BOOL CTransfer::OnRead()
{
	if ( ! CConnection::OnRead() ) return FALSE;
	
	IPacket* pPacket = ReadBuffer( m_pInput );
	if ( pPacket == NULL ) return TRUE;

	BOOL bResult = OnPacket( pPacket );
	pPacket->Release();
		
	return bResult;
}

BOOL CTransfer::OnPacket(IPacket* pPacket)
{
	if ( pPacket->IsCommandId( ICP_OK ) )
	{
		return OnOkPacket( (IOkPacket*)pPacket );
	}
	else
	if ( pPacket->IsCommandId( ICP_ERR ) )
	{
		return OnErrPacket( (IErrPacket*)pPacket );
	}
	
	return TRUE;
}

BOOL CTransfer::OnOkPacket(IOkPacket* pPacket)
{
	return TRUE;
}

BOOL CTransfer::OnErrPacket(IErrPacket* pPacket)
{
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CTransfer packet unserialize

IPacket* CTransfer::ReadBuffer(CBuffer* pBuffer)
{
	CString strLine;
	ASSERT( pBuffer != NULL );
	if ( ! pBuffer->ReadString( strLine ) ) return NULL;
	
	LPBYTE pByte = (LPBYTE)strLine.GetBuffer(0);
	int nLength = strLine.GetLength();

	IPacket* pPacket = IPacket::ReadBuffer( pByte, nLength );

	if ( pPacket == NULL )
	{
#ifdef _DEBUG
		afxDump << _T("Invalid packet: ") << (LPCTSTR)strLine << "\n";
#endif
		SendPacket( new IErrPacket(ICP_ERROR) ); return NULL;
	}

	return pPacket;
}

//////////////////////////////////////////////////////////////////////
// CTransfer send packet

BOOL CTransfer::SendPacket(IPacket* pPacket)
{
	pPacket->EncodeBody( m_pOutput );
	CConnection::OnWrite();
	pPacket->Release();

	return TRUE;
}
