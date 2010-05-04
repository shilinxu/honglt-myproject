//
// WizardSheet.h
//

#if !defined(AFX_WIZARDSHEET_H__EF29F315_612A_4012_B911_AFCDCCE56B57__INCLUDED_)
#define AFX_WIZARDSHEET_H__EF29F315_612A_4012_B911_AFCDCCE56B57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWizardPage;

class CWizardSheet : public CPropertySheet  
{
// Construction
public:
	CWizardSheet(CWnd *pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CWizardSheet();

// Attributes
public:

// Operations
public:
	static BOOL	RunWizard(CWnd* pParent = NULL);
	CWizardPage*	FindPage(CRuntimeClass* pClass);

// Implementation
protected:
	//{{AFX_MSG(CWizardSheet)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CWizardPage : public CPropertyPage
{
// Construction
public:
	CWizardPage(UINT nID = 0);
	virtual ~CWizardPage();

	DECLARE_DYNCREATE(CWizardPage)

// Attributes
public:
	COLORREF	m_crWhite;
	CBrush		m_brWhite;

// Operations
public:
	CWizardSheet*	GetSheet();
	void			SetWizardButtons(DWORD dwFlags);

// Implementation
protected:
	//{{AFX_MSG(CWizardPage)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_WIZARDSHEET_H__EF29F315_612A_4012_B911_AFCDCCE56B57__INCLUDED_)
