//
// Interlink.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Interlink.h"

#include "Neighbour.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CInterlink construction

CInterlink::CInterlink()
{
}

CInterlink::CInterlink(HANDLE hPipe) : CPipeline(hPipe)
{
	OnConnected();
}

CInterlink::~CInterlink()
{
	CInterlink::Disconnect();
}

//////////////////////////////////////////////////////////////////////
// CInterlink disconnect

void CInterlink::Disconnect()
{
	CancelIo( m_hPipe );
	WaitForSingleObject( Neighbour.m_pWakeup, 250 );
	
	CPipeline::Close();
}

//////////////////////////////////////////////////////////////////////
// CInterlink run function

BOOL CInterlink::OnRun()
{
	if ( ! CPipeline::OnRun() ) return FALSE;
	
	CPacket* pPacket = ReadBuffer( m_pInput );
	if ( pPacket == NULL ) return TRUE;
	
	BOOL bResult = OnPacket( pPacket );
	pPacket->Release();
	
	return bResult;
}

//////////////////////////////////////////////////////////////////////
// CInterlink pipe event handlers

BOOL CInterlink::OnConnected()
{
	if ( ! IsConnected() ) CPipeline::OnConnected();
	
	m_pReadIoOver->hEvent	= Neighbour.m_pWakeup;
	m_pWriteIoOver->hEvent	= Neighbour.m_pWakeup;
	
	return TRUE;
}

BOOL CInterlink::OnDropped()
{
	return FALSE;
}

void CInterlink::DoPrint(BYTE* pBuffer, DWORD nLength, LPCTSTR lpszHeader)
{
//	CPipeline::DoPrint( pBuffer, nLength, lpszHeader );
}

//////////////////////////////////////////////////////////////////////
// CInterlink packet unserialize

CPacket* CInterlink::ReadBuffer(CBuffer* pBuffer)
{
	return NULL;
}

BOOL CInterlink::OnPacket(CPacket* pPacket)
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

BOOL CInterlink::OnOkPacket(COkPacket* pPacket)
{
	return TRUE;
}

BOOL CInterlink::OnErrPacket(CErrPacket* pPacket)
{
	return TRUE;
}

BOOL CInterlink::OnByePacket(CByePacket* pPacket)
{
	Disconnect(); return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CInterlink send packet

BOOL CInterlink::SendPacket(const void* pData, DWORD nLength)
{
	m_pOutput->Add( pData, nLength );
	CPipeline::OnWrite();
	
	return TRUE;
}

BOOL CInterlink::SendPacket(CPacket* pPacket, BOOL bPeek)
{
	pPacket->EncodeBody( m_pOutput );
	CPipeline::OnWrite();
	
	if ( ! bPeek ) pPacket->Release();

	return TRUE;
}
