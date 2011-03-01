//
// ComService.cpp
//
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// #include <Windows.h>
// #include <stdio.h>
// #include <stdarg.h>
// #include <wchar.h>

#include "ComService.h"

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason==DLL_PROCESS_ATTACH)
	{
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
	}

	return TRUE;
}
