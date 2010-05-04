/*
 * dllmain.cxx
 *
 * DLL main entry point for xInfoChannel.dll
 *
 */

#include "stdafx.h"
#include "ChannelInfo.h"

BOOL APIENTRY DllMain (HANDLE, DWORD, LPVOID)					 
{
    return TRUE;
}

CChannel* Create(int nChan, LPCTSTR xFeeCode)
{
	return new CInfoChannel( nChan, xFeeCode );
}
