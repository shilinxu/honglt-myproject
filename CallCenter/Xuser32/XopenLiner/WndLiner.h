//
// WndLiner.h
//

#if !defined(AFX_WNDLINER_H__6E66B11D_2F06_421D_AC8F_0F9DD9824D01__INCLUDED_)
#define AFX_WNDLINER_H__6E66B11D_2F06_421D_AC8F_0F9DD9824D01__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndChild.h"
#include "CtrlLinerPanel.h"

/////////////////////////////////////////////////////////////////////////////
// CLinerWnd window

class CLinerWnd : public CChildWnd  
{
// Construction
public:
	CLinerWnd();
	virtual ~CLinerWnd();
	
	DECLARE_SERIAL(CLinerWnd)
		
// Attributes
public:
protected:
	CLinerPanel		m_wndPanel;
	
// Operations
public:
	
// Overrides
public:
	//{{AFX_VIRTUAL(CLinerWnd)
	//}}AFX_VIRTUAL
	
// Implementation
protected:
	//{{AFX_MSG(CLinerWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_WNDLINER_H__6E66B11D_2F06_421D_AC8F_0F9DD9824D01__INCLUDED_)
