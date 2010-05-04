//
// Transfer.cpp
//

#include "stdafx.h"
#include "Transfer.h"

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
	return CConnection::ConnectTo( (IN_ADDR*)&nAddress, nPort );
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
	return CConnection::OnRead();
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

BOOL CTransfer::SendPacket(const void * pData, DWORD nLength)
{
	m_pOutput->Add( pData, nLength );
	CConnection::OnWrite();

	return TRUE;
}
