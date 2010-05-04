//
// WndMain.h
//

#if !defined(AFX_WNDMAIN_H__B7029645_0B22_4BD4_B8E1_48491100728B__INCLUDED_)
#define AFX_WNDMAIN_H__B7029645_0B22_4BD4_B8E1_48491100728B__INCLUDED_

#pragma once

#include "WindowManager.h"

/////////////////////////////////////////////////////////////////////////////
// CMainWnd frame

class CMainWnd : public CMDIFrameWnd
{
// Construction
public:
	CMainWnd();
	virtual ~CMainWnd();

	DECLARE_DYNCREATE(CMainWnd)

// Attributes
public:
	CWindowManager		m_pWindows;
protected:
	CStatusBar			m_wndStatusBar;

// Operations
public:

// Overrides
public:
	//{{AFX_VIRTUAL(CMainWnd)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CMainWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnNetworkConnect();
	afx_msg void OnToolsSettings();
	afx_msg void OnHelpUpdate();
	afx_msg void OnAppAbout();
	afx_msg void OnTabConnect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDMAIN_H__B7029645_0B22_4BD4_B8E1_48491100728B__INCLUDED_)
