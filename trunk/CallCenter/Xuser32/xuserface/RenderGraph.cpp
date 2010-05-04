//
// RenderGraph.cpp
//

#include "stdafx.h"
#include "RenderGraph.h"
#include "FilterRtpReceiver.h"
#include "FilterRtpSender.h"

#define NR_AUDIO_FRAME ((float) 0.05)  /* 50 milliseconds*/

//////////////////////////////////////////////////////////////////////
// CGraphRender construction

CGraphRender::CGraphRender(LPCTSTR lpszNamed) : CFilterGraph(lpszNamed)
{
}

CGraphRender::~CGraphRender()
{
}

//////////////////////////////////////////////////////////////////////
// CGraphRender render stream

HRESULT CGraphRender::RenderStream(LPCWSTR lpszCapture, WAVEFORMATEX& pwfx, DWORD nReserved)
{
	HRESULT hr = S_OK;
	if ( ! nReserved ) return RecycleStream( L"Audio Sender" );
	
	// Capture
	
	CComPtr<IBaseFilter> pCapture;
	hr = CreateFilter( lpszCapture, L"Audio Capture", &pCapture );
#if 1	
	CComPtr<IPin> pPinOut;
	hr = m_pCapGraphBuilder->FindPin( pCapture, PINDIR_OUTPUT, NULL, NULL, TRUE, 0, &pPinOut );
	
    CComPtr<IAMBufferNegotiation> pNeg;
	if ( SUCCEEDED(hr) && SUCCEEDED(pPinOut->QueryInterface(IID_IAMBufferNegotiation, (void**)&pNeg)) )
	{
		ALLOCATOR_PROPERTIES AllocProp;
		AllocProp.cbAlign = -1;
 		AllocProp.cbBuffer = (long) ((float)pwfx.nAvgBytesPerSec * NR_AUDIO_FRAME);
		AllocProp.cbPrefix = -1;
		AllocProp.cBuffers = -1;
		hr = pNeg->SuggestAllocatorProperties( &AllocProp );
	}
#endif	
#if 0
	// Renderer
	
	CComPtr<IBaseFilter> pRenderer;
	
	if ( SUCCEEDED(hr) && pCapture )
	hr = InstallRender( new CRtpSenderFilter(&pwfx, this, nReserved), L"Audio Sender", &pRenderer );
	
	// RenderStream
	
	if ( SUCCEEDED(hr) ) hr =
	m_pCapGraphBuilder->RenderStream( &PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Audio, pCapture, NULL, pRenderer );
	
	return hr;
#else
	return RenderStream( pCapture, &pwfx, &PIN_CATEGORY_CAPTURE, nReserved );
#endif
}

HRESULT CGraphRender::RenderStream(LPCWSTR lpszCapture, VIDEOINFOHEADER& pvih, DWORD nReserved)
{
	HRESULT hr = S_OK;
	if ( ! nReserved ) return RecycleStream( L"Video Sender" );
	
	// Capture
	
	CComPtr<IBaseFilter> pCapture;
	hr = CreateFilter( lpszCapture, L"Video Capture", &pCapture );
	
#if 0
	// Renderer
	
	CComPtr<IBaseFilter> pRenderer;
	
	if ( SUCCEEDED(hr) && pCapture )
	hr = InstallRender( new CRtpSenderFilter(&pvih, this, nReserved), L"Video Sender", &pRenderer );
	
	// RenderStream
	
	if ( SUCCEEDED(hr) ) hr =
	m_pCapGraphBuilder->RenderStream( &PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Video, pCapture, NULL, pRenderer );
	
	return hr;
#else
	return RenderStream( pCapture, &pvih, &PIN_CATEGORY_CAPTURE, nReserved );
#endif

}

