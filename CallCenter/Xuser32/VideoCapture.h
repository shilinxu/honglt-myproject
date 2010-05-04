//
// VideoCapture.h
//

#if !defined(AFX_VIDEOCAPTURE_H__E4013111_40FD_4FAE_B813_074529014D48__INCLUDED_)
#define AFX_VIDEOCAPTURE_H__E4013111_40FD_4FAE_B813_074529014D48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FilterGraph.h"

class CVideoCapture : public CFilterGraph
{
// Construction
public:
	CVideoCapture();
	virtual ~CVideoCapture();
	
// Attributes
protected:

// Operations
public:
	virtual HRESULT	StartPreview(const RECT& rect, HWND hWnd);
	virtual void	StopPreview();
public:
	virtual HRESULT	ConnnectTo(LPCWSTR lpszCapture);
protected:
	HRESULT			PutIntoWindow(HWND hWndOwner, const RECT rect, BOOL bVisible = TRUE);
};

//extern AFX_EXT_CLASS CVideoCapture VideoCapture;

#endif // !defined(AFX_VIDEOCAPTURE_H__E4013111_40FD_4FAE_B813_074529014D48__INCLUDED_)
