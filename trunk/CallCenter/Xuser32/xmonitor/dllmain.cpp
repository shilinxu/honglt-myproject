/*
 * dllmain.cxx
 *
 * DLL main entry point for Xmonitor.dll
 *
 */

#include "stdafx.h"
#include "WndMonitor.h"
#include "WindowManager.h"

class CXMLElement;

BOOL APIENTRY DllMain (HANDLE, DWORD, LPVOID)
{
    return TRUE;
}

CChildWnd* Create(CXMLElement* pXML, CWindowManager* pWindows)
{
	if ( CChildWnd* pChildWnd = pWindows->Find( RUNTIME_CLASS(CMonitorWnd) ) )
	{
		FreeLibrary( AfxGetResourceHandle() );
	}
	
	CChildWnd* pChildWnd = pWindows->Open( RUNTIME_CLASS(CMonitorWnd) );
//	((CMonitorWnd *)pChildWnd)->LayoutJobs( pXML );
	
	return pChildWnd;
}
