//
// WndNetwork.h
//

#if !defined(AFX_WNDNETWORK_H__B01CB059_198A_4A2C_B15C_857612750AB0__INCLUDED_)
#define AFX_WNDNETWORK_H__B01CB059_198A_4A2C_B15C_857612750AB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndChild.h"
#include "CtrlText.h"
#include "CtrlNetworkPanel.h"
#include "CtrlNetworkHeaderBar.h"
#include "CtrlNetworkChild.h"
#include "CtrlNetworkMonitor.h"

class CMonitorChannel;
class CXMLElement;
/////////////////////////////////////////////////////////////////////////////
// CNetworkWnd window

class CNetworkWnd : public CChildWnd  
{
// Construction
public:
	CNetworkWnd();
	virtual ~CNetworkWnd();

	DECLARE_DYNCREATE(CNetworkWnd)

// Attributes
public:
	CTextCtrl			m_wndText;
	CNetworkPanel		m_wndPanel;
	CNetworkChild		m_wndChild;
	CNetworkHeaderBar	m_wndHeaderBar;
protected:
	CMonitorChannel*	m_pMonitor;
	int					m_nText;

// Operations
public:
	void			Message(int nType, LPCTSTR pszText);
	BOOL			LayoutThumb(CXMLElement* pXML);
protected:
	BOOL			DoSizeText();
	void			ShowStartupText();

// Overrides
public:
	//{{AFX_VIRTUAL(CNetworkWnd)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
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
		
	friend class CNetworkHeaderBar;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define	IDC_NETWORK_CHILD	1002
#define	IDC_NETWORK_TEXT	1001

#endif // !defined(AFX_WNDNETWORK_H__B01CB059_198A_4A2C_B15C_857612750AB0__INCLUDED_)
