//
// PageSettingsBlock.h
//

#if !defined(AFX_PAGESETTINGSBLOCK_H__4E0DA242_36FC_44AE_A98A_87446772C98C__INCLUDED_)
#define AFX_PAGESETTINGSBLOCK_H__4E0DA242_36FC_44AE_A98A_87446772C98C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

/////////////////////////////////////////////////////////////////////////////
// CBlockSettingsPage dialog

class CBlockSettingsPage : public CSettingsPage
{
// Construction
public:
	CBlockSettingsPage();
	virtual ~CBlockSettingsPage();

	DECLARE_DYNCREATE(CBlockSettingsPage)

// Dialog Data
public:
	//{{AFX_DATA(CBlockSettingsPage)
	enum { IDD = IDD_SETTINGS_BLOCK };
	CEdit	m_wndUsrCycle;
	CButton	m_wndGiven;
	CEdit	m_wndGivenId;
	CEdit	m_wndGivenLen;
	CEdit	m_wndUsrTime;
	CEdit	m_wndUsrDate;
	//}}AFX_DATA

// Operations
protected:
	BOOL		LoadSettings();
	BOOL		EditSettings();

// Overrides
public:
	//{{AFX_VIRTUAL(CBlockSettingsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CBlockSettingsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnGivenEnable();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESETTINGSBLOCK_H__4E0DA242_36FC_44AE_A98A_87446772C98C__INCLUDED_)
