//
// CtrlP2jobPanel.h
//

#if !defined(AFX_CTRLP2JOBPANEL_H__089F2565_529B_4BC3_BA2A_7B8BDA135D4B__INCLUDED_)
#define AFX_CTRLP2JOBPANEL_H__089F2565_529B_4BC3_BA2A_7B8BDA135D4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CtrlTaskPanel.h"
#include "CtrlIconButton.h"

class CP2jobTaskBox : public CTaskBox
{
// Construction
public:
	CP2jobTaskBox();
	virtual ~CP2jobTaskBox();
	DECLARE_DYNAMIC(CP2jobTaskBox)

// Attributes
protected:
	CDateTimeCtrl	m_wndPagStart;
	CDateTimeCtrl	m_wndPagClose;
	CIconButtonCtrl	m_wndStart;
	CIconButtonCtrl	m_wndClear;
	
// Operations
public:

// Overrides
public:
	//{{AFX_VIRTUAL(CP2jobTaskBox)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CP2jobTaskBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnStart();
	afx_msg void OnClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CP2jobPanel : public CTaskPanel
{
// Construction
public:
	CP2jobPanel();
	virtual ~CP2jobPanel();
	DECLARE_DYNAMIC(CP2jobPanel)

// Attributes
protected:
	CP2jobTaskBox		m_wndTask;

// Operations
public:

// Overrides
public:
	//{{AFX_VIRTUAL(CP2jobPanel)
	virtual BOOL Create(CWnd* pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CP2jobPanel)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_P2JOB_PANEL		101
#define IDC_TIME_START		1001
#define IDC_TIME_CLOSE		1002
#define IDC_P2JOB_START		1003
#define IDC_P2JOB_CLEAR		1004
#define ID_P2JOB_START		3801
#define ID_P2JOB_CLEAR		3802
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLP2JOBPANEL_H__089F2565_529B_4BC3_BA2A_7B8BDA135D4B__INCLUDED_)
