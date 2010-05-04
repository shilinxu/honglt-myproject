//
// CtrlTelecomPanel.h
//

#if !defined(AFX_CTRLTELECOMPANEL_H__089F2565_529B_4BC3_BA2A_7B8BDA135D4B__INCLUDED_)
#define AFX_CTRLTELECOMPANEL_H__089F2565_529B_4BC3_BA2A_7B8BDA135D4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CtrlTaskPanel.h"
#include "CtrlIconButton.h"
#include "CtrlP2jobPanel.h"
/*
class CTelecomTaskBox : public CTaskBox
{
// Construction
public:
	CTelecomTaskBox();
	virtual ~CTelecomTaskBox();
	DECLARE_DYNAMIC(CTelecomTaskBox)

// Attributes
protected:
	CComboBox		m_wndGroup;
	CEdit			m_wndVoice;
	CIconButtonCtrl	m_wndStart;
	CIconButtonCtrl	m_wndClear;
	
// Operations
public:

// Overrides
public:
	//{{AFX_VIRTUAL(CTelecomTaskBox)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CTelecomTaskBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnStart();
	afx_msg void OnClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
*/
class CTelecomPanel : public CTaskPanel
{
// Construction
public:
	CTelecomPanel();
	virtual ~CTelecomPanel();
	DECLARE_DYNAMIC(CTelecomPanel)

// Attributes
protected:
	CP2jobTaskBox		m_wndP2job;

// Operations
public:

// Overrides
public:
	//{{AFX_VIRTUAL(CTelecomPanel)
	virtual BOOL Create(CWnd* pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CTelecomPanel)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_TELECOM_PANEL		101
//#define IDC_TELECOM_GROUP		1001
//#define IDC_TELECOM_VOICE		1002
//#define IDC_TELECOM_START		1003
//#define IDC_TELECOM_CLEAR		1004
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLTELECOMPANEL_H__089F2565_529B_4BC3_BA2A_7B8BDA135D4B__INCLUDED_)
