//
// PageTesterChat.h
//

#if !defined(AFX_PAGETESTERCHAT_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_)
#define AFX_PAGETESTERCHAT_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

/////////////////////////////////////////////////////////////////////////////
// CChatTesterPage dialog

class CChatTesterPage : public CSettingsPage
{
// Construction
public:
	CChatTesterPage();
	virtual ~CChatTesterPage();

	DECLARE_DYNCREATE(CChatTesterPage)

// Dialog Data
public:
	//{{AFX_DATA(CChatTesterPage)
	enum { IDD = IDD_TESTER_CHAT };
	CStatic	m_wndPreview;
	CEdit	m_wndPhone;
	CStatic	m_wndStatus;
	//}}AFX_DATA

// Operations
protected:
	static void CALLBACK TimerProcA(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime);
protected:
	virtual BOOL	OnSyncHostFile(CXMLElement* pXML);

// Overrides
public:
	//{{AFX_VIRTUAL(CChatTesterPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CChatTesterPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnConnect();
	afx_msg void OnDialup();
	afx_msg void OnHangup();
	afx_msg void OnRecordFile();
	afx_msg void OnPressTest();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class CXfaceTestDlg;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGETESTERCHAT_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_)
