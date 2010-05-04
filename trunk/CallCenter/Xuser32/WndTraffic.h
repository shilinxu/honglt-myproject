//
// WndTraffic.h
//

#if !defined(AFX_WNDTRAFFIC_H__B01CB059_198A_4A2C_B15C_857612750AB0__INCLUDED_)
#define AFX_WNDTRAFFIC_H__B01CB059_198A_4A2C_B15C_857612750AB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndChild.h"
#include "CtrlTrafficPanel.h"
#include "CtrlCoolHeaderBar.h"

/////////////////////////////////////////////////////////////////////////////
// CTrafficWnd window

class CTrafficWnd : public CChildWnd  
{
// Construction
public:
	CTrafficWnd();
	virtual ~CTrafficWnd();

	DECLARE_DYNCREATE(CTrafficWnd)

// Attributes
protected:
	CTrafficPanel		m_wndPanel;
	CCoolHeaderBarCtrl	m_wndHeaderBar;
	CCoolBarCtrl	m_wndBottom;

// Operations
protected:

// Overrides
public:
	//{{AFX_VIRTUAL(CTrafficWnd)
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CTrafficWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTrafficAccept();
	afx_msg void OnTrafficReject();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_WNDTRAFFIC_H__B01CB059_198A_4A2C_B15C_857612750AB0__INCLUDED_)
