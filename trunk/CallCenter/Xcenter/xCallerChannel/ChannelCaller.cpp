//
// ChannelCaller.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Network.h"
#include "Settings.h"
#include "ChannelCaller.h"
#include "Profiles.h"
#include "Registry.h"

#include "Neighbour.h"
#include "Circuitry.h"

#include "WndMain.h"
#include "WndUser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CCallerChannel construction

CCallerChannel::CCallerChannel(int nChan, LPCTSTR xFeeCode) : CChannel( nChan, xFeeCode )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	FeeLogin.initiate		= (INITIATE)FeeLogin_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	rcvMFsig.initiate		= (INITIATE)rcvMFsig_init;
	WaitDial.process		= (PROCESS)WaitDial_cmplt;
	RcvTalk.initiate		= (INITIATE)RcvTalk_init;
	RcvTalk.process			= (PROCESS)RcvTalk_cmplt;
	
	FwdCaller.initiate		= (INITIATE)FwdCaller_init;
	FwdCaller.process		= (PROCESS)FwdCaller_cmplt;
	FindUser.initiate		= (INITIATE)FindUser_init;
	FindUser.process		= (PROCESS)FindUser_cmplt;
	
	m_pUserWnd		= NULL;
	m_fnKiller		= NULL;
	
	CCallerChannel::Setup();
}

CCallerChannel::~CCallerChannel()
{
}

//////////////////////////////////////////////////////////////////////
// CCallerChannel attributes

void CCallerChannel::Setup()
{
	CChannel::Setup();
	fcnp = &FeeLogin;
	
	CMainWnd* pMainWnd	= (CMainWnd*)theApp.m_pMainWnd;
	ASSERT( pMainWnd->GetSafeHwnd() != NULL );

	m_pUserWnd			= (CUserWnd*)pMainWnd->GetWindow( RUNTIME_CLASS(CUserWnd) );
	ASSERT( m_pUserWnd->GetSafeHwnd() != NULL );
}

struct state* CCallerChannel::FwdNotify(LPCTSTR xLocalId, struct state* lpszDefaultState)
{
	CopyMemory( LocalId, xLocalId, MAX_PHONE );
	CopyMemory( CallType, CA_TALK, MAX_CALLTYPE );
	
	CChannel::OnConnected();
	return lpszDefaultState;
}

void CCallerChannel::KillLinker()
{
	if ( m_fnKiller ) {(this->*m_fnKiller)(); return;}
	
	if ( m_nTaskId == -1 ) return;
	
	SsmPutUserEvent( E_CHG_ChState, m_nTaskId,
		MAKEVENT(m_nChan & 0xFFFF, S_CALL_PENDING) );
	
	CChannel::KillLinker();
}

void CCallerChannel::SetLinker(int nChan)
{
	KillLinker();

	if ( SsmTalkWithEx(m_nChan, 1, nChan, 1) )
	{
		CHAR sError[1024];
		SsmGetLastErrMsg( sError );
		theApp.Message( MSG_ERROR, "SsmTalkWith:%i,%i ->%s", m_nChan,nChan,sError );
	}

	CChannel* pChannel = Network.FindChannel( nChan );
	if ( ! pChannel ) throw "Invalid channel";
	pChannel->m_nTaskId = m_nChan;
	m_nTaskId = nChan;

//#ifdef _DEBUG
	theApp.Message(MSG_DEBUG,"SetLinker: %i with %i", m_nChan, nChan);
//#endif
}

