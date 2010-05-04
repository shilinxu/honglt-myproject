//
// CtrlReportPanel.h
//

#if !defined(AFX_CTRLREPORTPANEL_H__1101B74A_75E8_4375_8C06_EBDDB48D3C1C__INCLUDED_)
#define AFX_CTRLREPORTPANEL_H__1101B74A_75E8_4375_8C06_EBDDB48D3C1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CtrlTaskPanel.h"
#include "CtrlIconButton.h"

class CReportTaskBox : public CTaskBox
{
// Construction
public:
	CReportTaskBox();
	virtual ~CReportTaskBox();
	DECLARE_DYNAMIC(CReportTaskBox)

// Attributes
protected:
	CComboBox		m_wndFamily;
	CDateTimeCtrl	m_wndObject;
	CIconButtonCtrl	m_wndStart;
	CIconButtonCtrl	m_wndClear;
	
// Operations
public:
	CString		GetSearch();

// Overrides
public:
	//{{AFX_VIRTUAL(CReportTaskBox)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CReportTaskBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSelChangeFamily();
	afx_msg void OnSearch();
	afx_msg void OnClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CReportFilterBox : public CTaskBox
{
// Construction
public:
	CReportFilterBox();
	virtual ~CReportFilterBox();
	DECLARE_DYNAMIC(CReportFilterBox)

// Attributes
protected:
	CComboBox		m_wndFilter;
	CComboBox		m_wndDialer;
	
// Operations
public:
	CString		GetFilter(BOOL bPeek = FALSE);

// Overrides
public:
	//{{AFX_VIRTUAL(CReportFilterBox)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CReportFilterBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CReportResultBox : public CTaskBox
{
// Construction
public:
	CReportResultBox();
	virtual ~CReportResultBox();
	DECLARE_DYNAMIC(CReportResultBox)

// Attributes
protected:
	CStatic		m_wndTotal;
	CStatic		m_wndIgnore;
	CBrush		m_brBlue;
	
// Operations
public:
	void		ReportCount(int nCount, int nIgnore);

// Overrides
public:
	//{{AFX_VIRTUAL(CReportResultBox)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CReportResultBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CReportPanel window

class CReportPanel : public CTaskPanel
{
// Construction
public:
	CReportPanel();
	virtual ~CReportPanel();
	DECLARE_DYNAMIC(CReportPanel)

// Attributes
public:
	CReportTaskBox		m_wndSearch;
	CReportFilterBox	m_wndFilter;
	CReportResultBox	m_wndResult;

// Operations
public:
	void	ExecuteSearch();
	void	ExecuteClear();
	CString	GetSearch();

// Overrides
public:
	//{{AFX_VIRTUAL(CReportPanel)
	virtual BOOL Create(CWnd* pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CReportPanel)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LONG OnReportSearch(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#define WM_REPORTSEARCH		(WM_USER + 1)
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_REPORT_PANEL		101
#define IDC_REPORT_FAMILY		1001
#define IDC_REPORT_OBJECT		1002
#define IDC_REPORT_FILTER		1003
#define IDC_REPORT_SEARCH		1004
#define IDC_REPORT_CLEAR		1005
#define IDC_REPORT_DIALER		1006
#define ID_REPORT_SEARCH		3801
#define ID_REPORT_CLEAR			3802
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLREPORTPANEL_H__1101B74A_75E8_4375_8C06_EBDDB48D3C1C__INCLUDED_)
