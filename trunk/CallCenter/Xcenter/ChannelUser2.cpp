//
// ChannelUser2.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "ChannelUser2.h"
#include "Network.h"
#include "Settings.h"
#include "Profiles.h"

#include "WndMain.h"
#include "WndUser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CUserChannel2 construction

CUserChannel2::CUserChannel2(int nChan, LPCTSTR xUserId) : CUserChannel( nChan )
{
	bkOffHk.initiate		= (INITIATE)bkOffHk_init;
	rcvMFsig.initiate		= (INITIATE)rcvMFsig_init;
	rcvMFsig.process		= (PROCESS)rcvMFsig_cmplt;
	RcvTalk.process			= (PROCESS)RcvTalk_cmplt;
	
	GetUserId()				= _tcsdup( xUserId );
	m_pUserWnd				= NULL;
	m_nThirdId				= -1;
	
	CUserChannel2::Setup();
}

CUserChannel2::~CUserChannel2()
{
}

//////////////////////////////////////////////////////////////////////
// CUserChannel2 attributes

void CUserChannel2::Setup()
{
	CUserChannel::Setup();
	m_nThirdId	= -1;
	m_bDialing	= FALSE;
	
	CMainWnd* pMainWnd	= (CMainWnd*)theApp.m_pMainWnd;
	
	m_pUserWnd = (CUserWnd*)pMainWnd->GetWindow( RUNTIME_CLASS(CUserWnd) );
	ASSERT ( m_pUserWnd->GetSafeHwnd() != NULL );
}

void CUserChannel2::Disconnect()
{
	CTransfer::Disconnect();
	m_pUserWnd->RemoveItem( this );
	
	CUserChannel* pChannel = new CUserChannel( m_nChan );
	pChannel->Clone( this ); Network.SetChannel( pChannel );
}

void CUserChannel2::PrintChState(EVTBLK* evtblk)
{
	CChannel::PrintChState( evtblk );
	
	m_pUserWnd->SetItem( this, evtblk->dwParam & 0xFFFF );
}

//////////////////////////////////////////////////////////////////////
// CUserChannel2 socket event handlers

void CUserChannel2::OnHandshake()
{
	SendPacket( new COkPacket );
	
	CTransfer::OnConnected();
}

//////////////////////////////////////////////////////////////////////
// CUserChannel2 TCP Transfer func

BOOL CUserChannel2::OnPacket(CPacket* pPacket)
{
	if ( pPacket->IsCommandId( ICore_Deliver ) )
	{
		return OnDeliverPacket( (CDeliverPacket*)pPacket );
	}
	if ( pPacket->IsCommandId( ICore_Waiting ) )
	{
		return OnWaitPacket( (CWaitPacket*)pPacket );
	}
	if ( pPacket->IsCommandId( ICore_Ringing ) )
	{
		return OnRingPacket( (CRingPacket*)pPacket );
	}
#if 1
	if ( pPacket->IsCommandId( "CTC" ) )
	{
		return OnRingPacket( (CRingPacket*)pPacket );
	}
#endif
	
	return CUserChannel::OnPacket( pPacket );
}

BOOL CUserChannel2::OnBindPacket(CBindPacket* pPacket)
{
//	if ( IsConnected() ) return TRUE;
	
	CHAR xUserId[MAX_PHONE + 1];
	xUserId[ pPacket->GetUserId( xUserId ) ] = '\0';
	
	free( m_xFeeCode ); m_xFeeCode = NULL;
	GetUserId()			= _tcsdup( xUserId );
	
	ASSERT( _tcscmp(xUserId, GetUserId()) == 0 );
	m_pUserWnd->AddItem( this );
	
	OnHandshake(); return TRUE;
}

BOOL CUserChannel2::OnWaitPacket(CWaitPacket* pPacket)
{
	SsmStopSendTone( m_nChan ); SsmStopPlay( m_nThirdId );
	CUserChannel::SetLinker( m_nThirdId ); m_nThirdId = -1;
	
	CHAR xMsgType[MAX_BUFFER];
	xMsgType[ pPacket->GetMsgType(xMsgType) ] = '\0';
	
	if ( ! _tcscmp(xMsgType, "STOP") ) return TRUE;
	
	m_nThirdId = m_nTaskId; KillLinker();
	CChannel* pChannel = Network.FindChannel( m_nThirdId );
	
	if ( pChannel->PlayFile(FILE_USER_WAITING, FT_PROMPT, FALSE) )
	{
		CHAR sError[1024];
		SsmGetLastErrMsg( sError );
		theApp.Message( MSG_ERROR, "error on channel %d: %s", m_nThirdId, sError );
	}
	
	return TRUE;
}

