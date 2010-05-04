//
// PageSettingsTrunk.h
//

#if !defined(AFX_PAGESETTINGSTRUNK_H__96BE0AAC_2556_4254_A09F_D013DB0F1B21__INCLUDED_)
#define AFX_PAGESETTINGSTRUNK_H__96BE0AAC_2556_4254_A09F_D013DB0F1B21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

/////////////////////////////////////////////////////////////////////////////
// CTrunkSettingsPage dialog

class CTrunkSettingsPage : public CSettingsPage
{
// Construction
public:
	CTrunkSettingsPage();
	virtual ~CTrunkSettingsPage();

	DECLARE_DYNCREATE(CTrunkSettingsPage)

// Dialog Data
public:
	//{{AFX_DATA(CTrunkSettingsPage)
	enum { IDD = IDD_SETTINGS_TRUNK };
	CEdit		m_wndTimeOut;
	CButton		m_wndInLimit;
	CEdit		m_wndInLimited;
	CEdit		m_wndLookOut;
	CEdit		m_wndOutPhone;
	CButton		m_wndOutLimit;
	CEdit		m_wndOutLimited;
	CEdit		m_wndOutReserved;
	CEdit		m_wndRecFile;
	//}}AFX_DATA

// Operations
protected:
	BOOL		LoadSettings();
	BOOL		EditSettings();

// Overrides
public:
	//{{AFX_VIRTUAL(CTrunkSettingsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CTrunkSettingsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnInboundLimit();
	afx_msg void OnOutboundLimit();
	afx_msg void OnRecFileDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESETTINGSTRUNK_H__96BE0AAC_2556_4254_A09F_D013DB0F1B21__INCLUDED_)
