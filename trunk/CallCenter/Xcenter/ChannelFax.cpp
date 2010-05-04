//
// ChannelFax.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "ChannelFax.h"

#include "CtrlNetworkChild.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CFaxChannel construction

CFaxChannel::CFaxChannel(int nChan) : CChannel(nChan)
{
	bkIdle.process			= (PROCESS)bkIdle_cmplt;
	bkOffHk.initiate		= (INITIATE)bkOffHk_init;
	
	FaxPage.initiate		= (INITIATE)FaxPage_init;
	FaxPage.process			= (PROCESS)FaxPage_cmplt;
}

CFaxChannel::~CFaxChannel()
{
}

/////////////////////////////////////////////////////////////////////////////
// CFaxChannel OVERLOAD function

struct state* CFaxChannel::bkIdle_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_CHG_FaxChState )
	{
		return &FaxPage;
	}
	
	return CChannel::bkIdle_cmplt(evtblk);
}

/////////////////////////////////////////////////////////////////////////////
// CFaxChannel fax function

int CFaxChannel::FaxPage_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;

	Reserved |= 0x08;	// ÖÃ×´Ì¬¡°ÕýÔÚ¡±
	m_pOwner->SetItem( nChan, evtblk->dwParam & 0xFFFF );
	
	return 0;
}

struct state* CFaxChannel::FaxPage_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_CHG_FaxPages )
	{
		SsmPutUserEvent( E_CHG_FaxPages, m_nTaskId,
			MAKEVENT(nChan & 0xFFFF, evtblk->dwParam & 0xFFFF) );
	}

	if ( evtblk->wEventCode == E_PROC_FaxEnd )
	{
		SsmPutUserEvent( E_PROC_FaxEnd, m_nTaskId,
			MAKEVENT(nChan & 0xFFFF, evtblk->dwParam & 0xFFFF) );
		
		m_pOwner->SetItem( nChan, S_CALL_STANDBY );
		Reserved = 0; return &bkIdle;
	}

	return ChkDTSignal( evtblk, &FaxPage );
}
