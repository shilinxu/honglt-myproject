/*
 * dllmain.cxx
 *
 * DLL main entry point for xDialerChannel.dll
 *
 */

#include "stdafx.h"
#include "ChannelDialer.h"

BOOL APIENTRY DllMain (HANDLE, DWORD, LPVOID)					 
{
    return TRUE;
}

CChannel* Create(int nChan, LPCTSTR xFeeCode)
{
	return new CDialerChannel( nChan, xFeeCode );
}
