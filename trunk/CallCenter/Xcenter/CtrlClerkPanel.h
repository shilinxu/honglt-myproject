//
// CtrlClerkPanel.h
//

#if !defined(AFX_CTRLCLERKPANEL_H__089F2565_529B_4BC3_BA2A_7B8BDA135D4B__INCLUDED_)
#define AFX_CTRLCLERKPANEL_H__089F2565_529B_4BC3_BA2A_7B8BDA135D4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CtrlTaskPanel.h"
#include "CtrlIconButton.h"
#include "EDClock.h"

class CClerkTaskBox : public CTaskBox
{
// Construction
public:
	CClerkTaskBox();
	virtual ~CClerkTaskBox();
	DECLARE_DYNAMIC(CClerkTaskBox)

// Attributes
protected:
	CComboBox		m_wndMode;
	CStatic			m_wndProfile;
	CIconButtonCtrl	m_wndStart;
	CIconButtonCtrl	m_wndClear;
protected:
	COLORREF		m_crClient;
	CBrush			m_brClient;
	
// Operations
public:

// Overrides
public:
	//{{AFX_VIRTUAL(CClerkTaskBox)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CClerkTaskBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnStart();
	afx_msg void OnClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CClerkResultBox : public CTaskBox
{
	friend class CClerkPanel;
	friend class CClerkTaskBox;
	
// Construction
public:
	CClerkResultBox();
	virtual ~CClerkResultBox();
	DECLARE_DYNAMIC(CClerkResultBox)

// Attributes
protected:
	CEDClock		m_wndClock;
	
// Operations
protected:

// Overrides
public:
	//{{AFX_VIRTUAL(CClerkResultBox)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CClerkResultBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg LONG OnTimerOut(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CClerkPanel : public CTaskPanel
{
	friend class CClerkWnd;
	
// Construction
public:
	CClerkPanel();
	virtual ~CClerkPanel();
	DECLARE_DYNAMIC(CClerkPanel)

// Attributes
protected:
	CClerkTaskBox		m_wndTask;
	CClerkResultBox		m_wndResult;

// Operations
public:
	void		ExecuteStart();
	void		ExecuteClear();
	void		ExecuteFlowBack(DWORD nFlowCount);

// Overrides
public:
	//{{AFX_VIRTUAL(CClerkPanel)
	virtual BOOL Create(CWnd* pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CClerkPanel)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//
// User Messages
//

#define WM_NEXTTIMER		(WM_USER + 1)

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_CLERK_PANEL		101
#define IDC_CLERK_MODE		1001
#define IDC_CLERK_START		1003
#define IDC_CLERK_CLEAR		1004
#define IDC_CLERK_CLOCK		1007
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLCLERKPANEL_H__089F2565_529B_4BC3_BA2A_7B8BDA135D4B__INCLUDED_)