BOOL CCallerChannel::IsNotUserTime()
{
	CRegistry pRegistry;
	
	if ( ! pRegistry.GetInt( "Caller", "NoUser" ) ) return FALSE;
	
	CString strValue = pRegistry.GetString( "Caller", "UserTime" );
	
	CString str	= strValue.SpanExcluding( _T("-") );
	WORD nLTime = _ttoi( str );
	
	strValue	= strValue.Mid( str.GetLength() + 1 );
	WORD nHTime = _ttoi( strValue );
	
	CTime tNow = CTime::GetCurrentTime();
	WORD nNowTime = tNow.GetHour()* 100 + tNow.GetMinute();
	
	return nNowTime < nLTime || nNowTime > nHTime;
}
#if 0
int CCallerChannel::ReadUserId(LPCTSTR xPhone, CString& xUserId)
{
	CString strQuery;
	strQuery.Format( "select * from xenterprisephone"
					" where xphone = '%s'", xUserId );
	TRACE( strQuery + "\n" );
	theApp.Message( MSG_DEBUG, strQuery );
	
	_RecordsetPtr xRecordset;
	xRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	
	if ( xRecordset->adoEOF ) return -1;
	
	_variant_t TheValue = xRecordset->GetCollect( "xuserid" );
	xUserId = (LPCTSTR)(_bstr_t)TheValue;
	
	TheValue = xRecordset->GetCollect( "xshift" );
	if ( VT_NULL != TheValue.vt ) return _ttoi( (LPCTSTR)(_bstr_t)TheValue );
	
	return 0;
}
#else
BOOL CCallerChannel::ReadOperInfo(LPCTSTR xPhone, CString& xOperId, CString& xUserId)
{
	CString strQuery;
	
	strQuery.Format( "select * from xoperator"
					" where xcode = (select xcode from xuser_info where xphone = '%s')", xPhone );
//					" and xshift = '1' and len(xcallphone) > 0", xPhone );
	
	_RecordsetPtr xRecordtrs;
	xRecordtrs = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	
	if ( xRecordtrs->adoEOF ) return FALSE;
	
	_variant_t TheValue = xRecordtrs->GetCollect( "xcode" );
	if ( VT_NULL != TheValue.vt ) xOperId = (LPCTSTR)(_bstr_t)TheValue;
	
	TheValue = xRecordtrs->GetCollect( "xcallphone" );
	if ( VT_NULL != TheValue.vt ) xUserId = (LPCTSTR)(_bstr_t)TheValue;
	
	TheValue = xRecordtrs->GetCollect( "xshift" );
	if ( VT_NULL == TheValue.vt || ! _ttoi((LPCTSTR)(_bstr_t)TheValue) ) return FALSE;
	
	return TRUE;
}
#endif

CChannel* CCallerChannel::ReadOperUser(LPCTSTR xLocalId, LPCTSTR xOper)
{
	int nCount = 0; m_pUserWnd->IsWellAlived( &nCount );
	
	for ( CChannel* pChannel = m_pUserWnd->ReadItem( xLocalId ); pChannel && nCount > 0; nCount -- )
	{
		if ( _tcsicmp( xOper, pChannel->m_xFeeCode ) ) continue;
		
		if ( SsmGetChState( pChannel->m_nChan ) == S_CALL_STANDBY &&
			 pChannel->m_nTaskId == -1 )
		{
			SsmPutUserEvent( E_CHG_ChState, pChannel->m_nChan,
				MAKEVENT(m_nChan & 0xFFFF, S_CALL_RINGING) );
		}
		else
		{
			pChannel->SendPacket( new CRingPacket( RemoteId, LocalId, m_nChan ) );
//			pChannel->SendPacket( new CByePacket );
			SetTimer( 5 * 1000 );
		}
		
		return pChannel;
	}
	
	return NULL;
}

CChannel* CCallerChannel::ReadOperUser(LPCTSTR xLocalId)
{
	CChannel* pChannel = m_pUserWnd->ReadItem( xLocalId, S_CALL_STANDBY );
	if ( ! pChannel || pChannel->m_nTaskId != -1 ) return NULL;
	
	SsmPutUserEvent( E_CHG_ChState, pChannel->m_nChan,
		MAKEVENT(m_nChan & 0xFFFF, S_CALL_RINGING) );
	
	return pChannel;
}

struct state* CCallerChannel::CallRemoteUser(LPCTSTR xRemoteId, LPCTSTR xCallerId, struct state* lpszDefaultState)
{
	CSettings::Item* pItem = Settings.GetSetting( _T("Trunk.TrunkFind") );
	
	CChannel* pChannel = Network.LookOutPcm( *pItem->m_pString );
	if ( pChannel == NULL ) return lpszDefaultState;
	
	CDialPacket* pPacket = new CDialPacket;
	pPacket->SetLocalId( "057496031" );	pPacket->SetRemoteId( xRemoteId );
	pPacket->SetCallerId( xCallerId );
	
	pChannel->OnDialPacket(pPacket); pPacket->Release();
	SetLinker( pChannel->m_nChan );
	
	return lpszDefaultState;
}

