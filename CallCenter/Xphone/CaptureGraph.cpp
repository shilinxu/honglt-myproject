//
// CaptureGraph.cpp
//

#include "stdafx.h"
#include "CaptureGraph.h"
#include "FilterRtpSender.h"

#include <initguid.h>
#include <G711uids.h>
#include <mmreg.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// {C9076CE2-FB56-11CF-906C-00AA00A59F69}
DEFINE_GUID(CLSID_H263Encoder, 
0xc9076ce2, 0xfb56, 0x11cf, 0x90, 0x6c, 0x0, 0xaa, 0x0, 0xa5, 0x9f, 0x69);

CGraphCapture CaptureGraph;

//////////////////////////////////////////////////////////////////////
// CGraphCapture construction

CGraphCapture::CGraphCapture()
{
}

CGraphCapture::~CGraphCapture()
{
}

//////////////////////////////////////////////////////////////////////
// CGraphCapture connection

HRESULT CGraphCapture::InstallAudio(LPCWSTR lpszCapture)
{
	Stop();
	HRESULT hr = S_OK;
	
	// Capture
	
	CComPtr<IBaseFilter> pCapture;
	if ( lpszCapture )
	{
		CreateFilter( lpszCapture, L"Audio Capture", &pCapture );
	}
	else
	{
		EnumCapture( CLSID_AudioInputDeviceCategory, &pCapture );
		InstallFilter( pCapture, L"Audio Capture" );
	}
	
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

HRESULT CGraphCapture::InstallVideo(LPCWSTR lpszCapture)
{
	Stop();
	HRESULT hr = S_OK;
	
	// Capture
	
	CComPtr<IBaseFilter> pCapture;
	if ( lpszCapture )
	{
		CreateFilter( lpszCapture, L"Video Capture", &pCapture );
	}
	else
	{
		EnumCapture( CLSID_VideoInputDeviceCategory, &pCapture );
		InstallFilter( pCapture, L"Video Capture" );
	}
	
	// Compressor
	
	CComPtr<IBaseFilter> pCompressor;
	CreateFilter( CLSID_H263Encoder, L"H.263 Encoder", &pCompressor );
	
	// Renderer
	
	CRtpSenderFilter* pSender = new CRtpSenderFilter(NULL, &hr);
	
	VIDEOINFOHEADER vih;
	ZeroMemory( &vih, sizeof( vih ) );
	vih.bmiHeader.biCompression	= MAKEFOURCC('H','2','6','3');
	pSender->SetupMediaType( &vih );
	
	CComPtr<IBaseFilter> pRenderer;
	pSender->QueryInterface( IID_IBaseFilter, (void**)&pRenderer );
	InstallFilter( pRenderer, L"Video Sender" );
	
	// RenderStream
	
	if ( SUCCEEDED(hr) ) hr =
	m_pCapGraphBuilder->RenderStream( &PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Video, pCapture, pCompressor, pRenderer );
	
	Run(); return hr;
}

HRESULT CGraphCapture::EnumCapture(REFCLSID rclsid, IBaseFilter** ppFilter)
{
	CComPtr<ICreateDevEnum> icDevEnum;
	if ( CoCreateInstance( CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (void**)&icDevEnum ) != S_OK ) return E_FAIL;
	
	CComPtr<IEnumMoniker> ieMoniker;
	if ( icDevEnum->CreateClassEnumerator( rclsid,
		&ieMoniker, 0 ) != S_OK || ieMoniker == NULL ) return E_FAIL;
	
	IMoniker* pMoniker = NULL;
	while ( ieMoniker->Next(1, &pMoniker, NULL) == S_OK )
	{
#ifdef _DEBUG
		CComPtr<IPropertyBag> pBag;
		if ( pMoniker->BindToStorage( 0, 0, IID_IPropertyBag,
			(void **)&pBag ) != S_OK ) continue;
		
		CComVariant TheValue;
		pBag->Read( L"FriendlyName", &TheValue, NULL );
		
		afxDump << L"FriendlyName: " << TheValue.bstrVal << "\n";
#endif
		if ( pMoniker->BindToObject( 0, 0, IID_IBaseFilter,
			(void**)ppFilter ) == S_OK ) return S_OK;
	}
	
	return E_FAIL;
}

/////////////////////////////////////////////////////////////////////////////
// CGraphCapture preview

HRESULT CGraphCapture::StartPreview()
{
	StopPreview();
	
	CComPtr<IBaseFilter> pCapture;
	m_pGraphBuilder->FindFilterByName( L"Audio Capture", &pCapture );
	
	HRESULT hr = 
	m_pCapGraphBuilder->RenderStream( &PIN_CATEGORY_PREVIEW, 
		&MEDIATYPE_Audio, pCapture, NULL, NULL );
	
	Run(); return hr;
}

HRESULT CGraphCapture::StartPreview(const RECT& rect, HWND hWnd)
{
	StopPreview();
	
	CComPtr<IBaseFilter> pCapture;
	m_pGraphBuilder->FindFilterByName( L"Video Capture", &pCapture );
	
	HRESULT hr =
	m_pCapGraphBuilder->RenderStream( &PIN_CATEGORY_PREVIEW, 
		&MEDIATYPE_Video, pCapture, NULL, NULL );
	
	if ( SUCCEEDED(hr) ) hr =
	PutIntoWindow( hWnd, rect, TRUE );
	
	Run(); return hr;
}

void CGraphCapture::StopPreview()
{
	this->Stop();
	
	CComPtr<IVideoWindow> pVideoWindow;
	m_pGraphBuilder->QueryInterface( IID_IVideoWindow, (void**)&pVideoWindow);
	
	pVideoWindow->put_Owner( (OAHWND)NULL );
	pVideoWindow->put_Visible( OAFALSE );
}

HRESULT CGraphCapture::PutIntoWindow(HWND hWndOwner, const RECT rect, BOOL bVisible)
{
	CComPtr<IVideoWindow> pVideoWindow;
	m_pGraphBuilder->QueryInterface( IID_IVideoWindow, (void**)&pVideoWindow);
	
	pVideoWindow->SetWindowPosition( rect.left, rect.top,
		rect.right - rect.left, rect.bottom - rect.top );
	
	pVideoWindow->put_Owner( (OAHWND)hWndOwner );
	pVideoWindow->put_WindowStyle( WS_CHILD | WS_CLIPSIBLINGS );
	pVideoWindow->put_Visible( bVisible );
	
	return Run();
}
