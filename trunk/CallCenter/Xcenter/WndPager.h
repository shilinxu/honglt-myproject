//
// WndPager.h
//

#if !defined(AFX_WNDPAGER_H__C9270841_1D88_457F_B84C_A10ED5631ADD__INCLUDED_)
#define AFX_WNDPAGER_H__C9270841_1D88_457F_B84C_A10ED5631ADD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndChild.h"
#include "CtrlPagerPanel.h"
#include "CtrlCoolBar.h"

/////////////////////////////////////////////////////////////////////////////
// CPagerWnd window

class CPagerWnd : public CChildWnd
{
// Construction
public:
	CPagerWnd();
	virtual ~CPagerWnd();
	
	DECLARE_SERIAL(CPagerWnd)

// Attributes
public:
	CPagerPanel		m_wndPanel;
	CListCtrl		m_wndList;
	CCoolBarCtrl	m_wndToolBar;
	CEdit			m_wndSearch;
protected:
	_RecordsetPtr	m_pRecordset;
	BOOL			m_bIsOpen;

// Operations
public:
	BOOL			AddItem(DWORD nTaskId, LPCTSTR xChState = NULL);
protected:
	BOOL			OnRunPager();
	BOOL			DoRunPager();
	BOOL			VerifyRecordset();
//	CString			ReadPagerType(LPCTSTR xPath, LPCTSTR xDefault) const;

// Overrides
	//{{AFX_VIRTUAL(CPagerWnd)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CPagerWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSearchNumber();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	friend class CSettingsManagerDlg;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_SEARCH_NUMBER		1002
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDPAGER_H__C9270841_1D88_457F_B84C_A10ED5631ADD__INCLUDED_)
