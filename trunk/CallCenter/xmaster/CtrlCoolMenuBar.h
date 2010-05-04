//
// CtrlCoolMenuBar.h
//

#if !defined(AFX_CTRLCOOLMENUBAR_H__26AF9FCF_9131_4943_BC04_ED7562A3A004__INCLUDED_)
#define AFX_CTRLCOOLMENUBAR_H__26AF9FCF_9131_4943_BC04_ED7562A3A004__INCLUDED_

#pragma once

#include "CtrlCoolBar.h"


class CCoolMenuBarCtrl : public CCoolBarCtrl
{
// Construction
public:
	CCoolMenuBarCtrl();
	virtual ~CCoolMenuBarCtrl();

// Attributes
protected:
	CPoint			m_pMouse;
	HMENU			m_hMenu;
	CCoolBarItem*	m_pSelect;

// Operations
public:
	void	SetMenu(HMENU hMenu);
	void	OpenMenuBar();
	BOOL	OpenMenuChar(UINT nChar);
protected:
	void	ShowMenu();
	void	UpdateWindowMenu(CMenu* pMenu);
	void	ShiftMenu(int nOffset);
	BOOL	OnMenuMessage(MSG* pMsg);

// Overrides
public:
	//{{AFX_VIRTUAL(CCoolMenuBarCtrl)
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	//}}AFX_VIRTUAL

// Statics
protected:
	static LRESULT CALLBACK MenuFilter(int nCode, WPARAM wParam, LPARAM lParam);
	static CCoolMenuBarCtrl*	m_pMenuBar;
	static HHOOK				m_hMsgHook;

// Implementation
protected:
	//{{AFX_MSG(CCoolMenuBarCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	afx_msg void OnEnterIdle(UINT nWhy, CWnd* pWho);
	//}}AFX_MSG
	afx_msg void OnEnterMenuLoop(BOOL bIsTrackPopupMenu);
	afx_msg void OnExitMenuLoop(BOOL bIsTrackPopupMenu);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_CTRLCOOLMENUBAR_H__26AF9FCF_9131_4943_BC04_ED7562A3A004__INCLUDED_)
