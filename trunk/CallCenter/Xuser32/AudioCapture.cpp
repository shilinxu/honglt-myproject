//
// AudioCapture.cpp
//

#include "stdafx.h"
#include "AudioCapture.h"
#include "FilterRtpSender.h"

#include <initguid.h>
#include <G711uids.h>
#include <mmreg.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CAudioCapture AudioCapture;

//////////////////////////////////////////////////////////////////////
// CAudioCapture construction

CAudioCapture::CAudioCapture()
{
}

CAudioCapture::~CAudioCapture()
{
}

//////////////////////////////////////////////////////////////////////
// CAudioCapture connection

HRESULT CAudioCapture::ConnnectTo(LPCWSTR lpszCapture)
{
	HRESULT hr = S_OK;
	
	// Capture
	
	CComPtr<IBaseFilter> pCapture;
	CreateFilter( lpszCapture, L"Audio Capture", &pCapture );
	
	// Compressor
	
	CComPtr<IBaseFilter> pCompressor;
	CreateFilter( CLSID_G711Codec, L"G.711 Encoder", &pCompressor );
	
	// Renderer
	
	CRtpSenderFilter* pSender = new CRtpSenderFilter(NULL, &hr);
	
	WAVEFORMATEX wfx;
	ZeroMemory( &wfx, sizeof( wfx ) );
	wfx.wFormatTag = WAVE_FORMAT_ALAW;
	pSender->SetupMediaType( &wfx );
	
	CComPtr<IBaseFilter> pRenderer;
	pSender->QueryInterface( IID_IBaseFilter, (void**)&pRenderer );
	InstallFilter( pRenderer, L"Audio Sender" );
	
	// RenderStream
	
	if ( SUCCEEDED(hr) ) hr =
	m_pCapGraphBuilder->RenderStream( &PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Audio, pCapture, pCompressor, pRenderer );
	
	Run(); return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CAudioCapture preview

HRESULT CAudioCapture::StartPreview()
{
	StopPreview();
	
	CComPtr<IBaseFilter> pCapture;
	m_pGraphBuilder->FindFilterByName( L"Audio Capture", &pCapture );
	
	HRESULT hr = 
	m_pCapGraphBuilder->RenderStream( &PIN_CATEGORY_PREVIEW, 
		&MEDIATYPE_Audio, pCapture, NULL, NULL );
	
	Run(); return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CAudioCapture operations
