/*
 * dllmain.cxx
 *
 * DLL main entry point for xMailChannel.dll
 *
 */

#include "stdafx.h"
#include "ChannelMail.h"
#include "ChannelPager.h"

BOOL APIENTRY DllMain (HANDLE, DWORD, LPVOID)					 
{
    return TRUE;
}

CChannel* Create(int nChan, LPCTSTR xFeeCode)
{
	return new CMailChannel( nChan, xFeeCode );
}
