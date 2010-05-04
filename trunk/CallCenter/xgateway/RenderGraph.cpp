//
// RenderGraph.cpp
//

#include "stdafx.h"
#include "RenderGraph.h"
#include "FilterRtpReceiver.h"

//////////////////////////////////////////////////////////////////////
// CGraphRender construction

CGraphRender::CGraphRender(LPCTSTR xGraphNamed) : CFilterGraph(xGraphNamed)
{
}

CGraphRender::~CGraphRender()
{
}

//////////////////////////////////////////////////////////////////////
// CGraphRender connection

HRESULT CGraphRender::RenderPreview(LPCWSTR lpszCapture, WAVEFORMATEX& pwfx)
{
	HRESULT hr = S_OK;
	
	// Capture
	
	CComPtr<IBaseFilter> pCapture;
	hr = CreateFilter( lpszCapture, L"Audio Capture", &pCapture );
	
	// RenderStream
	
	if ( SUCCEEDED(hr) ) hr =
	m_pCapGraphBuilder->RenderStream( &PIN_CATEGORY_PREVIEW,
		&MEDIATYPE_Audio, pCapture, NULL, NULL );
	
	return hr;
}

HRESULT CGraphRender::RenderPreview(LPCWSTR lpszCapture, VIDEOINFOHEADER& pvih)
{
	HRESULT hr = S_OK;
	
	// Capture
	
	CComPtr<IBaseFilter> pCapture;
	hr = CreateFilter( lpszCapture, L"Video Capture", &pCapture );
	
	// RenderStream
	
	if ( SUCCEEDED(hr) ) hr =
	m_pCapGraphBuilder->RenderStream( &PIN_CATEGORY_PREVIEW,
		&MEDIATYPE_Video, pCapture, NULL, NULL );
	
	return hr;
}

HRESULT CGraphRender::RenderPreview(LPCWSTR, WAVEFORMATEX& pwfx, LPVOID lv)
{
	HRESULT hr = S_OK;
	
	// Capture
	
//	CComPtr<IBaseFilter> pCapture;
//	hr = CreateFilter( lpszCapture, L"Audio Renderer", &pCapture );
	
	// Renderer
	
	CComPtr<IBaseFilter> pRenderer;
	
	if ( SUCCEEDED(hr) /*&& pCapture*/ && FAILED(
		hr = m_pGraphBuilder->FindFilterByName( L"Audio Receiver", &pRenderer )) )
	{
		hr = InstallRender( new CRtpReceiverFilter(&pwfx, this, lv), L"Audio Receiver", &pRenderer );
	}
	
	// RenderStream
	
	CComPtr<IPin> pPinOut;
	pRenderer->FindPin( L"Preview", &pPinOut );
	
	hr = m_pGraphBuilder->Render( pPinOut );
	return hr;
}

HRESULT CGraphRender::RenderPreview(LPCWSTR, VIDEOINFOHEADER& pvih, LPVOID lv)
{
	HRESULT hr = S_OK;
	
	// Capture
	
//	CComPtr<IBaseFilter> pCapture;
//	hr = CreateFilter( lpszCapture, L"Video Renderer", &pCapture );
	
	// Renderer
	
	CComPtr<IBaseFilter> pRenderer;
	
	if ( SUCCEEDED(hr) /*&& pCapture*/ && FAILED(
		hr = m_pGraphBuilder->FindFilterByName( L"Video Receiver", &pRenderer )) )
	{
		hr = InstallRender( new CRtpReceiverFilter(&pvih, this, lv), L"Video Receiver", &pRenderer );
	}
	
	// RenderStream
	
	CComPtr<IPin> pPinOut;
	pRenderer->FindPin( L"Preview", &pPinOut );
	
	hr = m_pGraphBuilder->Render( pPinOut );
	return hr;
}

HRESULT CGraphRender::RenderPreview(WORD nPayloadMask, LPOLESTR pDisplayName, LPVOID lv)
{
	ASSERT( PreviewPayload(0) & nPayloadMask );
	
	if ( PreviewPayload(nPayloadMask) & PayloadMask_Audio )
	{
		WAVEFORMATEX wfx = {0}; DeriveCompressFormat( wfx );
		
		WAVEFORMATEX wfxSrc = {0}; DeriveCompressFormat( wfxSrc, &wfx );
		RenderPreview( pDisplayName, (WAVEFORMATEX&)wfxSrc, lv );
	}
	if ( PreviewPayload(nPayloadMask) & PayloadMask_Video )
	{
		VIDEOINFOHEADER vih = {0}; DeriveCompressFormat( vih );
		
		VIDEOINFOHEADER vihSrc = {0}; DeriveCompressFormat( vihSrc, &vih );
		RenderPreview( NULL, (VIDEOINFOHEADER&)vihSrc, lv );
	}
	
	return Run();
}

//////////////////////////////////////////////////////////////////////
// CGraphRender operations

