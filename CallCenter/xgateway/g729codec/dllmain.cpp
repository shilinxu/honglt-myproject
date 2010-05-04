/*
 * dllmain.cxx
 *
 * DLL main entry point for g729aCodec
 *
 */

#include "stdafx.h"
#include "G729Codec.h"

CFactoryTemplate g_Templates [] = 
{
//	{ L"G.729 Encoder", &CLSID_G729Encoder, CG729EncoderFilter::CreateInstance, NULL, &sudEncoder },
//	{ L"G.729 Decoder", &CLSID_G729Decoder, CG729DecoderFilter::CreateInstance, NULL, &sudDecoder },
	{ L"G.729A Codec", &CLSID_G729ACodec, CG729Codec::CreateInstance, NULL, &sudCodec },
};

int g_cTemplates = sizeof (g_Templates) / sizeof (g_Templates[0]);

/////////////////////////////////////////////////////////////////////////////
// register and unregister entry points

#if 0
STDAPI DllRegisterServer()
{
    return AMovieDllRegisterServer2 (TRUE);
}

STDAPI DllUnregisterServer()
{
	return AMovieDllRegisterServer2 (FALSE);
}

#else
REGFILTER2 rf2FilterReg = 
{
	1, MERIT_NORMAL, 2, psudPins
};

STDAPI DllRegisterServer()
{
    HRESULT hr = AMovieDllRegisterServer2 (TRUE);
	if ( FAILED( hr ) ) return hr;
	
	IFilterMapper2* pFM2;
	
	hr = CoCreateInstance(CLSID_FilterMapper2, NULL, CLSCTX_INPROC_SERVER,
						IID_IFilterMapper2, (void **)&pFM2);
	if ( FAILED( hr ) ) return hr;
	
	hr = pFM2->RegisterFilter(CLSID_G729ACodec, L"G.729A Codec", NULL,
						&CLSID_AudioCompressorCategory, L"G.729A Codec", &rf2FilterReg);
	
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
	
	hr = pFM2->UnregisterFilter(&CLSID_AudioCompressorCategory, L"G.729A Codec",
						CLSID_G729ACodec);
	
    pFM2->Release();
    return hr;
}
#endif

extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  dwReason, LPVOID lpReserved)
{
	return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}
