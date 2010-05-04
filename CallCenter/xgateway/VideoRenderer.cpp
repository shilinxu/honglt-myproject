//
// VideoRenderer.cpp
//

#include "stdafx.h"
#include "VideoRenderer.h"
#include "FilterRtpReceiver.h"

#include <initguid.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// {33363248-0000-0010-8000-00AA00389B71}
DEFINE_GUID(CLSID_H263Decoder, 
0x33363248, 0x0, 0x10, 0x80, 0x0, 0x0, 0xaa, 0x0, 0x38, 0x9b, 0x71);

//////////////////////////////////////////////////////////////////////
// CVideoRenderer construction

CVideoRenderer::CVideoRenderer() : CFilterGraph( TRUE )
{
}

CVideoRenderer::~CVideoRenderer()
{
}

//////////////////////////////////////////////////////////////////////
// CVideoRenderer connection

HRESULT CVideoRenderer::Connnect()
{
	HRESULT hr = S_OK;
	
	// Capture
	
	CRtpReceiverFilter* pReceiver = new CRtpReceiverFilter(NULL, &hr);
	
	VIDEOINFOHEADER vih;
	ZeroMemory( &vih, sizeof( vih ) );
	vih.bmiHeader.biCompression	= MAKEFOURCC('H','2','6','3');
	pReceiver->SetupMediaType( &vih );
	
	CComPtr<IBaseFilter> pCapture;
	pReceiver->QueryInterface( IID_IBaseFilter, (void**)&pCapture );
	InstallFilter( pCapture, L"Video Receiver" );
	
	// RenderStream
	
	CComPtr<IBaseFilter> pSmartTee;
	CreateFilter( CLSID_SmartTee, L"Smart Tee", &pSmartTee );
	
	return ConnectFilter( pCapture, pSmartTee );
}

/////////////////////////////////////////////////////////////////////////////
// CVideoRenderer preview

HRESULT CVideoRenderer::StartPreview(const RECT& rect, HWND hWnd)
{
	StopPreview();
	
	CComPtr<IBaseFilter> pSmartTee;
	m_pGraphBuilder->FindFilterByName( L"Smart Tee", &pSmartTee );
	
	// Compressor
	
	CComPtr<IBaseFilter> pCompressor;
	CreateFilter( CLSID_H263Decoder, L"H.263 Decoder", &pCompressor );
	
	CComPtr<IPin> pPinOut;
	pSmartTee->FindPin( L"Preview", &pPinOut );
	
	if ( SUCCEEDED( m_pGraphBuilder->Render( pPinOut ) ) )
	{
		PutIntoWindow( hWnd, rect, TRUE );
	}
	
	return Run();
}

void CVideoRenderer::StopPreview()
{
	CFilterGraph::StopPreview();
	
	CComPtr<IVideoWindow> pVideoWindow;
	m_pGraphBuilder->QueryInterface( IID_IVideoWindow, (void**)&pVideoWindow);
	
	pVideoWindow->put_Owner( (OAHWND)NULL );
	pVideoWindow->put_Visible( OAFALSE );
}

HRESULT CVideoRenderer::PutIntoWindow(HWND hWndOwner, const RECT rect, BOOL bVisible)
{
	CComPtr<IVideoWindow> pVideoWindow;
	m_pGraphBuilder->QueryInterface( IID_IVideoWindow, (void**)&pVideoWindow);
	
	pVideoWindow->SetWindowPosition( rect.left, rect.top,
		rect.right - rect.left, rect.bottom - rect.top );
	
	pVideoWindow->put_Owner( (OAHWND)hWndOwner );
	pVideoWindow->put_MessageDrain( (OAHWND)hWndOwner );
	pVideoWindow->put_WindowStyle( WS_CHILD | WS_CLIPSIBLINGS );
	pVideoWindow->put_Visible( bVisible );
	
	return Run();
}

/////////////////////////////////////////////////////////////////////////////
// CVideoRenderer operations

HRESULT CVideoRenderer::DeliverBuffer(BYTE* pByte, DWORD nLength)
{
	CComPtr<IBaseFilter> pCapture;
	m_pGraphBuilder->FindFilterByName( L"Video Receiver", &pCapture );
	
	CRtpReceiverFilter* pReceiver = (CRtpReceiverFilter*)(IBaseFilter*)pCapture;
	
	return pReceiver->DeliverBuffer( pByte, nLength );
}
