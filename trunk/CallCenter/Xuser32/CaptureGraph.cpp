//
// CaptureGraph.cpp
//

#include "stdafx.h"
#include "CaptureGraph.h"
#include "FilterRtpSender.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CGraphCapture CaptureGraph;

//////////////////////////////////////////////////////////////////////
// CGraphCapture construction

CGraphCapture::CGraphCapture() : CFilterGraph("")
{
}

CGraphCapture::~CGraphCapture()
{
}

//////////////////////////////////////////////////////////////////////
// CGraphCapture connection

HRESULT CGraphCapture::AudioCapture(REFCLSID clsidCompressor, LPCWSTR lpszName)
{
	Stop();
	HRESULT hr = S_OK;
	
	// Capture
	
	CComPtr<IBaseFilter> pCapture;
	GetNextDevice( CLSID_AudioInputDeviceCategory, &pCapture );
	
	hr = InstallFilter( pCapture, L"Audio Capture" );
	if ( FAILED(hr) ) return hr;
	
	// Compressor
	
	CComPtr<IBaseFilter> pCompressor;
	if ( SUCCEEDED(hr) ) hr =
	CreateFilter( clsidCompressor, lpszName, &pCompressor );
	
	// RenderStream
	
	m_pCapGraphBuilder->RenderStream( &PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Audio, pCapture, NULL, pCompressor );
	
	CComPtr<IPin> pPinOut;
	m_pCapGraphBuilder->FindPin( pCompressor, PINDIR_OUTPUT, NULL, NULL, TRUE, 0, &pPinOut );
	
	m_pGraphBuilder->Render( pPinOut );
	
	Run();return hr;
}

HRESULT CGraphCapture::InstallRender(REFGUID psubType, WAVEFORMATEX* pwfx)
{
	Stop();
	HRESULT hr = S_OK;
	
	// Renderer
	
	CRtpSenderFilter* pSender = new CRtpSenderFilter(NULL, &hr);
	pSender->SetupMediaType( psubType, pwfx );
	
	CComPtr<IBaseFilter> pRenderer;
	pSender->QueryInterface( IID_IBaseFilter, (void**)&pRenderer );
	
	hr = InstallFilter( pRenderer, L"Audio Renderer" );
	if ( FAILED(hr) ) return hr;
	
	// RenderStream
	
//	if ( SUCCEEDED(hr) ) hr =
	m_pCapGraphBuilder->RenderStream( NULL,
		NULL, NULL, NULL, pRenderer );
	
	Run(); return hr;
}

HRESULT CGraphCapture::VideoCapture(REFCLSID clsidCompressor, LPCWSTR lpszName)
{
	Stop();
	HRESULT hr = S_OK;
	
	// Capture
	
	CComPtr<IBaseFilter> pCapture;
	FindFilterByName(CLSID_VideoInputDeviceCategory, NULL, &pCapture);
	
	hr = InstallFilter( pCapture, L"Video Capture" );
	if ( FAILED(hr) ) return hr;
	
	// Compressor
	
	CComPtr<IBaseFilter> pCompressor;
	if ( SUCCEEDED(hr) ) hr =
	CreateFilter( clsidCompressor, lpszName, &pCompressor );
	
	// RenderStream
	
	m_pCapGraphBuilder->RenderStream( &PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Video, pCapture, NULL, pCompressor );
	
	CComPtr<IPin> pPinOut;
	m_pCapGraphBuilder->FindPin( pCompressor, PINDIR_OUTPUT, NULL, NULL, TRUE, 0, &pPinOut );
	
	m_pGraphBuilder->Render( pPinOut );
	
	Run(); return hr;
}

HRESULT CGraphCapture::InstallRender(REFGUID psubType, VIDEOINFOHEADER* pvih)
{
	Stop();
	HRESULT hr = S_OK;
	
	// Renderer
	
	CRtpSenderFilter* pSender = new CRtpSenderFilter(NULL, &hr);
	pSender->SetupMediaType( psubType, pvih );
	
	CComPtr<IBaseFilter> pRenderer;
	pSender->QueryInterface( IID_IBaseFilter, (void**)&pRenderer );
	
	hr = InstallFilter( pRenderer, L"Video Renderer" );
	if ( FAILED(hr) ) return hr;
	
	// RenderStream
	
//	if ( SUCCEEDED(hr) ) hr =
	m_pCapGraphBuilder->RenderStream( NULL,
		NULL, NULL, NULL, pRenderer );
	
	Run(); return hr;
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
