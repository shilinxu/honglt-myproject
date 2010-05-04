//
// WndReport.h
//

#if !defined(AFX_WNDREPORT_H__FEDB13C1_30BC_449F_BC99_B27A6EACD1B9__INCLUDED_)
#define AFX_WNDREPORT_H__FEDB13C1_30BC_449F_BC99_B27A6EACD1B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndChild.h"
#include "CtrlReportPanel.h"
#include "CtrlReportList.h"

/////////////////////////////////////////////////////////////////////////////
// CReportWnd window

class CReportWnd : public CChildWnd
{
// Construction
public:
	CReportWnd();
	virtual ~CReportWnd();
	
	DECLARE_SERIAL(CReportWnd)

// Attributes
public:
	CReportPanel		m_wndPanel;
	CReportListCtrl		m_wndList;

// Operations
protected:

// Overrides
public:
	//{{AFX_VIRTUAL(CReportWnd)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CReportWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnReportPrint();
	afx_msg void OnReportSearch();
	afx_msg void OnReportClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define	IDC_REPORT_LIST	1001
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDREPORT_H__FEDB13C1_30BC_449F_BC99_B27A6EACD1B9__INCLUDED_)
