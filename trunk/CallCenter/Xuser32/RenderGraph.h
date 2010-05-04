//
// RenderGraph.h
//

#if !defined(AFX_RENDERGRAPH_H__644578D8_3F63_4BFA_B5FB_568AD2935728__INCLUDED_)
#define AFX_RENDERGRAPH_H__644578D8_3F63_4BFA_B5FB_568AD2935728__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FilterGraph.h"

class AFX_EXT_CLASS CGraphRender : public CFilterGraph  
{
// Construction
public:
	CGraphRender(LPCTSTR xGraphNamed);
	virtual ~CGraphRender();
	
// Attributes
protected:
	CComPtr	<IBaseFilter>	m_pMultiplexer;
	CComPtr	<IBaseFilter>	m_pFileSink;
	
// Operations
public:
	virtual	HRESULT	RenderStream(LPCWSTR lpszCapture, WAVEFORMATEX& pwfx);
	virtual	HRESULT	RenderStream(LPCWSTR lpszCapture, VIDEOINFOHEADER& pvih);
public:
	virtual HRESULT RenderPreview(LPCWSTR lpszCapture, WAVEFORMATEX& pwfx);
	virtual HRESULT RenderPreview(LPCWSTR lpszCapture, VIDEOINFOHEADER& pvih);
	virtual HRESULT	RenderPreview(LPCWSTR lpszCapture, WAVEFORMATEX& pwfx, DWORD nReserved);
	virtual HRESULT RenderPreview(LPCWSTR lpszCapture, VIDEOINFOHEADER& pvih, DWORD nReserved);
public:
	virtual HRESULT	StartPreview(const RECT& rect, HWND hWnd);
	virtual HRESULT RecordFile(LPCWSTR lpszFile, WORD nPayloadMask = 0x0FFF);
	virtual void	StopPreview();
	virtual HRESULT StopRecord();
protected:
	virtual CString	GetRecordFile(LPCTSTR pszPath, LPCTSTR fMask=_T("<USER>\\<DATE><TIME>.AVI")) const;
protected:
	virtual HRESULT InstallCapture(LPCWSTR lpszCapture, LPCWSTR lpszName);
	virtual HRESULT InstallCapture(REFCLSID clsidDevClass, LPCWSTR lpszName);
protected:
	HRESULT			InstallRender(CBaseFilter* pBaseFilter, LPCWSTR lpszName, IBaseFilter** ppFilter = NULL);
	HRESULT			PutIntoWindow(HWND hWndOwner, const RECT rect, BOOL bVisible = TRUE);
	
// inline
public:
	inline	BOOL	IsRecording() const
	{
		return m_pMultiplexer != NULL && m_pFileSink != 0;
	}
};

#define QCIF_WIDTH  176
#define QCIF_HEIGHT 144

#define RenderPayload(a)	( (1 << 12) | (a & 0x0FFF) )
#define PreviewPayload(a)	( (2 << 12) | (a & 0x0FFF) )
#define RecordPayload(a)	( (4 << 12) | (a & 0x0FFF) )

enum PayloadMask
{
	PayloadMask_Audio	= 0x1,
	PayloadMask_Video	= 0x2,
	PayloadMask_File	= 0x4,
	PayloadMask_Control	= 0x8,
	PayloadMask_Unknown	= 0xf,
};

#endif // !defined(AFX_RENDERGRAPH_H__644578D8_3F63_4BFA_B5FB_568AD2935728__INCLUDED_)
