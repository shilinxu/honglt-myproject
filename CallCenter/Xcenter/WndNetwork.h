//
// WndNetwork.h
//

#if !defined(AFX_WNDNETWORK_H__31D5245D_A09E_45BB_BFAA_E88747B67C9B__INCLUDED_)
#define AFX_WNDNETWORK_H__31D5245D_A09E_45BB_BFAA_E88747B67C9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CChannel;

#include "WndChild.h"
#include "CtrlText.h"
#include "CtrlNetworkTree.h"
#include "CtrlNetworkChild.h"

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
	CTextCtrl			m_wndText;
	CNetworkTree		m_wndTree;
	CNetworkChild		m_wndChild;
protected:
	int					m_nPanel;
	int					m_nChild;
	HWND				m_hChildWnd;
	HWND				m_hDefaultChild;

// Operations
public:
	BOOL			Connect();
	void			Disconnect();
	void			Message(int nType, LPCTSTR pszText);
protected:
	BOOL			DoSizePanel();
	BOOL			DoSizeSession();
	void			ShowStartupText();
	void			PaintPanelHeader(CDC& dc, CRect& rcBar);
public:
	void			SelectChild(HWND hCtrlChild);
	CNetworkChild*	AddChannel(CNetworkChild* pCtrlChild, CChannel* pChannel);
	CChannel*		FindChannel(int nChType, int nChState = -1) const;
	int				GetChStateCount(int nChType, int nChState) const;
	CNetworkChild*	FindChild(int nChType = -1, HTREEITEM* pItem = NULL) const;

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
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define	IDC_NETWORK_TEXT	1001
#define	IDC_NETWORK_TREE	1002
#define	IDC_NETWORK_CHILD	1003
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDNETWORK_H__31D5245D_A09E_45BB_BFAA_E88747B67C9B__INCLUDED_)