HRESULT CGraphRender::RenderStream(IBaseFilter* pFilter, WAVEFORMATEX* pwfx, const GUID* pCategory, DWORD nReserved)
{
	HRESULT hr = S_OK;
	
	// Renderer
	
	CComPtr<IBaseFilter> pRenderer;
	
	if ( SUCCEEDED(hr) ) hr =
	InstallRender( new CRtpSenderFilter(pwfx, this, nReserved), L"Audio Sender", &pRenderer );
	
	// RenderStream
	
	if ( SUCCEEDED(hr) ) hr =
	m_pCapGraphBuilder->RenderStream( pCategory,
		&MEDIATYPE_Audio, pFilter, NULL, pRenderer );
	
	return hr;
}

HRESULT CGraphRender::RenderStream(IBaseFilter* pFilter, VIDEOINFOHEADER* pvih, const GUID* pCategory, DWORD nReserved)
{
	HRESULT hr = S_OK;
	
	// Renderer
	
	CComPtr<IBaseFilter> pRenderer;
	
	if ( SUCCEEDED(hr) ) hr =
	InstallRender( new CRtpSenderFilter(pvih, this, nReserved), L"Video Sender", &pRenderer );
	
	// RenderStream
	
	if ( SUCCEEDED(hr) ) hr =
	m_pCapGraphBuilder->RenderStream( pCategory,
		&MEDIATYPE_Video, pFilter, NULL, pRenderer );
	
	return hr;
}

HRESULT CGraphRender::RecycleStream(LPCWSTR lpszName)
{
	this->Stop();
	return RemoveFilter( lpszName );
}

//////////////////////////////////////////////////////////////////////
// CGraphRender render preview

HRESULT CGraphRender::RenderPreview(LPCWSTR lpszRender, WAVEFORMATEX*)
{
	HRESULT hr = S_OK;
	
	// Capture
	
	CComPtr<IBaseFilter> pCapture;
	hr = m_pGraphBuilder->FindFilterByName( L"Audio Capture", &pCapture );
	
	// Renderer
	
	if ( lpszRender ) InstallCapture( lpszRender, L"Audio Renderer" );
	
	CComPtr<IBaseFilter> pRenderer;
	hr = m_pGraphBuilder->FindFilterByName( L"Audio Renderer", &pRenderer );
	
	// RenderStream
	
	if ( SUCCEEDED(hr) ) hr =
	m_pCapGraphBuilder->RenderStream( &PIN_CATEGORY_PREVIEW,
		&MEDIATYPE_Audio, pCapture, NULL, pRenderer );
	
	return hr;
}

HRESULT CGraphRender::RenderPreview(LPCWSTR, VIDEOINFOHEADER*)
{
	HRESULT hr = S_OK;
	
	// Capture
	
	CComPtr<IBaseFilter> pCapture;
	hr = m_pGraphBuilder->FindFilterByName( L"Video Capture", &pCapture );
	
	// RenderStream
	
	if ( SUCCEEDED(hr) ) hr =
	m_pCapGraphBuilder->RenderStream( &PIN_CATEGORY_PREVIEW,
		&MEDIATYPE_Video, pCapture, NULL, NULL );
	
	return hr;
}

HRESULT CGraphRender::RenderPreview(LPCWSTR lpszRender, WAVEFORMATEX& pwfx, DWORD nReserved)
{
	HRESULT hr = S_OK;
	
	// Renderer
	
	if ( lpszRender ) InstallCapture( lpszRender, L"Audio Renderer" );
	
	// Capture
	
	CComPtr<IBaseFilter> pCapture;
	
	if ( SUCCEEDED(hr) /*&& pCapture*/ && FAILED(
		hr = m_pGraphBuilder->FindFilterByName( L"Audio Receiver", &pCapture )) )
	{
		hr = InstallRender( new CRtpReceiverFilter(&pwfx, this, nReserved), L"Audio Receiver", &pCapture );
	}
	
	// RenderStream
	
	CComPtr<IPin> pPinOut;
	pCapture->FindPin( L"Preview", &pPinOut );
	
	hr = m_pGraphBuilder->Render( pPinOut );
	return hr;
}

