//
// WndMain.h
//

#if !defined(AFX_WNDMAIN_H__B7029645_0B22_4BD4_B8E1_48491100728B__INCLUDED_)
#define AFX_WNDMAIN_H__B7029645_0B22_4BD4_B8E1_48491100728B__INCLUDED_

#pragma once

#include "WindowManager.h"
#include "CtrlCoolBar.h"
class CXMLElement;

/////////////////////////////////////////////////////////////////////////////
// CMainWnd frame

class CMainWnd : public CMDIFrameWnd
{
// Construction
public:
	CMainWnd();
	virtual ~CMainWnd();

	DECLARE_DYNCREATE(CMainWnd)

// Attributes
public:
	CWindowManager		m_pWindows;
protected:
	CCoolBarCtrl		m_wndToolBar;
	CStatusBar			m_wndStatusBar;
protected:
	BOOL				m_bInTimer;

// Operations
public:
	CChildWnd*		GetWindow(CRuntimeClass* pClass);
protected:
	virtual void	GetMessageString(UINT nID, CString& rMessage) const;
	virtual void	UpdateMessages();
protected:
	virtual BOOL	OnFaultResp(int nError);
protected:
	static void CALLBACK TimerProcA(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime);

// Overrides
public:
	//{{AFX_VIRTUAL(CMainWnd)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CMainWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnToolsSettings();
	afx_msg void OnUpdateWindowToolBar(CCmdUI* pCmdUI);
	afx_msg void OnWindowToolBar();
	afx_msg void OnUpdateWindowStatusBar(CCmdUI* pCmdUI);
	afx_msg void OnWindowStatusBar();
	afx_msg void OnAppAbout();
	afx_msg void OnTabConnect();
	afx_msg void OnTabNetwork();
	afx_msg void OnUpdateTabNetwork(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg LONG OnLog(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnSyncNotify(WPARAM wParam, CXMLElement* pXML);
	DECLARE_MESSAGE_MAP()
};

AFX_INLINE CChildWnd* CMainWnd::GetWindow(CRuntimeClass* pClass)
{
	return m_pWindows.Find( pClass );
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDW_MENU_BAR		0xE810
#define IDW_TOOL_BAR		AFX_IDW_TOOLBAR
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDMAIN_H__B7029645_0B22_4BD4_B8E1_48491100728B__INCLUDED_)
