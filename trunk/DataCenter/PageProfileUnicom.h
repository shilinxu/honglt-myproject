//
// PageProfileUnicom.h
//

#if !defined(AFX_PAGEPROFILEUNICOM_H__B3A00FB3_4601_472E_8233_30F16E3CCC4D__INCLUDED_)
#define AFX_PAGEPROFILEUNICOM_H__B3A00FB3_4601_472E_8233_30F16E3CCC4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

class CXMLElement;

/////////////////////////////////////////////////////////////////////////////
// CUnicomProfilePage dialog

class CUnicomProfilePage : public CSettingsPage
{
// Construction
public:
	CUnicomProfilePage(CXMLElement* pXML = NULL);
	virtual ~CUnicomProfilePage();

	DECLARE_DYNCREATE(CUnicomProfilePage)

// Dialog Data
public:
	//{{AFX_DATA(CUnicomProfilePage)
	enum { IDD = IDD_PROFILE_UNICOM };
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
	//{{AFX_VIRTUAL(CUnicomProfilePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CUnicomProfilePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEPROFILEUNICOM_H__B3A00FB3_4601_472E_8233_30F16E3CCC4D__INCLUDED_)
