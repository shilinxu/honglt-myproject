//
// FilterGraph.cpp
//

#include "stdafx.h"
#include "FilterGraph.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CFilterGraph construction

CFilterGraph::CFilterGraph(LPCTSTR lpszNamed)
{
	m_pszNamed	= _tcsdup( lpszNamed );
	
	Setup();
}

CFilterGraph::~CFilterGraph()
{
	Release();
	
	free( m_pszNamed );
}

//////////////////////////////////////////////////////////////////////
// CFilterGraph setup

void CFilterGraph::Setup()
{
	Release();
	
	m_pGraphBuilder.CoCreateInstance( CLSID_FilterGraph );
	
	m_pCapGraphBuilder.CoCreateInstance( CLSID_CaptureGraphBuilder2 );
	m_pCapGraphBuilder->SetFiltergraph( m_pGraphBuilder );
	
#ifdef _DEBUG	
	AddToRot( m_pGraphBuilder, &m_nFilterGraph );
#endif
}

void CFilterGraph::Release()
{
	if ( m_pGraphBuilder )
		StopPreview();
	
	if ( m_pGraphBuilder )
		ClearFilters();
	
	m_pCapGraphBuilder.Release();
	m_pGraphBuilder.Release();
	
#ifdef _DEBUG
	RemoveFromRot( m_nFilterGraph );
#endif
}

void CFilterGraph::ClearFilters()
{
	CComPtr<IEnumFilters> pEnumFilters;
	m_pGraphBuilder->EnumFilters( &pEnumFilters );
	
	CComPtr<IBaseFilter> pFilter;
	while ( pFilter.Release(), pEnumFilters->Next(1, &pFilter, NULL) == S_OK )
	{
		RemoveFilter( pFilter );
		
		pEnumFilters->Reset();
	}
}

//////////////////////////////////////////////////////////////////////
// CFilterGraph attributes

LPCTSTR CFilterGraph::GetNamed() const
{
	return (LPCTSTR)m_pszNamed;
}

BOOL CFilterGraph::IsNamed(LPCTSTR pszNamed) const
{
	if ( ! pszNamed && ! m_pszNamed ) return TRUE;
	if ( ! pszNamed ^ ! m_pszNamed ) return FALSE;
	
	return _tcsicmp( m_pszNamed, pszNamed ) == 0;
}

/////////////////////////////////////////////////////////////////////////////
// CFilterGraph operations

HRESULT CFilterGraph::CreateFilter(REFCLSID rclsid, LPCWSTR lpszName, IBaseFilter** ppFilter)
{
	CComPtr<IBaseFilter> pFilter;
	HRESULT hr = pFilter.CoCreateInstance( rclsid );
	
	if ( FAILED( hr ) ) return hr;
	if ( ppFilter ) pFilter->QueryInterface( IID_IBaseFilter, (void**)ppFilter );
	
	return InstallFilter( pFilter, lpszName );
}

HRESULT CFilterGraph::CreateFilter(LPCWSTR lpszFilter, LPCWSTR lpszName, IBaseFilter** ppFilter)
{
	CComPtr<IBaseFilter> pFilter;
	if ( ! ppFilter ) ppFilter = &pFilter;
	
	HRESULT hr = m_pGraphBuilder->FindFilterByName( lpszName, ppFilter );
	if ( SUCCEEDED( hr ) ) return S_OK;
	
	if ( ! GetFilterByName(lpszFilter, &pFilter) ) return VFW_S_NO_MORE_ITEMS;
	
	pFilter->QueryInterface( IID_IBaseFilter, (void**)ppFilter);
	
	return InstallFilter( pFilter, lpszName );
}

HRESULT CFilterGraph::InstallFilter(IBaseFilter* pCapFilter, LPCWSTR lpszName)
{
	RemoveFilter( lpszName );
	
#ifdef _DEBUG
	DbgLog((LOG_TRACE, 0, TEXT("Install: %ls"), lpszName));
#endif
	return m_pGraphBuilder->AddFilter( pCapFilter, lpszName );
}

