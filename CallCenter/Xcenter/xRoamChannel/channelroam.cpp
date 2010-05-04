//
// ChannelRoam.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Network.h"
#include "Settings.h"
#include "ChannelRoam.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CRoamChannel construction

CRoamChannel::CRoamChannel(int nChan, LPCTSTR xFeeCode) : CChannel( nChan, xFeeCode )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	RcvTalk.initiate		= (INITIATE)RcvTalk_init;
	
	FwdPlay.initiate		= (INITIATE)FwdPlay_init;
	FwdPlay.process			= (PROCESS)FwdPlay_cmplt;
	
	CRoamChannel::Setup();
}

CRoamChannel::~CRoamChannel()
{
	if ( PageNumber ) delete [] PageNumber;
	if ( UserNumber ) delete [] UserNumber;
}

//////////////////////////////////////////////////////////////////////
// CRoamChannel attributes

void CRoamChannel::Setup()
{
	CChannel::Setup();
	fcnp = &FeeLogin;
}

void CRoamChannel::KillLinker()
{
	if ( m_nTaskId != -1 )
	SsmPutUserEvent( E_CHG_ChState, m_nTaskId,
		MAKEVENT(m_nChan & 0xFFFF, S_CALL_PENDING) );
	
	CChannel::KillLinker();
}

struct state* CRoamChannel::ShExitCode_cmplt(EVTBLK *evtblk, int nExitCode)
{
	ExitCode = nExitCode;
	Reserved = 0; return &ExitPlay;
}

BOOL CRoamChannel::ReadUserAgent(LPCTSTR xUserId, LPCTSTR xLocalId, CString* pxAgentId)
{
	CString strQuery;
	
	strQuery.Format( "select a.*, d.xphonenum from xuser_info a"
					" inner join xusefee b on a.xjsjbh = b.xjsjbh"
					" inner join Xfeetable c on b.xfeecode = c.xfeetableid"
#if 0
					" left join xhistory2000 d on a.xjsjbh = d.xjsjbh and d.xphonenum = '%s'"
#else
					" left join xhistory2000 d on a.xphone = d.xlocalid and d.xphonenum = '%s'"
#endif
					" where c.xphone = '%s' order by d.xtimestart, newid()", xUserId, xLocalId );
	TRACE( strQuery + "\n" );
theApp.Message(MSG_TEMP, strQuery );
	
	_RecordsetPtr xRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	if ( xRecordset->adoEOF ) return FALSE;
	
	xRecordset->Find( (_bstr_t)"xphonenum <> null", 0, adSearchForward, "" );
	if ( xRecordset->adoEOF ) xRecordset->MoveFirst();
	
	_variant_t TheValue = xRecordset->GetCollect( "xphone" );
	if ( pxAgentId ) *pxAgentId = (LPCTSTR)(_bstr_t)TheValue;
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CRoamChannel OVERLOAD function

int CRoamChannel::bkIdle_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	CChannel* pChannel = new CChannel( nChan );
	pChannel->Clone( this ); Network.SetChannel( pChannel );

	return (pChannel->*(pChannel->fcnp)->initiate)(evtblk);
}

struct state* CRoamChannel::FeeLogin_cmplt(EVTBLK *evtblk)
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
		if ( SsmGetChState(nChan) == S_CALL_PICKUPED )	// For CallOut
		{
			KillTimer(); Reserved &= 0; return &WaitDial;
		}
	}
	
	return CChannel::FeeLogin_cmplt(evtblk);
}

int CRoamChannel::RcvTalk_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	CChannel::RcvTalk_init(evtblk);
	
	fcnp = &FwdPlay; Reserved = 0;
	return (this->*fcnp->initiate)(evtblk);
}

/////////////////////////////////////////////////////////////////////////////
// CRoamChannel Play function

// 语音接入
int CRoamChannel::FwdPlay_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	CString strFile = FILE_TALK_WELCOME;
	strFile += '.'; strFile += m_xFeeCode;
	
	if ( PlayFile( strFile, FT_PROMPT, TRUE ) && PlayFile( FILE_TALK_WELCOME, FT_PROMPT, TRUE ) )
		return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	RcvUlWord_cmplt( evtblk );
	return 0;
}

struct state* CRoamChannel::FwdPlay_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		SsmTalkWith( m_nChan, m_nTaskId );
		
		Reserved = 0; return &RcvTalk;
			//RcvUlWord_cmplt( evtblk );
	}
	
	return ChkDTSignal( evtblk, &FwdPlay );
}

// 愉快交流
int CRoamChannel::RcvUlWord_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	CChannel::RcvTalk_init(evtblk);
	Reserved |= 0x08; return 0;
}

struct state* CRoamChannel::RcvUlWord_cmplt(EVTBLK *evtblk)
{
	CSettings::Item* pItem = Settings.GetSetting( _T("Trunk.TrunkFind") );
	CChannel* pChannel = Network.LookOutPcm( *pItem->m_pString );
	if ( pChannel == NULL ) return ShExitCode_cmplt( evtblk, EXIT_BUSY );
	
	CString xAgentId;
	if ( ! ReadUserAgent( RemoteId, LocalId, &xAgentId ) ) return ShExitCode_cmplt( evtblk, EXIT_BUSY );
	
	CDialPacket* pPacket = new CDialPacket; pPacket->SetLocalId( "057496031" );
	pPacket->SetRemoteId( xAgentId ); pPacket->SetFeeNumber( "057496031" );
	pPacket->SetCallerId( RemoteId );
	
	pChannel->OnDialPacket( pPacket ); pPacket->Release();
#if 0
	SetLinker( pChannel->m_nChan );
#else
	m_nTaskId = pChannel->m_nChan;
	pChannel->m_nTaskId = m_nChan;
#endif
	SsmSetPlayDest( pChannel->m_nChan, 1 );	// 上总线，允许运行于欠费提示
	
	RcvTalk.initiate = (INITIATE)RcvUlWord_init;
	Reserved &= 0x00; return &RcvTalk;
}
