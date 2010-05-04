//
// VideoBuilder.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "Settings.h"
#include "VideoBuilder.h"
#include "FilterRtpSender.h"
#include "FilterRtpReceiver.h"

#include <initguid.h>
#include <G711uids.h>

// {C9076CE2-FB56-11CF-906C-00AA00A59F69}
DEFINE_GUID(CLSID_H263Encoder, 
0xc9076ce2, 0xfb56, 0x11cf, 0x90, 0x6c, 0x0, 0xaa, 0x0, 0xa5, 0x9f, 0x69);

// {33363248-0000-0010-8000-00AA00389B71}
DEFINE_GUID(CLSID_H263Decoder, 
0x33363248, 0x0, 0x10, 0x80, 0x0, 0x0, 0xaa, 0x0, 0x38, 0x9b, 0x71);

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CVideoBuilder construction

CVideoBuilder::CVideoBuilder()
{
}

CVideoBuilder::~CVideoBuilder()
{
}

//////////////////////////////////////////////////////////////////////
// CVideoBuilder build graph

BOOL CVideoBuilder::BuildGraph()
{
	HRESULT hr = S_OK;
	BOOL bResult = FALSE;
	
	// Sender
	
	if ( bResult ) bResult =
	InstallFilter( new CRtpSenderFilter(NULL, &hr), L"Rtp Sender" );
	
	CComPtr<IBaseFilter> pSender;
	m_pGraphBuilder->FindFilterByName( L"Rtp Sender", &pSender );
	
	if ( bResult ) bResult |= AudioCapture( Settings.Audio.DevInput, pSender );
	
	return bResult && Run();
}

BOOL CVideoBuilder::AudioCapture(LPCTSTR lpszCapture, IBaseFilter* pRenderer)
{
	BOOL bResult = TRUE;
	
	// Capture
	
	CComPtr<IBaseFilter> pCapture;
	if ( bResult ) bResult =
	CreateFilter( (CComBSTR)lpszCapture, L"Audio Capture", &pCapture );
	
	// Compressor
	
	CComPtr<IBaseFilter> pCompressor;
	if ( bResult ) //bResult =
	CreateFilter( CLSID_G711Codec, L"G.711 Encoder", &pCompressor );
	
	// RenderStream
	
	if ( bResult ) bResult = SUCCEEDED(
	m_pCapGraphBuilder->RenderStream( &PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Audio, pCapture, pCompressor, pRenderer ) );
	
	return bResult && Run();
}

BOOL CVideoBuilder::StreamCapture(LPCTSTR lpszAudio, LPCTSTR lpszVideo)
{
	HRESULT hr = S_OK;
	BOOL bResult = TRUE;
	
	// Avi Mux
	
	CComPtr<IBaseFilter> pMuxer;
	if ( bResult ) bResult =
	CreateFilter( CLSID_AviDest, L"Avi Mux", &pMuxer );
	
	// Capture
	
	if ( lpszAudio && bResult )
	bResult |= InfluxAudio( lpszAudio, pMuxer );
	if ( lpszVideo && bResult )
	bResult |= InfluxVideo( lpszVideo, pMuxer );
	
	// Renderer
	
	if ( bResult ) bResult =
	InstallFilter( new CRtpSenderFilter(NULL, &hr), L"Rtp Sender" );
	
	CComPtr<IBaseFilter> pRenderer;
	m_pGraphBuilder->FindFilterByName( L"Rtp Sender", &pRenderer );
	
	return ConnectFilter( pMuxer, pRenderer ) && Run();
}

BOOL CVideoBuilder::StreamRender(LPCTSTR lpszAudio, LPCTSTR lpszVideo)
{
	HRESULT hr = S_OK;
	BOOL bResult = TRUE;
	
	// Capture
	
	if ( bResult ) bResult =
	InstallFilter( new CRtpReceiverFilter(NULL, &hr), L"Rtp Receiver" );
	
	CComPtr<IBaseFilter> pCapture;
	m_pGraphBuilder->FindFilterByName( L"Rtp Receiver", &pCapture );
	
	// Avi Demux
	
	CComPtr<IBaseFilter> pDemuxer;
	if ( bResult ) bResult =
	CreateFilter( CLSID_AviSplitter, L"Avi Demux", &pDemuxer );
	
	if ( ! ConnectFilter( pCapture, pDemuxer ) ) return FALSE;
	
	// Renderer
	
	if ( lpszAudio && bResult )
	bResult |= DemuxAudio( lpszAudio, pDemuxer );
	if ( lpszVideo && bResult )
	bResult |= DemuxVideo( lpszVideo, pDemuxer );
	
	return bResult && Run();
}