BOOL CCallerChannel::PutUserRing(LPCTSTR xLocalId, LPCTSTR xRemoteId, int nChan)
{
	CChannel* pChannel = m_pUserWnd->ReadItem( xLocalId );
	if ( ! pChannel ) return FALSE;
	
	SsmPutUserEvent( E_CHG_ChState, pChannel->m_nChan,
		MAKEVENT(nChan & 0xFFFF, S_CALL_RINGING) );
	
	return TRUE;
}

BOOL CCallerChannel::PutNetworkRing(LPCTSTR xLocalId, LPCTSTR xRemoteId, int nChan)
{
	CString strQuery;
	
	strQuery.Format( "select * from xoperator"
					" where xentercode in (select xentercode from xenterprise"
					" where left('%s',len(xphone)) = xphone)", xLocalId );
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr xRecordset;
	xRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	
	for ( ; ! xRecordset->adoEOF; xRecordset->MoveNext() )
	{
		CSingleLock pLock( &Neighbour.m_pSection, TRUE );
		
		_variant_t TheValue = xRecordset->GetCollect( "xcode" );
		
		CCircuitry* pChannel = Neighbour.FindChannel( (LPCTSTR)(_bstr_t)TheValue );
		if ( ! pChannel || ! pChannel->IsConnected() ) continue;
		
		CPacket* pPacket = new CRingPacket(xLocalId, xRemoteId, nChan);
		pChannel->OnPacket( pPacket ); pPacket->Release();
		
		m_nTaskId = (DWORD)pChannel;
		m_fnKiller= KillNetwork;
		return TRUE;
	}
	
	return FALSE;
}

BOOL CCallerChannel::PutForwardRing(LPCTSTR xLocalId, LPCTSTR xRemoteId, int nChan)
{
	CXMLElement* pParent = Profiles[ "Caller" ];
	
	if ( CXMLElement* pXML = Profiles.LookupKey(pParent, xLocalId) )
	{
		if ( pXML->GetAttributeValue( "Enable" ) != "是" ) return FALSE;
		
		CChannel* pChannel;
		return PutForwardRing(pXML, pChannel);
	}
	return FALSE;
}

BOOL CCallerChannel::PutForwardRing(CXMLElement* pXML, CChannel*& pChannel)
{
	CString strValue = pXML->GetAttributeValue( "Forward" );
	
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		if ( pChannel = Network.LookOutPhs( (LPCTSTR)str ) )
		{
			CDialPacket* pPacket = new CDialPacket;
			pPacket->SetLocalId( "96031" ); pPacket->SetRemoteId( str );
			pPacket->SetCallerId( pXML->GetAttributeValue( "ID" ) );
			pPacket->SetFeeNumber( pXML->GetAttributeValue( "ID" ) );
			
			pChannel->OnDialPacket( pPacket ); pPacket->Release();
			SetLinker( pChannel->m_nChan );
		}
		break;
	}
	return TRUE;
}

void CCallerChannel::KillNetwork()
{
	ASSERT( m_fnKiller && m_nTaskId );
	
	CSingleLock pLock( &Neighbour.m_pSection, TRUE );
	
	CCircuitry* pChannel = (CCircuitry*)m_nTaskId;
	pChannel->m_nTaskId = -1 ; pChannel->Reset();
	
	m_fnKiller	= NULL;
	m_nTaskId	= -1;
}

//////////////////////////////////////////////////////////////////////
// CCallerChannel OVERLOAD function

int CCallerChannel::bkIdle_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	KillLinker();
	SsmSetPlayDest( nChan, 0 );	// 取消上总线
	
	CChannel* pChannel = new CChannel( nChan );
	pChannel->Clone( this ); Network.SetChannel( pChannel );
	
	return (pChannel->*(pChannel->fcnp)->initiate)(evtblk);
}

int CCallerChannel::FeeLogin_init(EVTBLK *evtblk)
{
	return 0;
}

struct state* CCallerChannel::FeeLogin_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_RINGING )
		{
			KillTimer(); Reserved &= 0; return &rcvMFsig;
		}
	}
	
	if ( evtblk->wEventCode == E_FEE_Login &&
		(evtblk->dwParam & 0xFFFF) == S_FEE_Connected )
	{
		if ( SsmGetChState(nChan) == S_CALL_TALKING ||	// 其他业务转入
			_tcsncmp(CallType, CA_DEBG, 4) == 0 )		// For Debug
		{
			SsmPutUserEvent( E_CHG_ChState, nChan, S_CALL_RINGING );
		}
		else
		if ( SsmGetChState(nChan) == S_CALL_PICKUPED )	// For CallOut
		{
			KillTimer(); Reserved &= 0; return &WaitDial;
		}
	}
	
	return CChannel::FeeLogin_cmplt(evtblk);
}

