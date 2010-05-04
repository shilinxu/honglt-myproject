//
// ChannelVip.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "ChannelVip.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CVipChannel construction

CVipChannel::CVipChannel(LPCTSTR xUserId, int nChan) : m_xUserId( _tcsdup(xUserId) ), m_nChan( nChan )
{
}

CVipChannel::~CVipChannel()
{
}

//////////////////////////////////////////////////////////////////////
// CVipChannel attributes

#ifdef _DEBUG
/*
BOOL CVipChannel::ReadBlock(EVTBLK* evtblk)
{
	static i = 0;
	if ( Network.m_nMaxChan <=0 ) return FALSE;
	
	if ( i++ == 0 )
	{
		evtblk->nReference = 0;
		evtblk->wEventCode = E_CHG_ChState;
		evtblk->dwParam    = MAKEVENT(0 & 0xFFFF, S_CALL_PICKUPED);
	}
	return TRUE;
}
*/
#endif
