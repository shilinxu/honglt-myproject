/*
 * dllmain.cxx
 *
 * DLL main entry point for xNotifyChannel.dll
 *
 */

#include "stdafx.h"
#include "ChannelNotify.h"
#include "PageProfilePlay.h"
#include "PageSettingsPlay.h"
#include "XML.h"

BOOL APIENTRY DllMain (HANDLE, DWORD, LPVOID)					 
{
    return TRUE;
}

CChannel* Create(int nChan, LPCTSTR xFeeCode)
{
	return new CNotifyChannel( nChan, xFeeCode );
}

CSettingsPage* XPageSettings()
{
	CRuntimeClass* pClass = RUNTIME_CLASS( CPlaySettingsPage );
	return (CSettingsPage*)pClass->CreateObject();
}

CSettingsPage* XPageProfiles(CXMLElement* pXML)
{
	return new CPlayProfilePage( pXML->GetElementByName( _T("Notify"), TRUE ) );
}
