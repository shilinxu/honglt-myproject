//
// WizardUpdatePage.h
//

#if !defined(AFX_WIZARDUPDATEPAGE_H__BCED7BDA_0D4D_4CF2_835A_55F0A6CF0AC8__INCLUDED_)
#define AFX_WIZARDUPDATEPAGE_H__BCED7BDA_0D4D_4CF2_835A_55F0A6CF0AC8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WizardSheet.h"

class CWizardUpdatePage : public CWizardPage
{
// Construction
public:
	CWizardUpdatePage();
	virtual ~CWizardUpdatePage();

	DECLARE_DYNCREATE(CWizardUpdatePage)

// Dialog Data
public:
	//{{AFX_DATA(CWizardUpdatePage)
	enum { IDD = IDD_WIZARD_UPDATE };
	CStatic	m_wndWelcome;
	CStatic	m_wndProgress;
	//}}AFX_DATA

// Attributes
protected:
	UINT	m_nNextPage;

// Overrides
public:
	//{{AFX_VIRTUAL(CWizardUpdatePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT OnWizardNext();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CWizardUpdatePage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg void OnCheckOut(LPCTSTR lpszText);
	afx_msg LONG OnNextPage(UINT nPageId, BOOL bContinue = FALSE);
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_WIZARDUPDATEPAGE_H__BCED7BDA_0D4D_4CF2_835A_55F0A6CF0AC8__INCLUDED_)
