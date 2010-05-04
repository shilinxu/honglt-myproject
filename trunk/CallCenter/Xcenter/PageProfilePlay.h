//
// PageProfilePlay.h
//

#if !defined(AFX_PAGEPROFILEPLAY_H__D5D80DA1_F5D5_4C3A_9032_272CCF9B75B0__INCLUDED_)
#define AFX_PAGEPROFILEPLAY_H__D5D80DA1_F5D5_4C3A_9032_272CCF9B75B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

class CXMLElement;

/////////////////////////////////////////////////////////////////////////////
// CPlayProfilePage dialog

class CPlayProfilePage : public CSettingsPage
{
// Construction
public:
	CPlayProfilePage(CXMLElement* pXML = NULL);
	virtual ~CPlayProfilePage();

	DECLARE_DYNCREATE(CPlayProfilePage)

// Dialog Data
public:
	//{{AFX_DATA(CPlayProfilePage)
	enum { IDD = IDD_PROFILE_PLAY };
	CButton	m_wndDelete;
	CEdit	m_wndName;
	CComboBox	m_wndIndex;
	CListCtrl	m_wndList;
	//}}AFX_DATA
protected:
	CXMLElement*	m_pXML;

// Operations
protected:
	BOOL			LoadXML();

// Overrides
public:
	//{{AFX_VIRTUAL(CPlayProfilePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CPlayProfilePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEPROFILEPLAY_H__D5D80DA1_F5D5_4C3A_9032_272CCF9B75B0__INCLUDED_)
