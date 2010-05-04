//
// PageSettingsVideo.h
//

#if !defined(AFX_PAGESETTINGSVIDEO_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_)
#define AFX_PAGESETTINGSVIDEO_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"
class CVideoCapture;

/////////////////////////////////////////////////////////////////////////////
// CVideoSettingsPage dialog

class CVideoSettingsPage : public CSettingsPage
{
// Construction
public:
	CVideoSettingsPage();
	virtual ~CVideoSettingsPage();

	DECLARE_DYNCREATE(CVideoSettingsPage)

// Dialog Data
public:
	//{{AFX_DATA(CVideoSettingsPage)
	enum { IDD = IDD_SETTINGS_VIDEO };
	CStatic		m_wndPreview;
	CButton		m_wndProperty;
	CComboBox	m_wndInput;
	//}}AFX_DATA
	
protected:
	CVideoCapture*	m_pPreview;

// Operations
protected:
	BOOL			LoadSettings();
	BOOL			EditSettings();
	int				EnumVideoDevices();
protected:
	BOOL			StartPreview(IBaseFilter* pCapture);
	void			StopPreview();

// Overrides
public:
	//{{AFX_VIRTUAL(CVideoSettingsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CVideoSettingsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeDevInput();
	afx_msg void OnVideoProperty();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESETTINGSVIDEO_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_)
