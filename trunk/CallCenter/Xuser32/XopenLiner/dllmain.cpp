/*
 * dllmain.cxx
 *
 * DLL main entry point for XopenLiner.dll
 *
 */

#include "stdafx.h"
#include "WndLiner.h"
#include "WindowManager.h"

class CXMLElement;

BOOL APIENTRY DllMain (HANDLE, DWORD, LPVOID)
{
    return TRUE;
}

CChildWnd* Create(CXMLElement* pXML, CWindowManager* pWindows)
{
	if ( CChildWnd* pChildWnd = pWindows->Find( RUNTIME_CLASS(CLinerWnd) ) )
	{
		FreeLibrary( AfxGetResourceHandle() );
	}
	
	CChildWnd* pChildWnd = pWindows->Open( RUNTIME_CLASS(CLinerWnd) );
//	((CAgencyWnd *)pChildWnd)->LayoutJobs( pXML );
	
	return pChildWnd;
}
