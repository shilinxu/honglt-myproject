//
// CtrlUserPanel.h
//

#if !defined(AFX_CTRLUSERPANEL_H__3515C099_F6AD_43C4_917B_5D7CB35B2701__INCLUDED_)
#define AFX_CTRLUSERPANEL_H__3515C099_F6AD_43C4_917B_5D7CB35B2701__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CtrlTaskPanel.h"
#include "HisReport.h"
class CXMLElement;

class CUserTaskBox : public CTaskBox
{
// Construction
public:
	CUserTaskBox();
	virtual ~CUserTaskBox();
	DECLARE_DYNAMIC(CUserTaskBox)
		
// Attributes
protected:
	CStatic		m_wndReady;
	CStatic		m_wndNavigate;
	COLORREF	m_crWhite;
	CBrush		m_brWhite;

// Operations
public:
	void		UpdateReport(int nReady, int nNavigate);
	
// Overrides
public:
	//{{AFX_VIRTUAL(CUserTaskBox)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CUserTaskBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CUserFluxBox : public CTaskBox, protected CHisReport
{
// Construction
public:
	CUserFluxBox();
	virtual ~CUserFluxBox();
	DECLARE_DYNAMIC(CUserFluxBox)
	
// Attributes
protected:
	CListCtrl	m_wndList;
protected:
	CXMLElement*	m_pXMLMonth;
	CXMLElement*	m_pXMLToday;

// Operations
public:
	virtual void	Register(LPCTSTR xUser, LPCTSTR xPart, int nValue);
	virtual void	Register(LPCTSTR xUser, LPCTSTR xPart);
protected:
	void			Register(CXMLElement* pParent, LPCTSTR pszPart);
	virtual BOOL	LoadReport();
protected:
	
// Overrides
public:
	//{{AFX_VIRTUAL(CUserFluxBox)
	virtual BOOL Create(CTaskPanel* pPanel, int nHeight = 0, LPCTSTR pszCaption = NULL, UINT nIDIcon = 0);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CUserFluxBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CUserPanel : public CTaskPanel
{
	friend class CUserWnd;
	
// Construction
public:
	CUserPanel();
	virtual ~CUserPanel();
	DECLARE_DYNAMIC(CUserPanel)

// Attributes
protected:
	CUserTaskBox	m_wndTask;
	CUserFluxBox	m_wndFlux;

// Operations
public:
	void	ExecuteReport();

// Overrides
public:
	//{{AFX_VIRTUAL(CUserPanel)
	virtual BOOL Create(CWnd* pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CUserPanel)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_USER_PANEL		101
#define IDC_FLUX_LIST		1001
#define ID_USER_REPORT		3801
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLUSERPANEL_H__3515C099_F6AD_43C4_917B_5D7CB35B2701__INCLUDED_)
