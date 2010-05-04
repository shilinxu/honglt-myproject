//
// PageProfileMonitor.h
//

#if !defined(AFX_PAGEPROFILEMONITOR_H__4E0DA242_36FC_44AE_A98A_87446772C98C__INCLUDED_)
#define AFX_PAGEPROFILEMONITOR_H__4E0DA242_36FC_44AE_A98A_87446772C98C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

class CXMLElement;

/////////////////////////////////////////////////////////////////////////////
// CMonitorProfilePage dialog

class CMonitorProfilePage : public CSettingsPage
{
// Construction
public:
	CMonitorProfilePage(CXMLElement* pXML = NULL);
	virtual ~CMonitorProfilePage();

	DECLARE_DYNCREATE(CMonitorProfilePage)

// Dialog Data
public:
	//{{AFX_DATA(CMonitorProfilePage)
	enum { IDD = IDD_PROFILE_MONITOR };
	CListCtrl	m_wndList;
	CComboBox	m_wndHost;
	CEdit		m_wndPort;
	CEdit		m_wndPath;
	CEdit		m_wndPhone;
	CEdit		m_wndContent;
	CButton		m_wndDelete;
	//}}AFX_DATA
protected:
	CXMLElement*	m_pXML;

// Operations
protected:
	BOOL			LoadXML();
	BOOL			EditXML();

// Overrides
public:
	//{{AFX_VIRTUAL(CMonitorProfilePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CMonitorProfilePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnPathBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEPROFILEMONITOR_H__4E0DA242_36FC_44AE_A98A_87446772C98C__INCLUDED_)
