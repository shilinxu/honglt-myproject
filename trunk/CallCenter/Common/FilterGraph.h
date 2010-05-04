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
	CFilterGraph(LPCTSTR lpszNamed = NULL);
	virtual ~CFilterGraph();

// Attributes
protected:
	LPTSTR		m_pszNamed;
protected:
	CComPtr	<IGraphBuilder>		m_pGraphBuilder;
	CComPtr	<ICaptureGraphBuilder2>	m_pCapGraphBuilder;
	
// Operations
public:
	virtual void	Setup();
	virtual void	Release();
public:
	LPCTSTR			GetNamed() const;
	BOOL			IsNamed(LPCTSTR pszNamed) const;
public:
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
	HRESULT		RemoveFilter(LPCWSTR lpszName);
	void		ClearFilters();
protected:
	HRESULT		RemoveFilter(IBaseFilter* pFilter);
	HRESULT		ConnectFilter(IBaseFilter* pSrcFilter, IBaseFilter* pRndFilter);
public:
	HRESULT		GetNextDevice(REFCLSID clsidDeviceClass, IBaseFilter** ppFilter, LPCWSTR lpszName = NULL);
	HRESULT		GetNextDevice(IEnumMoniker* ieMoniker, IBaseFilter** ppFilter, LPCWSTR lpszName = NULL);
	HRESULT		InstallCapture(LPCWSTR lpszCapture, LPCWSTR lpszName);
	HRESULT		InstallCapture(REFCLSID clsidDevClass, LPCWSTR lpszName, LPOLESTR* ppszDisplayName = NULL);
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

enum Payload
{
	Payload_mulaw	= 0,
	Payload_Alaw	= 8,
	Payload_G729	= 18,
	Payload_H263	= 34,
	Payload_XviD	= 41,
	Payload_File	= 96,
	Payload_Unknown	= 127,
};

#endif // !defined(AFX_FILTERGRAPH_H__B58A0D56_CE7D_4B98_92ED_C0E1BF2861FD__INCLUDED_)
