//
// WndNetwork.h
//

#if !defined(AFX_WNDNETWORK_H__31D5245D_A09E_45BB_BFAA_E88747B67C9B__INCLUDED_)
#define AFX_WNDNETWORK_H__31D5245D_A09E_45BB_BFAA_E88747B67C9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndChild.h"
#include "CtrlText.h"
#include "CtrlNetworkPanel.h"
#include "CtrlNetworkChild.h"

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
	CTextCtrl		m_wndText;
	CNetworkPanel	m_wndPanel;
	CNetworkChild	m_wndChild;
protected:
	int				m_nText;

// Operations
public:
	BOOL			Connect();
	void			Disconnect();
	void			Message(int nType, LPCTSTR pszText);
	int				Attach(CChannel* pChannel);
	void			Detach(CChannel* pChannel);
public:
protected:
	BOOL			DoSizeText();
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
	afx_msg void OnTextOpen();
	afx_msg void OnTextClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define	IDC_NETWORK_TEXT	1001
#define	IDC_NETWORK_CHILD	2002
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDNETWORK_H__31D5245D_A09E_45BB_BFAA_E88747B67C9B__INCLUDED_)
