//
// ChannelPager.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Network.h"
#include "ChannelPager.h"

#include "WndMain.h"
#include "WndNetwork.h"
#include "Registry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define _PENDING_	"P"
#define _DELIVRD_	"D"
#define _REJECTD_	"R"
#define _UNKNOWN_	"U"
#define _UNDELIV_	"0"

//////////////////////////////////////////////////////////////////////
// CPagerChannel construction

CPagerChannel::CPagerChannel(int nChan, LPCTSTR xFeeCode) : CChannel( nChan, xFeeCode )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	FeeLogin.initiate		= (INITIATE)FeeLogin_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	WaitDial.process		= (PROCESS)WaitDial_cmplt;
	RcvTalk.initiate		= (INITIATE)RcvTalk_init;
	
	PlyFxByte.initiate		= (INITIATE)PlyFxByte_init;
	PlyFxByte.process		= (PROCESS)PlyFxByte_cmplt;
	
	m_bPending	= FALSE;
	CPagerChannel::Setup();
}

CPagerChannel::~CPagerChannel()
{
}

//////////////////////////////////////////////////////////////////////
// CPagerChannel attributes

void CPagerChannel::Setup()
{
	CChannel::Setup();
	fcnp = &FeeLogin;
}

HMODULE CPagerChannel::Release()
{
	if ( m_bPending	) UpdateState( _UNDELIV_ );
	
	return CChannel::Release();
}

