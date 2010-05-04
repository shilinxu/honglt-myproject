//
// PageSettingsVista.h
//

#if !defined(AFX_PAGESETTINGSVISTA_H__96BE0AAC_2556_4254_A09F_D013DB0F1B21__INCLUDED_)
#define AFX_PAGESETTINGSVISTA_H__96BE0AAC_2556_4254_A09F_D013DB0F1B21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

/////////////////////////////////////////////////////////////////////////////
// CVistaSettingsPage dialog

class CVistaSettingsPage : public CSettingsPage
{
// Construction
public:
	CVistaSettingsPage();
	virtual ~CVistaSettingsPage();

	DECLARE_DYNCREATE(CVistaSettingsPage)

// Dialog Data
public:
	//{{AFX_DATA(CVistaSettingsPage)
	enum { IDD = IDD_SETTINGS_VISTA };
	CEdit		m_wndHost;
	//}}AFX_DATA

// Operations
protected:
	BOOL		LoadSettings();
	BOOL		EditSettings();

// Overrides
public:
	//{{AFX_VIRTUAL(CVistaSettingsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CVistaSettingsPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESETTINGSVISTA_H__96BE0AAC_2556_4254_A09F_D013DB0F1B21__INCLUDED_)
