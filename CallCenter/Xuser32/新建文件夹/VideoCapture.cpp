//
// VideoCapture.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "VideoCapture.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CVideoCapture construction

CVideoCapture::CVideoCapture()
{
	m_pGraphBuilder		= NULL;
	m_pCapGraphBuilder	= NULL;
	
	CVideoCapture::Setup();
}

CVideoCapture::~CVideoCapture()
{
	CVideoCapture::Release();
}

//////////////////////////////////////////////////////////////////////
// CVideoCapture setup

void CVideoCapture::Setup()
{
	CVideoCapture::Release();
	
	m_pGraphBuilder.CoCreateInstance( CLSID_FilterGraph );
	
	m_pCapGraphBuilder.CoCreateInstance( CLSID_CaptureGraphBuilder2 );
	m_pCapGraphBuilder->SetFiltergraph( m_pGraphBuilder );
	
//#ifdef _DEBUG	
	AddToRot( m_pGraphBuilder, &m_dwRegister );
//#endif
}

void CVideoCapture::Release()
{
//#ifdef _DEBUG
	RemoveFromRot( m_dwRegister );
//#endif
	
	if ( m_pGraphBuilder )
	StopPreview();
	
	if ( m_pGraphBuilder )
	ClearFilters();
	
	m_pCapGraphBuilder.Release();
	m_pGraphBuilder.Release();
}

