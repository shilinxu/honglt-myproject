//
// WndChild.h
//

#if !defined(AFX_WNDCHILD_H__082500C0_14CD_4066_95F9_DA21AD8C7E72__INCLUDED_)
#define AFX_WNDCHILD_H__082500C0_14CD_4066_95F9_DA21AD8C7E72__INCLUDED_

#pragma once

class CMainWnd;
class CWindowManager;

/////////////////////////////////////////////////////////////////////////////
// CChildWnd frame

class CChildWnd : public CMDIChildWnd
{
// Construction
public:
	CChildWnd(BOOL bFixing = FALSE);
	virtual ~CChildWnd();

	DECLARE_DYNCREATE(CChildWnd)

// Attributes
public:
	UINT		m_nResID;
	HINSTANCE	m_hInstance;
protected:
	CRect		m_rcClose;
	BOOL		m_bPanelClose;
protected:
	COLORREF	m_crPanelBack;
	CBitmap		m_bmPanelMark;
	COLORREF	m_crPanelText;
	COLORREF	m_crPanelBorder;
private:
	static CChildWnd*	m_pCmdMsg;

// Operations
public:
	CMainWnd*		GetMainWnd();
	CWindowManager*	GetManager();
	BOOL			IsActive(BOOL bFocused = FALSE);
protected:
	void			PaintCaption(CDC& dc);

// Overrides
public:
	//{{AFX_VIRTUAL(CChildWnd)
	public:
	virtual BOOL Create(UINT nID, BOOL bVisible = TRUE);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CChildWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnNcPaint();
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#define IDC_PANEL	101

#endif // !defined(AFX_WNDCHILD_H__00A48FD8_064A_4C79_A1E5_DB9209555F96__INCLUDED_)
