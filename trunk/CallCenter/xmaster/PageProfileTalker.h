//
// PageProfileTalker.h
//

#if !defined(AFX_PAGEPROFILETALKER_H__4E0DA242_36FC_44AE_A98A_87446772C98C__INCLUDED_)
#define AFX_PAGEPROFILETALKER_H__4E0DA242_36FC_44AE_A98A_87446772C98C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

class CXMLElement;

/////////////////////////////////////////////////////////////////////////////
// CTalkerProfilePage dialog

class CTalkerProfilePage : public CSettingsPage
{
// Construction
public:
	CTalkerProfilePage(CXMLElement* pXML = NULL);
	virtual ~CTalkerProfilePage();

	DECLARE_DYNCREATE(CTalkerProfilePage)

// Dialog Data
public:
	//{{AFX_DATA(CTalkerProfilePage)
	enum { IDD = IDD_PROFILE_TALKER };
	CComboBox	m_wndLeng1st;
	CEdit		m_wndCost1st;
	CComboBox	m_wndLeng1st2;
	CEdit		m_wndCost1st2;
	//}}AFX_DATA
protected:
	CXMLElement*	m_pXML;

// Operations
protected:
	BOOL			LoadXML();
	BOOL			EditXML();

// Overrides
public:
	//{{AFX_VIRTUAL(CTalkerProfilePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CTalkerProfilePage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEPROFILETALKER_H__4E0DA242_36FC_44AE_A98A_87446772C98C__INCLUDED_)
