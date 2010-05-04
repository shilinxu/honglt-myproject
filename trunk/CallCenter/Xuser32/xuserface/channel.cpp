//
// Channel.cpp
//

#include "stdafx.h"
#include "Channel.h"

#include <afxpriv.h>
#include "Network.h"
#include "XML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CChannel construction

CChannel::CChannel(SOCKET hSocket, SOCKADDR_IN* pHost) : CTransfer(hSocket, pHost)
{
	m_bNavigated	= FALSE;
	m_tNavigated	= 0;
	m_nTransactionId= 0;
	
	m_xUserId		= NULL;
	m_nLastError	= 0;
	
	ZeroMemory( m_xCallType, MAX_CALLTYPE + 1 );
	
	m_pXMLCookie	= NULL;
	m_hEventResp = CreateEvent(NULL, FALSE, FALSE, NULL);
	
	CChannel::Setup();
}

CChannel::CChannel(LPCTSTR xUserId) : CTransfer(0)
{
	m_bNavigated	= FALSE;
	m_tNavigated	= 0;
	m_nTransactionId= 0;
	
	m_xUserId	 = _tcsdup( xUserId );
	m_nLastError	= 0;
	
	ZeroMemory( m_xCallType, MAX_CALLTYPE + 1 );
	
	m_pXMLCookie	= NULL;
	m_hEventResp = CreateEvent(NULL, FALSE, FALSE, NULL);
	
	CChannel::Setup();
}

CChannel::~CChannel()
{
	free( m_xUserId ); m_xUserId = NULL;
	
	CloseHandle( m_hEventResp );
	if ( m_pXMLCookie ) m_pXMLCookie->Delete();
}

//////////////////////////////////////////////////////////////////////
// CChannel attributes

