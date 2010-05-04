//
// PageProfileSecure.h
//

#if !defined(AFX_PAGEPROFILESECURE_H__A3CE5943_6247_40A3_B802_13228F454CF7__INCLUDED_)
#define AFX_PAGEPROFILESECURE_H__A3CE5943_6247_40A3_B802_13228F454CF7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

class CXMLElement;

/////////////////////////////////////////////////////////////////////////////
// CSecureProfilePage dialog

class CSecureProfilePage : public CSettingsPage
{
// Construction
public:
	CSecureProfilePage(CXMLElement* pXML = NULL);
	virtual ~CSecureProfilePage();

	DECLARE_DYNCREATE(CSecureProfilePage)

// Dialog Data
public:
	//{{AFX_DATA(CSecureProfilePage)
	enum { IDD = IDD_PROFILE_SECURE };
	CButton		m_wndDelete;
	CEdit		m_wndDesc;
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
	//{{AFX_VIRTUAL(CSecureProfilePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CSecureProfilePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEPROFILESECURE_H__A3CE5943_6247_40A3_B802_13228F454CF7__INCLUDED_)
