//
// WndMusic.h
//

#if !defined(AFX_WNDMUSIC_H__FEDB13C1_30BC_449F_BC99_B27A6EACD1B9__INCLUDED_)
#define AFX_WNDMUSIC_H__FEDB13C1_30BC_449F_BC99_B27A6EACD1B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndChild.h"
#include "CtrlMusicPanel.h"

/////////////////////////////////////////////////////////////////////////////
// CMusicWnd window

class CMusicWnd : public CChildWnd
{
// Construction
public:
	CMusicWnd();
	virtual ~CMusicWnd();
	
	DECLARE_SERIAL(CMusicWnd)

// Attributes
public:
	CMusicPanel		m_wndPanel;
	CListCtrl		m_wndList;
protected:
	_RecordsetPtr	m_pRecordset;

// Operations
protected:
	int		LoadRecordset(LPCTSTR pszFilter, LPCTSTR pszGroup = NULL);
	void	LoadRecord(int nItem, LPCTSTR pszGroup = NULL);

// Overrides
public:
	//{{AFX_VIRTUAL(CMusicWnd)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CMusicWnd)
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

#endif // !defined(AFX_WNDMUSIC_H__FEDB13C1_30BC_449F_BC99_B27A6EACD1B9__INCLUDED_)
