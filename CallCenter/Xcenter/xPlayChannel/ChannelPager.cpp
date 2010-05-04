//
// ChannelPager.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Network.h"
#include "ChannelPager.h"
#include "ChannelPlay.h"

#include "Registry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define _PENDING_	"PENDING"
#define _DELIVRD_	"DELIVRD"
#define _REJECTD_	"REJECTD"
#define _UNKNOWN_	"UNKNOWN"
#define _UNDELIV_	"UNDELIV"

//////////////////////////////////////////////////////////////////////
// CPagerChannel construction

CPagerChannel::CPagerChannel(int nChan) : CChannel( nChan, "SMV" )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	WaitDial.process		= (PROCESS)WaitDial_cmplt;
	RcvTalk.initiate		= (INITIATE)RcvTalk_init;
	
	PlyLvWord.initiate		= (INITIATE)PlyLvWord_init;
	PlyLvWord.process		= (PROCESS)PlyLvWord_cmplt;
	
	RcvFnOper.initiate		= (INITIATE)RcvFnOper_init;
	RcvFnOper.process		= (PROCESS)RcvFnOper_cmplt;
	
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
	
	SsmSetEvent(E_PROC_PlayFile, m_nChan, FALSE, NULL);
	SsmSetEvent(E_PROC_RecordFile, m_nChan, FALSE, NULL);
	SsmSetEvent(E_SYS_BargeIn, m_nChan, FALSE, NULL);
	SsmSetEvent(E_SYS_NoSound, m_nChan, FALSE, NULL);
}

HMODULE CPagerChannel::Release()
{
	if ( m_bPending	) UpdateState( _UNDELIV_ );
	
	return CChannel::Release();
}

struct state* CPagerChannel::FwdNotify(LPCTSTR xLocalId)
{
#if 1	
	// 移除局方0574
	if ( ! _tcsncmp( xLocalId, "0574", 4 ) ) xLocalId += 4;
#endif
	CopyMemory( LocalId, xLocalId, MAX_PHONE );
	CopyMemory( CallType, CA_TALK, MAX_CALLTYPE );
	FeeLogin.initiate	= (INITIATE)FeeRebind_init;
	
	return fcnp = &FeeLogin;
}

BOOL CPagerChannel::LoadFrom(_RecordsetPtr xRecordset)
{
	m_pRecordset = xRecordset->Clone( adLockOptimistic );
	m_pRecordset->Filter = "xjsjbh = " + (_bstr_t)xRecordset->GetCollect("xjsjbh");
	
	CDialPacket* pPacket = new CDialPacket; pPacket->SetLocalId( "SMV" );
	
	_variant_t TheValue = m_pRecordset->GetCollect( "xPhoneNum" );
	if ( VT_NULL == TheValue.vt ) pPacket->SetRemoteId( "" );
	else pPacket->SetRemoteId( (LPCTSTR)(_bstr_t)TheValue );
	
	TheValue = m_pRecordset->GetCollect( "xCaller" );
	if ( VT_NULL == TheValue.vt ) pPacket->SetFeeNumber( "" );
	else pPacket->SetFeeNumber( (LPCTSTR)(_bstr_t)TheValue );
	
	CHAR xCallerId[MAX_PHONE + 1]; ZeroMemory(xCallerId, MAX_PHONE + 1);
	pPacket->GetFeeNumber(xCallerId); pPacket->SetCallerId(xCallerId);
#if 1	// 重新指定计费号码
	TheValue = m_pRecordset->GetCollect( "xFeeNumber" );
	if ( VT_NULL != TheValue.vt ) pPacket->SetFeeNumber( (LPCTSTR)(_bstr_t)TheValue );
#endif
	
	OnDialPacket( pPacket ); pPacket->Release();
	
#if 1	
	TheValue = m_pRecordset->GetCollect( "xPCMName" );
	CString xUsrLvFile = (LPCTSTR)(_bstr_t)TheValue;
	xUsrLvFile.TrimLeft(); xUsrLvFile.TrimRight();
	
	CString xCallType = ReadPagerType( xUsrLvFile, CA_HOOK );
	if ( xCallType.CompareNoCase(CallType) ) CopyMemory( CallType, (LPCTSTR)(xCallType), MAX_CALLTYPE );
	//if ( IsMusic(pPager->UsrLvFile) ) CopyMemory( pPager->CallType, "BEST", MAX_CALLTYPE );
#endif
	
	UpdateState( _PENDING_ );
	m_bPending	= TRUE;
	return TRUE;
}

