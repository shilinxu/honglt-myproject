//
// CtrlNetworkPanel.h
//

#if !defined(AFX_CTRLNETWORKPANEL_H__D98652AB_D857_4582_AB42_6CCFDAAB3F6E__INCLUDED_)
#define AFX_CTRLNETWORKPANEL_H__D98652AB_D857_4582_AB42_6CCFDAAB3F6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CtrlTaskPanel.h"
#include "HisReport.h"

class CNetworkTaskBox : public CTaskBox
{
// Construction
public:
	CNetworkTaskBox();
	virtual ~CNetworkTaskBox();
	DECLARE_DYNAMIC(CNetworkTaskBox)
		
// Attributes
protected:
	CStatic		m_wndToday;
	CStatic		m_wndMonth;
	CStatic		m_wndSearch;
	COLORREF	m_crWhite;
	CBrush		m_brWhite;
	CHisReport	m_pReport;
	DWORD		m_tLastChk;

// Operations
public:
	void		Register(int nInk = 1);
protected:
	BOOL		LoadReport();
	
// Overrides
public:
	//{{AFX_VIRTUAL(CNetworkTaskBox)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CNetworkTaskBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CNetworkTextBox : public CTaskBox
{
// Construction
public:
	CNetworkTextBox();
	virtual ~CNetworkTextBox();
	DECLARE_DYNAMIC(CNetworkTextBox)
		
// Attributes
protected:
	CStatic		m_wndOpen;
	CStatic		m_wndClear;
	COLORREF	m_crWhite;
	CBrush		m_brWhite;

// Operations
public:
	
// Overrides
public:
	//{{AFX_VIRTUAL(CNetworkTextBox)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CNetworkTextBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CNetworkPanel : public CTaskPanel
{
// Construction
public:
	CNetworkPanel();
	virtual ~CNetworkPanel();
	DECLARE_DYNAMIC(CNetworkPanel)

// Attributes
public:
	CNetworkTaskBox	m_wndTask;
	CNetworkTextBox	m_wndText;

// Operations
public:
	void		ExecuteCopy();
	void		ExecuteClear();

// Overrides
public:
	//{{AFX_VIRTUAL(CNetworkPanel)
	virtual BOOL Create(CWnd* pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CNetworkPanel)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_NETWORK_PANEL		101
#define IDC_NETWORK_TASK		102
//#define IDC_TASK_TODAY		1001
//#define IDC_TASK_MONTH		1002
#define ID_TEXT_OPEN		3801
#define ID_TEXT_CLEAR		3802
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLNETWORKPANEL_H__D98652AB_D857_4582_AB42_6CCFDAAB3F6E__INCLUDED_)