BOOL CUserChannel2::OnDeliverPacket(CDeliverPacket* pPacket)
{
	CChannel* pChannel = Network.FindChannel( m_nTaskId );
	if ( ! pChannel || ! pChannel->IsConnected() ) throw "Invalid channel";
	
	CHAR xMsgType[MAX_PHONE + 1];
	xMsgType[ pPacket->GetMsgType(xMsgType) ] = '\0';
	
	LPTSTR xMsgContent; pPacket->GetContentPtr( xMsgContent );
	theApp.Message( MSG_TEMP, "USER[%i]: {%s}{%s}{%s}", m_nChan, ICore_Deliver, xMsgType, xMsgContent );
	
	CString xModule = _T("");
	Profiles.FindFinal( xMsgType, &xModule );
	
	this->ConvertFrom( xModule, pChannel, xMsgContent );
	
	KillLinker(); SendPacket( new COkPacket );
	SsmPutUserEvent( E_CHG_ChState, m_nChan, S_CALL_PENDING );
	
	return TRUE;
}

BOOL CUserChannel2::OnDialPacket(CDialPacket* pPacket)
{
	if ( SsmGetChState( m_nChan ) != S_CALL_PICKUPED ) return FALSE;
	m_bDialing	= TRUE;
	
	CUserChannel::OnDialPacket( pPacket );
	
	SendPacket( new CEchnPacket(m_nTaskId, SsmGetChState(m_nTaskId)) );
	return TRUE;
}

BOOL CUserChannel2::OnRingPacket(CRingPacket* pPacket)
{
	if ( SsmGetChState(m_nChan) != S_CALL_PICKUPED ) return FALSE;
	
	CChannel* pChannel = Network.FindChannel( m_nTaskId );
	if ( pChannel == NULL ) pChannel = this;
	
	SsmStopSendTone( m_nChan );
	SsmStopPlay( pPacket->GetLineId() );
	
	if ( SsmGetChType( pPacket->GetLineId() ) == CHN_USER )
		SsmPutUserEvent( E_CHG_ChState, pPacket->GetLineId(),
		MAKEVENT( pChannel->m_nChan & 0xFFFF, S_CALL_RINGING) );
	else
		pChannel->SetLinker( pPacket->GetLineId() );
	
	if ( m_nTaskId != -1 ) SsmPutUserEvent( E_CHG_ChState, m_nChan, S_CALL_PENDING );
	
	return TRUE;
}

CChannel* CUserChannel2::ConvertFrom(LPCTSTR xInstance, CChannel* pParent, LPCTSTR xPacket)
{
	HINSTANCE hInstance = LoadLibrary( xInstance );
	if ( hInstance == NULL ) return NULL;
	
	typedef CChannel* (* PCTOR) (CChannel*, LPCTSTR);
	PCTOR pCtor = (PCTOR) GetProcAddress( hInstance, _T("ConvertFrom") );
	
	if ( NULL != pCtor )
	{
		CChannel* pChannel = pCtor( pParent, xPacket );
		if ( ! pChannel ) FreeLibrary( hInstance );
		else pChannel->m_hInstance = hInstance;
		
		return pChannel;
	}
	
	FreeLibrary( hInstance );
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CUserChannel2 OVERLOAD function

int CUserChannel2::bkOffHk_init(EVTBLK *evtblk)
{
	SendPacket( new CByePacket );
	return CUserChannel::bkOffHk_init(evtblk);
}

int CUserChannel2::rcvMFsig_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	CUserChannel::rcvMFsig_init(evtblk);
	
	m_pUserWnd->SetItem( this, evtblk->dwParam & 0xFFFF );
	SendPacket( new CRingPacket( RemoteId, LocalId, m_nTaskId ) );
	
	return 0;
}

struct state* CUserChannel2::rcvMFsig_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_CHG_ChState &&
		(evtblk->dwParam & 0xFFFF) == S_CALL_PENDING )
	{
		SendPacket( new CByePacket );
	}
	
	return CUserChannel::rcvMFsig_cmplt(evtblk);
}

struct state* CUserChannel2::RcvTalk_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;

	if ( evtblk->wEventCode == E_CHG_ChState &&
		(evtblk->dwParam & 0xFFFF) == S_CALL_STANDBY )
	{
		SendPacket( new CByePacket );
	}
	if ( evtblk->wEventCode == E_CHG_ChState &&
		(evtblk->dwParam & 0xFFFF) == S_CALL_PENDING && m_bDialing )
	{
		m_bDialing = FALSE;
		SsmSendTone( nChan, 3 ); return &RcvTalk;
	}
	
	return CUserChannel::RcvTalk_cmplt(evtblk);
}