double CPagerChannel::CalcUserBlance(LPCTSTR xUsrNumber)
{
	CString	strQuery;
	
	strQuery.Format( "select * from xuser_info"
					" where xjsjbh ="
						" (select top 1 xjsjbh from xuser"
						" where xphone = '%s')", xUsrNumber );
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr Record = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	if ( Record->adoEOF ) return 0;
//	ASSERT( Record->adoEOF == FALSE );

	// Current Balance:= xpreBala + xBalance

	double nBalance = 0;
	_variant_t TheValue;

	TheValue = Record->GetCollect( "xpreBala" );
	if ( VT_NULL != TheValue.vt ) nBalance += (double)TheValue;
	TheValue = Record->GetCollect( "xBalance" );
	if ( VT_NULL != TheValue.vt ) nBalance += (double)TheValue;

	return nBalance;
}

//////////////////////////////////////////////////////////////////////
// CPagerChannel operations

CChannel* CPagerChannel::PagerFrom(CChannel* pParent, LPCTSTR, LPCTSTR, DWORD nReserved)
{
	CPagerChannel* pChannel = new CPagerChannel( pParent->m_nChan );
	pChannel->Clone( pParent ); Network.SetChannel( pChannel );
	
	pChannel->LoadFrom( (_RecordsetPtr&)nReserved );
	return pChannel;
}

BOOL CPagerChannel::UpdateState(LPCTSTR xState)
{
	try
	{
		m_pRecordset->PutCollect( "xTheState", _variant_t(xState) );
		
		COleDateTime tt = COleDateTime::GetCurrentTime();
		m_pRecordset->PutCollect( "xsendtime", _variant_t(tt) );
		
		m_pRecordset->Update();
		return TRUE;
	}
	catch(_com_error e) 
	{ 
		theApp.Message( MSG_ERROR, "error on CPagerChannel[%i]: %s", m_nChan, e.ErrorMessage() );
		return FALSE;
	}
}

void CPagerChannel::RecUsrBack(CHAR xKey)
{
	_variant_t TheValue = m_pRecordset->GetCollect( "xUsrBack" );
	CString	strValue = (TheValue.vt != VT_NULL) ? (LPCTSTR)(_bstr_t)TheValue : "";
	
	if ( strValue.GetLength() < 30 ) strValue += xKey;
	m_pRecordset->PutCollect( "xUsrBack", _variant_t(strValue) );
	
	m_pRecordset->Update();
}

CString CPagerChannel::ReadPagerType(LPCTSTR xPath, LPCTSTR xDefault) const
{
	CString xFile = xPath;
	if ( xFile.Find( ';' ) >= 0 )
		xFile = xFile.Left( xFile.Find( ';' ) );
	if ( xFile.ReverseFind( '\\' ) >= 0 )
		xFile = xFile.Left( xFile.ReverseFind( '\\' ) );
	if ( xFile.ReverseFind( '\\' ) >= 0 )
		xFile = xFile.Mid( xFile.ReverseFind( '\\' ) + 1 );
	
	CString strQuery;
	
	strQuery.Format( "select * from xmusictype"
					" where xtypename = '%s'", (LPCTSTR)xFile );
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr RecordPtr = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	if ( RecordPtr->adoEOF ) return xDefault;
	
	_variant_t TheValue = RecordPtr->GetCollect( "xfeetype" );
	if ( TheValue.vt == VT_NULL ) return xDefault;
	
	return (LPCTSTR)(_bstr_t)TheValue;
}

//////////////////////////////////////////////////////////////////////
// CPagerChannel TCP Transfer func

BOOL CPagerChannel::OnPacket(CPacket* pPacket)
{
	if ( pPacket->IsCommandId( ICore_BindLink ) )
	{
		if ( FeeLogin.initiate == (INITIATE)FeeRebind_init )
		return CChannel::OnBindPacket( (CBindPacket*)pPacket );
	}
	
	return CChannel::OnPacket( pPacket );
}

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
#if 1
		if ( _tcsnicmp(CallType, "FOC", 3) == 0 )
		{
			CChannel::RcvTalk_init( NULL );
		}
#endif
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

int CPagerChannel::FeeRebind_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	CChannel::OnConnected();
	SetTimer( 15 * 1000 );
	
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
		if ( SsmGetChState(nChan) == S_CALL_PICKUPED )	// For CallOut
		{
#if 1
			SsmSetIsupFlag( nChan, ISUP_CallerParam, 0x1003, NULL );
#endif
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
	CChannel::RcvTalk_init(evtblk);
	
	fcnp = &PlyLvWord; Reserved = 0;
	return (this->*fcnp->initiate)(evtblk);
}

/////////////////////////////////////////////////////////////////////////////
// CPagerChannel play a Lvw to user