HRESULT CFilterGraph::RemoveFilter(LPCWSTR lpszName)
{
	HRESULT hr = S_OK;
	
	CComPtr<IBaseFilter> pFilter;
	hr = m_pGraphBuilder->FindFilterByName( lpszName, &pFilter );
	
	if ( FAILED(hr) ) return hr;
	return RemoveFilter( pFilter );
}

HRESULT CFilterGraph::RemoveFilter(IBaseFilter* pFilter)
{
#ifdef _DEBUG
	FILTER_INFO info;
	if ( SUCCEEDED(pFilter->QueryFilterInfo( &info )) )
	{
		DbgLog((LOG_TRACE, 0, TEXT("Remove: %ls"), info.achName));
	}
#endif
	
	pFilter->Stop();
	return m_pGraphBuilder->RemoveFilter( pFilter );
}

HRESULT CFilterGraph::ConnectFilter(LPCWSTR xSrcFilter, LPCWSTR xRndFilter)
{
	HRESULT hr = S_OK;
	
	CComPtr<IBaseFilter> pSrcFilter;
	m_pGraphBuilder->FindFilterByName( xSrcFilter, &pSrcFilter );
	
	CComPtr<IBaseFilter> pRndFilter;
	m_pGraphBuilder->FindFilterByName( xRndFilter, &pRndFilter );
	
	return ConnectFilter( pSrcFilter, pRndFilter );
}

HRESULT CFilterGraph::ConnectFilter(IBaseFilter* pSrcFilter, IBaseFilter* pRndFilter)
{
	CComPtr<IPin> pPinOut;
	m_pCapGraphBuilder->FindPin( pSrcFilter, PINDIR_OUTPUT, NULL, NULL, TRUE, 0, &pPinOut );
	
	CComPtr<IPin> pPinIn;
	m_pCapGraphBuilder->FindPin( pRndFilter, PINDIR_INPUT, NULL, NULL, TRUE, 0, &pPinIn );
	
	return m_pGraphBuilder->Connect( pPinOut, pPinIn );
}

HRESULT CFilterGraph::InstallCapture(LPCWSTR lpszCapture, LPCWSTR lpszName)
{
	HRESULT hr = S_OK;
	CComPtr<IBaseFilter> pCapture;
	
	hr = m_pGraphBuilder->FindFilterByName( lpszName, &pCapture );
	if ( SUCCEEDED( hr ) ) return S_OK;
	
	// Capture
	
	hr = CreateFilter( lpszCapture, lpszName, &pCapture );
	return hr;
}

HRESULT CFilterGraph::InstallCapture(REFCLSID clsidDevClass, LPCWSTR lpszName, LPOLESTR* ppszDisplayName)
{
	HRESULT hr = S_OK;
	
	CComPtr<ICreateDevEnum> icDevEnum;
	hr = CoCreateInstance( CLSID_SystemDeviceEnum, NULL,
		CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&icDevEnum );
	if ( hr != S_OK ) return hr;
	
	CComPtr<IEnumMoniker> ieMoniker;
	hr = icDevEnum->CreateClassEnumerator( clsidDevClass, &ieMoniker, 0 );
	if ( hr != S_OK || ieMoniker == NULL ) return hr;
	
	IMoniker* pMoniker;
	hr = ieMoniker->Next( 1, &pMoniker, NULL );
	if ( hr != S_OK || pMoniker == NULL ) return hr;
	
	LPOLESTR pDisplayName = NULL;
	LPOLESTR& pDisplayName2 = ppszDisplayName ? *ppszDisplayName : pDisplayName;
	pMoniker->GetDisplayName( NULL, NULL, &pDisplayName2 );
	
	hr = InstallCapture(pDisplayName, lpszName);
	CoTaskMemFree( pDisplayName );
	
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CFilterGraph preview

HRESULT CFilterGraph::StartPreview()
{
	return Run();
}

void CFilterGraph::StopPreview()
{
	this->Stop();
}

/////////////////////////////////////////////////////////////////////////////
// CFilterGraph run

HRESULT CFilterGraph::Run()
{
	CComPtr<IMediaControl> pMediaControl;
	m_pGraphBuilder->QueryInterface(IID_IMediaControl, (void **)&pMediaControl);
	
	OAFilterState fs = 0;
	if ( pMediaControl) pMediaControl->GetState(0, &fs);
	
	if ( fs == State_Running ) return S_OK;
	return pMediaControl->Run();
}

void CFilterGraph::Stop()
{
	CComPtr<IMediaControl> pMediaControl;
	m_pGraphBuilder->QueryInterface( IID_IMediaControl, (void **)&pMediaControl );
	
	if ( pMediaControl ) pMediaControl->Stop();
}

#ifdef _DEBUG
HRESULT CFilterGraph::AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister)
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

