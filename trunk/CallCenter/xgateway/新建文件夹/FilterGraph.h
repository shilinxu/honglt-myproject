//
// FilterGraph.h
//

#if !defined(AFX_FILTERGRAPH_H__B58A0D56_CE7D_4B98_92ED_C0E1BF2861FD__INCLUDED_)
#define AFX_FILTERGRAPH_H__B58A0D56_CE7D_4B98_92ED_C0E1BF2861FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFilterGraph
{
// Construction
public:
	CFilterGraph();
	virtual ~CFilterGraph();

// Attributes
protected:
	CComPtr	<IGraphBuilder>		m_pGraphBuilder;
	CComPtr	<ICaptureGraphBuilder2>	m_pCapGraphBuilder;
	
// Operations
public:
	virtual void	Setup();
	virtual void	Release();
	virtual HRESULT	StartPreview();
	virtual void	StopPreview();
protected:
	virtual HRESULT	Run();
	virtual void	Stop();
public:
	HRESULT		InstallFilter(IBaseFilter* pCapFilter, LPCWSTR lpszName);
	HRESULT		CreateFilter(REFCLSID rclsid, LPCWSTR lpszName, IBaseFilter** ppFilter = NULL);
	HRESULT		CreateFilter(LPCWSTR lpszFilter, LPCWSTR lpszName, IBaseFilter** ppFilter = NULL);
	HRESULT		ConnectFilter(LPCWSTR xSrcFilter, LPCWSTR xRndFilter);
	void		RemoveFilter(LPCWSTR lpszName);
	void		ClearFilters();
protected:
	void		RemoveFilter(IBaseFilter* pFilter);
	HRESULT		ConnectFilter(IBaseFilter* pSrcFilter, IBaseFilter* pRndFilter);
protected:
	BOOL		FindFilterByName(REFCLSID clsidDeviceClass, LPCWSTR lpszName, IBaseFilter** pCapFilter);
	BOOL		GetFilterByName(LPCWSTR lpszFilter, IBaseFilter** ppFilter);
	
// Register
#ifdef _DEBUG
protected:
	DWORD		m_nFilterGraph;
	HRESULT		AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
	void		RemoveFromRot(DWORD pdwRegister);
#endif
};

#endif // !defined(AFX_FILTERGRAPH_H__B58A0D56_CE7D_4B98_92ED_C0E1BF2861FD__INCLUDED_)
