//
// ChannelVirtual.cpp
//

#include "stdafx.h"
#include "ChannelVirtual.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CVirtualChannel construction

CVirtualChannel::CVirtualChannel()
{
	m_pBuilder	= NULL;
}

CVirtualChannel::~CVirtualChannel()
{
	if ( m_pBuilder ) delete m_pBuilder;
}

//////////////////////////////////////////////////////////////////////
// CVirtualChannel run function

BOOL CVirtualChannel::OnRun()
{
	if ( !ConnectNamedPipe( m_hPipe, NULL ) ) return FALSE;
	
	return CPipeline::OnRun();
}

//////////////////////////////////////////////////////////////////////
// CVirtualChannel Pipe Transfer func

BOOL CVirtualChannel::OnPacket(CPacket* pPacket)
{
	if ( pPacket->IsCommandId( IBase_LookOutReq ) )
	{
		return OnLookOutReq( (CLookOutReq*)pPacket );
	}
	
	return TRUE;
}

BOOL CVirtualChannel::OnLookOutReq(CLookOutReq* pPacket)
{
	return TRUE;
}
