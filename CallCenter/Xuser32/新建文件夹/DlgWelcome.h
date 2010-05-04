//
// DlgWelcome.h
//

#if !defined(AFX_DLGWELCOME_H__B99F0E55_6587_4EF8_88AD_66787BEE3240__INCLUDED_)
#define AFX_DLGWELCOME_H__B99F0E55_6587_4EF8_88AD_66787BEE3240__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CWelcomeDlg dialog

class CWelcomeDlg : public CDialog
{
// Construction
public:
	CWelcomeDlg();
	virtual ~CWelcomeDlg();
	DECLARE_DYNAMIC(CWelcomeDlg)
	
// Dialog Data
protected:
	//{{AFX_DATA(CWelcomeDlg)
	enum { IDD = IDD_WELCOME };
	//}}AFX_DATA
	
// Attributes
protected:
	CString	m_sCaption;
	
// Operations
public:
	BOOL	LoadDefaultCaption();
	BOOL	Create(const RECT& rect, CWnd* pSheetWnd);
	
// Overrides
public:
	//{{AFX_VIRTUAL(CWelcomeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
//	CWelcomeDlg(CWnd* pParent = NULL);   // standard constructor
	//{{AFX_MSG(CWelcomeDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGWELCOME_H__B99F0E55_6587_4EF8_88AD_66787BEE3240__INCLUDED_)
