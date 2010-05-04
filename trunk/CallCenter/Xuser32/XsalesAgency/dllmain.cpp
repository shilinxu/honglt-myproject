/*
 * dllmain.cxx
 *
 * DLL main entry point for XsalesAgency.dll
 *
 */

#include "stdafx.h"
#include "WndAgency.h"
#include "WindowManager.h"

BOOL APIENTRY DllMain (HANDLE, DWORD, LPVOID)
{
    return TRUE;
}

CChildWnd* Create(CXMLElement* pXML, CWindowManager* pWindows)
{
	if ( CChildWnd* pChildWnd = pWindows->Find( RUNTIME_CLASS(CAgencyWnd) ) )
	{
		FreeLibrary( AfxGetResourceHandle() );
	}
	
	CChildWnd* pChildWnd = pWindows->Open( RUNTIME_CLASS(CAgencyWnd) );
	((CAgencyWnd *)pChildWnd)->LayoutJobs( pXML );
	
	return pChildWnd;
}
