//
// Channel.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "Channel.h"

#include <afxpriv.h>
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
	
	ZeroMemory( m_xCallType, MAX_CALLTYPE + 1 );
	
	CChannel::Setup();
}

CChannel::CChannel(LPCTSTR xUserId) : CTransfer(0)
{
	m_bNavigated	= FALSE;
	m_tNavigated	= 0;
	m_nTransactionId= 0;
	
	m_xUserId	 = _tcsdup( xUserId );
	
	ZeroMemory( m_xCallType, MAX_CALLTYPE + 1 );
	
	CChannel::Setup();
}

CChannel::~CChannel()
{
	free( m_xUserId ); m_xUserId = NULL;
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

//////////////////////////////////////////////////////////////////////
// CChannel disconnect

void CChannel::Disconnect()
{
	CTransfer::Disconnect(); delete this;
}

//////////////////////////////////////////////////////////////////////
// CChannel operations

BOOL CChannel::JoinTalk(LPCTSTR xRemoteId, LPCTSTR xCallerId)
{
	ASSERT( IsConnected() );
	
	CXMLElement* pXML = new CXMLElement( NULL, ISyncHostLink );
	pXML->AddAttribute( "method", "dial" );
	
	CXMLElement* pSync = pXML->AddElement( "RemoteId" );
	pSync->SetValue( xRemoteId );
	
	if ( xCallerId && _tcslen(xCallerId) )
	{
		pSync = pXML->AddElement( "CallerId" );
		pSync->SetValue( xCallerId );
	}
	
	SendPacket( new CSyncHostLink(pXML) );
	delete pXML;
	
	m_bNavigated = TRUE;
	m_tNavigated = GetTickCount();

	CopyMemory( m_xCallType, CA_TALK, MAX_CALLTYPE );
	
	return TRUE;
}

BOOL CChannel::LeaveTalk()
{
	if ( ! IsNavigated() ) return TRUE;;
	
	CXMLElement* pXML = new CXMLElement( NULL, ISyncHostLink );
	pXML->AddAttribute( "method", "bye" );
	
	SendPacket( new CSyncHostLink(pXML) );
	delete pXML;
	
	m_bNavigated = FALSE;
	m_tNavigated = GetTickCount();
	
	CopyMemory( m_xCallType, CA_IDLE, MAX_CALLTYPE );
	
	return TRUE;
}

BOOL CChannel::PutDialup(LPCTSTR xRemoteId, LPCTSTR xCallerId)
{
	ASSERT( IsConnected() );
	CXMLElement* pSync = new CXMLElement( NULL, "SyncOrderRelation" );
	
	CXMLElement* pSub = pSync->AddElement( "MsgType" );
	pSub->SetValue( "Dialup" );
	
//	pSub = pSync->AddElement( "LocalId" );
//	pSub->SetValue( ( xLocalId && _tcslen(xLocalId) ) ? xLocalId : m_xUserId );
	
	pSub = pSync->AddElement( "RemoteId" );
	pSub->SetValue( xRemoteId );
	
	pSub = pSync->AddElement( "CallType" );
	pSub->SetValue( CA_HOOK );
	
//	SendPacket( new CSyncOrderRelation( pSync ) ); delete pSync;
	
	m_bNavigated = TRUE;
	m_tNavigated = GetTickCount();
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CChannel socket event handlers

void CChannel::OnHandshake()
{
	CTransfer::OnHandshake();
}

void CChannel::OnDropped(int nError)
{
	CXMLElement* pXML = new CXMLElement( NULL, "FaultResp" );
	
	CString strValue;
	strValue.Format( "%i", nError );
	
	pXML->AddAttribute( "faultcode", strValue );
	theApp.m_pMainWnd->SendMessage( WM_SYNCNOTIFY, GetCurrentThreadId(), (LPARAM)pXML );
	
	delete pXML;
}

//////////////////////////////////////////////////////////////////////
// CChannel TCP Transfer func

BOOL CChannel::OnPacket(CPacket* pPacket)
{
	if ( pPacket->IsCommandId(IBase_SyncOrderRelation) )
	{
		return OnSyncOrderRelation( ((CPacketBase*)pPacket)->m_pXML );
	}
	if ( pPacket->IsCommandId(IBase_SyncHostLink) )
	{
		return OnSyncHostLink( ((CPacketBase*)pPacket)->m_pXML );
	}
	if ( pPacket->IsCommandId(IBase_FaultResp) )
	{
		return OnFaultResp( ((CPacketBase*)pPacket)->m_pXML );
	}
	
	return CTransfer::OnPacket(pPacket);
}

BOOL CChannel::OnSyncOrderRelation(CXMLElement* pXML)
{
	return SendMessageToOwner( pXML );
}

BOOL CChannel::OnSyncHostLink(CXMLElement* pXML)
{
	return SendMessageToOwner( pXML );
}

BOOL CChannel::OnFaultResp(CXMLElement* pXML)
{
	return SendMessageToOwner( pXML );
}

BOOL CChannel::SendMessageToOwner(CXMLElement* pXML)
{
	if ( theApp.m_pMainWnd->GetSafeHwnd() )
	theApp.m_pMainWnd->SendMessage( WM_SYNCNOTIFY, GetCurrentThreadId(), (LPARAM)pXML );
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
// CChannel packet unserialize

CPacket* CChannel::ReadBuffer(CBuffer* pBuffer)
{
	CString strLine;
	ASSERT( pBuffer != NULL );
	
	if ( ! pBuffer->ReadLine( strLine, TRUE ) ) return NULL;
	
	CPacket* pPacket = CPacketBase::ReadBuffer( pBuffer->m_pBuffer, pBuffer->m_nLength );
	if ( ! pPacket ) return CTransfer::ReadBuffer(pBuffer);
	
	if ( pPacket->GetBufferSize() )
	{
		int nLength = pPacket->GetBufferSize();
		pBuffer->Remove( nLength + 1 + (pBuffer->m_pBuffer[nLength] == '\r') ); // + [\r] \n"
		
		return pPacket;
	}
	
	pPacket->Release();
	return NULL;
}

BOOL CChannel::SendPacket(CPacket* pPacket)
{
	if ( m_hSocket == INVALID_SOCKET ) return FALSE;
	
	return CTransfer::SendPacket( pPacket );
}
