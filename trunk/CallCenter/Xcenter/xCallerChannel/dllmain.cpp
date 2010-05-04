/*
 * dllmain.cxx
 *
 * DLL main entry point for xCallerChannel.dll
 *
 */

#include "stdafx.h"
#include "ChannelCaller.h"
#include "PageSettingsCaller.h"
#include "PageProfilePlay.h"
#include "XML.h"

BOOL APIENTRY DllMain (HANDLE, DWORD, LPVOID)					 
{
    return TRUE;
}

CChannel* Create(int nChan, LPCTSTR xFeeCode)
{
	return new CCallerChannel( nChan, xFeeCode );
}

CSettingsPage* XPageSettings()
{
	CRuntimeClass* pClass = RUNTIME_CLASS( CCallerSettingsPage );
	return (CSettingsPage*)pClass->CreateObject();
}

CSettingsPage* XPageProfiles(CXMLElement* pXML)
{
	return new CPlayProfilePage( pXML->GetElementByName( _T("Caller"), TRUE ) );
}
