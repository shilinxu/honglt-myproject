//
// VideoCapture.cpp
//

#include "stdafx.h"
#include "VideoCapture.h"
#include "FilterRtpSender.h"

#include <initguid.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// {C9076CE2-FB56-11CF-906C-00AA00A59F69}
DEFINE_GUID(CLSID_H263Encoder, 
0xc9076ce2, 0xfb56, 0x11cf, 0x90, 0x6c, 0x0, 0xaa, 0x0, 0xa5, 0x9f, 0x69);

CVideoCapture VideoCapture;

//////////////////////////////////////////////////////////////////////
// CVideoCapture construction

CVideoCapture::CVideoCapture()
{
}

CVideoCapture::~CVideoCapture()
{
}

//////////////////////////////////////////////////////////////////////
// CVideoCapture connection

HRESULT CVideoCapture::ConnnectTo(LPCWSTR lpszCapture)
{
	HRESULT hr = S_OK;
	
	// Capture
	
	CComPtr<IBaseFilter> pCapture;
	CreateFilter( lpszCapture, L"Video Capture", &pCapture );
	
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

/////////////////////////////////////////////////////////////////////////////
// CVideoCapture preview

HRESULT CVideoCapture::StartPreview(const RECT& rect, HWND hWnd)
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

void CVideoCapture::StopPreview()
{
	this->Stop();
	
	CComPtr<IVideoWindow> pVideoWindow;
	m_pGraphBuilder->QueryInterface( IID_IVideoWindow, (void**)&pVideoWindow);
	
	pVideoWindow->put_Owner( (OAHWND)NULL );
	pVideoWindow->put_Visible( OAFALSE );
}

HRESULT CVideoCapture::PutIntoWindow(HWND hWndOwner, const RECT rect, BOOL bVisible)
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
