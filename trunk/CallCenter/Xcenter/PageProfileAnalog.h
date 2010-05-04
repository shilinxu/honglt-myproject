//
// PageProfileAnalog.h
//

#if !defined(AFX_PAGEPROFILEANALOG_H__D5D80DA1_F5D5_4C3A_9032_272CCF9B75B0__INCLUDED_)
#define AFX_PAGEPROFILEANALOG_H__D5D80DA1_F5D5_4C3A_9032_272CCF9B75B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

class CXMLElement;

/////////////////////////////////////////////////////////////////////////////
// CAnalogProfilePage dialog

class CAnalogProfilePage : public CSettingsPage
{
// Construction
public:
	CAnalogProfilePage(CXMLElement* pXML = NULL);
	virtual ~CAnalogProfilePage();

	DECLARE_DYNCREATE(CAnalogProfilePage)

// Dialog Data
public:
	//{{AFX_DATA(CAnalogProfilePage)
	enum { IDD = IDD_PROFILE_ANALOG };
	CButton		m_wndDelete;
	CEdit		m_wndLinkId;
	CComboBox	m_wndHost;
	CListCtrl	m_wndList;
	CEdit		m_wndLocalId;
	//}}AFX_DATA
protected:
	CXMLElement*	m_pXML;

// Operations
protected:
	BOOL			LoadXML();

// Overrides
public:
	//{{AFX_VIRTUAL(CAnalogProfilePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAnalogProfilePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEPROFILEANALOG_H__D5D80DA1_F5D5_4C3A_9032_272CCF9B75B0__INCLUDED_)
