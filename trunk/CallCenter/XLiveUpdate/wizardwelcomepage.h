//
// WizardWelcomePage.h
//

#if !defined(AFX_WIZARDWELCOMEPAGE_H__BCED7BDA_0D4D_4CF2_835A_55F0A6CF0AC8__INCLUDED_)
#define AFX_WIZARDWELCOMEPAGE_H__BCED7BDA_0D4D_4CF2_835A_55F0A6CF0AC8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WizardSheet.h"

class CWizardWelcomePage : public CWizardPage
{
// Construction
public:
	CWizardWelcomePage();
	virtual ~CWizardWelcomePage();

	DECLARE_DYNCREATE(CWizardWelcomePage)

// Dialog Data
public:
	//{{AFX_DATA(CWizardWelcomePage)
	enum { IDD = IDD_WIZARD_WELCOME };
	CStatic	m_wndWelcome;
	//}}AFX_DATA

// Overrides
public:
	//{{AFX_VIRTUAL(CWizardWelcomePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CWizardWelcomePage)
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_WIZARDWELCOMEPAGE_H__BCED7BDA_0D4D_4CF2_835A_55F0A6CF0AC8__INCLUDED_)
