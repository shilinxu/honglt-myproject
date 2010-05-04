//
// WndInner.h
//

#if !defined(AFX_WNDWAITER_H__6E66B11D_2F06_421D_AC8F_0F9DD9824D01__INCLUDED_)
#define AFX_WNDWAITER_H__6E66B11D_2F06_421D_AC8F_0F9DD9824D01__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndChild.h"
#include "CtrlCoolBar.h"

/////////////////////////////////////////////////////////////////////////////
// CWaiterWnd window

class CWaiterWnd : public CChildWnd  
{
// Construction
public:
	CWaiterWnd();
	virtual ~CWaiterWnd();
	
	DECLARE_SERIAL(CWaiterWnd)
		
// Attributes
public:
	_ConnectionPtr	m_pConnection;
	
// Operations
protected:
	BOOL			OpenDatabase(LPCTSTR xHost, LPCTSTR xCatalog = "XsalesAgency");
	BOOL			CloseDatabase();
	
// Overrides
public:
	//{{AFX_VIRTUAL(CWaiterWnd)
	//}}AFX_VIRTUAL
	
// Implementation
protected:
	//{{AFX_MSG(CWaiterWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg void OnUpdateNetworkConnect(CCmdUI* pCmdUI);
	afx_msg void OnNetworkConnect();
	afx_msg void OnInnerRefresh();
	afx_msg void OnInnerReply();
	//}}AFX_MSG
	afx_msg LONG OnPacketReq(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_USER_NUMBER		1002
#define IDC_USER_PHONE		1003
#define IDC_INNER_LIST		1006
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDWAITER_H__6E66B11D_2F06_421D_AC8F_0F9DD9824D01__INCLUDED_)
