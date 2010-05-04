/*
 * dllmain.cxx
 *
 * DLL main entry point for xPlayChannel.dll
 *
 */

#include "stdafx.h"
#include "PageSettingsPager.h"
#include "PageProfilePlay.h"
#include "XML.h"

#include "ChannelPlay.h"
#include "ChannelMesger.h"
#include "Registry.h"
#include "Network.h"

BOOL APIENTRY DllMain (HANDLE, DWORD, LPVOID)
{
    return TRUE;
}

CChannel* Create(int nChan, LPCTSTR xFeeCode)
{
	if ( ! _tcsicmp(xFeeCode, "MSGR") )
		return new CMesgerChannel( nChan, xFeeCode );
	else
		return new CPlayChannel( nChan, xFeeCode );
}

CSettingsPage* XPageSettings()
{
	CRuntimeClass* pClass = RUNTIME_CLASS( CPagerSettingsPage );
	return (CSettingsPage*)pClass->CreateObject();
}

CSettingsPage* XPageProfiles(CXMLElement* pXML)
{
	return new CPlayProfilePage( pXML->GetElementByName( _T("Play"), TRUE ) );
}

CChannel* LookOut(LPCTSTR xRemoteId, LPCTSTR xFeeNumber)
{
	CChannel* pChannel = Network.LookOutPhs( xRemoteId, xFeeNumber );
	if ( pChannel ) return pChannel;
	
	CRegistry pRegistry;
	CString strValue = pRegistry.GetString( "Pager", "PageFile" );
	
	return Network.LookOutPhs( xRemoteId, strValue );
}
