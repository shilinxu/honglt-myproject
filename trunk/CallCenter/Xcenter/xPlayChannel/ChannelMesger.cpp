//
// ChannelMesger.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Network.h"
#include "ChannelMesger.h"

#include "Settings.h"
#include "Registry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CMesgerChannel construction

CMesgerChannel::CMesgerChannel(int nChan, LPCTSTR xFeeCode) : CChannel( nChan, xFeeCode )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	FeeLogin.initiate		= (INITIATE)FeeLogin_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	rcvMFsig.initiate		= (INITIATE)rcvMFsig_init;
	RcvTalk.initiate		= (INITIATE)RcvTalk_init;
	
	FwdPlay.initiate		= (INITIATE)FwdPlay_init;
	FwdPlay.process			= (PROCESS)FwdPlay_cmplt;
	
	PlyLvFile.initiate		= (INITIATE)PlyLvFile_init;
	PlyLvFile.process		= (PROCESS)PlyLvFile_cmplt;
	PlyLvWord.initiate		= (INITIATE)PlyLvWord_init;
	PlyLvWord.process		= (PROCESS)PlyLvWord_cmplt;
	
	CMesgerChannel::Setup();
}

CMesgerChannel::~CMesgerChannel()
{
}

//////////////////////////////////////////////////////////////////////
// CMesgerChannel attributes

void CMesgerChannel::Setup()
{
	CChannel::Setup();
	fcnp = &FeeLogin;
}

BOOL CMesgerChannel::OpenMailbox(int nMaxTopic)
{
	CString strQuery;
	
	strQuery.Format( "select top %i * from xuservoice"
					" where xCaller = '%s' order by xjsjbh desc", nMaxTopic, RemoteId );
	TRACE( strQuery + "\n" );
	
	xOpenRecordset(m_pRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
	
	return ! m_pRecordset->adoEOF;
}

int CMesgerChannel::ShPlyNavFile(int nChan, LPCTSTR xNavFile)
{
	CRegistry pRegistry;
	if ( ! pRegistry.GetInt( "Pager", "NavPlay" ) ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	CString strValue = pRegistry.GetString( "Pager", "NavOffset" );
	if ( ! strValue.GetLength() ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	SsmSetDtmfStopPlay( nChan, FALSE );
	SsmClearRxDtmfBuf( nChan );
	SsmClearFileList( nChan );
	
	DWORD nOffset1 = 0, nOffset2 = 0;
	_stscanf( strValue, "%i-%i", &nOffset1, &nOffset2 );
	
	SsmAddToFileList( nChan, (LPTSTR)xNavFile, 6, 0, nOffset1 );
	
	strValue.Format( "%i", m_pRecordset->RecordCount );
	CompDigital( (LPCTSTR)strValue, TRUE );
	
	SsmAddToFileList( nChan, (LPTSTR)xNavFile, 6, nOffset2, 0xFFFFFFFF );
	
	if ( SsmPlayFileList( nChan ) ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	return 0;
}

int CMesgerChannel::ShRcvTalk_init(EVTBLK *evtblk, struct state* lpszDefaultState)
{
	CChannel::RcvTalk_init(evtblk);
	
	fcnp = lpszDefaultState; Reserved = 0;
	return (this->*fcnp->initiate)(evtblk);
}

int CMesgerChannel::ShExitPlay_init(EVTBLK *evtblk, int nExitCode)
{
	ExitCode = nExitCode; fcnp = &ExitPlay;
	Reserved = 0; return (this->*fcnp->initiate)(evtblk);
}

struct state* CMesgerChannel::ShExitPlay_cmplt(EVTBLK *evtblk, int nExitCode)
{
	ExitCode = nExitCode;
	Reserved = 0; return &ExitPlay;
}

void CMesgerChannel::RecUsrBack(CHAR cDtmfKey)
{
	_variant_t TheValue = m_pRecordset->GetCollect( "xUsrBack" );
	CString	strValue = (TheValue.vt != VT_NULL) ? (LPCTSTR)(_bstr_t)TheValue : "";
	
	if ( strValue.GetLength() < 30 ) strValue += cDtmfKey;
	m_pRecordset->PutCollect( "xUsrBack", _variant_t(strValue) );
	
	m_pRecordset->Update();
}

//////////////////////////////////////////////////////////////////////
// CMesgerChannel OVERLOAD function

int CMesgerChannel::bkIdle_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	CChannel* pChannel = new CChannel( nChan );
	pChannel->Clone( this ); Network.SetChannel( pChannel );
	
	return (pChannel->*(pChannel->fcnp)->initiate)(evtblk);
}

int CMesgerChannel::FeeLogin_init(EVTBLK *evtblk)
{
	return 0;
}

struct state* CMesgerChannel::FeeLogin_cmplt(EVTBLK *evtblk)
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

int CMesgerChannel::rcvMFsig_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( SsmGetChState(nChan) == S_CALL_TALKING ||	// 其他业务转入
		_tcsncmp(CallType, CA_DEBG, 4) == 0 )		// For Debug
	{
		return SsmPutUserEvent( E_CHG_ChState, nChan, S_CALL_TALKING );
	}
	
	return CChannel::rcvMFsig_init(evtblk);
}

int CMesgerChannel::RcvTalk_init(EVTBLK *evtblk)
{
	return ShRcvTalk_init( evtblk, &FwdPlay );
}

/////////////////////////////////////////////////////////////////////////////
// CMesgerChannel Pager function

int CMesgerChannel::FwdPlay_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	if ( PlayFile( FILE_MSGER_WELCOME, FT_PROMPT, TRUE ) )
		return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	return 0;
}

struct state* CMesgerChannel::FwdPlay_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved = 0; return &PlyLvFile;
	}
	
	return ChkDTSignal( evtblk, &FwdPlay );
}

