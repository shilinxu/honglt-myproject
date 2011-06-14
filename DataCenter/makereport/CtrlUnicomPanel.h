//
// CtrlUnicomPanel.h
//

#if !defined(AFX_CTRLUNICOMPANEL_H__089F2565_529B_4BC3_BA2A_7B8BDA135D4B__INCLUDED_)
#define AFX_CTRLUNICOMPANEL_H__089F2565_529B_4BC3_BA2A_7B8BDA135D4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CtrlTaskPanel.h"
#include "CtrlIconButton.h"
#include "EDClock.h"

class CUnicomTaskBox : public CTaskBox
{
	friend class CUnicomPanel;
	
// Construction
public:
	CUnicomTaskBox();
	virtual ~CUnicomTaskBox();
	DECLARE_DYNAMIC(CUnicomTaskBox)
		
// Attributes
protected:
	CComboBox		m_wndMode;
	CStatic			m_wndProfile;
	CIconButtonCtrl	m_wndStart;
	CIconButtonCtrl	m_wndClear;
	COLORREF	m_crClient;
	CBrush		m_brClient;

// Operations
protected:
	
// Overrides
public:
	//{{AFX_VIRTUAL(CUnicomTaskBox)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CUnicomTaskBox)
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

class CUnicomResultBox : public CTaskBox
{
	friend class CUnicomPanel;
	friend class CUnicomTaskBox;
	
// Construction
public:
	CUnicomResultBox();
	virtual ~CUnicomResultBox();
	DECLARE_DYNAMIC(CUnicomResultBox)

// Attributes
protected:
	CEDClock		m_wndClock;
	
// Operations
protected:

// Overrides
public:
	//{{AFX_VIRTUAL(CUnicomResultBox)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CUnicomResultBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg LONG OnTimerOut(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CUnicomPanel : public CTaskPanel
{
	friend class CUnicomWnd;
	
// Construction
public:
	CUnicomPanel();
	virtual ~CUnicomPanel();
	DECLARE_DYNAMIC(CUnicomPanel)

// Attributes
public:
	CUnicomTaskBox		m_wndTask;
	CUnicomResultBox	m_wndResult;

// Operations
protected:
	void		ExecuteStart();
	void		ExecuteClear();
	void		ExecuteFlowBack(DWORD nFlowCount);

// Overrides
public:
	//{{AFX_VIRTUAL(CUnicomPanel)
	virtual BOOL Create(CWnd* pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CUnicomPanel)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_UNICOM_PANEL	101
#define IDC_UNICOM_MODE		1001
#define IDC_UNICOM_START	1005
#define IDC_UNICOM_CLEAR	1006
#define IDC_UNICOM_CLOCK	1007
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLUNICOMPANEL_H__089F2565_529B_4BC3_BA2A_7B8BDA135D4B__INCLUDED_)
