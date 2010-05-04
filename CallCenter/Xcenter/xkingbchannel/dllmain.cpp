/*
 * dllmain.cxx
 *
 * DLL main entry point for xKingbChannel.dll
 *
 */

#include "stdafx.h"
#include "ChannelCaller.h"

BOOL APIENTRY DllMain (HANDLE, DWORD, LPVOID)					 
{
    return TRUE;
}

CChannel* Create(int nChan, LPCTSTR xFeeCode)
{
	return new CCallerChannel( nChan, xFeeCode );
}
