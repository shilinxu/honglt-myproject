/*
 * dllmain.cxx
 *
 * DLL main entry point for xProfitChannel.dll
 *
 */

#include "stdafx.h"
//#include "ChannelProfit.h"
#include "ChannelPlay.h"
#include "PageSettingsPlay.h"

BOOL APIENTRY DllMain (HANDLE, DWORD, LPVOID)					 
{
    return TRUE;
}
/*
CChannel* Create(int nChan, LPCTSTR xFeeCode)
{
	if ( xFeeCode && ! _tcscmp(xFeeCode, "2888") )
	return new CProfitChannel( nChan, xFeeCode, 0 );
	else
	return new CProfitChannel( nChan, xFeeCode );
}
*/

CChannel* Create(int nChan, LPCTSTR xFeeCode)
{
	return new CPlayChannel( nChan, xFeeCode );
}

CSettingsPage* XPageSettings()
{
	CRuntimeClass* pClass = RUNTIME_CLASS( CPlaySettingsPage );
	return (CSettingsPage*)pClass->CreateObject();
}
