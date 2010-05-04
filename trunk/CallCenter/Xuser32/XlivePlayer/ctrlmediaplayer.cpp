//
// CtrlMediaPlayer.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "CtrlMediaPlayer.h"
#include "Datagrams.h"
#include "Network.h"

#include <initguid.h>
#include <qnetwork.h>

// {6B6D0800-9ADA-11d0-A520-00A0D10129C0}
DEFINE_GUID(CLSID_NetShowSource, 
0x6b6d0800, 0x9ada, 0x11d0, 0xa5, 0x20, 0x0, 0xa0, 0xd1,0x1, 0x29, 0xc0);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMediaPlayer construction

CMediaPlayer::CMediaPlayer() : CGraphRender( Network.m_xUserId )
{
}

CMediaPlayer::~CMediaPlayer()
{
}

/////////////////////////////////////////////////////////////////////////////
// CMediaPlayer message handlers

HRESULT CMediaPlayer::RenderUrl(LPCWSTR lpszUrl, IBaseFilter** ppCapture)
{
	HRESULT hr = S_OK;
#if 0
	CComPtr<IAMNetShowConfig> pNetSource;
	hr = CoCreateInstance( CLSID_NetShowSource, NULL, CLSCTX_INPROC, IID_IAMNetShowConfig, (void **)&pNetSource );  
	
	CComPtr<IBaseFilter> pFilter;
	if ( SUCCEEDED(hr) ) hr =
	pNetSource->QueryInterface( IID_IBaseFilter, (void**)&pFilter );
	
	if ( SUCCEEDED(hr) ) hr = InstallFilter( pFilter, lpszUrl );
	if ( ppCapture ) * ppCapture = pFilter;
	
	CComPtr<IFileSourceFilter> pFileSink;
	if ( SUCCEEDED(hr) ) hr =
	pNetSource->QueryInterface( IID_IFileSourceFilter, (void**)&pFileSink );
#else
	CComPtr<IBaseFilter> pFilter;
	hr = CreateFilter( CLSID_WMAsfReader, lpszUrl, &pFilter );
	
	CComPtr<IFileSourceFilter> pFileSink;
	if ( SUCCEEDED(hr) ) hr =
	pFilter->QueryInterface( IID_IFileSourceFilter, (void**)&pFileSink );
#endif
	
	if ( SUCCEEDED(hr) ) hr =
	pFileSink->Load( lpszUrl, NULL );
	
	for ( ; SUCCEEDED(hr) ; )
	{
		CComPtr<IPin> pPinOut;
		hr = m_pCapGraphBuilder->FindPin( pFilter, PINDIR_OUTPUT, NULL, NULL, TRUE, 0, &pPinOut );
		
		if ( FAILED(hr) ) return S_OK;
		
		if ( SUCCEEDED(hr) ) hr =
		m_pGraphBuilder->Render( pPinOut );
	}
	
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CMediaPlayer render stream

HRESULT CMediaPlayer::RenderStream(IBaseFilter* pFilter)
{
	HRESULT hr = S_OK;
	
	CComPtr<IPin> pPinIn;
	hr = m_pCapGraphBuilder->FindPin( pFilter, PINDIR_INPUT, NULL, NULL, FALSE, 0, &pPinIn );
	
	AM_MEDIA_TYPE mt;
	if ( SUCCEEDED(hr) ) hr =
	pPinIn->ConnectionMediaType( &mt );
	
	if ( FAILED(hr) ) return hr;
	
	if ( mt.formattype == FORMAT_WaveFormatEx )
	{
		CDatagrams* pDatagram = Datagrams.SeekNext( Payload_Alaw );
		
		WAVEFORMATEX wfx = {0}; DeriveCompressFormat( wfx );
		//WAVEFORMATEX * pwfx = (WAVEFORMATEX *) mt.pbFormat;
		CGraphRender::RenderStream( pFilter, &wfx, NULL, (DWORD)pDatagram );
	}
#if 0
	else
	if ( mt.formattype == FORMAT_VideoInfo )
	{
		CDatagrams* pDatagram = Datagrams.SeekNext( Payload_H263 );
		
		VIDEOINFOHEADER vih = {0}; DeriveCompressFormat( vih );
		CGraphRender::RenderStream( pFilter, &vih, NULL, (DWORD)pDatagram );
	}
#endif
	
	return hr;
}

HRESULT CMediaPlayer::BranchStream(LPCWSTR lpszName, IBaseFilter** ppFilter)
{
	HRESULT hr = S_OK;
	ASSERT( ppFilter != NULL );
	
	// Renderer
	
	CComPtr<IBaseFilter> pRenderer;
	hr = m_pGraphBuilder->FindFilterByName( lpszName, &pRenderer );
	
	if ( FAILED(hr) ) return hr;
	
	CComPtr<IPin> pPinIn;
	hr = m_pCapGraphBuilder->FindPin( pRenderer, PINDIR_INPUT, NULL, NULL, FALSE, 0, &pPinIn );
	
	CComPtr<IPin> pPinOut;
	if ( SUCCEEDED(hr) ) hr = pPinIn->ConnectedTo( &pPinOut );
	
	if ( FAILED(hr) || FAILED(hr = pPinOut->Disconnect()) ) return hr;
	
	pPinIn->Disconnect();
	
	WCHAR wsz[256];
	swprintf(wsz, L"Inf Tee To %08x", (DWORD_PTR)(IUnknown*)pRenderer );
	
	CComPtr<IBaseFilter> pTee;
	CreateFilter( CLSID_InfTee, wsz, &pTee );
	if ( ppFilter ) * ppFilter = pTee;
	
	CComPtr<IPin> pPinIn2;
	m_pCapGraphBuilder->FindPin( pTee, PINDIR_INPUT, NULL, NULL, TRUE, 0, &pPinIn2 );
	hr = m_pGraphBuilder->Connect( pPinOut, pPinIn2 );
	
	pPinOut.Release();
	m_pCapGraphBuilder->FindPin( pTee, PINDIR_OUTPUT, NULL, NULL, TRUE, 0, &pPinOut );
	hr = m_pGraphBuilder->Connect( pPinOut, pPinIn );
	
	return RenderStream( pTee );
}

HRESULT CMediaPlayer::ResizeConfig(IBaseFilter* pCapture)
{
	HRESULT hr = S_OK;
	
	CComPtr<IAMStreamConfig> pConfig;
	hr = m_pCapGraphBuilder->FindInterface( &PIN_CATEGORY_CAPTURE, &MEDIATYPE_Interleaved,
		pCapture, IID_IAMStreamConfig, (void **)&pConfig );
	
	if ( FAILED(hr) ) hr =
	m_pCapGraphBuilder->FindInterface( &PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
		pCapture, IID_IAMStreamConfig, (void **)&pConfig );
	
	if ( FAILED(hr) ) return hr;
	
	AM_MEDIA_TYPE* pmt;
	pConfig->GetFormat( &pmt );
	
	VIDEOINFOHEADER* pvih = (VIDEOINFOHEADER *) pmt->pbFormat;
	
	pvih->bmiHeader.biWidth = QCIF_WIDTH;
	pvih->bmiHeader.biHeight = QCIF_HEIGHT;
	
	return pConfig->SetFormat( pmt );
}
