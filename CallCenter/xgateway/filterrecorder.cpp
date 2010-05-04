//
// FilterRecorderer.cpp
//

#include "stdafx.h"
#include "Xgateway.h"
#include "FilterRecorder.h"
#include "FilterRtpReceiver.h"

#include "AudioRenderer.h"
#include "VideoRenderer.h"

#include "Buffer.h"
#include <mmreg.h>

//////////////////////////////////////////////////////////////////////
// CFilterRecorder construction

CFilterRecorder::CFilterRecorder() : CFilterGraph( TRUE )
{
	m_pAudioRender	= NULL;
	m_pVideoRender	= NULL;
}

CFilterRecorder::~CFilterRecorder()
{
	if ( m_pAudioRender ) delete m_pAudioRender;
	if ( m_pVideoRender ) delete m_pVideoRender;
	
	if ( m_pBuffer ) delete m_pBuffer;
}

//////////////////////////////////////////////////////////////////////
// CFilterRecorder connection

HRESULT CFilterRecorder::RecordFile(LPCWSTR lpszFile)
{
	HRESULT hr = S_OK;
	
	// File
	
	CComPtr<IBaseFilter> pMux;
	hr = m_pCapGraphBuilder->SetOutputFileName( &MEDIASUBTYPE_Avi, lpszFile, &pMux, NULL );
	
	// RenderStream
	
	if ( SUCCEEDED(hr) ) hr = RenderAudio( NULL );
	if ( SUCCEEDED(hr) ) hr = RenderVideo( NULL );
	
	CComPtr<IConfigAviMux> pConfigAviMux;
	if ( SUCCEEDED(hr) ) hr =
	pMux->QueryInterface( IID_IConfigAviMux, (void **)&pConfigAviMux);
	
	if ( SUCCEEDED(hr) )
	{
		pConfigAviMux->SetOutputCompatibilityIndex( TRUE );
		pConfigAviMux->SetMasterStream( 0 );
	}
	
	return hr;
}

HRESULT CFilterRecorder::ConnectTo(const RECT& rect, HWND hWnd)
{
	m_pAudioRender	= new CAudioRenderer;
	m_pAudioRender->Connnect();
	m_pAudioRender->StartPreview();
	
	m_pVideoRender	= new CVideoRenderer;
	m_pVideoRender->Connnect();
	m_pVideoRender->StartPreview( rect, hWnd );
	
	m_pBuffer		= new CBuffer;
	m_nTimestamp	= 0;
	
	return TRUE;
}

HRESULT CFilterRecorder::RenderAudio(IBaseFilter** ppCapture)
{
	Stop();
	HRESULT hr = S_OK;
	
	// Capture
	
	CRtpReceiverFilter* pReceiver = new CRtpReceiverFilter(NULL, &hr);
	
	WAVEFORMATEX wfx;
	ZeroMemory( &wfx, sizeof( wfx ) );
	wfx.wFormatTag = WAVE_FORMAT_ALAW;
	pReceiver->SetupMediaType( &wfx );
	
	CComPtr<IBaseFilter> pCapture;
	if ( ! ppCapture ) ppCapture = &pCapture;
	
	pReceiver->QueryInterface( IID_IBaseFilter, (void**)ppCapture );
	InstallFilter( *ppCapture, L"Audio Receiver" );
	
	// RenderStream
	
	CComPtr<IPin> pPinOut;
	m_pCapGraphBuilder->FindPin( *ppCapture, PINDIR_OUTPUT, NULL, NULL, TRUE, 0, &pPinOut );
	
	hr = m_pGraphBuilder->Render( pPinOut );
	hr = Run();
	return hr;
}

HRESULT CFilterRecorder::RenderVideo(IBaseFilter** ppCapture)
{
	Stop();
	HRESULT hr = S_OK;
	
	// Capture
	
	CRtpReceiverFilter* pReceiver = new CRtpReceiverFilter(NULL, &hr);
	
	VIDEOINFOHEADER vih;
	ZeroMemory( &vih, sizeof( vih ) );
	vih.bmiHeader.biCompression	= MAKEFOURCC('H','2','6','3');
	pReceiver->SetupMediaType( &vih );
	
	CComPtr<IBaseFilter> pCapture;
	if ( ! ppCapture ) ppCapture = &pCapture;
	
	pReceiver->QueryInterface( IID_IBaseFilter, (void**)ppCapture );
	InstallFilter( *ppCapture, L"Video Receiver" );
	
	// RenderStream
	
	CComPtr<IPin> pPinOut;
	m_pCapGraphBuilder->FindPin( *ppCapture, PINDIR_OUTPUT, NULL, NULL, TRUE, 0, &pPinOut );
	
	hr = m_pGraphBuilder->Render( pPinOut );
	hr = Run();
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CFilterRecorder deliver buffer

HRESULT CFilterRecorder::DeliverBuffer(BYTE nPayload, CBuffer* pBuffer)
{
	CComPtr<IBaseFilter> pCapture;
	
	if ( nPayload == Payload_Audio && m_pAudioRender )
	{
		m_pAudioRender->DeliverBuffer( pBuffer->m_pBuffer, pBuffer->m_nLength );
	}
	if ( nPayload == Payload_Video && m_pVideoRender )
	{
		m_pVideoRender->DeliverBuffer( m_pBuffer->m_pBuffer, m_pBuffer->m_nLength );
	}
	
	if ( nPayload == Payload_Audio && SUCCEEDED(
		m_pGraphBuilder->FindFilterByName( L"Audio Receiver", &pCapture )) )
	{
		CRtpReceiverFilter* pReceiver = (CRtpReceiverFilter *) pCapture.p;
		return pReceiver->DeliverBuffer( pBuffer->m_pBuffer, pBuffer->m_nLength );
	}
	if ( nPayload == Payload_Video && SUCCEEDED(
		m_pGraphBuilder->FindFilterByName( L"Video Receiver", &pCapture )) )
	{
		CRtpReceiverFilter* pReceiver = (CRtpReceiverFilter *) pCapture.p;
		return pReceiver->DeliverBuffer( pBuffer->m_pBuffer, pBuffer->m_nLength );
	}
	
	return S_OK;
}
