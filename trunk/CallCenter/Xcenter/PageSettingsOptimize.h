//
// PageSettingsOptimize.h
//

#if !defined(AFX_PAGESETTINGSOPTIMIZE_H__96BE0AAC_2556_4254_A09F_D013DB0F1B21__INCLUDED_)
#define AFX_PAGESETTINGSOPTIMIZE_H__96BE0AAC_2556_4254_A09F_D013DB0F1B21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

/////////////////////////////////////////////////////////////////////////////
// COptimizeSettingsPage dialog

class COptimizeSettingsPage : public CSettingsPage
{
// Construction
public:
	COptimizeSettingsPage();
	virtual ~COptimizeSettingsPage();

	DECLARE_DYNCREATE(COptimizeSettingsPage)

// Dialog Data
public:
	//{{AFX_DATA(COptimizeSettingsPage)
	enum { IDD = IDD_SETTINGS_OPTIMIZE };
	CEdit		m_wndPoseCaller;
	CEdit		m_wndPoseNumb;
	CButton		m_wndPoseChk;
	CEdit		m_wndPoseExcept;
	//}}AFX_DATA

// Operations
protected:
	BOOL		LoadSettings();
	BOOL		EditSettings();

// Overrides
public:
	//{{AFX_VIRTUAL(COptimizeSettingsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(COptimizeSettingsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnPoseCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESETTINGSOPTIMIZE_H__96BE0AAC_2556_4254_A09F_D013DB0F1B21__INCLUDED_)
