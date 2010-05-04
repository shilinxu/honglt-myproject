//
// WizardCheckerPage.h
//

#if !defined(AFX_WIZARDCHECKERPAGE_H__BCED7BDA_0D4D_4CF2_835A_55F0A6CF0AC8__INCLUDED_)
#define AFX_WIZARDCHECKERPAGE_H__BCED7BDA_0D4D_4CF2_835A_55F0A6CF0AC8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WizardSheet.h"

class CWizardCheckerPage : public CWizardPage
{
// Construction
public:
	CWizardCheckerPage();
	virtual ~CWizardCheckerPage();

	DECLARE_DYNCREATE(CWizardCheckerPage)

// Dialog Data
public:
	//{{AFX_DATA(CWizardCheckerPage)
	enum { IDD = IDD_WIZARD_CHECKER };
	CStatic	m_wndWelcome;
	CStatic	m_wndProgress;
	//}}AFX_DATA

// Attributes
protected:
	UINT	m_nNextPage;

// Operations
protected:
	BOOL	CheckProcess(CString& xName);
	BOOL	GetProcessNameByID(DWORD nPId, CString& strName);

// Overrides
public:
	//{{AFX_VIRTUAL(CWizardCheckerPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT OnWizardNext();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CWizardCheckerPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg void OnCheckOut(LPCTSTR lpszText);
	afx_msg LONG OnNextPage(UINT nPageId, BOOL bContinue = FALSE);
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_WIZARDCHECKERPAGE_H__BCED7BDA_0D4D_4CF2_835A_55F0A6CF0AC8__INCLUDED_)
