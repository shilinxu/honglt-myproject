//
// VideoCapture.h
//

#if !defined(AFX_VIDEOCAPTURE_H__E4013111_40FD_4FAE_B813_074529014D48__INCLUDED_)
#define AFX_VIDEOCAPTURE_H__E4013111_40FD_4FAE_B813_074529014D48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVideoCapture  
{
// Construction
public:
	CVideoCapture();
	virtual ~CVideoCapture();
	
// Attributes
public:
	CComPtr	<IGraphBuilder>		m_pGraphBuilder;
	CComPtr	<ICaptureGraphBuilder2>	m_pCapGraphBuilder;

// Operations
public:
	virtual void	Setup();
	virtual void	Release();
	virtual BOOL	StartPreview();
	virtual BOOL	StartPreview(const RECT& rect, HWND hWnd);
	virtual void	StopPreview();
	virtual BOOL	Run();
protected:
	virtual void	Stop();
	virtual void	ClearFilters();
public:
	BOOL		CreateFilter(REFCLSID rclsid, LPCWSTR lpszName);
	BOOL		InstallFilter(LPCWSTR xDisplayName, LPCWSTR lpszName);
	BOOL		InstallFilter(IBaseFilter* pCapFilter, LPCWSTR lpszName);
	BOOL		RemoveFilter(LPCWSTR lpszName);
	BOOL		ConnectFilter(LPCWSTR xSrcFilter, LPCWSTR xRndFilter);
	BOOL		FindFilterByName(REFCLSID clsidDeviceClass, LPCWSTR lpszName, IBaseFilter** pCapFilter);
protected:
	void		ReleaseFilter(LPCWSTR lpszName);
	void		ReleaseFilter(IBaseFilter* pFilter);
	BOOL		RemoveFilter(IBaseFilter* pFilter);
	BOOL		PutIntoWindow(HWND hWndOwner, const RECT rect, BOOL bVisible = TRUE);
	BOOL		ConnectFilter(IBaseFilter* pSrcFilter, IBaseFilter* pRndFilter);
protected:
	BOOL		GetCapFilter(LPCWSTR xDisplayName, IBaseFilter** pCapFilter);
#ifdef _DEBUG
	DWORD		m_dwRegister;
	HRESULT		AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
	void		RemoveFromRot(DWORD pdwRegister);
#endif

#ifdef _DEBUG
protected:
	void		EnumFilters();
	void		EnumPins(IBaseFilter* pFilter);
	void		PrintGraph(IBaseFilter* pFilter, PIN_DIRECTION pinDir);
	void		PrintGraph();
#endif
};

#endif // !defined(AFX_VIDEOCAPTURE_H__E4013111_40FD_4FAE_B813_074529014D48__INCLUDED_)
