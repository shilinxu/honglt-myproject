//
// WndAgency.h
//

#if !defined(AFX_WNDAGENCY_H__FEDB13C1_30BC_449F_BC99_B27A6EACD1B9__INCLUDED_)
#define AFX_WNDAGENCY_H__FEDB13C1_30BC_449F_BC99_B27A6EACD1B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndChild.h"
#include "CtrlAgencyList.h"
#include "CtrlAgencyPanel.h"
#include "CtrlCoolHeaderBar.h"
#include "CtrlCoolBar.h"

using namespace Excel ;

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
	LPTSTR			m_xsaleinfo;
protected:
	CAgencyPanel	m_wndPanel;
	CAgencyList		m_wndList;
	CCoolHeaderBarCtrl	m_wndHeaderBar;
	CCoolBarCtrl	m_wndBottom;
	CEdit			m_wndPhone;

// Operations
protected:
	BOOL			ReadGroupInfo(LPCTSTR lpszFilter, LPTSTR& xsi);
	void			ResetWorkbook(_WorkbookPtr pWorkbook);
private:
	CString			GetFilterString(LPCTSTR lpszFilter, LPCTSTR lpszReq) const;

// Overrides
public:
	//{{AFX_VIRTUAL(CAgencyWnd)
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAgencyWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnQuickSearch();
	afx_msg void OnQuickClear();
	afx_msg void OnAgencyAlbumNew();
	afx_msg void OnAgencyProperties();
	afx_msg void OnAgencyImport();
	afx_msg void OnAgencyExport();
	afx_msg void OnUpdateAgencyXproperty(CCmdUI* pCmdUI);
	afx_msg void OnAgencyXproperty();
	afx_msg void OnAgencyXremove();
	afx_msg void OnAgencyXadd();
	afx_msg void OnAgencyXquick();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	friend class CAgencyXpropertyDlg;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_USER_NUMBER	1002
#define IDC_AGENCY_LIST	1006
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDAGENCY_H__FEDB13C1_30BC_449F_BC99_B27A6EACD1B9__INCLUDED_)
