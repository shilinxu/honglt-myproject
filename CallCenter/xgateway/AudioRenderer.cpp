//
// AudioRenderer.cpp
//

#include "stdafx.h"
#include "AudioRenderer.h"
#include "FilterRtpReceiver.h"

#include <initguid.h>
#include <G711uids.h>
#include <mmreg.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CAudioRenderer construction

CAudioRenderer::CAudioRenderer() : CFilterGraph( TRUE )
{
}

CAudioRenderer::~CAudioRenderer()
{
}

//////////////////////////////////////////////////////////////////////
// CAudioRenderer connection

HRESULT CAudioRenderer::Connnect()
{
	HRESULT hr = S_OK;
	
	// Capture
	
	CRtpReceiverFilter* pReceiver = new CRtpReceiverFilter(NULL, &hr);
	
	WAVEFORMATEX wfx;
	ZeroMemory( &wfx, sizeof( wfx ) );
	wfx.wFormatTag = WAVE_FORMAT_ALAW;
	pReceiver->SetupMediaType( &wfx );
	
	CComPtr<IBaseFilter> pCapture;
	pReceiver->QueryInterface( IID_IBaseFilter, (void**)&pCapture );
	InstallFilter( pCapture, L"Audio Receiver" );
	
	// RenderStream
	
	CComPtr<IBaseFilter> pSmartTee;
	CreateFilter( CLSID_SmartTee, L"Smart Tee", &pSmartTee );
	
	return ConnectFilter( pCapture, pSmartTee );
}

/////////////////////////////////////////////////////////////////////////////
// CAudioRenderer preview

HRESULT CAudioRenderer::StartPreview()
{
	StopPreview();
	
	CComPtr<IBaseFilter> pSmartTee;
	m_pGraphBuilder->FindFilterByName( L"Smart Tee", &pSmartTee );
	
	CComPtr<IPin> pPinOut;
	pSmartTee->FindPin( L"Preview", &pPinOut );
	
	HRESULT hr = m_pGraphBuilder->Render( pPinOut );
	Run(); return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CAudioRenderer deliver buffer

HRESULT CAudioRenderer::DeliverBuffer(BYTE* pByte, DWORD nLength)
{
	CComPtr<IBaseFilter> pCapture;
	m_pGraphBuilder->FindFilterByName( L"Audio Receiver", &pCapture );
	
	CRtpReceiverFilter* pReceiver = (CRtpReceiverFilter*)(IBaseFilter*)pCapture;
	
	return pReceiver->DeliverBuffer( pByte, nLength );
}