int CCallerChannel::rcvMFsig_init(EVTBLK *evtblk)
{
//	if ( CXMLElement* pXML = Profiles.FindAnalog( m_nChan, "0" ) )
	{
		SsmPickup( m_nChan ); RcvTalk_init( evtblk );
	}
	
	Reserved &= 0; fcnp = &FwdCaller;
	return (this->*fcnp->initiate)(evtblk);
}

struct state* CCallerChannel::WaitDial_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_TALKING )
		{
			RcvTalk_init( evtblk ); return &rcvMFsig;
		}
	}
	
	return CChannel::WaitDial_cmplt(evtblk);
}

/////////////////////////////////////////////////////////////////////////////
// CCallerChannel caller function

int CCallerChannel::FwdCaller_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
#if 1
	if ( PutForwardRing( LocalId, RemoteId, nChan ) )
	{
		SetTimer( 45 * 1000 );
		Reserved &= 0; fcnp = &FindUser;
		return (this->*fcnp->initiate)(evtblk);
	}
#endif
	
	if ( PlayFile( FILE_CALLER_WELCOME, FT_PROMPT, FALSE ) )
		return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	return 0;
}

struct state* CCallerChannel::FwdCaller_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{	
#if 1	// 非坐席服务时段
		if ( IsNotUserTime() || (_tcsncmp(RemoteId, "0574", 4) && _tcsncmp(RemoteId, "1", 1)) )
		{
			return FwdNotify( "96031111", &FeeLogin );
		}
#endif
		SsmPutUserEvent( E_SYS_TIMEOUT, m_nChan, 0 );
		Reserved &= 0; return &FindUser;
	}
	
	return ChkDTSignal( evtblk, &FwdCaller );
}

int CCallerChannel::FindUser_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	SsmSetPlayDest( nChan, 1 );	// 上总线，阻止被SsmTalkWith中断放音
	
	return PlayFile( FILE_CALLER_FINDUSR, FT_PROMPT, FALSE );
}

struct state* CCallerChannel::FindUser_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;

	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved &= 0; return &FindUser; // 继续等待
	}
	
	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		// 消息来自TalkWith
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_TALKING )
		{
			SsmPickup( nChan );
		//	m_nTaskId = -1;	// 跳过KillLinker
		//	SetLinker( evtblk->dwParam >> 16 );
			Reserved &= 0; return &RcvTalk;
		}
	}
	
	if ( evtblk->wEventCode == E_SYS_TIMEOUT )
	{
		if ( PutNetworkRing(LocalId, RemoteId, nChan) )
		{
			FindUser.process	= (PROCESS)FindVip_cmplt;
			SetTimer( 30 * 1000 );
		}
		else if ( PutForwardRing( LocalId, RemoteId, nChan ) )
		{
			SetTimer( 45 * 1000 );
		}
		else
		{
			SetTimer( 5 * 1000 );
		}
		//else return FindVip_cmplt(evtblk);
	}
	
	return ChkDTSignal( evtblk, &FindUser );
}

struct state* CCallerChannel::FindVip_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved &= 0; return &FindUser; // 继续等待
	}
	
	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		// 消息来自TalkWith
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_TALKING )
		{
			SsmPickup( nChan );
			Reserved &= 0; return &RcvTalk;
		}
	}
	
	if ( evtblk->wEventCode == E_SYS_TIMEOUT )
	{
		KillNetwork();
		
		FindUser.process	= (PROCESS)FindUser_cmplt;
		PutUserRing( LocalId, RemoteId, nChan );
		SetTimer( 5 * 1000 );
	}
	
	return ChkDTSignal( evtblk, &FindUser );
}

int CCallerChannel::RcvTalk_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	CChannel::RcvTalk_init(evtblk);
	
	Reserved |= 0x08;	// 置状态“正在”
	return SsmStopPlayFile( nChan );
}

struct state* CCallerChannel::RcvTalk_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_PENDING )
		{
			KillLinker(); return &bkOffHk;
		}
	}
	
	return CChannel::RcvTalk_cmplt(evtblk);
}
