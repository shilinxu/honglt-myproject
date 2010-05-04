/*
 * dllmain.cxx
 *
 * DLL main entry point for XlivePlayer.dll
 *
 */

#include "stdafx.h"
#include "WndPlayer.h"
#include "WindowManager.h"

class CXMLElement;

BOOL APIENTRY DllMain (HANDLE, DWORD, LPVOID)
{
    return TRUE;
}

CChildWnd* Create(CXMLElement* pXML, CWindowManager* pWindows)
{
	if ( CChildWnd* pChildWnd = pWindows->Find( RUNTIME_CLASS(CPlayerWnd) ) )
	{
		FreeLibrary( AfxGetResourceHandle() );
	}
	
	CChildWnd* pChildWnd = pWindows->Open( RUNTIME_CLASS(CPlayerWnd) );
	
	return pChildWnd;
}
