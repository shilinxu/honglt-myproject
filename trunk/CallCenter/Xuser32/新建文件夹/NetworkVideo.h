//
// NetworkVideo.h
//

#if !defined(AFX_NETWORKVIDEO_H__6BD3DE2C_C908_4D33_BAE2_63EAFFAF17B6__INCLUDED_)
#define AFX_NETWORKVIDEO_H__6BD3DE2C_C908_4D33_BAE2_63EAFFAF17B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VideoCapture.h"

/////////////////////////////////////////////////////////////////////////////
// CNetworkVideo window

class CNetworkVideo : public CWnd
{
// Construction
public:
	CNetworkVideo(CVideoCapture* pCapture);
	virtual ~CNetworkVideo();
	DECLARE_DYNAMIC(CNetworkVideo)

// Attributes
protected:
	CVideoCapture*	m_pCapture;

// Operations
public:
	BOOL			StartPreview(LPCWSTR xDisplayName, const RECT& rect);
	BOOL			StartPreview(IBaseFilter* pCapFilter, const RECT& rect);

// Overrides
public:
	//{{AFX_VIRTUAL(CNetworkVideo)
	virtual BOOL Create(const RECT& rect, CWnd* pParentWnd, LPCTSTR pszCaption = NULL);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CNetworkVideo)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NETWORKVIDEO_H__6BD3DE2C_C908_4D33_BAE2_63EAFFAF17B6__INCLUDED_)
