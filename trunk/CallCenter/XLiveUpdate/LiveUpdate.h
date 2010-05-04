//
// Xuser32.h
//

#if !defined(AFXLIVEUPDATE_H__0E9E774A_836F_409F_B01B_8CA215E1F411__INCLUDED_)
#define AFXLIVEUPDATE_H__0E9E774A_836F_409F_B01B_8CA215E1F411__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "Resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CLiveUpdateApp:

class CLiveUpdateApp : public CWinApp
{
// Construction
public:
	CLiveUpdateApp();

// Attributes
public:
	DWORD		m_nProcessId;
public:
	CFont		m_fntNormal;
	CFont		m_fntBold;
	CFont		m_fntUnder;
	CFont		m_fntCaption;
	CFont		m_fntItalic;

// Operations
public:
	CString		GetErrorString();
protected:
	void		CreateFonts(LPCTSTR pszFace = NULL, int nSize = 0);

// Overrides
public:
	//{{AFX_VIRTUAL(CLiveUpdateApp)
	public:
	virtual BOOL	InitInstance();
	virtual int		ExitInstance();
	virtual void	WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	//}}AFX_VIRTUAL

// Implementation
public:
	//{{AFX_MSG(CLiveUpdateApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CLiveUpdateApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFXLIVEUPDATE_H__0E9E774A_836F_409F_B01B_8CA215E1F411__INCLUDED_)
