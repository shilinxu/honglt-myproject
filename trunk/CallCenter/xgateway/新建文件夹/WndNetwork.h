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
class IUserPacket;
class IExitPacket;

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
public:
	CNetworkTree	m_wndTree;
	CNetworkTreeBar	m_wndTreeTop;
	CNetworkHeaderBar	m_wndHeaderBar;
	CNetworkChild	m_wndChild;
	CTextCtrl		m_wndText;
protected:
	int				m_nText;
	int				m_nPanel;
	HWND			m_hChildWnd;
	HWND			m_hDefaultChild;

// Operations
public:
	void			Message(int nType, LPCTSTR pszText);
	int				BindToUser(CChannel* pChannel, LPCTSTR xUserId);
public:
	void			SelectChild(HWND hCtrlChild);
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
	afx_msg void OnUpdateNetworkVideo(CCmdUI* pCmdUI);
	afx_msg void OnNetworkVideo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define	IDC_NETWORK_TEXT	1001
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDNETWORK_H__60B941D9_B0B2_4F11_A04F_E006A4B94532__INCLUDED_)
