//
// PageProfileFinal.h
//

#if !defined(AFX_PAGEPROFILEFINAL_H__B3A00FB3_4601_472E_8233_30F16E3CCC4D__INCLUDED_)
#define AFX_PAGEPROFILEFINAL_H__B3A00FB3_4601_472E_8233_30F16E3CCC4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

class CXMLElement;

/////////////////////////////////////////////////////////////////////////////
// CFinalProfilePage dialog

class CFinalProfilePage : public CSettingsPage
{
// Construction
public:
	CFinalProfilePage(CXMLElement* pXML = NULL);
	virtual ~CFinalProfilePage();

	DECLARE_DYNCREATE(CFinalProfilePage)

// Dialog Data
public:
	//{{AFX_DATA(CFinalProfilePage)
	enum { IDD = IDD_PROFILE_FINAL };
	CComboBox	m_wndHost;
	CComboBox	m_wndModel;
	CEdit		m_wndName;
	CButton		m_wndDelete;
	CListCtrl	m_wndList;
	//}}AFX_DATA
protected:
	CXMLElement*	m_pXML;

// Operations
protected:
	BOOL			LoadXML();
private:
	void			MutiHostToCombo(LPCTSTR lpszHost);
	void			MutiHostFromCombo(CString& strHost);

// Overrides
public:
	//{{AFX_VIRTUAL(CFinalProfilePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CFinalProfilePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEPROFILEFINAL_H__B3A00FB3_4601_472E_8233_30F16E3CCC4D__INCLUDED_)
