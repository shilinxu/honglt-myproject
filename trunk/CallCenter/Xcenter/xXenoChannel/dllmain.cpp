/*
 * dllmain.cxx
 *
 * DLL main entry point for xXenoChannel.dll
 *
 */

#include "stdafx.h"
#include "ChannelXeno.h"

BOOL APIENTRY DllMain (HANDLE, DWORD, LPVOID)					 
{
    return TRUE;
}

CChannel* Create(int nChan, LPCTSTR xFeeCode)
{
	return new CXenoChannel( nChan, xFeeCode );
}
