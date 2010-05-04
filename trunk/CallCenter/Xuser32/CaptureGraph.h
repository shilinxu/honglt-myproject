//
// CaptureGraph.h
//

#if !defined(AFX_CAPTUREGRAPH_H__6B133952_C1A9_489F_86B1_4A43FC7F361B__INCLUDED_)
#define AFX_CAPTUREGRAPH_H__6B133952_C1A9_489F_86B1_4A43FC7F361B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FilterGraph.h"

class CGraphCapture : public CFilterGraph  
{
// Construction
public:
	CGraphCapture();
	virtual ~CGraphCapture();
	
// Attributes
protected:
	
// Operations
public:
	HRESULT			AudioCapture(REFCLSID clsidCompressor, LPCWSTR lpszName);
	HRESULT			VideoCapture(REFCLSID clsidCompressor, LPCWSTR lpszName);
	HRESULT			InstallRender(REFGUID psubType, WAVEFORMATEX* pwfx);
	HRESULT			InstallRender(REFGUID psubType, VIDEOINFOHEADER* pvih);
public:
//	virtual HRESULT	InstallAudio(LPCWSTR lpszCapture);
	virtual HRESULT	StartPreview();
	virtual HRESULT	StartPreview(const RECT& rect, HWND hWnd);
	virtual void	StopPreview();
protected:
	HRESULT			PutIntoWindow(HWND hWndOwner, const RECT rect, BOOL bVisible = TRUE);
};

extern CGraphCapture CaptureGraph;

#endif // !defined(AFX_CAPTUREGRAPH_H__6B133952_C1A9_489F_86B1_4A43FC7F361B__INCLUDED_)
