//
// ChannelUser.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "ChannelUser.h"
#include "PacketCore.h"
#include "Settings.h"
#include "Traffic.h"
#include "XML.h"

//#include "DlgJoinVisitor.h"
#include "CtrlNetworkPanel.h"
#include "Network.h"

//////////////////////////////////////////////////////////////////////
// CUserChannel construction

CUserChannel::CUserChannel(SOCKET hSocket, SOCKADDR_IN* pHost) : CChannel(hSocket, pHost)
{
}

CUserChannel::CUserChannel(LPCTSTR xUserId) : CChannel(xUserId)
{
	m_pVisitorWnd= NULL;
}

CUserChannel::~CUserChannel()
{
//	if ( m_pVisitorWnd ) delete m_pVisitorWnd;
}

//////////////////////////////////////////////////////////////////////
// CUserChannel attributes

void CUserChannel::Setup()
{
	CChannel::Setup();
}

void CUserChannel::SetUserId(LPCTSTR xUserId)
{
	free( m_xUserId );
	
	m_xUserId = _tcsdup( xUserId );
}
/*
BOOL CUserChannel::IsNavigated(LPCTSTR xCallType) const
{
	return CChannel::IsNavigated() && IsCallType( xCallType );
}
*/

//////////////////////////////////////////////////////////////////////
// CUserChannel disconnect

void CUserChannel::Disconnect()
{
	theApp.m_pMainWnd->PostMessage( WM_CLOSE );
	
	CChannel::Disconnect();
}

//////////////////////////////////////////////////////////////////////
// CUserChannel HostLink sync func

BOOL CUserChannel::OnSyncHostLink(CXMLElement* pXML)
{
	CString strMethod = pXML->GetAttributeValue("method");
	
	if ( ! strMethod.CompareNoCase("dial") )
	{
		return OnSyncHostshaking( pXML );
	}
	else if ( ! strMethod.CompareNoCase("bye") )
	{
		return OnSyncHostBreaking( pXML );
	}
	
	return CChannel::OnSyncHostLink(pXML);
}

BOOL CUserChannel::OnSyncHostshaking(CXMLElement* pXML)
{
	CSingleLock pLock( &Network.m_pSection, TRUE );
	
	CXMLElement* pSync = pXML->GetElementByName( "RemoteId" );
	CString xRemoteId = pSync->GetValue();
	
	pSync = pXML->GetElementByName( "CallerId" );
	Network.JoinTalk( xRemoteId, pSync->GetValue() );
	
	return TRUE;
}

