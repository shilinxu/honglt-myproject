//
// ComService.cpp
//

#include "stdafx.h"

BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason==DLL_PROCESS_ATTACH)
	{
//		::DisableThreadLibraryCalls(hInstance);
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
	}
	
	return TRUE;
}