void CFilterGraph::RemoveFromRot(DWORD pdwRegister)
{
	IRunningObjectTable* pROT;
	if (SUCCEEDED(GetRunningObjectTable(0, &pROT)))
	{
		pROT->Revoke(pdwRegister);
		pROT->Release();
	}
}
#endif

//////////////////////////////////////////////////////////////////////
// CFilterGraph helper

HRESULT CFilterGraph::GetNextDevice(REFCLSID clsidDeviceClass, IBaseFilter** ppFilter, LPCWSTR lpszName)
{
	HRESULT hr = S_OK;
	
	CComPtr<ICreateDevEnum> icDevEnum;
	hr = CoCreateInstance( CLSID_SystemDeviceEnum, NULL,
		CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&icDevEnum );
	if ( hr != S_OK ) return hr;
	
	CComPtr<IEnumMoniker> ieMoniker;
	hr = icDevEnum->CreateClassEnumerator( clsidDeviceClass, &ieMoniker, 0 );
	if ( hr != S_OK || ieMoniker == NULL ) return hr;
	
	return GetNextDevice( ieMoniker, ppFilter );
}

HRESULT CFilterGraph::GetNextDevice(IEnumMoniker* ieMoniker, IBaseFilter** ppFilter, LPCWSTR lpszName)
{
	IMoniker* pMoniker;
	HRESULT hr = ieMoniker->Next( 1, &pMoniker, NULL );
	
	if ( hr != S_OK ) return hr;
	
	CComPtr<IPropertyBag> pBag;
	if ( pMoniker->BindToStorage( 0, 0, IID_IPropertyBag, (void **)&pBag ) == S_OK )
	{
		CComVariant TheValue;
		pBag->Read( L"FriendlyName", &TheValue, NULL );
		
		TRACE( (CString)TheValue.bstrVal + '\n' );
		if ( ! lpszName || wcsicmp(TheValue.bstrVal, lpszName) == 0 )
		{
			return pMoniker->BindToObject( 0, 0, IID_IBaseFilter, (void **)ppFilter );
		}
	}
	
	return GetNextDevice( ieMoniker, ppFilter, lpszName );
}

BOOL CFilterGraph::FindFilterByName(REFCLSID clsidDeviceClass, LPCWSTR lpszName, IBaseFilter** ppFilter)
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
		if ( ! lpszName || wcscmp(TheValue.bstrVal, lpszName) == 0 )
		{
			return pMoniker->BindToObject( 0, 0, IID_IBaseFilter,
				(void **)ppFilter ) == S_OK;
		}
	}
	
	return FALSE;
}

BOOL CFilterGraph::GetFilterByName(LPCWSTR lpszFilter, IBaseFilter** ppFilter)
{
	DWORD dwEaten;
	CComPtr<IMoniker> pMoniker = NULL;
	
	CComPtr<IBindCtx> pBindCtx = NULL;
	if ( FAILED(CreateBindCtx(0, &pBindCtx)) ) return FALSE;
	
	if ( FAILED(MkParseDisplayName(pBindCtx, lpszFilter, &dwEaten, &pMoniker)) ) return FALSE;
	
	if ( FAILED(pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)ppFilter)) ) return FALSE;
	
	return (*ppFilter != NULL);
}
