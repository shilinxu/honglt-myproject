//
// WndNetwork.h
//

#if !defined(AFX_WNDNETWORK_H__B01CB059_198A_4A2C_B15C_857612750AB0__INCLUDED_)
#define AFX_WNDNETWORK_H__B01CB059_198A_4A2C_B15C_857612750AB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndChild.h"
#include "CtrlCoolHeaderBar.h"
#include "CtrlNetworkThumb.h"

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
	CCoolHeaderBarCtrl	m_wndHeaderBar;
	CNetworkThumb		m_wndThumb;

// Operations
protected:
	virtual void	OnSyncOrderRelation();
protected:
	virtual BOOL	OnSyncOrderRelation(CXMLElement* pXML);
	virtual BOOL	OnSyncOrderRelation(CXMLElement* pXML, DWORD);

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
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_WNDNETWORK_H__B01CB059_198A_4A2C_B15C_857612750AB0__INCLUDED_)
