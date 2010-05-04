/*
 * dllmain.cxx
 *
 * DLL main entry point for g729Codec
 *
 */

#include "stdafx.h"
#include "FilterG729Encoder.h"
#include "FilterG729Decoder.h"

CFactoryTemplate g_Templates [] = 
{
	{ L"G.729 Encoder", &CLSID_G729Encoder, CG729EncoderFilter::CreateInstance, NULL, &sudEncoder },
	{ L"G.729 Decoder", &CLSID_G729Decoder, CG729DecoderFilter::CreateInstance, NULL, &sudDecoder },
};

int g_cTemplates = sizeof (g_Templates) / sizeof (g_Templates[0]);

/////////////////////////////////////////////////////////////////////////////
// register and unregister entry points

STDAPI DllRegisterServer()
{
    return AMovieDllRegisterServer2 (TRUE);
}

STDAPI DllUnregisterServer()
{
    return AMovieDllRegisterServer2 (FALSE);
}

extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  dwReason, LPVOID lpReserved)
{
	return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}
