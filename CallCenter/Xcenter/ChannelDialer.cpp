//
// ChannelDialer.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Network.h"
#include "ChannelDialer.h"

#include "CtrlNetworkChild.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CDialerChannel construction

CDialerChannel::CDialerChannel(int nChan, LPCTSTR xFeeCode) : CChannel( nChan, xFeeCode )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	bkOffHk.initiate		= (INITIATE)bkOffHk_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	WaitDial.process		= (PROCESS)WaitDial_cmplt;
	RcvTalk.initiate		= (INITIATE)RcvTalk_init;
	RcvTalk.process			= (PROCESS)RcvTalk_cmplt;
	
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

//////////////////////////////////////////////////////////////////////
// CDialerChannel OVERLOAD function

int CDialerChannel::bkIdle_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	CChannel* pChannel = new CChannel( nChan );
	pChannel->Clone( this ); Network.SetChannel( pChannel );

	return (pChannel->*(pChannel->fcnp)->initiate)(evtblk);
}

int CDialerChannel::bkOffHk_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	SsmPutUserEvent( E_CHG_ChState, m_nTaskId,
		MAKEVENT(nChan & 0xFFFF, S_CALL_PENDING) );

	return CChannel::bkOffHk_init(evtblk);
}

struct state* CDialerChannel::FeeLogin_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_FEE_Login &&
		(evtblk->dwParam & 0xFFFF) == S_FEE_Connected )
	{
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
		if ( PlayEnterVox( RemoteId, m_nTaskId ) )
		{
			SsmSetPlayDest( m_nTaskId, 0 );
			SsmStopTalkWith( nChan, m_nTaskId );
		}
	}
	
	if ( evtblk->wEventCode == E_CHG_ChState &&
		(evtblk->dwParam & 0xFFFF) == S_CALL_TALKING )
	{
		FreeBuffer();
		SsmStopPlayFile( m_nTaskId );
		SsmTalkWith( nChan, m_nTaskId );
		
		SsmPutUserEvent( E_CHG_ChState, m_nTaskId,
				MAKEVENT(nChan & 0xFFFF, S_CALL_TALKING) );
	}

	return CChannel::WaitDial_cmplt(evtblk);
}

int CDialerChannel::RcvTalk_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;

	Reserved |= 0x08;	// 置状态“正在”
	m_pOwner->SetItem( nChan, "正在对话", DS_PLAY );
	
	return CChannel::RcvTalk_init(evtblk);
}

struct state* CDialerChannel::RcvTalk_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_PENDING )
		{
			SsmPutUserEvent( E_CHG_ChState, m_nTaskId,
					MAKEVENT(nChan & 0xFFFF, S_CALL_PENDING) );
			KillLinker(); return &bkOffHk;
		}
	}
	
	return CChannel::RcvTalk_cmplt(evtblk);
}
