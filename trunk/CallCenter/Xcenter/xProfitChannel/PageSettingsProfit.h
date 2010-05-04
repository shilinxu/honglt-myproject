//
// PageSettingsProfit.h
//

#if !defined(AFX_PAGESETTINGSPROFIT_H__96BE0AAC_2556_4254_A09F_D013DB0F1B21__INCLUDED_)
#define AFX_PAGESETTINGSPROFIT_H__96BE0AAC_2556_4254_A09F_D013DB0F1B21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

/////////////////////////////////////////////////////////////////////////////
// CProfitSettingsPage dialog

class CProfitSettingsPage : public CSettingsPage
{
// Construction
public:
	CProfitSettingsPage();
	virtual ~CProfitSettingsPage();

	DECLARE_DYNCREATE(CProfitSettingsPage)

// Dialog Data
public:
	//{{AFX_DATA(CProfitSettingsPage)
	enum { IDD = IDD_SETTINGS_PROFIT };
	CEdit	m_wndSeekNum;
	CButton	m_wndPlyNumb;
	CEdit	m_wndMinGivLeng;
	CEdit	m_wndMaxGivLong;
	//}}AFX_DATA

// Operations
protected:
	BOOL		LoadSettings();
	BOOL		EditSettings();

// Overrides
public:
	//{{AFX_VIRTUAL(CProfitSettingsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CProfitSettingsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnPlayNumb();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESETTINGSPROFIT_H__96BE0AAC_2556_4254_A09F_D013DB0F1B21__INCLUDED_)
