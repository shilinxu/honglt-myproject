//
// WndNetwork.h
//

#if !defined(AFX_WNDNETWORK_H__60B941D9_B0B2_4F11_A04F_E006A4B94532__INCLUDED_)
#define AFX_WNDNETWORK_H__60B941D9_B0B2_4F11_A04F_E006A4B94532__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndChild.h"
#include "CtrlText.h"
#include "CtrlCoolBar.h"
#include "CtrlNetworkTree.h"
#include "CtrlNetworkChild.h"
#include "CtrlNetworkTreeBar.h"
#include "CtrlNetworkHeaderBar.h"

class CChannel;

/////////////////////////////////////////////////////////////////////////////
// CNetworkWnd window

class CNetworkWnd : public CChildWnd  
{
// Construction
public:
	CNetworkWnd();
	virtual ~CNetworkWnd();
	DECLARE_SERIAL(CNetworkWnd)

// Attributes
protected:
	CNetworkTree	m_wndTree;
	CNetworkTreeBar	m_wndTreeTop;
	CTextCtrl		m_wndText;
	CNetworkChild	m_wndChild;
	CNetworkHeaderBar	m_wndHeaderBar;
protected:
	int				m_nText;
	int				m_nPanel;
	HWND			m_hChildWnd;
	HWND			m_hDefaultChild;

// Operations
public:
	void			Message(int nType, LPCTSTR pszText);
	int				OnUserChanged(LPCTSTR xUserId, CChannel*& pChannel);
	void			SetActivePage(HWND hCtrlChild);
protected:
	BOOL			DoSizeText();
	BOOL			DoSizePanel();
	void			ShowStartupText();

// Overrides
public:
	//{{AFX_VIRTUAL(CNetworkWnd)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CNetworkWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSelectPage(NM_TREEVIEW* pNotify, LRESULT *pResult);
	afx_msg void OnUpdateNetworkRecord(CCmdUI* pCmdUI);
	afx_msg void OnNetworkRecord();
	afx_msg void OnUpdateNetworkRecMask(CCmdUI* pCmdUI);
	afx_msg void OnNetworkRecMask();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define	IDC_NETWORK_TEXT	1001
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDNETWORK_H__60B941D9_B0B2_4F11_A04F_E006A4B94532__INCLUDED_)
