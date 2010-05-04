//
// PageSettingsTalker.h
//

#if !defined(AFX_PAGESETTINGSTALKER_H__96BE0AAC_2556_4254_A09F_D013DB0F1B21__INCLUDED_)
#define AFX_PAGESETTINGSTALKER_H__96BE0AAC_2556_4254_A09F_D013DB0F1B21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

/////////////////////////////////////////////////////////////////////////////
// CTalkerSettingsPage dialog

class CTalkerSettingsPage : public CSettingsPage
{
// Construction
public:
	CTalkerSettingsPage();
	virtual ~CTalkerSettingsPage();

	DECLARE_DYNCREATE(CTalkerSettingsPage)

// Dialog Data
public:
	//{{AFX_DATA(CTalkerSettingsPage)
	enum { IDD = IDD_SETTINGS_TALKER };
	CButton		m_wndTrickEnable;
	//}}AFX_DATA

// Operations
protected:
	BOOL		LoadSettings();
	BOOL		EditSettings();

// Overrides
public:
	//{{AFX_VIRTUAL(CTalkerSettingsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CTalkerSettingsPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESETTINGSTALKER_H__96BE0AAC_2556_4254_A09F_D013DB0F1B21__INCLUDED_)
