//
// WndHome.h
//

#if !defined(AFX_WNDHOME_H__B01CB059_198A_4A2C_B15C_857612750AB0__INCLUDED_)
#define AFX_WNDHOME_H__B01CB059_198A_4A2C_B15C_857612750AB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndChild.h"
#include "CtrlHomePanel.h"

/////////////////////////////////////////////////////////////////////////////
// CHomeWnd window

class CHomeWnd : public CChildWnd  
{
// Construction
public:
	CHomeWnd();
	virtual ~CHomeWnd();

	DECLARE_DYNCREATE(CHomeWnd)

// Attributes
public:
	CHomePanel	m_wndPanel;

// Operations
public:
	BOOL		StartCapture();

// Overrides
public:
	//{{AFX_VIRTUAL(CHomeWnd)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CHomeWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_USER_NUMBER		101
#define IDC_USER_PASSWORD	1002
#define IDC_SAVE_PASSWORD	1003
#define IDC_USER_LOGIN		1004
#define IDC_USER_LOGOUT		1005

#endif // !defined(AFX_WNDHOME_H__B01CB059_198A_4A2C_B15C_857612750AB0__INCLUDED_)
