//
// RecordGraph.cpp
//

#include "stdafx.h"
#include "RecordGraph.h"
#include "FilterRtpReceiver.h"
#include "Buffer.h"

#include <initguid.h>
#include <mmreg.h>

//////////////////////////////////////////////////////////////////////
// CGraphRecord construction

CGraphRecord::CGraphRecord(BOOL bInit) : CFilterGraph( bInit )
{
}

CGraphRecord::~CGraphRecord()
{
}

//////////////////////////////////////////////////////////////////////
// CGraphRecord connection

HRESULT CGraphRecord::RecordFile(LPCWSTR lpszFile)
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
		pConfigAviMux->SetMasterStream( 1 );
	}
	
	return hr;
}

HRESULT CGraphRecord::RenderAudio(IBaseFilter** ppCapture)
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

HRESULT CGraphRecord::RenderVideo(IBaseFilter** ppCapture)
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
// CGraphRecord deliver buffer

HRESULT CGraphRecord::DeliverBuffer(BYTE nPayload, CBuffer* pBuffer)
{
	CComPtr<IBaseFilter> pCapture;
	
	// Recorder
	
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

HRESULT CGraphRecord::DeliverBuffer(BYTE nPayload, BYTE* pByte, DWORD nLength, DWORD nTimestamp)
{
	CComPtr<IBaseFilter> pCapture;
	
	// Recorder
	
	if ( nPayload == Payload_Audio && SUCCEEDED(
		m_pGraphBuilder->FindFilterByName( L"Audio Receiver", &pCapture )) )
	{
		CRtpReceiverFilter* pReceiver = (CRtpReceiverFilter *) pCapture.p;
		return pReceiver->DeliverBuffer( pByte, nLength, nTimestamp );
	}
	if ( nPayload == Payload_Video && SUCCEEDED(
		m_pGraphBuilder->FindFilterByName( L"Video Receiver", &pCapture )) )
	{
		CRtpReceiverFilter* pReceiver = (CRtpReceiverFilter *) pCapture.p;
		return pReceiver->DeliverBuffer( pByte, nLength, nTimestamp );
	}
	
	return S_OK;
}