BOOL CPagerChannel::LoadFrom(DWORD nPagerId)
{
	CString strQuery;
	
	strQuery.Format( "select * from xfax"
					" where xbh = %i", nPagerId );
	TRACE( strQuery + "\n" );
	
	xOpenRecordset(m_pRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
	if ( m_pRecordset->adoEOF ) return FALSE;
	
	CDialPacket* pPacket = new CDialPacket;
	pPacket->SetLocalId( "057496031000" );
	
	_variant_t TheValue = m_pRecordset->GetCollect( "xPhoneNum" );
	if ( VT_NULL == TheValue.vt ) pPacket->SetRemoteId( "" );
	else pPacket->SetRemoteId( (LPCTSTR)(_bstr_t)TheValue );
	
	TheValue = m_pRecordset->GetCollect( "xfeenumber" );
	if ( VT_NULL == TheValue.vt ) pPacket->SetFeeNumber( "" );
	else pPacket->SetFeeNumber( (LPCTSTR)(_bstr_t)TheValue );
	
	//pPacket->SetCallerId( "057496031000" );
	
	OnDialPacket( pPacket ); pPacket->Release();
	
	UpdateState( _PENDING_ );
	m_bPending	= TRUE;
	return TRUE;
}

BOOL CPagerChannel::LoadFrom(_RecordsetPtr xRecordset)
{
	m_pRecordset = xRecordset->Clone( adLockOptimistic );
	m_pRecordset->Filter = "xbh = " + (_bstr_t)xRecordset->GetCollect("xbh");
	
	CDialPacket* pPacket = new CDialPacket;
	pPacket->SetLocalId( "057496031000" );
	
	_variant_t TheValue = m_pRecordset->GetCollect( "xPhoneNum" );
	if ( VT_NULL == TheValue.vt ) pPacket->SetRemoteId( "" );
	else pPacket->SetRemoteId( (LPCTSTR)(_bstr_t)TheValue );
	
	TheValue = m_pRecordset->GetCollect( "xfeenumber" );
	if ( VT_NULL == TheValue.vt ) pPacket->SetFeeNumber( "" );
	else pPacket->SetFeeNumber( (LPCTSTR)(_bstr_t)TheValue );
	
	//pPacket->SetCallerId( "057496031000" );
	
	OnDialPacket( pPacket ); pPacket->Release();
	
	UpdateState( _PENDING_ );
	m_bPending	= TRUE;
	return TRUE;
}

CChannel* CPagerChannel::LookOut(int nChType)
{
#ifdef NPICKUP
	CChannel* pChannelFax = new CChannel(10);
	Network.SetChannel( pChannelFax );
	return pChannelFax;
#endif
	CMainWnd* pMainWnd	= (CMainWnd*)theApp.m_pMainWnd;
	ASSERT( pMainWnd->GetSafeHwnd() != NULL );
	
	CNetworkWnd* pHomeWnd	= (CNetworkWnd*)pMainWnd->GetWindow( RUNTIME_CLASS(CNetworkWnd) );
	ASSERT( pHomeWnd->GetSafeHwnd() != NULL );
	
	CChannel* pChannel = pHomeWnd->FindChannel( nChType, S_CALL_STANDBY );
	if ( pChannel == NULL ) return NULL;
	
theApp.Message(MSG_ERROR,"debug[%i]: SetLinker to %i", m_nChan, pChannel->m_nChan);
	SetLinker( pChannel->m_nChan );
	return pChannel;
}

//////////////////////////////////////////////////////////////////////
// CPagerChannel operations

CChannel* CPagerChannel::ConvertFrom(CChannel* pParent, DWORD nTaskId)
{
	CPagerChannel* pChannel = new CPagerChannel( pParent->m_nChan );
	pChannel->CChannel::Clone( pParent ); pChannel->m_nPagerId = nTaskId;
	
	if ( ! pChannel->LookOut(CHN_SOFTFAX) || ! pChannel->LoadFrom(nTaskId) )
	{
		delete pChannel; return NULL;
	}
	
	Network.SetChannel( pChannel ); return pChannel;
}

CChannel* CPagerChannel::ConvertFrom(CChannel* pParent, _RecordsetPtr xRecordset)
{
	CPagerChannel* pChannel = new CPagerChannel( pParent->m_nChan );
	pChannel->CChannel::Clone( pParent );
	
	if ( ! pChannel->LookOut(CHN_SOFTFAX) || ! pChannel->LoadFrom(xRecordset) )
	{
		delete pChannel; return NULL;
	}
	
	Network.SetChannel( pChannel ); return pChannel;
}

BOOL CPagerChannel::UpdateState(LPCTSTR xState)
{
	try
	{
		m_pRecordset->PutCollect( "xstate", _variant_t(xState) );
		
		COleDateTime tt = COleDateTime::GetCurrentTime();
		m_pRecordset->PutCollect( "xsenddate", _variant_t(tt) );
		
		m_pRecordset->Update();
		return TRUE;
	}
	catch(_com_error e) 
	{ 
		theApp.Message( MSG_ERROR, "error on CPagerChannel[%i]: %s", m_nChan, e.ErrorMessage() );
		return FALSE;
	}
}

//////////////////////////////////////////////////////////////////////
// CPagerChannel TCP Transfer func

BOOL CPagerChannel::OnBindPacket(CBindPacket* pPacket)
{
	CTransfer::OnConnected();
	CString xFeeCode;
	
	int nLength = pPacket->GetFeeCode( xFeeCode.GetBuffer(4) );
	xFeeCode.ReleaseBuffer( nLength );
	
	CChannel* pChannel = this;
	if ( ! xFeeCode.CompareNoCase( this->m_xFeeCode ) )
	{
		if ( _tcslen(pChannel->FeeNumber) )
		theApp.Message( MSG_DEFAULT, "Rcv[%i][%s]: Local=[%s], Remote=[%s], Fee=[%s]",
			pChannel->m_nChan, pChannel->m_xFeeCode, pChannel->LocalId, pChannel->RemoteId, pChannel->FeeNumber );
		else
		theApp.Message( MSG_DEFAULT, "Rcv[%i][%s]: Local=[%s], Remote=[%s]",
			pChannel->m_nChan, pChannel->m_xFeeCode, pChannel->LocalId, pChannel->RemoteId );
		
		SsmPutUserEvent( E_FEE_Login, pChannel->m_nChan, S_FEE_Connected );
	}
	else
	{
		theApp.Message( MSG_ERROR, "N/A[%i][%s]: Local=[%s], Remote=[%s], Fee=[%s]",
			m_nChan, xFeeCode, LocalId, RemoteId, FeeNumber );
		
		SsmPutUserEvent( E_FEE_Login, m_nChan,
			MAKEVENT(ECore_Unknown & 0xFFFF, S_FEE_Disconnected) );
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CPagerChannel OVERLOAD function

int CPagerChannel::bkIdle_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	CChannel* pChannel = new CChannel( nChan );
	pChannel->Clone( this ); Network.SetChannel( pChannel );

	return (pChannel->*(pChannel->fcnp)->initiate)(evtblk);
}

int CPagerChannel::FeeLogin_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	CRegistry pRegistry;
	CString strHost = pRegistry.GetString( "Pager", "Master", "127.0.0.1" );
	int nPort = pRegistry.GetInt( "Pager", "Port", 9901 );
	
	ConnectTo( strHost, nPort );
	SetTimer( 60 * 1000 );
	
	return 0;
}

struct state* CPagerChannel::FeeLogin_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_TALKING )
		{
			KillTimer(); Reserved &= 0; return &RcvTalk;
		}
	}
	if ( evtblk->wEventCode == E_FEE_Login &&
		(evtblk->dwParam & 0xFFFF) == S_FEE_Connected )
	{
#ifdef NPICKUP
		{
			KillTimer(); Reserved &= 0; return &WaitDial;
		}
#endif
		if ( SsmGetChState(nChan) == S_CALL_PICKUPED )	// For CallOut
		{
			KillTimer(); Reserved &= 0; return &WaitDial;
		}
	}
	
	return CChannel::FeeLogin_cmplt(evtblk);
}

