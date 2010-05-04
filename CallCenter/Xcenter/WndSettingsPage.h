//
// WndSettingsPage.h
//

#if !defined(AFX_WNDSETTINGSPAGE_H__4D0308EB_0F4D_4D42_AC54_6115C5666C4B__INCLUDED_)
#define AFX_WNDSETTINGSPAGE_H__4D0308EB_0F4D_4D42_AC54_6115C5666C4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSettingsSheet;

/////////////////////////////////////////////////////////////////////////////
// CSettingsPage dialog

class CSettingsPage : public CDialog
{
// Construction
public:
	CSettingsPage(UINT nIDTemplate, LPCTSTR pszCaption = NULL);
	virtual ~CSettingsPage();

	DECLARE_DYNAMIC(CSettingsPage)

// Attributes
public:
	CString	m_sCaption;
	BOOL	m_bGroup;
public:
	HINSTANCE	m_hInstance;

// Operations
public:
	BOOL	Create(CRect& rcPage, CWnd* pSheetWnd);
	BOOL	LoadDefaultCaption();

	CSettingsPage* GetPage(CRuntimeClass* pClass) const;

	inline CSettingsSheet* GetSheet() const
	{
		return (CSettingsSheet*)GetParent();
	}

	inline LPCTSTR GetTemplateName() const
	{
		return m_lpszTemplateName;
	}

public:
	virtual void SetModified(BOOL bChanged = TRUE);
	virtual BOOL OnApply();
	virtual void OnReset();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();

// Overrides
protected:
	//{{AFX_VIRTUAL(CSettingsPage)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CSettingsPage)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDSETTINGSPAGE_H__4D0308EB_0F4D_4D42_AC54_6115C5666C4B__INCLUDED_)
