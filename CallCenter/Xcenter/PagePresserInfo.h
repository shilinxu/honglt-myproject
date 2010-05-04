//
// PagePresserUser.h
//

#if !defined(AFX_PAGEPRESSERINFO_H__B3A00FB3_4601_472E_8233_30F16E3CCC4D__INCLUDED_)
#define AFX_PAGEPRESSERINFO_H__B3A00FB3_4601_472E_8233_30F16E3CCC4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

/////////////////////////////////////////////////////////////////////////////
// CInfoPresserPage dialog

class CInfoPresserPage : public CSettingsPage
{
// Construction
public:
	CInfoPresserPage();
	virtual ~CInfoPresserPage();

	DECLARE_DYNCREATE(CInfoPresserPage)

// Dialog Data
public:
	//{{AFX_DATA(CInfoPresserPage)
	enum { IDD = IDD_PRESSER_INFO };
	CComboBox	m_wndUserId;
	CComboBox	m_wndExpire;
	//}}AFX_DATA

// Operations
protected:
	BOOL		PressRecord(LPCTSTR xUserId, int nExpire);

// Overrides
public:
	//{{AFX_VIRTUAL(CInfoPresserPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CInfoPresserPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnUserPress();
	afx_msg void OnUserView();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEPRESSERINFO_H__B3A00FB3_4601_472E_8233_30F16E3CCC4D__INCLUDED_)
