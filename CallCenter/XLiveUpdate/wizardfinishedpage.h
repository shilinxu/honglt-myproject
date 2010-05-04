//
// WizardFinishedPage.h
//

#if !defined(AFX_WIZARDFINISHEDPAGE_H__BCED7BDA_0D4D_4CF2_835A_55F0A6CF0AC8__INCLUDED_)
#define AFX_WIZARDFINISHEDPAGE_H__BCED7BDA_0D4D_4CF2_835A_55F0A6CF0AC8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WizardSheet.h"

class CWizardFinishedPage : public CWizardPage
{
// Construction
public:
	CWizardFinishedPage();
	virtual ~CWizardFinishedPage();

	DECLARE_DYNCREATE(CWizardFinishedPage)

// Dialog Data
public:
	//{{AFX_DATA(CWizardFinishedPage)
	enum { IDD = IDD_WIZARD_FINISHED };
	CStatic		m_wndWelcome;
	CStatic		m_wndVersion;
	CStatic		m_wndWeb;
	//}}AFX_DATA

// Operations
protected:
	
// Overrides
public:
	//{{AFX_VIRTUAL(CWizardFinishedPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CWizardFinishedPage)
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_WIZARDFINISHEDPAGE_H__BCED7BDA_0D4D_4CF2_835A_55F0A6CF0AC8__INCLUDED_)
