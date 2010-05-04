//
// VideoRenderer.h
//

#if !defined(AFX_VIDEORENDERER_H__E4013111_40FD_4FAE_B813_074529014D48__INCLUDED_)
#define AFX_VIDEORENDERER_H__E4013111_40FD_4FAE_B813_074529014D48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FilterGraph.h"

class CVideoRenderer : public CFilterGraph
{
// Construction
public:
	CVideoRenderer();
	virtual ~CVideoRenderer();
	
// Attributes
protected:

// Operations
public:
	virtual HRESULT	StartPreview(const RECT& rect, HWND hWnd);
	virtual void	StopPreview();
	virtual HRESULT	Connnect();
public:
	virtual HRESULT	DeliverBuffer(BYTE* pByte, DWORD nLength);
protected:
	HRESULT			PutIntoWindow(HWND hWndOwner, const RECT rect, BOOL bVisible = TRUE);
};

#endif // !defined(AFX_VIDEORENDERER_H__E4013111_40FD_4FAE_B813_074529014D48__INCLUDED_)