HRESULT CGraphRender::InstallRender(CBaseFilter* pBaseFilter, LPCWSTR lpszName, IBaseFilter** ppFilter)
{
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

HRESULT CGraphRender::StartPreview()
{
	StopPreview();
	
	CComPtr<IBaseFilter> pCapture;
	m_pGraphBuilder->FindFilterByName( L"Audio Receiver", &pCapture );
	
	CComPtr<IPin> pPinOut;
	pCapture->FindPin( L"Preview", &pPinOut );
	
	m_pGraphBuilder->Render( pPinOut );
	
	return Run();
}

HRESULT CGraphRender::StartPreview(const RECT& rect, HWND hWnd)
{
	StopPreview();
	
	HRESULT hr =
	PutIntoWindow( hWnd, rect, TRUE );
	
	Run(); return hr;
}

void CGraphRender::StopPreview()
{
	this->Stop();
	
	CComPtr<IVideoWindow> pVideoWindow;
	m_pGraphBuilder->QueryInterface( IID_IVideoWindow, (void**)&pVideoWindow);
	
	pVideoWindow->put_Visible( OAFALSE );
	pVideoWindow->put_Owner( (OAHWND)NULL );
}

HRESULT CGraphRender::PutIntoWindow(HWND hWndOwner, const RECT rect, BOOL bVisible)
{
	CComPtr<IVideoWindow> pVideoWindow;
	m_pGraphBuilder->QueryInterface( IID_IVideoWindow, (void**)&pVideoWindow);
	
	pVideoWindow->SetWindowPosition( rect.left, rect.top,
		rect.right - rect.left, rect.bottom - rect.top );
	
	pVideoWindow->put_Owner( (OAHWND)hWndOwner );
	pVideoWindow->put_MessageDrain( (OAHWND)hWndOwner );
	pVideoWindow->put_WindowStyle( WS_CHILD | WS_CLIPSIBLINGS );
	pVideoWindow->put_Visible( bVisible );
	
//	return Run();
	return S_OK;
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
		pConfigAviMux->SetOutputCompatibilityIndex( TRUE );
		pConfigAviMux->SetMasterStream( 0 );
	}
	
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
/*
CString CGraphRender::GetRecordFile(LPCTSTR pszPath, LPCTSTR fMask) const
{
	CString xNewFile = fMask ? fMask : _T("<USER>\\<DATE><TIME>.AVI");
	
	if ( xNewFile.Find( "<USER>" ) >= 0 )
	{
		Replace( xNewFile, "<USER>", GetNamed() );
	}
	
	if ( xNewFile.Find( "<DATE>" ) >= 0 )
	{
		SYSTEMTIME pTime;
		GetLocalTime( &pTime );
		
		CString strValue;
		strValue.Format( "%04i年%02i月%02i日",
			pTime.wYear, pTime.wMonth, pTime.wDay );
		Replace( xNewFile, "<DATE>", strValue );
	}
	
	if ( xNewFile.Find( "<TIME>" ) >= 0 )
	{
		SYSTEMTIME pTime;
		GetLocalTime( &pTime );
		
		CString strValue;
		strValue.Format( "%02i时%02i分%02i秒%03i",
			pTime.wHour, pTime.wMinute, pTime.wSecond, pTime.wMilliseconds );
		Replace( xNewFile, "<TIME>", strValue );
	}
	
	CString strPath = pszPath ? pszPath : _T(".");
	
	for ( LPCTSTR xPath = xNewFile, xPathExt = NULL;
		  xPathExt = _tcsistr(xPath, "\\"); xPath = xPathExt + 1 )
	{
		CString strValue = xNewFile.Left(xPathExt - (LPCTSTR)xNewFile);
		CreateDirectory( strPath + "\\" + strValue, NULL );
	}
	
	return xNewFile;
}
*/
/////////////////////////////////////////////////////////////////////////////
// CGraphRender deliver buffer

HRESULT CGraphRender::DeliverBuffer(BYTE* pBuffer, DWORD nLength, BYTE nType, DWORD nTimestamp)
{
	CComPtr<IBaseFilter> pCapture;
	
	// Render
	
	if ( nType == Payload_Alaw && SUCCEEDED(
		m_pGraphBuilder->FindFilterByName( L"Audio Receiver", &pCapture )) )
	{
		ASSERT( FALSE );
	}
	
	if ( nType == Payload_H263 && SUCCEEDED(
		m_pGraphBuilder->FindFilterByName( L"Video Receiver", &pCapture )) )
	{
		CRtpReceiverFilter* pReceiver = (CRtpReceiverFilter *) pCapture.p;
		return CRtpReceiverFilter::RenderBuffer( pBuffer, nLength, nType, nTimestamp, pReceiver );
	}
	
	return S_OK;
}

HRESULT CGraphRender::RenderBuffer(BYTE* pBuffer, DWORD nLength, BYTE nType)
{
	CComPtr<IBaseFilter> pCapture;
	
	// Render
	
	if ( nType == Payload_Alaw && SUCCEEDED(
		m_pGraphBuilder->FindFilterByName( L"Audio Receiver", &pCapture )) )
	{
		CRtpReceiverFilter* pReceiver = (CRtpReceiverFilter *) pCapture.p;
		return pReceiver->RenderBuffer( pBuffer, nLength );
	}
	
	if ( nType == Payload_H263 && SUCCEEDED(
		m_pGraphBuilder->FindFilterByName( L"Video Receiver", &pCapture )) )
	{
		ASSERT( FALSE );
	}
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// derive format

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
	vihDest.bmiHeader.biCompression	= MAKEFOURCC('M','2','6','3');
	
	vihDest.bmiHeader.biBitCount	= pvihSrc->bmiHeader.biBitCount;
	vihDest.bmiHeader.biSize		= pvihSrc->bmiHeader.biSize;
	vihDest.bmiHeader.biWidth		= pvihSrc->bmiHeader.biWidth;
	vihDest.bmiHeader.biHeight		= pvihSrc->bmiHeader.biHeight;
	vihDest.bmiHeader.biPlanes		= pvihSrc->bmiHeader.biPlanes;
	vihDest.bmiHeader.biSizeImage	= 8192;//GetBitmapSize( &vihDest.bmiHeader );
	vihDest.bmiHeader.biClrImportant= pvihSrc->bmiHeader.biClrImportant;
	
	vihDest.AvgTimePerFrame			= pvihSrc->AvgTimePerFrame;
	vihDest.rcSource				= pvihSrc->rcSource;
	vihDest.rcTarget				= pvihSrc->rcTarget;
	vihDest.dwBitRate				= pvihSrc->dwBitRate;
}
