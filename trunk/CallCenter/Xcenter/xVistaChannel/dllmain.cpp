/*
 * dllmain.cxx
 *
 * DLL main entry point for xVistaChannel.dll
 *
 */

#include "stdafx.h"
#include "Xcenter.h"
#include "ChannelPlay.h"
#include "PageSettingsVista.h"

BOOL APIENTRY DllMain (HANDLE, DWORD, LPVOID)
{
    return TRUE;
}

CChannel* Create(int nChan, LPCTSTR xFeeCode)
{
	return new CPlayChannel( nChan, xFeeCode );
}

CSettingsPage* XPageSettings()
{
	CRuntimeClass* pClass = RUNTIME_CLASS( CVistaSettingsPage );
	return (CSettingsPage*)pClass->CreateObject();
}
