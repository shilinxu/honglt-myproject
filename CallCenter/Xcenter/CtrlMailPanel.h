//
// CtrlMailPanel.h
//

#if !defined(AFX_CTRLMAILPANEL_H__089F2565_529B_4BC3_BA2A_7B8BDA135D4B__INCLUDED_)
#define AFX_CTRLMAILPANEL_H__089F2565_529B_4BC3_BA2A_7B8BDA135D4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CtrlTaskPanel.h"
#include "CtrlIconButton.h"

class CMailTaskBox : public CTaskBox
{
	friend class CMailPanel;
	
// Construction
public:
	CMailTaskBox();
	virtual ~CMailTaskBox();
	DECLARE_DYNAMIC(CMailTaskBox)
		
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
	//{{AFX_VIRTUAL(CMailTaskBox)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CMailTaskBox)
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

class CMailPanel : public CTaskPanel
{
// Construction
public:
	CMailPanel();
	virtual ~CMailPanel();
	DECLARE_DYNAMIC(CMailPanel)

// Attributes
protected:
	CMailTaskBox	m_wndTask;

// Operations
public:
	int			ReportItem(LPCTSTR xCatalog);

// Overrides
public:
	//{{AFX_VIRTUAL(CMailPanel)
	virtual BOOL Create(CWnd* pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CMailPanel)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_MAIL_PANEL		101
#define IDC_MAIL_LIST		1001
#define IDC_MAIL_CLEAR		1002
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLMAILPANEL_H__089F2565_529B_4BC3_BA2A_7B8BDA135D4B__INCLUDED_)
