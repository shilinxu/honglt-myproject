//
// WndAgency.h
//

#if !defined(AFX_WNDAGENCY_H__FEDB13C1_30BC_449F_BC99_B27A6EACD1B9__INCLUDED_)
#define AFX_WNDAGENCY_H__FEDB13C1_30BC_449F_BC99_B27A6EACD1B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndChild.h"
#include "CtrlAgencyPanel.h"
#include "CtrlAgencyList.h"
#include "CtrlIconCombo.h"
#include "CtrlCoolBar.h"

/////////////////////////////////////////////////////////////////////////////
// CAgencyWnd window

class CAgencyWnd : public CChildWnd
{
// Construction
public:
	CAgencyWnd();
	virtual ~CAgencyWnd();
	
	DECLARE_SERIAL(CAgencyWnd)

// Attributes
public:
	_ConnectionPtr	m_pConnection;
protected:
	CAgencyPanel	m_wndPanel;
	CAgencyList		m_wndList;
	CIconComboCtrl	m_wndFamily;
	CCoolBarCtrl	m_wndHeaderBar;
	CCoolBarCtrl	m_wndBottom;
	CEdit			m_wndPhone;

// Operations
public:
	BOOL			LayoutJobs(CXMLElement* pXML);
protected:
	BOOL			LayoutJobs(CXMLElement* pXML, LPCTSTR lpszKey);
protected:
	BOOL			OpenDatabase(LPCTSTR xHost, LPCTSTR xCatalog = "XsalesAgency");
	BOOL			CloseDatabase();
	BOOL			OnSyncOrderRelation();

// Overrides
public:
	//{{AFX_VIRTUAL(CAgencyWnd)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAgencyWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg void OnSelChangeFamily();
	afx_msg void OnUpdateAgencyProperties(CCmdUI* pCmdUI);
	afx_msg void OnAgencyProperties();
	afx_msg void OnAgencyRefresh();
	afx_msg void OnUpdateAgencyLayout(CCmdUI* pCmdUI);
	afx_msg void OnAgencyLayout();
	afx_msg void OnAgencyXquick();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_AGENCY_FAMILY	1001
#define IDC_USER_NUMBER		1002
#define IDC_AGENCY_LIST		1006
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDAGENCY_H__FEDB13C1_30BC_449F_BC99_B27A6EACD1B9__INCLUDED_)
