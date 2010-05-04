//
// PageProfileInter.h
//

#if !defined(AFX_PAGEPROFILEINTER_H__B3A00FB3_4601_472E_8233_30F16E3CCC4D__INCLUDED_)
#define AFX_PAGEPROFILEINTER_H__B3A00FB3_4601_472E_8233_30F16E3CCC4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

class CXMLElement;

/////////////////////////////////////////////////////////////////////////////
// CInterProfilePage dialog

class CInterProfilePage : public CSettingsPage
{
// Construction
public:
	CInterProfilePage(CXMLElement* pXML = NULL);
	virtual ~CInterProfilePage();

	DECLARE_DYNCREATE(CInterProfilePage)

// Dialog Data
public:
	//{{AFX_DATA(CInterProfilePage)
	enum { IDD = IDD_PROFILE_INTER };
	CComboBox	m_wndVNet;
	CComboBox	m_wndUserId;
	CComboBox	m_wndJob;
	CComboBox	m_wndHost;
	CButton		m_wndDelete;
	CListCtrl	m_wndList;
	CEdit		m_wndLimited;
	CComboBox	m_wndLinkId;
	//}}AFX_DATA
protected:
	CXMLElement*	m_pXML;

// Operations
protected:
	BOOL			LoadXML();

// Overrides
public:
	//{{AFX_VIRTUAL(CInterProfilePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CInterProfilePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEPROFILEINTER_H__B3A00FB3_4601_472E_8233_30F16E3CCC4D__INCLUDED_)