int CPagerChannel::PlyLvWord_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	SsmSetDtmfStopPlay( nChan, FALSE );
	
	_variant_t TheValue = m_pRecordset->GetCollect( "xPCMName" );
	if ( VT_NULL == TheValue.vt ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	CString xUsrFile = (LPCTSTR)(_bstr_t)TheValue;
	xUsrFile.TrimLeft(); xUsrFile.TrimRight();
	
	int nPos = xUsrFile.Find( ';' );
	if ( nPos > 0 )
	{
		SsmClearFileList( nChan );
		
		// 设置5秒断点以播放插曲
		SsmAddToFileList( nChan, xUsrFile.Left(nPos).GetBuffer(0), 6, 0, 5 * 8000 );
		// 播放插曲
		SsmAddToFileList( nChan, xUsrFile.Right(xUsrFile.GetLength() - nPos -1).GetBuffer(0), 6, 0, 0xFFFFFFFF );
		// 恢复断点以播放主题曲
		SsmAddToFileList( nChan, xUsrFile.Left(nPos).GetBuffer(0), 6, 5 * 8000, 0xFFFFFFFF );
		
		if ( SsmPlayFileList( nChan ) ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
		return 0;
	}
	
	if ( PlayFile( (LPCTSTR)xUsrFile, NULL, FALSE ) ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	return 0;
}

struct state* CPagerChannel::PlyLvWord_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
#if 1
		VerifyPhoneNum( RemoteId );
#endif
#if 0
		CHAR nDtmfKey = '\0';
		Reserved = 0; KillTimer();
		
		SsmGet1stDtmf( nChan, &nDtmfKey );
		if ( nDtmfKey != '\0' ) return FwdNotify( FeeNumber );
#else
		if ( CalcUserBlance(RemoteId) > 0 )
		{
			Reserved = 0; return &RcvFnOper;
		}
#endif
		//ExitCode = EXIT_GOODBYE; return &ExitPlay;
		return &bkOffHk;
	}
#if 1
	if ( evtblk->wEventCode == E_CHG_RcvDTMF )
	{
		RecUsrBack( (CHAR)(evtblk->dwParam & 0xFFFF));
		theApp.Message(MSG_TEMP,"debug[%i]: RecUsrBack(%c)", m_nChan, (CHAR)(evtblk->dwParam & 0xFFFF));
	}
#endif
	
	return ChkDTSignal( evtblk, &PlyLvWord );
}

/////////////////////////////////////////////////////////////////////////////
// CPagerChannel Pager answer function

int CPagerChannel::RcvFnOper_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	if ( Reserved == 0 )
	{
		Reserved |= (0x08 | 0x01);
		SsmClearRxDtmfBuf(nChan);
		
		return PlayFile( FILE_PAGE_RCVOPER, FT_PROMPT, TRUE );
	}
	
	CHAR nDtmfKey = '\0';
	SsmGet1stDtmf( nChan, &nDtmfKey );
	
	if ( nDtmfKey == '1' )	// 回复
	{
		typedef CChannel* (* PCTOR) (CChannel*, LPCTSTR);
		PCTOR pCtor = (PCTOR)CPlayChannel::ConvertPager;
		
		CChannel* pChannel = pCtor( this, FeeNumber );
		pChannel->m_hDllClosing	= NULL;
		
		return (pChannel->*(pChannel->fcnp)->initiate)(evtblk);
	}
	else
	if ( nDtmfKey == '2' )	// 转发
	{
		typedef CChannel* (* PCTOR) (CChannel*, LPCTSTR);
		PCTOR pCtor = (PCTOR)CPlayChannel::ConvertForward;
		
		_variant_t TheValue = m_pRecordset->GetCollect( "xPCMName" );
		
		CChannel* pChannel = pCtor( this, (LPCTSTR)(_bstr_t)TheValue );
		pChannel->m_hDllClosing	= NULL;
		
		return (pChannel->*(pChannel->fcnp)->initiate)(evtblk);
	}
	
	fcnp = &bkOffHk; Reserved = 0;
	return (this->*fcnp->initiate)(evtblk);
}

struct state* CPagerChannel::RcvFnOper_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x04;	// 记录播放完毕标志
		SetTimer( 5000 );
	}
	
	if ( evtblk->wEventCode == E_SYS_TIMEOUT )
	{
		Reserved &= 0x03;	// 清除播放完毕标志
		return &bkOffHk;
	}
	
	if ( (Reserved & 0x04) && SsmGetRxDtmfLen( nChan ) )
	{
		KillTimer(); 
		Reserved &= 0x07; return &RcvFnOper;
	}
	
	return ChkDTSignal( evtblk, &RcvFnOper );
}
