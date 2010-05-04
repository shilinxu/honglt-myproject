//
// PageSettingsSecure.h
//

#if !defined(AFX_PAGESETTINGSSECURE_H__A3CE5943_6247_40A3_B802_13228F454CF7__INCLUDED_)
#define AFX_PAGESETTINGSSECURE_H__A3CE5943_6247_40A3_B802_13228F454CF7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

class CXMLElement;

/////////////////////////////////////////////////////////////////////////////
// CSecureSettingsPage dialog

class CSecureSettingsPage : public CSettingsPage
{
// Construction
public:
	CSecureSettingsPage(CXMLElement* pXML = NULL);
	virtual ~CSecureSettingsPage();

	DECLARE_DYNCREATE(CSecureSettingsPage)

// Dialog Data
public:
	//{{AFX_DATA(CSecureSettingsPage)
	enum { IDD = IDD_SETTINGS_SECURE };
	CButton	m_wndDelete;
	CEdit	m_wndDesc;
	CComboBox	m_wndHost;
	CListCtrl	m_wndList;
	CComboBox	m_wndAction;
	//}}AFX_DATA
protected:
	CXMLElement*	m_pXML;

// Operations
protected:
	BOOL			LoadXML();
	BOOL			EditXML();

// Overrides
public:
	//{{AFX_VIRTUAL(CSecureSettingsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CSecureSettingsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESETTINGSSECURE_H__A3CE5943_6247_40A3_B802_13228F454CF7__INCLUDED_)
