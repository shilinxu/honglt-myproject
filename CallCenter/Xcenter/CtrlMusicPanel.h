//
// CtrlMusicPanel.h
//

#if !defined(AFX_CTRLMUSICPANEL_H__1101B74A_75E8_4375_8C06_EBDDB48D3C1C__INCLUDED_)
#define AFX_CTRLMUSICPANEL_H__1101B74A_75E8_4375_8C06_EBDDB48D3C1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CtrlTaskPanel.h"
#include "CtrlIconButton.h"

class CMusicTaskBox : public CTaskBox
{
// Construction
public:
	CMusicTaskBox();
	virtual ~CMusicTaskBox();
	DECLARE_DYNAMIC(CMusicTaskBox)

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
	//{{AFX_VIRTUAL(CMusicTaskBox)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CMusicTaskBox)
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

class CMusicFilterBox : public CTaskBox
{
// Construction
public:
	CMusicFilterBox();
	virtual ~CMusicFilterBox();
	DECLARE_DYNAMIC(CMusicFilterBox)

// Attributes
protected:
	CComboBox		m_wndFilter;
	CComboBox		m_wndOrder;
	
// Operations
public:
	CString		GetFilter(CString& xGroup, BOOL bPeek = FALSE);

// Overrides
public:
	//{{AFX_VIRTUAL(CMusicFilterBox)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CMusicFilterBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CMusicResultBox : public CTaskBox
{
// Construction
public:
	CMusicResultBox();
	virtual ~CMusicResultBox();
	DECLARE_DYNAMIC(CMusicResultBox)

// Attributes
protected:
	CStatic		m_wndTotal;
	CBrush		m_brBlue;
	
// Operations
public:
	void		ReportCount(int nCount);

// Overrides
public:
	//{{AFX_VIRTUAL(CMusicResultBox)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CMusicResultBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CMusicPanel window

class CMusicPanel : public CTaskPanel
{
// Construction
public:
	CMusicPanel();
	virtual ~CMusicPanel();
	DECLARE_DYNAMIC(CMusicPanel)

// Attributes
public:
	CMusicTaskBox	m_wndSearch;
	CMusicFilterBox	m_wndFilter;
	CMusicResultBox	m_wndResult;

// Operations
public:
	void			ExecuteSearch();
	void			ExecuteClear();
	CString			GetSearch(CString& strGroup);

// Overrides
public:
	//{{AFX_VIRTUAL(CMusicPanel)
	virtual BOOL Create(CWnd* pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CMusicPanel)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LONG OnReportSearch(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#define WM_REPORTSEARCH		(WM_USER + 1)
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_MUSIC_PANEL		101
#define IDC_MUSIC_FAMILY	1001
#define IDC_MUSIC_OBJECT	1002
#define IDC_MUSIC_FILTER	1003
#define IDC_MUSIC_SEARCH	1004
#define IDC_MUSIC_CLEAR		1005
#define IDC_MUSIC_ORDER		1006
#define ID_MUSIC_SEARCH		3801
#define ID_MUSIC_CLEAR		3802
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLMUSICPANEL_H__1101B74A_75E8_4375_8C06_EBDDB48D3C1C__INCLUDED_)
