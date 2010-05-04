/*
 * dllmain.cxx
 *
 * DLL main entry point for xRadioChannel.dll
 *
 */

#include "stdafx.h"
#include "ChannelRadio.h"
#include "PageSettingsPlay.h"
#include "PageProfilePlay.h"
#include "XML.h"

BOOL APIENTRY DllMain (HANDLE, DWORD, LPVOID)					 
{
    return TRUE;
}

CChannel* Create(int nChan, LPCTSTR xFeeCode)
{
	return new CRadioChannel( nChan, xFeeCode );
}

CSettingsPage* XPageSettings()
{
	CRuntimeClass* pClass = RUNTIME_CLASS( CPlaySettingsPage );
	return (CSettingsPage*)pClass->CreateObject();
}

CSettingsPage* XPageProfiles(CXMLElement* pXML)
{
#ifdef ZHAOPIN
	return new CPlayProfilePage( pXML->GetElementByName( _T("ZhaoPin"), TRUE ) );
#else
	return new CPlayProfilePage( pXML->GetElementByName( _T("Radio"), TRUE ) );
#endif
}
