//
// WindowManager.h
//

#if !defined(AFX_WINDOWMANAGER_H__14BF8F18_C023_4B63_B2D6_29F5DBD9AFAA__INCLUDED_)
#define AFX_WINDOWMANAGER_H__14BF8F18_C023_4B63_B2D6_29F5DBD9AFAA__INCLUDED_

#pragma once

#include "WndChild.h"

/////////////////////////////////////////////////////////////////////////////
// CWindowManager window

class CWindowManager : public CWnd
{
// Construction
public:
	CWindowManager(CMDIFrameWnd* pParent = NULL);
	virtual ~CWindowManager();

// Attributes
public:
	CMDIFrameWnd*		m_pParent;
	CPtrList			m_pWindows;
	CRect				m_rcSize;
	BOOL				m_bIgnoreActivate;
	BOOL				m_bClosing;

// Operations
public:
	void		SetOwner(CMDIFrameWnd* pParent);
	CChildWnd*	GetActive() const;
	POSITION	GetIterator() const;
	CChildWnd*	GetNext(POSITION& pos) const;
	BOOL		Check(CChildWnd* pChild) const;
	CChildWnd*	Find(CRuntimeClass* pClass, CChildWnd* pAfter = NULL, CChildWnd* pExcept = NULL);
	CChildWnd*	Open(CRuntimeClass* pClass, BOOL bFocus = TRUE);
	CChildWnd*	FindFromPoint(const CPoint& point) const;
public:
	void		Close();
	void		AutoResize();
	void		Cascade(BOOL bActiveOnly = FALSE);
protected:
	void		Add(CChildWnd* pChild);
	void		Remove(CChildWnd* pChild);

// Message Map
protected:
	//{{AFX_MSG(CWindowManager)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG

	afx_msg LONG OnWindowCleanup(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	friend class CChildWnd;
};

#endif // !defined(AFX_WINDOWMANAGER_H__14BF8F18_C023_4B63_B2D6_29F5DBD9AFAA__INCLUDED_)
