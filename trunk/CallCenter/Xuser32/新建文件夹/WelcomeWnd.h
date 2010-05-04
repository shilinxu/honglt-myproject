//
// WelcomeWnd.h
//

#if !defined(AFX_WELCOMEWND_H__B01CB059_198A_4A2C_B15C_857612750AB0__INCLUDED_)
#define AFX_WELCOMEWND_H__B01CB059_198A_4A2C_B15C_857612750AB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndChild.h"

/////////////////////////////////////////////////////////////////////////////
// CTelecomWnd window

class CWelcomeWnd : public CChildWnd  
{
// Construction
public:
	CWelcomeWnd();
	virtual ~CWelcomeWnd();

	DECLARE_SERIAL(CWelcomeWnd)

// Attributes
protected:
	CBitmap		m_bmHeader;
	CComboBox	m_wndUser;
	CEdit		m_wndPass;
	CButton		m_wndSave;
	CButton		m_wndLogin;
	CButton		m_wndLogout;
protected:
	int			m_nLeftMargin;
	int			m_nTopMargin;
	COLORREF	m_crWhite;
	CBrush		m_brWhite;

// Operations
protected:
	void			PaintHeader(CDC& dc, CRect& rc);
	virtual void	DoPaint(CDC& dc, CRect& rc);
	int				LoadHistory();
	void			Layout();

// Overrides
public:
	//{{AFX_VIRTUAL(CWelcomeWnd)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CWelcomeWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void OnUsrLogin();
	virtual void OnUsrLogout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_USER_NUMBER		101
#define IDC_USER_PASSWORD	1002
#define IDC_SAVE_PASSWORD	1003
#define IDC_USER_LOGIN		1004
#define IDC_USER_LOGOUT		1005

#endif // !defined(AFX_WELCOMEWND_H__B01CB059_198A_4A2C_B15C_857612750AB0__INCLUDED_)