HRESULT CGraphRender::RenderPreview(LPCWSTR, VIDEOINFOHEADER& pvih, DWORD nReserved)
{
	HRESULT hr = S_OK;
	
	// Capture
	
	CComPtr<IBaseFilter> pCapture;
	
	if ( SUCCEEDED(hr) /*&& pCapture*/ && FAILED(
		hr = m_pGraphBuilder->FindFilterByName( L"Video Receiver", &pCapture )) )
	{
		hr = InstallRender( new CRtpReceiverFilter(&pvih, this, nReserved), L"Video Receiver", &pCapture );
	}
	
	// RenderStream
	
	CComPtr<IPin> pPinOut;
	pCapture->FindPin( L"Preview", &pPinOut );
	
	hr = m_pGraphBuilder->Render( pPinOut );
	return hr;
}

//////////////////////////////////////////////////////////////////////
// CGraphRender operations

HRESULT CGraphRender::InstallRender(CBaseFilter* pBaseFilter, LPCWSTR lpszName, IBaseFilter** ppFilter)
{
	Stop();
	HRESULT hr = S_OK;
	
	// Renderer
	
	CComPtr<IBaseFilter> pRenderer;
	if ( ! ppFilter ) ppFilter = &pRenderer;
	
	pBaseFilter->QueryInterface( IID_IBaseFilter, (void**)&pRenderer );
	pRenderer->QueryInterface( IID_IBaseFilter, (void**)ppFilter);
	
	hr = InstallFilter( pRenderer, lpszName );
	if ( FAILED(hr) ) return hr;
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CGraphRender preview

HRESULT CGraphRender::StartPreview(const RECT& rect, HWND hWnd)
{
	HRESULT hr = PutIntoWindow( hWnd, rect, TRUE );
	
	CFilterGraph::StartPreview(); return hr;
}

void CGraphRender::StopPreview()
{
	CComPtr<IVideoWindow> pVideoWindow;
	m_pGraphBuilder->QueryInterface( IID_IVideoWindow, (void**)&pVideoWindow);
	
	pVideoWindow->put_Visible( OAFALSE );
//	pVideoWindow->put_Owner( (OAHWND)NULL );
	
	CFilterGraph::StopPreview();
}

HRESULT CGraphRender::PutIntoWindow(HWND hWndOwner, const RECT rect, BOOL bVisible)
{
	CComPtr<IVideoWindow> pVideoWindow;
	m_pGraphBuilder->QueryInterface( IID_IVideoWindow, (void**)&pVideoWindow);
	
	OAHWND hWndOwner2;
	pVideoWindow->get_Owner( &hWndOwner2 );
	
	LONG bActive;
	pVideoWindow->get_Visible( &bActive );
	
	if ( (LONG)hWndOwner == hWndOwner2 && bVisible == bActive ) return S_OK;
	
	pVideoWindow->put_Owner( (OAHWND)hWndOwner );
	pVideoWindow->put_WindowStyle( WS_CHILD | WS_CLIPSIBLINGS );
	pVideoWindow->put_MessageDrain((OAHWND)hWndOwner);
	
	pVideoWindow->put_Visible( bVisible );
	
	return pVideoWindow->SetWindowPosition( rect.left, rect.top,
		rect.right - rect.left, rect.bottom - rect.top );
}

/////////////////////////////////////////////////////////////////////////////
// CGraphRender render stream

HRESULT CGraphRender::RenderStream(WORD nPayloadMask, LPOLESTR pDisplayName, DWORD nReserved)
{
	ASSERT( RenderPayload(0) & nPayloadMask );
	
	if ( RenderPayload(nPayloadMask) & PayloadMask_Audio )
	{
		if ( pDisplayName ) InstallCapture( pDisplayName, L"Audio Capture" );
		else InstallCapture(CLSID_AudioInputDeviceCategory, L"Audio Capture");
		
		WAVEFORMATEX wfx = {0}; DeriveCompressFormat( wfx );
		RenderStream( pDisplayName, (WAVEFORMATEX&)wfx, nReserved );
	}
	if ( RenderPayload(nPayloadMask) & PayloadMask_Video )
	{
		if ( pDisplayName ) InstallCapture( pDisplayName, L"Video Capture" );
		else InstallCapture(CLSID_VideoInputDeviceCategory, L"Video Capture");
		
		VIDEOINFOHEADER vih = {0}; DeriveCompressFormat( vih );
		RenderStream( pDisplayName, (VIDEOINFOHEADER&)vih, nReserved );
	}
	
	return S_OK;
}

HRESULT CGraphRender::RenderPreview(WORD nPayloadMask, LPOLESTR pDisplayName)
{
	HRESULT hr = S_OK;
	ASSERT( PreviewPayload(0) & nPayloadMask );
	
	if ( PreviewPayload(nPayloadMask) & PayloadMask_Audio )
	{
		//WAVEFORMATEX wfx = {0}; DeriveCompressFormat( wfx );
		RenderPreview( pDisplayName, (WAVEFORMATEX*)0 );
	}
	if ( PreviewPayload(nPayloadMask) & PayloadMask_Video )
	{
		//VIDEOINFOHEADER vih = {0}; DeriveCompressFormat( vih );
		RenderPreview( pDisplayName, (VIDEOINFOHEADER*)0 );
	}
	
	return S_OK;
}

HRESULT CGraphRender::RenderPreview(WORD nPayloadMask, LPOLESTR pDisplayName, DWORD nReserved)
{
	ASSERT( PreviewPayload(0) & nPayloadMask );
	
	if ( PreviewPayload(nPayloadMask) & PayloadMask_Audio )
	{
		WAVEFORMATEX wfx = {0}; DeriveCompressFormat( wfx );
		
		WAVEFORMATEX wfxSrc = {0}; DeriveCompressFormat( wfxSrc, &wfx );
		RenderPreview( pDisplayName, (WAVEFORMATEX&)wfxSrc, nReserved );
	}
	if ( PreviewPayload(nPayloadMask) & PayloadMask_Video )
	{
		VIDEOINFOHEADER vih = {0}; DeriveCompressFormat( vih );
		
		VIDEOINFOHEADER vihSrc = {0}; DeriveCompressFormat( vihSrc, &vih );
		RenderPreview( NULL, (VIDEOINFOHEADER&)vihSrc, nReserved );
	}
	
	return Run();
}

/////////////////////////////////////////////////////////////////////////////
// CGraphRender record file

HRESULT CGraphRender::RecordFile(LPCWSTR lpszFile, WORD nPayloadMask)
{
	HRESULT hr = S_OK;
	this->Stop();
	
	// File
	
	CComPtr<IFileSinkFilter> pFileSink;
	hr = m_pCapGraphBuilder->SetOutputFileName( &MEDIASUBTYPE_Avi, lpszFile, &m_pMultiplexer, &pFileSink );
	
	pFileSink->QueryInterface( IID_IBaseFilter, reinterpret_cast<void **>( &m_pFileSink ) );
	
	// Audio Capture
	
	CComPtr<IBaseFilter> pCapture;
	if ( SUCCEEDED(hr) && (RecordPayload(nPayloadMask) & PayloadMask_Audio) )
	{
		hr = m_pGraphBuilder->FindFilterByName( L"Audio Receiver", &pCapture );
	}
	
	CComPtr<IPin> pPinOut;
	if ( SUCCEEDED(hr) && pCapture && SUCCEEDED(
		hr = pCapture->FindPin( L"Capture", &pPinOut )) )
	{
		hr = m_pGraphBuilder->Render( pPinOut );
	}
	
	// Video Capture
	
	pCapture.Release();
	if ( SUCCEEDED(hr) && (RecordPayload(nPayloadMask) & PayloadMask_Video) )
	{
		hr = m_pGraphBuilder->FindFilterByName( L"Video Receiver", &pCapture );
	}
	
	pPinOut.Release();
	if ( SUCCEEDED(hr) && pCapture && SUCCEEDED(
		hr = pCapture->FindPin( L"Capture", &pPinOut )) )
	{
		hr = m_pGraphBuilder->Render( pPinOut );
	}
	
	// Config Stream
	
	CComPtr<IConfigAviMux> pConfigAviMux;
	if ( SUCCEEDED(hr) ) hr =
	m_pMultiplexer->QueryInterface( IID_IConfigAviMux, (void **)&pConfigAviMux);
	
	if ( SUCCEEDED(hr) )
	{
		hr=pConfigAviMux->SetOutputCompatibilityIndex( TRUE );
		hr=pConfigAviMux->SetMasterStream( 0 );
	}
	
	DbgLog((LOG_TRACE, 0, TEXT("Record[%s]: %ls"), GetNamed(), lpszFile));
	return Run();
}

HRESULT CGraphRender::StopRecord()
{
	HRESULT hr = S_OK;
	this->Stop();
	
	if ( m_pMultiplexer )
	{
		RemoveFilter( m_pMultiplexer );
		m_pMultiplexer = NULL;
	}
	if ( m_pFileSink )
	{
		RemoveFilter( m_pFileSink );
		m_pFileSink = NULL;
	}
	DbgLog((LOG_TRACE, 0, TEXT("StopRecord[%s]."), GetNamed()));
	
	return Run();
}

void DeriveCompressFormat(WAVEFORMATEX& wfx)
{
	wfx.wFormatTag		= WAVE_FORMAT_PCM;
	wfx.nSamplesPerSec	= 8000; wfx.wBitsPerSample	= 16; wfx.nChannels	= 1;
	wfx.nBlockAlign		= wfx.nChannels * ( wfx.wBitsPerSample / 8 );
	wfx.nAvgBytesPerSec	= wfx.nBlockAlign * wfx.nSamplesPerSec;
}

void DeriveCompressFormat(VIDEOINFOHEADER& vih)
{
	vih.bmiHeader.biCompression	= BI_RGB;
	vih.bmiHeader.biBitCount	= 24;
	vih.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
	vih.bmiHeader.biWidth		= QCIF_WIDTH;
	vih.bmiHeader.biHeight		= QCIF_HEIGHT;
	vih.bmiHeader.biPlanes		= 1;
	vih.bmiHeader.biSizeImage	= GetBitmapSize( &vih.bmiHeader );
	vih.bmiHeader.biClrImportant= 0;
	vih.AvgTimePerFrame			= UNITS / 30;
}

void DeriveCompressFormat(WAVEFORMATEX& wfxDest, WAVEFORMATEX* pwfxSrc)
{
	wfxDest.wFormatTag = WAVE_FORMAT_ALAW;
	wfxDest.wBitsPerSample = 8;
	wfxDest.cbSize = 0;
	
	wfxDest.nChannels = pwfxSrc->nChannels;
	wfxDest.nSamplesPerSec = pwfxSrc->nSamplesPerSec;
	
	wfxDest.nBlockAlign = wfxDest.nChannels * wfxDest.wBitsPerSample / 8;
	wfxDest.nAvgBytesPerSec = wfxDest.nBlockAlign * wfxDest.nSamplesPerSec;
}

void DeriveCompressFormat(VIDEOINFOHEADER& vihDest, VIDEOINFOHEADER* pvihSrc)
{
	vihDest.bmiHeader.biCompression	= MAKEFOURCC('H','2','6','3');
	
	vihDest.bmiHeader.biBitCount	= pvihSrc->bmiHeader.biBitCount;
	vihDest.bmiHeader.biSize		= pvihSrc->bmiHeader.biSize;
	vihDest.bmiHeader.biWidth		= pvihSrc->bmiHeader.biWidth;
	vihDest.bmiHeader.biHeight		= pvihSrc->bmiHeader.biHeight;
	vihDest.bmiHeader.biPlanes		= pvihSrc->bmiHeader.biPlanes;
	vihDest.bmiHeader.biSizeImage	= GetBitmapSize( &vihDest.bmiHeader );
	vihDest.bmiHeader.biClrImportant= pvihSrc->bmiHeader.biClrImportant;
	
	vihDest.AvgTimePerFrame			= pvihSrc->AvgTimePerFrame;
	vihDest.rcSource				= pvihSrc->rcSource;
	vihDest.rcTarget				= pvihSrc->rcTarget;
	vihDest.dwBitRate				= pvihSrc->dwBitRate;
}
