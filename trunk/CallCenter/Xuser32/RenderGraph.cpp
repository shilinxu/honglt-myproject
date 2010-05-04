//
// RenderGraph.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "RenderGraph.h"
#include "FilterRtpReceiver.h"
#include "FilterRtpSender.h"

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

HRESULT CGraphRender::RenderStream(LPCWSTR lpszCapture, WAVEFORMATEX& pwfx)
{
	HRESULT hr = S_OK;
	
	// Capture
	
	CComPtr<IBaseFilter> pCapture;
	hr = CreateFilter( lpszCapture, L"Audio Capture", &pCapture );
	
	// Renderer
	
	CComPtr<IBaseFilter> pRenderer;
	
	if ( SUCCEEDED(hr) && pCapture )
	hr = InstallRender( new CRtpSenderFilter(Payload_Alaw, &pwfx, &hr), L"Audio Sender", &pRenderer );
	
	// RenderStream
	
	if ( SUCCEEDED(hr) ) hr =
	m_pCapGraphBuilder->RenderStream( &PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Audio, pCapture, NULL, pRenderer );
	
	Run();return hr;
}

HRESULT CGraphRender::RenderStream(LPCWSTR lpszCapture, VIDEOINFOHEADER& pvih)
{
	HRESULT hr = S_OK;
	
	// Capture
	
	CComPtr<IBaseFilter> pCapture;
	hr = CreateFilter( lpszCapture, L"Video Capture", &pCapture );
	
	// Renderer
	
	CComPtr<IBaseFilter> pRenderer;
	
	if ( SUCCEEDED(hr) && pCapture )
	hr = InstallRender( new CRtpSenderFilter(Payload_H263, &pvih, &hr), L"Video Sender", &pRenderer );
	
	// RenderStream
	
	if ( SUCCEEDED(hr) ) hr =
	m_pCapGraphBuilder->RenderStream( &PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Video, pCapture, NULL, pRenderer );
	
	Run();return hr;
}

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

HRESULT CGraphRender::RenderPreview(LPCWSTR lpszCapture, WAVEFORMATEX& pwfx, DWORD nReserved)
{
	HRESULT hr = S_OK;
	
	// Capture
	
	CComPtr<IBaseFilter> pCapture;
	hr = CreateFilter( lpszCapture, L"Audio Renderer", &pCapture );
	
	// Renderer
	
	CComPtr<IBaseFilter> pRenderer;
	
	if ( SUCCEEDED(hr) && pCapture && FAILED(
		hr = m_pGraphBuilder->FindFilterByName( L"Audio Receiver", &pRenderer )) )
	{
		hr = InstallRender( new CRtpReceiverFilter(Payload_Alaw, &pwfx, this), L"Audio Receiver", &pRenderer );
	}
	
	// RenderStream
	
	CComPtr<IPin> pPinOut;
	pRenderer->FindPin( L"Preview", &pPinOut );
	
	hr = m_pGraphBuilder->Render( pPinOut );
	return hr;
}

HRESULT CGraphRender::RenderPreview(LPCWSTR, VIDEOINFOHEADER& pvih, DWORD nReserved)
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
		hr = InstallRender( new CRtpReceiverFilter(Payload_H263, &pvih, this), L"Video Receiver", &pRenderer );
	}
	
	// RenderStream
	
	CComPtr<IPin> pPinOut;
	pRenderer->FindPin( L"Preview", &pPinOut );
	
	hr = m_pGraphBuilder->Render( pPinOut );
	return hr;
}

//////////////////////////////////////////////////////////////////////
// CGraphRender operations

HRESULT CGraphRender::InstallCapture(LPCWSTR lpszCapture, LPCWSTR lpszName)
{
	HRESULT hr = S_OK;
	CComPtr<IBaseFilter> pCapture;
	
	hr = m_pGraphBuilder->FindFilterByName( lpszName, &pCapture );
	if ( SUCCEEDED( hr ) ) return S_OK;
	
	// Capture
	
	hr = CreateFilter( lpszCapture, lpszName, &pCapture );
	return hr;
}

HRESULT CGraphRender::InstallCapture(REFCLSID clsidDevClass, LPCWSTR lpszName)
{
	Stop();
	HRESULT hr = S_OK;
	
	// Capture
	
	CComPtr<IBaseFilter> pCapture;
	GetNextDevice( clsidDevClass, &pCapture );
	
	return InstallFilter( pCapture, lpszName );
}

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
	StopPreview();
	
	HRESULT hr =
	PutIntoWindow( hWnd, rect, TRUE );
	
	Run(); return hr;
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
	if ( SUCCEEDED(hr) && (RecordPayload(nPayloadMask) & PayloadMask_Audio) &&
		FAILED(hr = m_pGraphBuilder->FindFilterByName( L"Audio Receiver", &pCapture )) )
	{
		hr = InstallRender( new CRtpReceiverFilter(Payload_Alaw, (WAVEFORMATEX*)0, this), L"Audio Receiver", &pCapture );
	}
	
	CComPtr<IPin> pPinOut;
	if ( SUCCEEDED(hr) && pCapture && SUCCEEDED(
		hr = pCapture->FindPin( L"Capture", &pPinOut )) )
	{
		hr = m_pGraphBuilder->Render( pPinOut );
	}
	
	// Video Capture
	
	pCapture.Release();
	if ( SUCCEEDED(hr) && (RecordPayload(nPayloadMask) & PayloadMask_Video) &&
		FAILED(hr = m_pGraphBuilder->FindFilterByName(L"Video Receiver", &pCapture)) )
	{
		hr = InstallRender( new CRtpReceiverFilter(Payload_H263, (VIDEOINFOHEADER*)0, this), L"Video Receiver", &pCapture );
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
	
	return strPath + "\\" + xNewFile;
}
