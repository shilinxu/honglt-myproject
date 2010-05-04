//
// PagePresserUser.h
//

#if !defined(AFX_PAGEPRESSERUSER_H__B3A00FB3_4601_472E_8233_30F16E3CCC4D__INCLUDED_)
#define AFX_PAGEPRESSERUSER_H__B3A00FB3_4601_472E_8233_30F16E3CCC4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

/////////////////////////////////////////////////////////////////////////////
// CUserPresserPage dialog

class CUserPresserPage : public CSettingsPage
{
// Construction
public:
	CUserPresserPage();
	virtual ~CUserPresserPage();

	DECLARE_DYNCREATE(CUserPresserPage)

// Dialog Data
public:
	//{{AFX_DATA(CUserPresserPage)
	enum { IDD = IDD_PRESSER_USER };
	CComboBox	m_wndUserId;
	CComboBox	m_wndExpire;
	//}}AFX_DATA

// Operations
protected:
	BOOL		PressRecord(LPCTSTR xUserId, int nExpire);
private:
	BOOL		IsSysFile(LPCTSTR xPath) const;

// Overrides
public:
	//{{AFX_VIRTUAL(CUserPresserPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CUserPresserPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnUserPress();
	afx_msg void OnUserView();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEPRESSERUSER_H__B3A00FB3_4601_472E_8233_30F16E3CCC4D__INCLUDED_)
