//
// ChannelTalk.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Network.h"
#include "Settings.h"
#include "ChannelTalk.h"

#include "Registry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CTalkChannel construction

CTalkChannel::CTalkChannel(int nChan, LPCTSTR xFeeCode) : CChannel( nChan, xFeeCode )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	FeeLogin.initiate		= (INITIATE)FeeLogin_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	rcvMFsig.initiate		= (INITIATE)rcvMFsig_init;
	RcvTalk.initiate		= (INITIATE)RcvTalk_init;
	RcvTalk.process			= (PROCESS)RcvUlTalk_cmplt;
	
	FwdPlay.initiate		= (INITIATE)FwdPlay_init;
	FwdPlay.process			= (PROCESS)FwdPlay_cmplt;
	RcvLvTel.initiate		= (INITIATE)RcvLvTel_init;
	RcvLvTel.process		= (PROCESS)RcvLvTel_cmplt;
	
	PageNumber = UserNumber = NULL;
	CTalkChannel::Setup();
}

CTalkChannel::~CTalkChannel()
{
	if ( PageNumber ) delete [] PageNumber;
	if ( UserNumber ) delete [] UserNumber;
}

//////////////////////////////////////////////////////////////////////
// CTalkChannel attributes

void CTalkChannel::Setup()
{
	CChannel::Setup();
	fcnp = &FeeLogin;
}

void CTalkChannel::KillLinker()
{
	if ( m_nTaskId != -1 )
	SsmPutUserEvent( E_CHG_ChState, m_nTaskId,
		MAKEVENT(m_nChan & 0xFFFF, S_CALL_PENDING) );
	
	CChannel::KillLinker();
}

struct state* CTalkChannel::ShExitCode_cmplt(EVTBLK *evtblk, int nExitCode)
{
	ExitCode = nExitCode;
	Reserved = 0; return &ExitPlay;
}

BOOL CTalkChannel::ReadUserShortMobile(LPCTSTR xRemoted, LPTSTR xPageNumber)
{
	CString strQuery;
	
	strQuery.Format( "select * from xuser"
					" where xjsjbh in (select xjsjbh from XusrCommList"
						" where xshort_number = '%s')", xRemoted );
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr xRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	if ( xRecordset->adoEOF ) return FALSE;
	
	_variant_t TheValue = xRecordset->GetCollect( "xphone" );
	int nLength = _tcslen( (LPCTSTR)(_bstr_t)TheValue );
	
	CopyMemory( xPageNumber, (LPCTSTR)(_bstr_t)TheValue, min(MAX_PHONE, nLength) );
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CTalkChannel operations

CChannel* CTalkChannel::ConvertFrom(CChannel* pParent, LPCTSTR xString)
{
	CopyMemory(pParent->LocalId, "96031789", MAX_PHONE);
	CopyMemory(pParent->CallType, CA_TALK, MAX_CALLTYPE );
	
	pParent->OnConnected(); return NULL;
}

//////////////////////////////////////////////////////////////////////
// CTalkChannel OVERLOAD function

int CTalkChannel::bkIdle_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	CChannel* pChannel = new CChannel( nChan );
	pChannel->Clone( this ); Network.SetChannel( pChannel );

	return (pChannel->*(pChannel->fcnp)->initiate)(evtblk);
}

struct state* CTalkChannel::FeeLogin_cmplt(EVTBLK *evtblk)
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

int CTalkChannel::rcvMFsig_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( SsmGetChState(nChan) == S_CALL_TALKING ||	// 其他业务转入
		_tcsncmp(CallType, CA_DEBG, 4) == 0 )		// For Debug
	{
		return SsmPutUserEvent( E_CHG_ChState, nChan, S_CALL_TALKING );
	}
	
	return CChannel::rcvMFsig_init(evtblk);
}

int CTalkChannel::RcvTalk_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	CChannel::RcvTalk_init(evtblk);
	
	if ( SsmGetChState(nChan) != S_CALL_TALKING )
		SsmSetPlayDest( nChan, 1 );	// 上总线，用于内线测试
	
	fcnp = &FwdPlay; Reserved = 0;
	return (this->*fcnp->initiate)(evtblk);
}

