//
// ChannelDialer.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Network.h"
#include "ChannelDialer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define _DELIVRD_	"DELIVRD"
#define _REJECTD_	"REJECTD"
#define _UNKNOWN_	"UNKNOWN"
#define _UNDELIV_	"UNDELIV"

//////////////////////////////////////////////////////////////////////
// CDialerChannel construction

CDialerChannel::CDialerChannel(int nChan, LPCTSTR xFeeCode) : CChannel( nChan, xFeeCode )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	WaitDial.process		= (PROCESS)WaitDial_cmplt;
	RcvTalk.initiate		= (INITIATE)RcvTalk_init;
	
	BufferPtr = NULL; BufferSize = 0;
	CDialerChannel::Setup();
}

CDialerChannel::~CDialerChannel()
{
	FreeBuffer();
}

//////////////////////////////////////////////////////////////////////
// CDialerChannel attributes

void CDialerChannel::Setup()
{
	CChannel::Setup();
	fcnp = &FeeLogin;
	
	ZeroMemory( BufferCpg, 512 );
	m_bPending	= FALSE;
	CpgLength	= 0;
}

HMODULE CDialerChannel::Release()
{
	if ( m_bPending	) UpdateState( _UNDELIV_ );
	
	return CChannel::Release();
}

void CDialerChannel::KillLinker()
{
	if ( m_nTaskId != -1 )
	SsmPutUserEvent( E_CHG_ChState, m_nTaskId,
		MAKEVENT(m_nChan & 0xFFFF, S_CALL_PENDING) );
	
	CChannel::KillLinker();
}

LPVOID CDialerChannel::NewBuffer(DWORD nBufSize)
{
	FreeBuffer();
	
	BufferPtr  = new BYTE[nBufSize];
	BufferSize = nBufSize;
	
	return BufferPtr;
}

void CDialerChannel::SetBuffer(LPVOID pBuf, DWORD nBufSize)
{
	NewBuffer( nBufSize );
	
	CopyMemory( BufferPtr, pBuf, nBufSize );
}

void CDialerChannel::FreeBuffer()
{
	if ( BufferPtr ) delete [] BufferPtr;
	BufferPtr = NULL; BufferSize = 0;
}

BOOL CDialerChannel::PlayEnterVox(LPCTSTR xPhone, int nChan)
{
	CString strQuery;
	
	strQuery.Format( "select a.*"
					" from xphonenum a inner join"
					" xphonenum_info b on a.enterprise_bh = b.enterprise_bh"
					" where department_phone = '%s'", xPhone );
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr xRecordset;
	xRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	if ( xRecordset->adoEOF ) return FALSE;
	
	_variant_t TheValue = xRecordset->GetCollect( "xdate_finish" );
	if ( VT_NULL == TheValue.vt ) return FALSE;
	
	COleDateTime tt( TheValue );
	tt.SetDateTime( tt.GetYear(), tt.GetMonth(), tt.GetDay(), 23, 59, 59 );
	if ( COleDateTime::GetCurrentTime() > tt ) return FALSE;
	
	TheValue = xRecordset->GetCollect( "xfalse" );
	if ( VT_NULL == TheValue.vt ) return FALSE;
	if ( _tcscmp((LPCTSTR)(_bstr_t)TheValue, "1") )return FALSE;
	
	FieldPtr xPlyBlob = xRecordset->GetFields()->GetItem("xentervoice");
	return PlayBlob( nChan, xPlyBlob, FALSE ) == TRUE;
}

