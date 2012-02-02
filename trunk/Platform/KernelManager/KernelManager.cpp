//
// KernelManager.cpp
//

#include "stdafx.h"
#include "KernelManager.h"

extern "C" BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason==DLL_PROCESS_ATTACH)
	{
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
	}
	
	return TRUE;
}
