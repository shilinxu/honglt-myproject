/*
 * dllmain.cxx
 *
 * DLL main entry point for Xplugin.dll
 *
 */

#include "stdafx.h"
#include "WndPlugin.h"
#include "WindowManager.h"

// Registry

const TCHAR	gcszWindowName[]	= _T("CallCenter.Xmaster.Xplugin");
const TCHAR	gcszWindowClass[]	= _T("CallCenter.Xmaster.XpluginEx");

BOOL APIENTRY DllMain (HANDLE, DWORD, LPVOID)
{
    return TRUE;
}

BOOL ActiveMasterFee(LPCTSTR lpszClassName, LPCTSTR xUserId, WORD nType)
{
	CWnd* pWnd = CWnd::FindWindow( lpszClassName, NULL );
	if ( pWnd == NULL ) return FALSE;
	
	for ( CWnd* pChild = pWnd->GetWindow( GW_CHILD ) ; pChild ; pChild = pChild->GetNextWindow() )
	{
		if ( pChild->IsKindOf( RUNTIME_CLASS(CPluginWnd) ) ) break;
	}
	
	if ( pChild ) pChild->SendMessage( WM_USER + 111 );
	
	return TRUE;
}