int CDialerChannel::PlayBlob(int nChan, FieldPtr xPlyBlob, BOOL bTermDtmf)
{
	SsmSetDtmfStopPlay( nChan, bTermDtmf );
	SsmClearRxDtmfBuf( nChan );
	
	LONG lDataSize = xPlyBlob->ActualSize;
	if ( lDataSize <= 0 ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	_variant_t TheValue = xPlyBlob->GetChunk(lDataSize);
	LPTSTR *pBuf = NULL;
	SafeArrayAccessData(TheValue.parray, (void **)&pBuf);
	this->SetBuffer( pBuf, lDataSize );
	SafeArrayUnaccessData( TheValue.parray );
	
	if ( SsmPlayMem( nChan, 6, (LPBYTE)BufferPtr, BufferSize, 0, BufferSize - 1 ) )
	return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	return 0;
}

void CDialerChannel::UpdateState(LPCTSTR xState)
{
	CString strQuery;
	strQuery.Format( "insert into xhistory2000(xlocalid,xphonenum,xtimestart,xstate,xtrunk)"
					" values('%s','%s',getdate(),'%s',%i)", LocalId, RemoteId, xState, m_nFriendId );
	TRACE( strQuery + "\n" );
	
	xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
}

struct state* CDialerChannel::ChkRemoteCpg(EVTBLK* evtblk, struct state* lpstDefaultState)
{
	int nChan = evtblk->nReference;
//theApp.Message(MSG_ERROR,"debug[%i]...ChkRemoteCpg:=%i..",m_nChan, SsmIsHaveCpg(nChan));
	if ( SsmIsHaveCpg(nChan) != 1 ) return lpstDefaultState;
	
	CHAR xCpgInfo[512];
	int nCpgLength;
	
	if ( SsmGetCpg(nChan, xCpgInfo, &nCpgLength) ) return lpstDefaultState;
	if ( nCpgLength == CpgLength && ! memcmp(BufferCpg, xCpgInfo, nCpgLength) )  return lpstDefaultState;
	
	CopyMemory( BufferCpg, xCpgInfo, CpgLength = nCpgLength );
	if ( IsTestNumber( RemoteId ) )
theApp.Message( MSG_TEMP, "debug[%i]: CPG[%i]->%s", nChan, nCpgLength,
			   ToHex((LPBYTE)xCpgInfo,nCpgLength));
	
	return lpstDefaultState;
}

//////////////////////////////////////////////////////////////////////
// CDialerChannel OVERLOAD function

int CDialerChannel::bkIdle_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	CChannel* pChannel = new CChannel( nChan );
	pChannel->Clone( this ); Network.SetChannel( pChannel );
	
	return (pChannel->*(pChannel->fcnp)->initiate)(evtblk);
}

struct state* CDialerChannel::FeeLogin_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_FEE_Login &&
		(evtblk->dwParam & 0xFFFF) == S_FEE_Connected )
	{
		m_bPending = TRUE; m_nFriendId= m_nTaskId;
		KillTimer(); Reserved &= 0; return &WaitDial;
	}
	
	return CChannel::FeeLogin_cmplt(evtblk);
}

struct state* CDialerChannel::WaitDial_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_AutoDial &&
		(evtblk->dwParam & 0xFFFF) == DIAL_ECHOTONE )
	{
#if 0
		if ( PlayEnterVox( RemoteId, m_nTaskId ) )
		{
			SsmSetPlayDest( m_nTaskId, 0 );
			SsmStopTalkWith( nChan, m_nTaskId );
		}
#endif
	}
/*	
	if ( evtblk->wEventCode == E_CHG_ChState &&
		(evtblk->dwParam & 0xFFFF) == S_CALL_TALKING )
	{
		FreeBuffer();
		SsmStopPlayFile( m_nTaskId );
		SsmTalkWith( nChan, m_nTaskId );
		
		SsmPutUserEvent( E_CHG_ChState, m_nTaskId,
				MAKEVENT(nChan & 0xFFFF, S_CALL_TALKING) );
	}
*/	
	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		SsmPutUserEvent( E_CHG_ChState, m_nTaskId, evtblk->dwParam );
		
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_TALKING )
		{
			FreeBuffer();
			SsmStopPlayFile( m_nTaskId );
			SsmTalkWith( nChan, m_nTaskId );
			
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
#if 0	
	if ( evtblk->wEventCode == E_SYS_TIMEOUT ||
		(evtblk->dwParam & 0xFFFF) == S_CALL_WAIT_REMOTE_PICKUP )
	//	SsmGetChState(nChan) == S_CALL_WAIT_REMOTE_PICKUP )
	{
		struct state* pNext = ChkRemoteCpg( evtblk, &WaitDial );
		if ( pNext != &WaitDial ) return pNext;
		
		SetTimer( 200 ); return &WaitDial;
	}
#endif
	
	return CChannel::WaitDial_cmplt(evtblk);
}

int CDialerChannel::RcvTalk_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	CChannel::RcvTalk_init(evtblk);
	Reserved |= 0x08; return 0;
}
