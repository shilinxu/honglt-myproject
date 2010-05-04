//
// PageSettingsAudio.h
//

#if !defined(AFX_PAGESETTINGSAUDIO_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_)
#define AFX_PAGESETTINGSAUDIO_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

/////////////////////////////////////////////////////////////////////////////
// CAudioSettingsPage dialog

class CAudioSettingsPage : public CSettingsPage
{
// Construction
public:
	CAudioSettingsPage();
	virtual ~CAudioSettingsPage();

	DECLARE_DYNCREATE(CAudioSettingsPage)

// Dialog Data
public:
	//{{AFX_DATA(CAudioSettingsPage)
	enum { IDD = IDD_SETTINGS_AUDIO };
	CComboBox	m_wndInput;
	CComboBox	m_wndOutput;
	//}}AFX_DATA

// Operations
protected:
	BOOL			LoadSettings();
	BOOL			EditSettings();
	int				EnumDevicesBox(REFCLSID rclsid, CComboBox* pCombo);
private:
	BOOL			LoadSettings(CComboBox* pCombo, LPCTSTR pDevName);
	BOOL			EditSettings(CComboBox* pCombo, CString& xDevName);

// Overrides
public:
	//{{AFX_VIRTUAL(CAudioSettingsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAudioSettingsPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESETTINGSAUDIO_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_)