void CVideoCapture::ClearFilters()
{
	CComPtr<IEnumFilters> pEnumFilters;
	m_pGraphBuilder->EnumFilters( &pEnumFilters );
	
	CComPtr<IBaseFilter> pFilter;
	while ( pFilter.Release(), pEnumFilters->Next(1, &pFilter, NULL) == S_OK )
	{
		ReleaseFilter( pFilter );
		
		pEnumFilters->Reset();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CVideoCapture preview

BOOL CVideoCapture::StartPreview()
{
	Stop();
	
	BOOL bResult = TRUE;
	CComPtr<IBaseFilter> pCapture;
	
	if ( bResult ) bResult = SUCCEEDED( 
	m_pGraphBuilder->FindFilterByName( L"Audio Capture", &pCapture ) );
	
	if ( bResult ) bResult = SUCCEEDED( 
	m_pCapGraphBuilder->RenderStream( &PIN_CATEGORY_PREVIEW, 
		&MEDIATYPE_Audio, pCapture, NULL, NULL ) );
	
#ifdef _DEBUG
//	EnumFilters();
//	PrintGraph();
#endif
	
	return Run() && bResult;
}

BOOL CVideoCapture::StartPreview(const RECT& rect, HWND hWnd)
{
	StopPreview();
	
	BOOL bResult = TRUE;
	CComPtr<IBaseFilter> pCapture;
	
	if ( bResult ) bResult = SUCCEEDED( 
	m_pGraphBuilder->FindFilterByName( L"Video Capture", &pCapture ) );
	
	if ( bResult ) bResult = SUCCEEDED( 
	m_pCapGraphBuilder->RenderStream( &PIN_CATEGORY_PREVIEW, 
		&MEDIATYPE_Video, pCapture, NULL, NULL ) );
	
	if ( bResult ) bResult =
	PutIntoWindow( hWnd, rect, TRUE );
	
#ifdef _DEBUG
//	EnumFilters();
//	PrintGraph();
#endif
	
	return Run() && bResult;
}

void CVideoCapture::StopPreview()
{
	this->Stop();
	
	CComPtr<IVideoWindow> pVideoWindow;
	m_pGraphBuilder->QueryInterface( IID_IVideoWindow, (void**)&pVideoWindow);
	
	pVideoWindow->put_Owner( (OAHWND)NULL );
	pVideoWindow->put_Visible( OAFALSE );
}

/////////////////////////////////////////////////////////////////////////////
// CVideoCapture operations

BOOL CVideoCapture::CreateFilter(REFCLSID rclsid, LPCWSTR lpszName, IBaseFilter** ppFilter)
{
	CComPtr<IBaseFilter> pFilter;
	if ( pFilter.CoCreateInstance( rclsid ) != S_OK ) return FALSE;
	
	if ( ppFilter )
	pFilter->QueryInterface( IID_IBaseFilter, (void**)ppFilter);
	
	return InstallFilter( pFilter, lpszName );
}

BOOL CVideoCapture::CreateFilter(LPCWSTR lpszFilter, LPCWSTR lpszName, IBaseFilter** ppFilter)
{
	CComPtr<IBaseFilter> pFilter;
	if ( ! GetFilterByName(lpszFilter, &pFilter) ) return FALSE;
	
	if ( ppFilter )
	pFilter->QueryInterface( IID_IBaseFilter, (void**)ppFilter);
	
	return InstallFilter( *ppFilter, lpszName );
}

void CVideoCapture::ReleaseFilter(IBaseFilter* pFilter)
{
#ifdef _DEBUG
	FILTER_INFO info;
	if ( SUCCEEDED(pFilter->QueryFilterInfo( &info )) )
	{
		TRACE( "Delete: " + (CString)info.achName + '\n' );
	}
#endif
	
	pFilter->Stop(); 
	RemoveFilter( pFilter );
}

void CVideoCapture::ReleaseFilter(LPCWSTR lpszName)
{
	CComPtr<IBaseFilter> pFilter;
	m_pGraphBuilder->FindFilterByName( lpszName, &pFilter );
	
	if ( pFilter ) ReleaseFilter( pFilter );
	{
		pFilter->Stop(); 
		RemoveFilter( pFilter );
	}
}

BOOL CVideoCapture::InstallFilter(IBaseFilter* pCapFilter, LPCWSTR lpszName)
{
	CComPtr<IBaseFilter> pFilter;
	m_pGraphBuilder->FindFilterByName( lpszName, &pFilter );
	
	if ( pFilter ) ReleaseFilter( pFilter );
	return SUCCEEDED(m_pGraphBuilder->AddFilter(pCapFilter, lpszName));
}

BOOL CVideoCapture::RemoveFilter(IBaseFilter* pFilter)
{
	return SUCCEEDED(m_pGraphBuilder->RemoveFilter(pFilter));
}

BOOL CVideoCapture::ConnectFilter(LPCWSTR xSrcFilter, LPCWSTR xRndFilter)
{
	BOOL bResult = TRUE;
	
	CComPtr<IBaseFilter> pSrcFilter;
	if ( bResult ) bResult = SUCCEEDED( 
	m_pGraphBuilder->FindFilterByName( xSrcFilter, &pSrcFilter ) );
	
	CComPtr<IBaseFilter> pRndFilter;
	if ( bResult ) bResult = SUCCEEDED( 
	m_pGraphBuilder->FindFilterByName( xRndFilter, &pRndFilter ) );
	
	if ( bResult ) bResult =
	ConnectFilter( pSrcFilter, pRndFilter );
	
	return bResult;
}

BOOL CVideoCapture::ConnectFilter(IBaseFilter* pSrcFilter, IBaseFilter* pRndFilter)
{
	Stop();
	
	CComPtr<IPin> pPinOut = NULL;
	m_pCapGraphBuilder->FindPin( pSrcFilter, PINDIR_OUTPUT, NULL, NULL, TRUE, 0, &pPinOut );
	
	CComPtr<IPin> pPinIn = NULL;
	m_pCapGraphBuilder->FindPin( pRndFilter, PINDIR_INPUT, NULL, NULL, TRUE, 0, &pPinIn );
	
HRESULT hr = m_pGraphBuilder->Connect( pPinOut, pPinIn );
TRACE("err: 0x%x\n", hr);
return SUCCEEDED(hr) && Run();
//	BOOL bResult = SUCCEEDED(m_pGraphBuilder->Connect( pPinOut, pPinIn ));
//	return Run() & bResult;
}

BOOL CVideoCapture::PutIntoWindow(HWND hWndOwner, const RECT rect, BOOL bVisible)
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

BOOL CVideoCapture::Run()
{
	CComPtr<IMediaControl> pMediaControl;
	BOOL bResult = SUCCEEDED( 
	m_pGraphBuilder->QueryInterface(IID_IMediaControl, (void **)&pMediaControl) );
	
	OAFilterState fs = 0;
	if ( bResult && pMediaControl) bResult =
	SUCCEEDED( pMediaControl->GetState(INFINITE, &fs) );
	
	if ( bResult && fs == State_Running ) return TRUE;
	return SUCCEEDED( pMediaControl->Run() );
}

void CVideoCapture::Stop()
{
	CComPtr<IMediaControl> pMediaControl;
	m_pGraphBuilder->QueryInterface( IID_IMediaControl, (void **)&pMediaControl );
	
	if ( pMediaControl ) pMediaControl->Stop();
}

BOOL CVideoCapture::FindFilterByName(REFCLSID clsidDeviceClass, LPCWSTR lpszName, IBaseFilter** ppFilter)
{
	CComPtr<ICreateDevEnum> icDevEnum;
	if ( CoCreateInstance( CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (void**)&icDevEnum ) != S_OK ) return FALSE;
	
	CComPtr<IEnumMoniker> ieMoniker = NULL;
	if ( icDevEnum->CreateClassEnumerator( clsidDeviceClass,
		&ieMoniker, 0 ) != S_OK || ieMoniker == NULL ) return 0;
	
	IMoniker* pMoniker = NULL;
	while ( ieMoniker->Next(1, &pMoniker, NULL) == S_OK )
	{
		CComPtr<IPropertyBag> pBag = NULL;
		if ( pMoniker->BindToStorage( 0, 0, IID_IPropertyBag,
			(void **)&pBag ) != S_OK ) continue;
		
		CComVariant TheValue;
		pBag->Read( L"FriendlyName", &TheValue, NULL );
		
		TRACE( (CString)TheValue.bstrVal + '\n' );
		if ( wcscmp(TheValue.bstrVal, lpszName) == 0 )
		{
			return pMoniker->BindToObject( 0, 0, IID_IBaseFilter,
				(void **)ppFilter ) == S_OK;
		}
	}
	
	return FALSE;
}

#ifdef _DEBUG
void CVideoCapture::EnumFilters()
{
    CComPtr<IEnumFilters> pEnumFilters;
    HRESULT hr = m_pGraphBuilder->EnumFilters( &pEnumFilters );
	
	CComPtr<IBaseFilter> pFilter;
	while ( pFilter.Release(), pEnumFilters->Next(1, &pFilter, NULL) == S_OK )
	{
		FILTER_INFO info;
		if ( SUCCEEDED(pFilter->QueryFilterInfo( &info )) )
		{
			TRACE( (CString)info.achName + '\n' );
		}
	}
}

void CVideoCapture::EnumPins(IBaseFilter* pFilter)
{
	FILTER_INFO info;
	if ( pFilter->QueryFilterInfo(&info) != S_OK ) return;
	
	CString strFilter = (CString)info.achName;
	
    CComPtr<IEnumPins> pEnumPins;
    HRESULT hr = pFilter->EnumPins( &pEnumPins );
	
	CComPtr<IPin> pPin;
	PIN_DIRECTION direction;
	
	while ( pPin.Release(), pEnumPins->Next(1, &pPin, NULL) == S_OK )
	{
		PIN_INFO info;
		if ( SUCCEEDED(pPin->QueryPinInfo( &info )) )
		{
			TRACE( strFilter + " ==> " + (CString)info.achName + '\n' );
		}
		if ( SUCCEEDED(pPin->QueryDirection( &direction )) )
		{
			CComPtr<IPin> pTmp;
			if ( SUCCEEDED(pPin->ConnectedTo(&pTmp)) )
			{
				TRACE("Is Running...\n");
			}
		}
	}
}

void CVideoCapture::PrintGraph()
{
	CComPtr<IEnumFilters> pEnumFilters;
	HRESULT hr = m_pGraphBuilder->EnumFilters( &pEnumFilters );
	
	CComPtr<IBaseFilter> pFilter;
	while ( pFilter.Release(), pEnumFilters->Next(1, &pFilter, NULL) == S_OK )
	{
		PrintGraph( pFilter, PINDIR_INPUT );
	}
}

void CVideoCapture::PrintGraph(IBaseFilter* pFilter, PIN_DIRECTION pinDir)
{
	FILTER_INFO fInfo;
	pFilter->QueryFilterInfo( &fInfo );
	//TRACE( (CString)fInfo.achName );
	
    CComPtr<IEnumPins> pEnumPins;
    HRESULT hr = pFilter->EnumPins( &pEnumPins );
	
	CComPtr<IPin> pPin;
	
	while ( pPin.Release(), pEnumPins->Next(1, &pPin, NULL) == S_OK )
	{
		PIN_INFO pInfo;
		pPin->QueryPinInfo( &pInfo );
		TRACE( (CString)fInfo.achName+'.' + (CString)pInfo.achName );
		
		CComPtr<IPin> pTmp;
		if ( FAILED(pPin->ConnectedTo(&pTmp)) )
		{
			TRACE( "\n" ); continue;
		}
		
		PIN_DIRECTION direction;
		if ( FAILED(pPin->QueryDirection( &direction ))
			|| direction != pinDir )
		{
			TRACE( "{OUTPUT}\n" ); continue;
		}
		
		TRACE( " --> " );
		
		PIN_INFO pInfo2;
		pTmp->QueryPinInfo( &pInfo2 );
		
		CComPtr<IBaseFilter> pFilter2;
		pFilter2 = pInfo2.pFilter;
		
		FILTER_INFO fInfo2;
		pFilter2->QueryFilterInfo( &fInfo2 );
		TRACE( (CString)fInfo2.achName );
		
		TRACE( '.' + (CString)pInfo2.achName );
		
		TRACE( ", " );
		PrintGraph( pFilter2, PINDIR_INPUT );
		TRACE( "\n" );
	}
}
#endif

HRESULT CVideoCapture::AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister)
{
	IMoniker* pMoniker;
	IRunningObjectTable* pROT;
	
	if (FAILED( GetRunningObjectTable(0, &pROT) )) return E_FAIL;
	
	WCHAR wsz[256];
	wsprintfW(wsz, L"FilterGraph %08x pid %08x", (DWORD_PTR)pUnkGraph, GetCurrentProcessId());
	
	HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
	if (SUCCEEDED(hr))
	{
		hr = pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph,
			pMoniker, pdwRegister);
		pMoniker->Release();
	}
	
	pROT->Release();
	return hr;
}

void CVideoCapture::RemoveFromRot(DWORD pdwRegister)
{
	IRunningObjectTable* pROT;
	if (SUCCEEDED(GetRunningObjectTable(0, &pROT)))
	{
		pROT->Revoke(pdwRegister);
		pROT->Release();
	}
}

//////////////////////////////////////////////////////////////////////
// CVideoCapture helper

BOOL CVideoCapture::GetFilterByName(LPCWSTR lpszFilter, IBaseFilter** ppFilter)
{
	DWORD dwEaten;
	CComPtr<IMoniker> pMoniker = NULL;
	
	CComPtr<IBindCtx> pBindCtx = NULL;
	if ( FAILED(CreateBindCtx(0, &pBindCtx)) ) return FALSE;
	
	if ( FAILED(MkParseDisplayName(pBindCtx, lpszFilter, &dwEaten, &pMoniker)) ) return FALSE;
	
	if ( FAILED(pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)ppFilter)) ) return FALSE;
	
	return (*ppFilter != NULL);
}