struct state* CPagerChannel::WaitDial_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_TALKING )
		{
			UpdateState( _DELIVRD_ ); m_bPending = FALSE;
		}
	}
	if ( evtblk->wEventCode == E_PROC_AutoDial )
	{
		switch (evtblk->dwParam & 0xFFFF)
		{
		//case	DIAL_DIALING:
		//	UpdateState( _PENDING_ ); break;
			
		//case	DIAL_VOICE:
		//	UpdateState( _DELIVRD_ ); break;
			
		case	DIAL_INVALID_PHONUM:
			UpdateState( _REJECTD_ ); m_bPending = FALSE;
			break;
			
		case	DIAL_NO_DIALTONE:
		case	DIAL_FAILURE:
			UpdateState( _UNKNOWN_ ); m_bPending = FALSE;
			break;
			
		case	DIAL_BUSYTONE:
		case	DIAL_NOANSWER:
			UpdateState( _UNDELIV_ ); m_bPending = FALSE;
			break;
		}
	}

	return CChannel::WaitDial_cmplt(evtblk);
}

int CPagerChannel::RcvTalk_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	CChannel::RcvTalk_init(evtblk);
	
	_variant_t TheValue = m_pRecordset->GetCollect( "xfeenumber" );
	if ( VT_NULL == TheValue.vt || ! _tcslen((LPCTSTR)(_bstr_t)TheValue) )
	fcnp = &bkOffHk;
	else
	fcnp = &PlyFxByte; Reserved = 0;
	return (this->*fcnp->initiate)(evtblk);
}

/////////////////////////////////////////////////////////////////////////////
// CPagerChannel play a fax to user

int CPagerChannel::PlyFxByte_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	return PlayFile( FILE_MAIL_PLYUFAX, FT_PROMPT, FALSE );
}

struct state* CPagerChannel::PlyFxByte_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		CChannel* pChannel = Network.FindChannel( m_nTaskId );
		if ( ! pChannel ) throw "Invalid channel";
		
		_variant_t TheValue = m_pRecordset->GetCollect( "xfaxpath" );
		pChannel->SendFax( (LPCTSTR)(_bstr_t)TheValue, NULL );
	}
	
	if ( evtblk->wEventCode == E_CHG_FaxPages && m_pRecordset != NULL )
	{
		m_pRecordset->PutCollect( "xstate", _variant_t("1") );
		
		COleDateTime tt = COleDateTime::GetCurrentTime();
		m_pRecordset->PutCollect( "xsenddate", _variant_t(tt) );
		
		m_pRecordset->Update();
		
		for ( m_pRecordset->MoveNext(); !m_pRecordset->adoEOF; m_pRecordset->MoveNext() )
		{
			CChannel* pChannel = Network.FindChannel( m_nTaskId );
			if ( ! pChannel ) throw "Invalid channel";
			
			_variant_t TheValue = m_pRecordset->GetCollect( "xfaxpath" );
			pChannel->SendFax( (LPCTSTR)(_bstr_t)TheValue, NULL, TRUE );
			
			m_pRecordset->PutCollect( "xstate", _variant_t("1") );
			
			COleDateTime tt = COleDateTime::GetCurrentTime();
			m_pRecordset->PutCollect( "xsenddate", _variant_t(tt) );
			
			m_pRecordset->Update();
		}
	}
	
	return ChkDTSignal( evtblk, &PlyFxByte );
}