/////////////////////////////////////////////////////////////////////////////
// CMesgerChannel read voice info

int CMesgerChannel::PlyLvFile_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	if ( ! OpenMailbox(10) ) return PlayFile( FILE_PROMPT_NORECD, FT_PROMPT, FALSE );
	
	CString strFile;
	CSettings::Item* pItem = Settings.GetSetting( _T(".Path") );
	strFile.Format( _T("%s\\%s\\%s"), *pItem->m_pString, FT_PROMPT, FILE_PAGE_PLYLVWRD );
	
	return ShPlyNavFile( nChan, (LPCTSTR)strFile );
}

struct state* CMesgerChannel::PlyLvFile_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved = 0; return &PlyLvWord;
	}
	
	return ChkDTSignal( evtblk, &PlyLvFile );
}

/////////////////////////////////////////////////////////////////////////////
// CMesgerChannel play lvwrd function

int CMesgerChannel::PlyLvWord_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	if ( m_pRecordset == NULL || m_pRecordset->adoEOF || m_pRecordset->BOF )
	{
		return ShExitPlay_init( evtblk, EXIT_GOODBYE );;
	}
	
	Reserved |= 0x08;	// 置状态“正在”
	SsmClearRxDtmfBuf(nChan);
	
	_variant_t TheValue = m_pRecordset->GetCollect( "xPCMName" );
	if ( VT_NULL == TheValue.vt ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	CString xUsrFile = (VT_NULL != TheValue.vt) ? (LPCTSTR)(_bstr_t)TheValue : "";
	xUsrFile.TrimLeft(); xUsrFile.TrimRight();
	
	if ( PlayFile( (LPCTSTR)xUsrFile, NULL, TRUE ) )
	{
		CHAR sError[1024];
		SsmGetLastErrMsg( sError );
		theApp.Message( MSG_ERROR, sError );
		
		if ( ! SsmStopPlayFile( nChan ) ) return 0;
		return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	}
	
	return 0;
}

struct state* CMesgerChannel::PlyLvWord_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		m_pRecordset->PutCollect( "xTheState", _variant_t("DELIVRD") );
		m_pRecordset->Update();
		Reserved |= 0x04;	// 记录播放完毕标志
	}
#if 1
	if ( evtblk->wEventCode == E_CHG_RcvDTMF )
	{
		RecUsrBack( (CHAR)(evtblk->dwParam & 0xFFFF));
	}
#endif
	
	if ( (Reserved & 0x04) == 0x4 )
	{
		CHAR nDtmfKey = '\0';
		SsmGetLastDtmf( nChan, &nDtmfKey );
		
		Reserved = 0; KillTimer();
		
		if ( nDtmfKey == '#' || ! nDtmfKey )
		{
			m_pRecordset->MoveNext();
		}
		else if ( nDtmfKey == '*' )	// 返回上层
		{
			xCloseRecordset(m_pRecordset);
		}
		else
		{
			m_pRecordset->MovePrevious();
		}
		
		return &PlyLvWord;
	}
	
	return ChkDTSignal( evtblk, &PlyLvWord );
}
