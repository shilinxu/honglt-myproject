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

CFilterGraph::CFilterGraph()
{
	Setup();
}

CFilterGraph::~CFilterGraph()
{
	Release();
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

/////////////////////////////////////////////////////////////////////////////
// CFilterGraph operations

HRESULT CFilterGraph::CreateFilter(REFCLSID rclsid, LPCWSTR lpszName, IBaseFilter** ppFilter)
{
	HRESULT hr = S_OK;
	
	CComPtr<IBaseFilter> pFilter;
	if ( SUCCEEDED( hr ) ) hr = pFilter.CoCreateInstance( rclsid );
	
	if ( ppFilter ) pFilter->QueryInterface( IID_IBaseFilter, (void**)ppFilter );
	
	return InstallFilter( pFilter, lpszName );
}

HRESULT CFilterGraph::CreateFilter(LPCWSTR lpszFilter, LPCWSTR lpszName, IBaseFilter** ppFilter)
{
	CComPtr<IBaseFilter> pFilter;
	if ( ! GetFilterByName(lpszFilter, &pFilter) ) return VFW_S_NO_MORE_ITEMS;
	
	if ( ppFilter ) pFilter->QueryInterface( IID_IBaseFilter, (void**)ppFilter);
	
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

void CFilterGraph::RemoveFilter(LPCWSTR lpszName)
{
	CComPtr<IBaseFilter> pFilter;
	m_pGraphBuilder->FindFilterByName( lpszName, &pFilter );
	
	if ( pFilter ) RemoveFilter( pFilter );
}

void CFilterGraph::RemoveFilter(IBaseFilter* pFilter)
{
#ifdef _DEBUG
	FILTER_INFO info;
	if ( SUCCEEDED(pFilter->QueryFilterInfo( &info )) )
	{
		DbgLog((LOG_TRACE, 0, TEXT("Remove: %ls"), info.achName));
	}
#endif
	
	pFilter->Stop();
	m_pGraphBuilder->RemoveFilter( pFilter );
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
	Stop();
	
	CComPtr<IPin> pPinOut;
	m_pCapGraphBuilder->FindPin( pSrcFilter, PINDIR_OUTPUT, NULL, NULL, TRUE, 0, &pPinOut );
	
	CComPtr<IPin> pPinIn;
	m_pCapGraphBuilder->FindPin( pRndFilter, PINDIR_INPUT, NULL, NULL, TRUE, 0, &pPinIn );
	
	HRESULT hr = m_pGraphBuilder->Connect( pPinOut, pPinIn );
	Run();
	
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CFilterGraph preview

HRESULT CFilterGraph::StartPreview()
{
	return S_OK;
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
	if ( pMediaControl) pMediaControl->GetState(INFINITE, &fs);
	
	if ( fs == State_Running ) return S_OK;
	return SUCCEEDED( pMediaControl->Run() );
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
		if ( wcscmp(TheValue.bstrVal, lpszName) == 0 )
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
