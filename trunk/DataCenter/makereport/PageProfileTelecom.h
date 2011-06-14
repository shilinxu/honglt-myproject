//
// PageProfileTelecom.h
//

#if !defined(AFX_PAGEPROFILETELECOM_H__B3A00FB3_4601_472E_8233_30F16E3CCC4D__INCLUDED_)
#define AFX_PAGEPROFILETELECOM_H__B3A00FB3_4601_472E_8233_30F16E3CCC4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

class CXMLElement;

/////////////////////////////////////////////////////////////////////////////
// CTelecomProfilePage dialog

class CTelecomProfilePage : public CSettingsPage
{
// Construction
public:
	CTelecomProfilePage(CXMLElement* pXML = NULL);
	virtual ~CTelecomProfilePage();

	DECLARE_DYNCREATE(CTelecomProfilePage)

// Dialog Data
public:
	//{{AFX_DATA(CTelecomProfilePage)
	enum { IDD = IDD_PROFILE_TELECOM };
	CComboBox	m_wndUserId;
	CComboBox	m_wndTalk;
	CComboBox	m_wndNews;
	CButton		m_wndDelete;
	CEdit		m_wndKey;
	CListCtrl	m_wndList;
	//}}AFX_DATA
protected:
	CXMLElement*	m_pXML;

// Operations
protected:
	BOOL			LoadXML();

// Overrides
public:
	//{{AFX_VIRTUAL(CTelecomProfilePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CTelecomProfilePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEPROFILETELECOM_H__B3A00FB3_4601_472E_8233_30F16E3CCC4D__INCLUDED_)
