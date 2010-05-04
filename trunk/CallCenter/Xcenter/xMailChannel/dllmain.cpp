/*
 * dllmain.cxx
 *
 * DLL main entry point for xMailChannel.dll
 *
 */

#include "stdafx.h"
#include "ChannelMail.h"
#include "PageProfilePlay.h"
#include "XML.h"

BOOL APIENTRY DllMain (HANDLE, DWORD, LPVOID)					 
{
    return TRUE;
}

CChannel* Create(int nChan, LPCTSTR xFeeCode)
{
	return new CMailChannel( nChan, xFeeCode );
}

CSettingsPage* XPageProfiles(CXMLElement* pXML)
{
	return new CPlayProfilePage( pXML->GetElementByName( _T("MailBox"), TRUE ) );
}
