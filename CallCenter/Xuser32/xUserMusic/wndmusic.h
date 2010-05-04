//
// WndMusic.h
//

#if !defined(AFX_WNDMUSIC_H__FEDB13C1_30BC_449F_BC99_B27A6EACD1B9__INCLUDED_)
#define AFX_WNDMUSIC_H__FEDB13C1_30BC_449F_BC99_B27A6EACD1B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndChild.h"

/////////////////////////////////////////////////////////////////////////////
// CMusicWnd window

class CMusicWnd : public CChildWnd
{
// Construction
public:
	CMusicWnd();
	virtual ~CMusicWnd();
	
	DECLARE_SERIAL(CMusicWnd)

// Attributes
public:

// Operations
protected:

// Overrides
public:
	//{{AFX_VIRTUAL(CMusicWnd)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CMusicWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDMUSIC_H__FEDB13C1_30BC_449F_BC99_B27A6EACD1B9__INCLUDED_)