//////////////////////////////////////////////////////////////////////
// CVideoBuilder mux stream

BOOL CVideoBuilder::InfluxAudio(LPCTSTR lpszCapture, IBaseFilter* pMuxer)
{
	HRESULT hr = S_OK;
	BOOL bResult = TRUE;
	
	// Capture
	
	CComPtr<IBaseFilter> pCapture;
	if ( bResult ) bResult =
	CreateFilter( (CComBSTR)lpszCapture, L"Audio Capture", &pCapture );
	
	// Compressor
	
	CComPtr<IBaseFilter> pCompressor;
	if ( bResult ) //bResult =
	CreateFilter( CLSID_G711Codec, L"G.711 Encoder", &pCompressor );
	
	// RenderStream
	
	if ( bResult ) bResult = SUCCEEDED(
	m_pCapGraphBuilder->RenderStream( &PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Audio, pCapture, pCompressor, pMuxer ) );
	
	return bResult && Run();
}

BOOL CVideoBuilder::InfluxVideo(LPCTSTR lpszCapture, IBaseFilter* pMuxer)
{
	HRESULT hr = S_OK;
	BOOL bResult = TRUE;
	
	// Capture
	
	CComPtr<IBaseFilter> pCapture;
	if ( bResult ) bResult =
	CreateFilter( (CComBSTR)lpszCapture, L"Video Capture", &pCapture );
	
	// Compressor
	
	CComPtr<IBaseFilter> pCompressor;
	if ( bResult ) //bResult =
	CreateFilter( CLSID_H263Encoder, L"H.263 Encoder", &pCompressor );
	
	// RenderStream
	
	if ( bResult ) bResult = SUCCEEDED(
	m_pCapGraphBuilder->RenderStream( &PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Video, pCapture, pCompressor, pMuxer ) );
	
	return bResult && Run();
}

//////////////////////////////////////////////////////////////////////
// CVideoBuilder demux stream

BOOL CVideoBuilder::DemuxAudio(LPCTSTR lpszRender, IBaseFilter* pDemuxer)
{
	HRESULT hr = S_OK;
	BOOL bResult = TRUE;
	
	// Render
	
	CComPtr<IBaseFilter> pRender;
	if ( bResult ) bResult =
	CreateFilter( (CComBSTR)lpszRender, L"Audio Renderer", &pRender );
	
	// Decompressor
	
	CComPtr<IBaseFilter> pDecompor;
	if ( bResult ) //bResult =
	CreateFilter( CLSID_G711Codec, L"G.711 Decoder", &pDecompor );
	
	// RenderStream
	
	if ( bResult ) bResult = SUCCEEDED(
	m_pCapGraphBuilder->RenderStream( &PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Audio, pDemuxer, pDecompor, pRender ) );
	
	return bResult && Run();
}

BOOL CVideoBuilder::DemuxVideo(LPCTSTR lpszRender, IBaseFilter* pDemuxer)
{
	HRESULT hr = S_OK;
	BOOL bResult = TRUE;
	
	// Render
	
	CComPtr<IBaseFilter> pRender;
	if ( bResult ) bResult =
	CreateFilter( (CComBSTR)lpszRender, L"Video Renderer", &pRender );
	
	// Decompressor
	
	CComPtr<IBaseFilter> pDecompor;
	if ( bResult ) //bResult =
	CreateFilter( CLSID_G711Codec, L"H.263 Decoder", &pDecompor );
	
	// RenderStream
	
	if ( bResult ) bResult = SUCCEEDED(
	m_pCapGraphBuilder->RenderStream( &PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Video, pRender, pDecompor, pDemuxer ) );
	
	return bResult && Run();
}
