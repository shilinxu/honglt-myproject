/*
 * dllmain.cxx
 *
 * DLL main entry point for xFriendChannel.dll
 *
 */

#include "stdafx.h"
#include "ChannelFriend.h"

BOOL APIENTRY DllMain (HANDLE, DWORD, LPVOID)					 
{
    return TRUE;
}

CChannel* Create(int nChan, LPCTSTR xFeeCode)
{
	if ( xFeeCode && ! _tcscmp(xFeeCode, "2999") )
	return new CFriendChannel( nChan, xFeeCode, 0 );
	else
	return new CFriendChannel( nChan, xFeeCode );
}
