//
// Channel.cpp
//

#include "stdafx.h"
#include "Xphone.h"
#include "Channel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CChannel construction

CChannel::CChannel()
{
	m_bNavigated	= FALSE;
	m_tNavigated	= 0;
	
	ZeroMemory( m_xCallType, MAX_CALLTYPE + 1 );
}

CChannel::~CChannel()
{
	Disconnect();
}

//////////////////////////////////////////////////////////////////////
// CChannel attributes

void CChannel::Setup()
{
}

void CChannel::Clone(CChannel* pParent)
{
	m_bNavigated	= pParent->m_bNavigated;
	m_tNavigated	= pParent->m_tNavigated;
	
	CopyMemory( m_xCallType, pParent->m_xCallType, MAX_CALLTYPE );
	
	if ( pParent->m_hSocket != INVALID_SOCKET )
	{
		this->AttachTo( pParent );
	}
}

BOOL CChannel::IsNavigated() const
{
	return IsConnected() && m_bNavigated;
}

//////////////////////////////////////////////////////////////////////
// CChannel socket event handlers

void CChannel::OnHandshake()
{
	CTransfer::OnHandshake();
}

void CChannel::OnDropped(int nError)
{
	CTransfer::OnDropped( nError );
	
	CErrPacket* pPacket = new CErrPacket( nError );
	theApp.m_pMainWnd->SendMessage( WM_PACKETREQ, 0L, (LPARAM)pPacket );
	
	pPacket->Release();
}

//////////////////////////////////////////////////////////////////////
// CChannel TCP Transfer func

BOOL CChannel::OnPacket(CPacket* pPacket)
{
	return CTransfer::OnPacket(pPacket);
}

BOOL CChannel::OnOkPacket(COkPacket* pPacket)
{
	if ( ! IsConnected() ) OnHandshake();
	
	return CTransfer::OnOkPacket( pPacket );
}

//////////////////////////////////////////////////////////////////////
// CChannel packet unserialize

CPacket* CChannel::ReadBuffer(CBuffer* pBuffer)
{
	CString strLine;
	ASSERT( pBuffer != NULL );
	
	if ( ! pBuffer->ReadLine( strLine, TRUE ) ) return NULL;
	
	LPBYTE pByte = (LPBYTE)strLine.GetBuffer(0);
	int nLength = strLine.GetLength();
	
	CPacket* pPacket = CPacketBase::ReadBuffer( pBuffer->m_pBuffer, pBuffer->m_nLength );
	
	if ( pPacket == NULL )
	{
		pBuffer->Remove( nLength + 1 + (pBuffer->m_pBuffer[nLength] == '\r') ); // + [\r] \n"
		SendPacket( new CErrPacket(EBase_Unknown) ); return NULL;
	}
	else
	if ( pPacket->CommandId != NULL )
	{
		nLength = pPacket->GetBufferSize();
		pBuffer->Remove( nLength + 1 + (pBuffer->m_pBuffer[nLength] == '\r') ); // + [\r] \n"
		return pPacket;
	}
	
	pPacket->Release();
	return NULL;
}
