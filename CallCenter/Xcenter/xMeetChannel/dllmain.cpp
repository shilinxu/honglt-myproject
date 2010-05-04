/*
 * dllmain.cxx
 *
 * DLL main entry point for xMeetChannel.dll
 *
 */

#include "stdafx.h"
#include "ChannelMeet.h"
#include "PageSettingsMeeting.h"

BOOL APIENTRY DllMain (HANDLE, DWORD, LPVOID)					 
{
    return TRUE;
}

CChannel* Create(int nChan, LPCTSTR xFeeCode)
{
	return new CMeetChannel( nChan, xFeeCode );
}

CSettingsPage* XPageSettings()
{
	CRuntimeClass* pClass = RUNTIME_CLASS( CMeetingSettingsPage );
	return (CSettingsPage*)pClass->CreateObject();
}
