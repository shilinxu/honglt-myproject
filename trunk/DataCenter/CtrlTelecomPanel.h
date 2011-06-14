//
// CtrlTelecomPanel.h
//

#if !defined(AFX_CTRLTELECOMPANEL_H__089F2565_529B_4BC3_BA2A_7B8BDA135D4B__INCLUDED_)
#define AFX_CTRLTELECOMPANEL_H__089F2565_529B_4BC3_BA2A_7B8BDA135D4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CtrlTaskPanel.h"
#include "CtrlIconButton.h"
#include "EDClock.h"

class CTelecomTaskBox : public CTaskBox
{
	friend class CTelecomPanel;
	
// Construction
public:
	CTelecomTaskBox();
	virtual ~CTelecomTaskBox();
	DECLARE_DYNAMIC(CTelecomTaskBox)
		
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
	void		ExecuteContinue();
	
// Overrides
public:
	//{{AFX_VIRTUAL(CTelecomTaskBox)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CTelecomTaskBox)
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

class CTelecomResultBox : public CTaskBox
{
	friend class CTelecomPanel;
	friend class CTelecomTaskBox;
	
// Construction
public:
	CTelecomResultBox();
	virtual ~CTelecomResultBox();
	DECLARE_DYNAMIC(CTelecomResultBox)

// Attributes
protected:
	CEDClock		m_wndClock;
	
// Operations
protected:

// Overrides
public:
	//{{AFX_VIRTUAL(CTelecomResultBox)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CTelecomResultBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg LONG OnTimerOut(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CTelecomPanel : public CTaskPanel
{
	friend class CTelecomWnd;
	
// Construction
public:
	CTelecomPanel();
	virtual ~CTelecomPanel();
	DECLARE_DYNAMIC(CTelecomPanel)

// Attributes
public:
	CTelecomTaskBox		m_wndTask;
	CTelecomResultBox	m_wndResult;

// Operations
protected:
	void		ExecuteStart();
	void		ExecuteClear();
	void		ExecuteNextMonth();
	void		ExecuteFlowBack(DWORD nFlowCount);

// Overrides
public:
	//{{AFX_VIRTUAL(CTelecomPanel)
	virtual BOOL Create(CWnd* pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CTelecomPanel)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LONG OnNextMonth(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//
// User Messages
//

#define WM_NEXTMONTH		(WM_USER + 1)

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_TELECOM_PANEL	101
#define IDC_TELECOM_MODE	1001
#define IDC_TELECOM_START	1005
#define IDC_TELECOM_CLEAR	1006
#define IDC_TELECOM_CLOCK	1007
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLTELECOMPANEL_H__089F2565_529B_4BC3_BA2A_7B8BDA135D4B__INCLUDED_)
