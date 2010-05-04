/*
 * dllmain.cxx
 *
 * DLL main entry point for xInterChannel.dll
 *
 */

#include "stdafx.h"
#include "ChannelInter.h"

BOOL APIENTRY DllMain (HANDLE, DWORD, LPVOID)					 
{
    return TRUE;
}

CChannel* Create(int nChan, LPCTSTR xFeeCode)
{
	return new CInterChannel( nChan, xFeeCode );
}
