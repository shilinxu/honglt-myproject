//
// CtrlPagerPanel.h
//

#if !defined(AFX_CTRLPAGERPANEL_H__089F2565_529B_4BC3_BA2A_7B8BDA135D4B__INCLUDED_)
#define AFX_CTRLPAGERPANEL_H__089F2565_529B_4BC3_BA2A_7B8BDA135D4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CtrlTaskPanel.h"
#include "CtrlIconButton.h"

class CPagerTaskBox : public CTaskBox
{
	friend class CPagerPanel;
	
// Construction
public:
	CPagerTaskBox();
	virtual ~CPagerTaskBox();
	DECLARE_DYNAMIC(CPagerTaskBox)
		
// Attributes
protected:
	CListCtrl		m_wndList;
	CIconButtonCtrl	m_wndClear;
protected:
	DWORD				m_tToday;
	CCriticalSection	m_pSection;

// Operations
protected:
	int			ReportItem(LPCTSTR xCatalog);
	
// Overrides
public:
	//{{AFX_VIRTUAL(CPagerTaskBox)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CPagerTaskBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CPagerPanel : public CTaskPanel
{
// Construction
public:
	CPagerPanel();
	virtual ~CPagerPanel();
	DECLARE_DYNAMIC(CPagerPanel)

// Attributes
protected:
	CPagerTaskBox	m_wndTask;

// Operations
public:
	int			ReportItem(LPCTSTR xCatalog);

// Overrides
public:
	//{{AFX_VIRTUAL(CPagerPanel)
	virtual BOOL Create(CWnd* pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CPagerPanel)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_PAGER_PANEL		101
#define IDC_PAGER_LIST		1001
#define IDC_PAGER_CLEAR		1002
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLPAGERPANEL_H__089F2565_529B_4BC3_BA2A_7B8BDA135D4B__INCLUDED_)
