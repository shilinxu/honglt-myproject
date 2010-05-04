//
// ChannelClerk.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Network.h"
#include "ChannelClerk.h"

#include "WndMain.h"
#include "WndClerk.h"
#include "CtrlNetworkChild.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CClerkChannel construction

CClerkChannel::CClerkChannel(int nChan, LPCTSTR xFeeCode) : CChannel( nChan, xFeeCode )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	WaitDial.initiate		= (INITIATE)WaitDial_init;
	RcvTalk.initiate		= (INITIATE)RcvTalk_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	
	PlyLvWord.initiate		= (INITIATE)PlyLvWord_init;
	PlyLvWord.process		= (PROCESS)PlyLvWord_cmplt;
	
	LvWordPtr = NULL; LvWordSize = 0;
	m_pClerkWnd				= NULL;
	CClerkChannel::Setup();
}

CClerkChannel::~CClerkChannel()
{
	if ( LvWordPtr ) delete [] LvWordPtr;
}

//////////////////////////////////////////////////////////////////////
// CClerkChannel attributes

void CClerkChannel::Setup()
{
	CChannel::Setup();
	fcnp = &FeeLogin;
	
	CMainWnd* pMainWnd	= (CMainWnd*)theApp.m_pMainWnd;
	
	m_pClerkWnd = (CClerkWnd*)pMainWnd->GetWindow( RUNTIME_CLASS(CClerkWnd) );
	ASSERT ( m_pClerkWnd->GetSafeHwnd() != NULL );
}

void CClerkChannel::SetBuffer(LPVOID pBuf, DWORD nBufSize)
{
	if ( LvWordPtr ) delete [] LvWordPtr;
	
	LvWordPtr = new BYTE[nBufSize];
	CopyMemory( LvWordPtr, pBuf, nBufSize );
	LvWordSize = nBufSize;
}

//////////////////////////////////////////////////////////////////////
// CClerkChannel operations

CClerkChannel* CClerkChannel::ConvertFrom(CChannel* pParent)
{
	CClerkChannel* pChannel = new CClerkChannel( pParent->m_nChan );
	pChannel->CChannel::Clone( pParent ); Network.SetChannel( pChannel );
	
	return pChannel;
}

void CClerkChannel::Clone(CChannel* pParent)
{
	CChannel::Clone( pParent );
	if ( ! pParent->m_xFeeCode || _tcscmp(pParent->m_xFeeCode, m_xFeeCode) ) return;

	CClerkChannel* pChannel = (CClerkChannel*)pParent;
	SetBuffer( pChannel->LvWordPtr, pChannel->LvWordSize );
	m_xClerkId	= pChannel->m_xClerkId;
}

//////////////////////////////////////////////////////////////////////
// CClerkChannel OVERLOAD function

int CClerkChannel::bkIdle_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	CChannel* pChannel = new CChannel( nChan );
	pChannel->Clone( this ); Network.SetChannel( pChannel );

	return (pChannel->*(pChannel->fcnp)->initiate)(evtblk);
}

struct state* CClerkChannel::FeeLogin_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_FEE_Login &&
		(evtblk->dwParam & 0xFFFF) == S_FEE_Connected )
	{
		KillTimer(); Reserved &= 0; return &WaitDial;
	}
	
	if ( evtblk->wEventCode == E_FEE_Login &&
		(evtblk->dwParam & 0xFFFF) == S_FEE_Disconnected )
	{
		KillTimer(); Reserved &= 0; return &bkOffHk;
	}
	
	return CChannel::FeeLogin_cmplt(evtblk);
}

int CClerkChannel::WaitDial_init(EVTBLK *evtblk)
{
	if ( ! CChannel::WaitDial_init(evtblk) ) return 0;
	
	int nChan = evtblk->nReference;
	CHAR sError[1024]; SsmGetLastErrMsg( sError );
	theApp.Message( MSG_ERROR, "error on channel %d: %s", nChan, sError );
	
	fcnp = &bkOffHk; Reserved &= 0;
	
	return (this->*fcnp->initiate)(evtblk);
}

int CClerkChannel::RcvTalk_init(EVTBLK *evtblk)
{
	CChannel::RcvTalk_init(evtblk);
	
	fcnp = &PlyLvWord; Reserved = 0;
	return (this->*fcnp->initiate)(evtblk);
}

/////////////////////////////////////////////////////////////////////////////
// CClerkChannel clerk function

int CClerkChannel::PlyLvWord_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	m_pOwner->SetItem( nChan, "正在送信", DS_PLAY );
	
	return SsmPlayMem( nChan, 6, (LPBYTE)LvWordPtr, LvWordSize, 0, 0xFFFFFFFF );
}

struct state* CClerkChannel::PlyLvWord_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved=0; return &bkOffHk;
	}
	
	return ChkDTSignal( evtblk, &PlyLvWord );
}
