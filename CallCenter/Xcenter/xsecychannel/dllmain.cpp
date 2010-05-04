/*
 * dllmain.cxx
 *
 * DLL main entry point for xSecyChannel.dll
 *
 */

#include "stdafx.h"
#include "ChannelTalk.h"

BOOL APIENTRY DllMain (HANDLE, DWORD, LPVOID)					 
{
    return TRUE;
}

CChannel* Create(int nChan, LPCTSTR xFeeCode)
{
	return new CTalkChannel( nChan, xFeeCode );
}
