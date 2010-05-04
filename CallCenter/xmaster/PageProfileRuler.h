//
// PageProfileRuler.h
//

#if !defined(AFX_PAGEPROFILERULER_H__4E0DA242_36FC_44AE_A98A_87446772C98C__INCLUDED_)
#define AFX_PAGEPROFILERULER_H__4E0DA242_36FC_44AE_A98A_87446772C98C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

class CXMLElement;

/////////////////////////////////////////////////////////////////////////////
// CRulerProfilePage dialog

class CRulerProfilePage : public CSettingsPage
{
// Construction
public:
	CRulerProfilePage(CXMLElement* pXML = NULL);
	virtual ~CRulerProfilePage();

	DECLARE_DYNCREATE(CRulerProfilePage)

// Dialog Data
public:
	//{{AFX_DATA(CRulerProfilePage)
	enum { IDD = IDD_PROFILE_RULER };
	CButton		m_wndExtAdd;
	CButton		m_wndExtRemove;
	CComboBox	m_wndExtList;
	//}}AFX_DATA
protected:
	CXMLElement*	m_pXML;

// Operations
protected:
	BOOL			LoadXML();
	BOOL			EditXML();

// Overrides
public:
	//{{AFX_VIRTUAL(CRulerProfilePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CRulerProfilePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnEditChangeExtList();
	afx_msg void OnSelChangeExtList();
	afx_msg void OnExtAdd();
	afx_msg void OnExtRemove();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEPROFILERULER_H__4E0DA242_36FC_44AE_A98A_87446772C98C__INCLUDED_)
