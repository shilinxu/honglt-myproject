//
// RenderGraph.h
//

#if !defined(AFX_RENDERGRAPH_H__644578D8_3F63_4BFA_B5FB_568AD2935728__INCLUDED_)
#define AFX_RENDERGRAPH_H__644578D8_3F63_4BFA_B5FB_568AD2935728__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FilterGraph.h"

class XT_EXT_CLASS CGraphRender : public CFilterGraph  
{
// Construction
public:
	CGraphRender(LPCTSTR lpszNamed = NULL);
	virtual ~CGraphRender();
	
// Attributes
protected:
	CComPtr	<IBaseFilter>	m_pMultiplexer;
	CComPtr	<IBaseFilter>	m_pFileSink;
	
// Operations
public:
	virtual	HRESULT	RenderStream(LPCWSTR lpszCapture, WAVEFORMATEX& pwfx, DWORD nReserved);
	virtual	HRESULT	RenderStream(LPCWSTR lpszCapture, VIDEOINFOHEADER& pvih, DWORD nReserved);
	virtual HRESULT RenderStream(WORD nPayloadMask, LPOLESTR pDisplayName, DWORD nReserved);
public:
	virtual HRESULT RenderPreview(LPCWSTR lpszRender, WAVEFORMATEX*);
	virtual HRESULT RenderPreview(LPCWSTR /*lpszCapture*/, VIDEOINFOHEADER*);
	virtual HRESULT	RenderPreview(LPCWSTR lpszCapture, WAVEFORMATEX& pwfx, DWORD nReserved);
	virtual HRESULT RenderPreview(LPCWSTR lpszCapture, VIDEOINFOHEADER& pvih, DWORD nReserved);
	virtual HRESULT RenderPreview(WORD nPayloadMask, LPOLESTR pDisplayName = NULL);
	virtual HRESULT RenderPreview(WORD nPayloadMask, LPOLESTR pDisplayName, DWORD nReserved);
public:
	virtual HRESULT	PutIntoWindow(HWND hWndOwner, const RECT rect, BOOL bVisible = TRUE);
	virtual HRESULT	StartPreview(const RECT& rect, HWND hWnd);
	virtual HRESULT RecordFile(LPCWSTR lpszFile, WORD nPayloadMask);
	virtual void	StopPreview();
	virtual HRESULT StopRecord();
protected:
	virtual HRESULT RenderStream(IBaseFilter* pFilter, WAVEFORMATEX* pwfx, const GUID* pCategory, DWORD nReserved);
	virtual HRESULT RenderStream(IBaseFilter* pFilter, VIDEOINFOHEADER* pvih, const GUID* pCategory, DWORD nReserved);
	virtual HRESULT RecycleStream(LPCWSTR lpszName);
protected:
	HRESULT			InstallRender(CBaseFilter* pBaseFilter, LPCWSTR lpszName, IBaseFilter** ppFilter = NULL);
	
// inline
public:
	inline	BOOL	IsRecording() const
	{
		return m_pMultiplexer != NULL && m_pFileSink != 0;
	}
};

extern XT_EXT_CLASS void DeriveCompressFormat(WAVEFORMATEX& wfx);
extern XT_EXT_CLASS void DeriveCompressFormat(VIDEOINFOHEADER& vih);
extern XT_EXT_CLASS void DeriveCompressFormat(WAVEFORMATEX& wfxDest, WAVEFORMATEX* pwfxSrc);
extern XT_EXT_CLASS void DeriveCompressFormat(VIDEOINFOHEADER& vihDest, VIDEOINFOHEADER* pvihSrc);

#endif // !defined(AFX_RENDERGRAPH_H__644578D8_3F63_4BFA_B5FB_568AD2935728__INCLUDED_)