/////////////////////////////////////////////////////////////////////////////
// CTalkChannel Play function

// 语音接入
int CTalkChannel::FwdPlay_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	if ( PlayFile( FILE_SECY_WELCOME, FT_PROMPT, TRUE ) )
		return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	return 0;
}

struct state* CTalkChannel::FwdPlay_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved = 0; return &RcvLvTel;
	}
	
	return ChkDTSignal( evtblk, &FwdPlay );
}

// 输入被叫
int CTalkChannel::RcvLvTel_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;

	Reserved |= 0x08;
	SsmClearRxDtmfBuf( nChan );
	
	if ( PageNumber ) delete PageNumber;
	PageNumber = new CHAR [MAX_PHONE + 1];
	ZeroMemory( PageNumber, MAX_PHONE + 1 );
	
	return PlayFile( FILE_TALK_RCVTELE, FT_PROMPT, TRUE );
}

struct state* CTalkChannel::RcvLvTel_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x04;	// 记录播放完毕标志
	}
	
	if ( evtblk->wEventCode == E_CHG_RcvDTMF )
	{
		int nDtmfLen = SsmGetRxDtmfLen( nChan );
		
		if ( '*' == (CHAR)(evtblk->dwParam & 0xFFFF) )
		{
			Reserved = 0; return &RcvLvTel;
		}
		
		if ( '#' == (CHAR)(evtblk->dwParam & 0xFFFF) || nDtmfLen >= MAX_PHONE )
		{
			if ( ! ReadDtmf( PageNumber, '#' ) ) SsmPutUserEvent( E_CHG_RcvDTMF, nChan, '*' );
		}
	}
	
	if ( (Reserved & 0x04) && _tcslen( PageNumber ) )
	{
		BOOL bShort = ReadUserShortMobile( PageNumber, PageNumber );
		Reserved &= 0x00; return RcvUlWord_cmplt( evtblk, bShort ? PageNumber : RemoteId, bShort ? "96031789" : "96031666" );
	}
	
	return ChkDTSignal( evtblk, &RcvLvTel );
}

// 愉快交流
int CTalkChannel::RcvUlWord_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	CChannel::RcvTalk_init(evtblk);
	Reserved |= 0x08; return 0;
}

struct state* CTalkChannel::RcvUlWord_cmplt(EVTBLK *evtblk, LPCTSTR xFeeNumber, LPCTSTR xLocalId)
{
	CSettings::Item* pItem = Settings.GetSetting( _T("Trunk.TrunkFind") );
	CChannel* pChannel = Network.LookOutPcm( *pItem->m_pString );
	if ( pChannel == NULL ) return ShExitCode_cmplt( evtblk, EXIT_BUSY );
	
	CDialPacket* pPacket = new CDialPacket; pPacket->SetLocalId( xLocalId );
	pPacket->SetRemoteId( PageNumber ); pPacket->SetCallerId( LocalId );
	pPacket->SetFeeNumber( xFeeNumber );
	
	pChannel->OnDialPacket( pPacket ); pPacket->Release();
	SetLinker( pChannel->m_nChan );
	SsmSetPlayDest( pChannel->m_nChan, 1 );	// 上总线，允许运行于欠费提示
	
	RcvTalk.initiate = (INITIATE)RcvUlWord_init;
	Reserved &= 0x00; return &RcvTalk;
}

struct state* CTalkChannel::RcvUlTalk_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_PENDING && m_nTaskId == -1 )
		{
			SsmPutUserEvent( E_CHG_ChState, nChan, S_CALL_TALKING );
			Reserved = 0; return &RcvLvTel;
		}
	}
	
	CHAR nDtmfKey; SsmGetLastDtmf( nChan, &nDtmfKey );
	
	if ( nDtmfKey == '*' || m_nTaskId == -1 )
	{
		KillLinker(); Reserved = 0; return &RcvLvTel;
	}
	
	return CChannel::RcvTalk_cmplt(evtblk);
}