void CChannel::Setup()
{
	m_bNavigated	= FALSE;
	m_tNavigated	= GetTickCount();
	
	CopyMemory( m_xCallType, CA_IDLE, MAX_CALLTYPE );
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

void CChannel::SetUserId(LPCTSTR xUserId)
{
	free( m_xUserId ); m_xUserId = NULL;
	m_xUserId	= _tcsdup( xUserId );
}

DWORD CChannel::CallWorker(CXMLElement** ppXML, DWORD nTimeOut)
{
	if (WaitForSingleObject(m_hEventResp, nTimeOut) == WAIT_OBJECT_0)
	{
		CSingleLock pLock( &Network.m_pSection, TRUE );
		
		if ( m_nLastError ) return m_nLastError;
		if ( ppXML ) *ppXML = m_pXMLCookie->Clone();
		
		return m_nLastError;
	}
	
	return WAIT_TIMEOUT;
}

//////////////////////////////////////////////////////////////////////
// CChannel disconnect

void CChannel::Disconnect()
{
	CTransfer::Disconnect(); delete this;
}

//////////////////////////////////////////////////////////////////////
// CChannel operations

BOOL CChannel::DialupWith(CXMLElement* pXML)
{
	ASSERT( IsConnected() );
	if ( IsNavigated() ) return FALSE;
	
	CSingleLock pLock( &Network.m_pSection, TRUE );
	
	ASSERT( ! pXML->GetAttributeValue("method").CompareNoCase( "dialup" ) );
	
	SendPacket( new CSyncHostFile(pXML) );
	
	m_bNavigated = TRUE;
	m_tNavigated = GetTickCount();
	
	CopyMemory( m_xCallType, CA_TALK, MAX_CALLTYPE );
	
	return TRUE;
}

BOOL CChannel::RingWith(CXMLElement* pXML)
{
	ASSERT( IsConnected() );
	if ( IsNavigated() ) return FALSE;
	
	CSingleLock pLock( &Network.m_pSection, TRUE );
	
	m_bNavigated = TRUE;
	m_tNavigated = GetTickCount();
	
	CopyMemory( m_xCallType, CA_HOOK, MAX_CALLTYPE );
	
	return TRUE;
}

BOOL CChannel::LeaveTalk()
{
	ASSERT( IsConnected() );
	if ( ! IsNavigated() ) return TRUE;
	
	CSingleLock pLock( &Network.m_pSection, TRUE );
	
	CXMLElement* pXML = new CXMLElement( NULL, ISyncHostFile );
	pXML->AddAttribute( "method", "bye" );
	
	SendPacket( new CSyncHostFile(pXML) );
	delete pXML;
	
	m_bNavigated = FALSE;
	m_tNavigated = GetTickCount();
	
	CopyMemory( m_xCallType, CA_IDLE, MAX_CALLTYPE );
	
	return TRUE;
}

BOOL CChannel::OnRecordFile()
{
	if ( ! IsNavigated() ) return FALSE;
	
	CSingleLock pLock( &Network.m_pSection, TRUE );
	
	CXMLElement* pXML = new CXMLElement( NULL, ISyncHostFile );
	pXML->AddAttribute( "method", "rec" );
	
	SendPacket( new CSyncHostFile(pXML) );
	delete pXML;
	
	return TRUE;
}

void CChannel::StartTalking()
{
	CSingleLock pLock( &Network.m_pSection, TRUE );
	
	CXMLElement* pXML = new CXMLElement( NULL, ISyncHostFile );
	pXML->AddAttribute( "method", "talk" );
	
	SendPacket( new CSyncHostFile(pXML) );
	delete pXML;
}

//////////////////////////////////////////////////////////////////////
// CChannel socket event handlers

BOOL CChannel::OnConnected()
{
	CTransfer::OnConnected();
	
	m_nLastError = 0;
	SetEvent( m_hEventResp );
	
	return TRUE;
}

void CChannel::OnDropped(int nError)
{
	CTransfer::OnDropped(nError);
	
	m_nLastError = nError;
	SetEvent( m_hEventResp );
}

void CChannel::OnPacketed(CXMLElement* pXML)
{
	m_pXMLCookie->Delete();
	m_pXMLCookie = pXML->Clone();
	
	m_nLastError = 0;
	SetEvent( m_hEventResp );
}

//////////////////////////////////////////////////////////////////////
// CChannel TCP Transfer func

BOOL CChannel::OnPacket(CPacket* pPacket)
{
	if ( pPacket->IsCommandId(IBase_SyncHostFile) )
	{
		return OnSyncHostFile( ((CPacketBase*)pPacket)->m_pXML );
	}
	if ( pPacket->IsCommandId(IBase_FaultResp) )
	{
		return OnFaultResp( ((CPacketBase*)pPacket)->m_pXML );
	}
	
	return CTransfer::OnPacket(pPacket);
}

BOOL CChannel::OnSyncHostFile(CXMLElement* pXML)
{
#if 0
	CString strMethod = pXML->GetAttributeValue("method");
	
	if ( ! strMethod.CompareNoCase("ring") )
	{
		RingWith( pXML );
	}
	else if ( ! strMethod.CompareNoCase("bye") )
	{
		LeaveTalk();
	}
	else
	{
		ASSERT( FALSE );
	}
#endif
	OnPacketed( pXML ); return TRUE;
}

BOOL CChannel::OnFaultResp(CXMLElement* pXML)
{
	OnPacketed( pXML ); return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CChannel packet unserialize

CPacket* CChannel::ReadBuffer(CBuffer* pBuffer)
{
	CString strLine;
	ASSERT( pBuffer != NULL );
	
	if ( ! pBuffer->ReadLine( strLine, TRUE ) ) return NULL;
	
	CPacket* pPacket = CPacketBase::ReadBuffer( pBuffer->m_pBuffer, pBuffer->m_nLength );
	
	if ( pPacket && pPacket->GetBufferSize() )
	{
		int nLength = pPacket->GetBufferSize();
		pBuffer->Remove( nLength + 1 + (pBuffer->m_pBuffer[nLength] == '\r') ); // + [\r] \n"
		
		return pPacket;
	}
	else if ( ! pPacket )
	{
		int nLength = strLine.GetLength();
		pBuffer->Remove( nLength + 1 + (pBuffer->m_pBuffer[nLength] == '\r') ); // + [\r] \n"
	}
	
	pPacket->Release();
	return NULL;
}


BOOL CChannel::SendPacket(CPacket* pPacket)
{
	CSingleLock pLock( &Network.m_pSection, TRUE );
	
	if ( m_hSocket == INVALID_SOCKET ) return FALSE;
	
	return CTransfer::SendPacket( pPacket );
}
