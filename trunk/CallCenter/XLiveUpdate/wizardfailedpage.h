//
// WizardFailedPage.h
//

#if !defined(AFX_WIZARDFAILEDPAGE_H__BCED7BDA_0D4D_4CF2_835A_55F0A6CF0AC8__INCLUDED_)
#define AFX_WIZARDFAILEDPAGE_H__BCED7BDA_0D4D_4CF2_835A_55F0A6CF0AC8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WizardSheet.h"

class CWizardFailedPage : public CWizardPage
{
// Construction
public:
	CWizardFailedPage();
	virtual ~CWizardFailedPage();

	DECLARE_DYNCREATE(CWizardFailedPage)

// Dialog Data
public:
	//{{AFX_DATA(CWizardFailedPage)
	enum { IDD = IDD_WIZARD_FAILED };
	CStatic		m_wndWelcome;
	CStatic		m_wndWeb;
	//}}AFX_DATA

// Operations
protected:
	
// Overrides
public:
	//{{AFX_VIRTUAL(CWizardFailedPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CWizardFailedPage)
	virtual BOOL OnSetActive();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_WIZARDFAILEDPAGE_H__BCED7BDA_0D4D_4CF2_835A_55F0A6CF0AC8__INCLUDED_)