BOOL CUserChannel::OnSyncHostBreaking(CXMLElement* pXML)
{
	CSingleLock pLock( &Network.m_pSection, TRUE );
	
	Network.LeaveTalk();
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CUserChannel operations
#if 0
BOOL CUserChannel::JoinTalk(LPCTSTR xRemoteId, LPCTSTR xCallerId)
{
	if ( m_pVisitorWnd ) return FALSE;
	
	m_pVisitorWnd = new CJoinVisitorDlg( this );
	
	m_pVisitorWnd->ShowWindow( SW_SHOWNORMAL );
	m_pVisitorWnd->BringWindowToTop();
	
	return CChannel::JoinTalk(xRemoteId, xCallerId);
}
#else
BOOL CUserChannel::JoinTalk(LPCTSTR xRemoteId, LPCTSTR xCallerId)
{
	CWaitCursor pCursor;
	
	// start local render
	
//	wndPanel.SendMessage( WM_STARTPREVIEW, RenderPayload(PayloadMask_Video|PayloadMask_Audio) );
//	wndPanel.SendMessage( WM_STARTPREVIEW, RenderPayload(PayloadMask_Audio) );
	
	// start remote preview
	
//	wndPanel.SendMessage( WM_STARTPREVIEW, PreviewPayload(PayloadMask_Video|PayloadMask_Audio), (LPARAM)xRemoteId );
	wndPanel.SendMessage( WM_STARTPREVIEW, PreviewPayload(PayloadMask_Audio), (LPARAM)"Xgateway" );
	
	CSingleLock pLock( &Traffic.m_pSection, TRUE );
	
	if ( RTPSessionLayer* pChannel = Traffic.FindChannel(RTP_Audio) )
	{
		pChannel->JoinRecord( TRUE );
	}
	
	return CChannel::JoinTalk(xRemoteId, xCallerId);
}

BOOL CUserChannel::LeaveTalk()
{
	CWaitCursor pCursor;
	
	wndPanel.SendMessage( WM_STOPRENDER );
	
	CSingleLock pLock( &Traffic.m_pSection, TRUE );
	
	if ( RTPSessionLayer* pChannel = Traffic.FindChannel(RTP_Audio) )
	{
		pChannel->JoinRecord( FALSE );
	}
	
	return CChannel::LeaveTalk();
}
#endif

//////////////////////////////////////////////////////////////////////
// CUserChannel send packet

BOOL CUserChannel::CallRemote(LPCTSTR xRemoteId, LPCTSTR xLocalId)
{
	ASSERT( IsConnected() );
	CDialPacket* pPacket = new CDialPacket;
	
	if ( xLocalId && _tcslen(xLocalId) )
		pPacket->SetLocalId( xLocalId );
	else
		pPacket->SetLocalId( "057496031" );
	
	pPacket->SetRemoteId( xRemoteId );
	
//	SendPacket( pPacket );
	CopyMemory( m_xCallType, CA_HOOK, MAX_CALLTYPE );
	
	m_bNavigated = TRUE;
	m_tNavigated = GetTickCount();
	
	return TRUE;
}

BOOL CUserChannel::PickupRemote()
{
	ASSERT( IsConnected() );
	ASSERT( IsCallType( CA_TALK ) );
	
//	SendPacket( new CTalkPacket );
	CopyMemory( m_xCallType, CA_TALK, MAX_CALLTYPE );
	
	m_bNavigated = TRUE;
	m_tNavigated = GetTickCount();
	
	return TRUE;
}

void CUserChannel::HangRemote()
{
//	if ( ! CChannel::IsNavigated() ) return;
//	SendPacket( new CByePacket );
	
	m_bNavigated = FALSE;
	m_tNavigated = GetTickCount();
}

void CUserChannel::PutHangup()
{
	CXMLElement* pSync = new CXMLElement( NULL, "SyncOrderRelation" );
	
	CXMLElement* pSub = pSync->AddElement( "MsgType" );
	pSub->SetValue( "BYE" );
	
//	SendPacket( new CSyncOrderRelation( pSync ) ); delete pSync;
	
	m_bNavigated = FALSE;
	m_tNavigated = GetTickCount();
}

//////////////////////////////////////////////////////////////////////
// CUserChannel socket event handlers

void CUserChannel::OnHandshake()
{
	CXMLElement* pXML = new CXMLElement( NULL, ISyncOrderRelation );
	pXML->AddAttribute( "method", "home" );
	
	SendPacket( new CSyncOrderRelation(pXML) );
	delete pXML; pXML = NULL;
	
	CChannel::OnHandshake();
}
/*
void CUserChannel::JoinMirror(UINT ssrc, BOOL bLeave)
{
	CXMLElement* pXML = new CXMLElement( NULL, ISyncOrderRelation );
	pXML->AddAttribute( "method", "mirror" );
	
	if ( bLeave ) pXML->AddAttribute( "leave" );
	
	CXMLElement* pSync = pXML->AddElement( "mirror" );
	
	CString strValue;
	strValue.Format( "%u", ssrc );
	
	pSync->AddAttribute( "SSRC", strValue );
	
	SendPacket( new CSyncOrderRelation(pXML) );
	delete pXML; pXML = NULL;
}

void CUserChannel::JoinMirror(LPCTSTR pszRender, UINT ssrc, BOOL bJoined)
{
	CXMLElement* pXML = new CXMLElement( NULL, ISyncOrderRelation );
	pXML->AddAttribute( "method", "mirror" );
	
	if ( ! bJoined ) pXML->AddAttribute( "leave" );
	
	if ( CXMLElement* pSync = pXML->AddElement( pszRender ) )
	{
		CString strValue;
		strValue.Format( "%u", ssrc );
		
		pSync->AddAttribute( "SSRC", strValue );
	}
	
	SendPacket( new CSyncOrderRelation(pXML) );
	delete pXML; pXML = NULL;
}

void CUserChannel::JoinMonitor(BOOL bLeave)
{
	CXMLElement* pXML = new CXMLElement( NULL, ISyncOrderRelation );
	pXML->AddAttribute( "method", "monitor" );
	
	if ( bLeave ) pXML->AddAttribute( "leave" );
	
	Network.SendPacket( new CSyncOrderRelation(pXML) );
	delete pXML;
}
*/
//////////////////////////////////////////////////////////////////////
// CUserChannel TCP Transfer func

BOOL CUserChannel::OnPacket(CPacket* pPacket)
{
	if ( pPacket->IsCommandId(IBase_SyncOrderRelation) )
	{
		return OnSyncOrderRelation( (CSyncOrderRelation*)pPacket );
	}
	if ( pPacket->IsCommandId(IBase_SyncDataView) )
	{
		return OnSyncDataView( ((CPacketBase*)pPacket)->m_pXML );
	}
	
	return CChannel::OnPacket(pPacket);
}

BOOL CUserChannel::OnSyncOrderRelation(CSyncOrderRelation* pPacket)
{
/*	CString strMethod = pPacket->GetMethod();
	
	if ( ! strMethod.CompareNoCase("mirror") )
	{
		return OnSyncMirrorView( pPacket->m_pXML );
	}
	else if ( ! strMethod.CompareNoCase("monitor") )
	{
		return OnSyncMonitorView( pPacket->m_pXML );
	}
*/	
	return SendMessageToOwner( pPacket->m_pXML );
}

BOOL CUserChannel::OnSyncDataView(CXMLElement* pXML)
{
	return SendMessageToOwner( pXML );
}
/*
BOOL CUserChannel::OnSyncMirrorView(CXMLElement* pXML)
{
	CSingleLock pLock( &Traffic.m_pSection, TRUE );
	
	for ( POSITION pos = pXML->GetElementIterator() ; pos ; )
	{
		CXMLElement* pSync = pXML->GetNextElement( pos );
		
		CString strValue = pSync->GetName();
		if ( RTPSessionLayer* pChannel = Traffic.FindChannel(strValue) )
		{
			CString strSSRC = pSync->GetAttributeValue( "SSRC" );
			
			if ( pXML->GetAttribute("leave") ) pChannel->LeavedMirror( "", 0 );
			else pChannel->JoinedMirror( "", _ttoi(strSSRC) );
		}
	}
	
	return TRUE;
}
*/
BOOL CUserChannel::OnSyncMonitorView(CXMLElement* pXML)
{
	CWaitCursor pCursor;
	CSingleLock pLock( &Traffic.m_pSection, TRUE );
	
	for ( POSITION pos = pXML->GetElementIterator() ; pos ; )
	{
		CXMLElement* pSync = pXML->GetNextElement( pos );
		
		CString strValue = pSync->GetName();
		if ( RTPSessionLayer* pChannel = Traffic.FindChannel(strValue) )
		{
			pChannel->JoinMirror( m_xUserId, pSync, ! pXML->GetAttribute("leave") );
		}
	}
	
	return TRUE;
}
