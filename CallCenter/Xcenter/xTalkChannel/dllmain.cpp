/*
 * dllmain.cxx
 *
 * DLL main entry point for xTalkChannel.dll
 *
 */

#include "stdafx.h"
#include "ChannelTalk.h"
#include "PageSettingsTalker.h"

BOOL APIENTRY DllMain (HANDLE, DWORD, LPVOID)					 
{
    return TRUE;
}

CChannel* Create(int nChan, LPCTSTR xFeeCode)
{
	if ( xFeeCode && ! _tcscmp(xFeeCode, "2666") )
	return new CTalkChannel( nChan, xFeeCode, 0 );
	else
	return new CTalkChannel( nChan, xFeeCode );
}

CSettingsPage* XPageSettings()
{
	CRuntimeClass* pClass = RUNTIME_CLASS( CTalkerSettingsPage );
	return (CSettingsPage*)pClass->CreateObject();
}
