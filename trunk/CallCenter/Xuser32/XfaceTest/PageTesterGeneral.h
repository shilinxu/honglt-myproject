//
// PageTesterGeneral.h
//

#if !defined(AFX_PAGETESTERGENERAL_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_)
#define AFX_PAGETESTERGENERAL_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

/////////////////////////////////////////////////////////////////////////////
// CGeneralTesterPage dialog

class CGeneralTesterPage : public CSettingsPage
{
// Construction
public:
	CGeneralTesterPage();
	virtual ~CGeneralTesterPage();

	DECLARE_DYNCREATE(CGeneralTesterPage)

// Dialog Data
public:
	//{{AFX_DATA(CGeneralTesterPage)
	enum { IDD = IDD_TESTER_GENERAL };
	//}}AFX_DATA

// Operations
protected:

// Overrides
public:
	//{{AFX_VIRTUAL(CGeneralTesterPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CGeneralTesterPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGETESTERGENERAL_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_)
