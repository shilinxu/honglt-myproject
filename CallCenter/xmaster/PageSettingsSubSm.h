//
// PageSettingsSubSm.h
//

#if !defined(AFX_PAGESETTINGSSUBSM_H__4E0DA242_36FC_44AE_A98A_87446772C98C__INCLUDED_)
#define AFX_PAGESETTINGSSUBSM_H__4E0DA242_36FC_44AE_A98A_87446772C98C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

/////////////////////////////////////////////////////////////////////////////
// CSubSmSettingsPage dialog

class CSubSmSettingsPage : public CSettingsPage
{
// Construction
public:
	CSubSmSettingsPage();
	virtual ~CSubSmSettingsPage();

	DECLARE_DYNCREATE(CSubSmSettingsPage)

// Dialog Data
public:
	//{{AFX_DATA(CSubSmSettingsPage)
	enum { IDD = IDD_SETTINGS_SUBSM };
	CEdit	m_wndSubId;
	CEdit	m_wndUsrTime;
	CEdit	m_wndUsrDate;
	CButton	m_wndReBack;
	CEdit	m_wndCookie;
	//}}AFX_DATA

// Operations
protected:
	BOOL		LoadSettings();
	BOOL		EditSettings();

// Overrides
public:
	//{{AFX_VIRTUAL(CSubSmSettingsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CSubSmSettingsPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESETTINGSSUBSM_H__4E0DA242_36FC_44AE_A98A_87446772C98C__INCLUDED_)
