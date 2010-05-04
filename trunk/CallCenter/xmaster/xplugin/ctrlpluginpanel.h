//
// CtrlPluginPanel.h
//

#if !defined(AFX_CTRLPLUGINPANEL_H__089F2565_529B_4BC3_BA2A_7B8BDA135D4B__INCLUDED_)
#define AFX_CTRLPLUGINPANEL_H__089F2565_529B_4BC3_BA2A_7B8BDA135D4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CtrlTaskPanel.h"

class CPluginTaskBox : public CTaskBox
{
// Construction
public:
	CPluginTaskBox();
	virtual ~CPluginTaskBox();
	DECLARE_DYNAMIC(CPluginTaskBox)
		
// Attributes
protected:
	CStatic		m_wndUsrTime;
	CStatic		m_wndPayTime;
	CStatic		m_wndSetting;
	COLORREF	m_crWhite;
	CBrush		m_brWhite;

// Operations
public:
	void		UpdateSettings();
	
// Overrides
public:
	//{{AFX_VIRTUAL(CPluginTaskBox)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CPluginTaskBox)
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

class CPluginPanel : public CTaskPanel
{
// Construction
public:
	CPluginPanel();
	virtual ~CPluginPanel();
	DECLARE_DYNAMIC(CPluginPanel)

// Attributes
protected:
	CPluginTaskBox	m_wndTask;

// Operations
public:

// Overrides
public:
	//{{AFX_VIRTUAL(CPluginPanel)
	virtual BOOL Create(CWnd* pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CPluginPanel)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_PLUGIN_PANEL		101
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLPLUGINPANEL_H__089F2565_529B_4BC3_BA2A_7B8BDA135D4B__INCLUDED_)
