/*
 * dllmain.cxx
 *
 * DLL main entry point for XvidCodec
 *
 */

#include "stdafx.h"
#include "XvidCodec.h"

CFactoryTemplate g_Templates [] = 
{
	{ L"Xvid MPEG-4 Codec", &CLSID_XvidCodec, CXvidCodec::CreateInstance, NULL, &sudCodec },
};

int g_cTemplates = sizeof (g_Templates) / sizeof (g_Templates[0]);

/////////////////////////////////////////////////////////////////////////////
// register and unregister entry points

REGFILTER2 rf2FilterReg = 
{
	1, 0x268800, 2, psudPins
};

STDAPI DllRegisterServer()
{
	HRESULT hr = AMovieDllRegisterServer2 (TRUE);
	if ( FAILED( hr ) ) return hr;
	
	IFilterMapper2* pFM2;
	
	hr = CoCreateInstance(CLSID_FilterMapper2, NULL, CLSCTX_INPROC_SERVER,
						IID_IFilterMapper2, (void **)&pFM2);
	if ( FAILED( hr ) ) return hr;
	
	hr = pFM2->RegisterFilter(CLSID_XvidCodec, L"Xvid MPEG-4 Codec", NULL,
						&CLSID_VideoCompressorCategory, L"Xvid MPEG-4 Codec", &rf2FilterReg);
	
    pFM2->Release();
    return hr;
}

STDAPI DllUnregisterServer()
{
	HRESULT hr = AMovieDllRegisterServer2 (FALSE);
	if ( FAILED( hr ) ) return hr;
	
	IFilterMapper2* pFM2;
	
	hr = CoCreateInstance(CLSID_FilterMapper2, NULL, CLSCTX_INPROC_SERVER,
						IID_IFilterMapper2, (void **)&pFM2);
	if ( FAILED( hr ) ) return hr;
	
	hr = pFM2->UnregisterFilter(&CLSID_VideoCompressorCategory, L"Xvid MPEG-4 Codec",
						CLSID_XvidCodec);
	
    pFM2->Release();
    return hr;
}

extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  dwReason, LPVOID lpReserved)
{
	return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}
