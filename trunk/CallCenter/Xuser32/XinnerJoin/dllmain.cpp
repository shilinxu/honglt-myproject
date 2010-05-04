/*
 * dllmain.cxx
 *
 * DLL main entry point for XopenLiner.dll
 *
 */

#include "stdafx.h"
#include "WndInner.h"
#include "WindowManager.h"

class CXMLElement;

BOOL APIENTRY DllMain (HANDLE, DWORD, LPVOID)
{
    return TRUE;
}

CChildWnd* Create(CXMLElement* pXML, CWindowManager* pWindows)
{
	if ( CChildWnd* pChildWnd = pWindows->Find( RUNTIME_CLASS(CInnerWnd) ) )
	{
		FreeLibrary( AfxGetResourceHandle() );
	}
	
	CChildWnd* pChildWnd = pWindows->Open( RUNTIME_CLASS(CInnerWnd) );
//	((CInnerWnd *)pChildWnd)->LayoutJobs( pXML );
	
	return pChildWnd;
}
