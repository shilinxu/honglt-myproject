//
// XfaceTest.h
//

#if !defined(AFXXFACETEST_H__0E9E774A_836F_409F_B01B_8CA215E1F411__INCLUDED_)
#define AFXXFACETEST_H__0E9E774A_836F_409F_B01B_8CA215E1F411__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "Resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CXfaceTestApp:

class CXfaceTestApp : public CWinApp
{
// Construction
public:
	CXfaceTestApp();

// Attributes
public:
	CMutex			m_pSection;
	BOOL			m_bInitialized;

// Operations
public:
	CString			GetErrorString();

// Overrides
public:
	//{{AFX_VIRTUAL(CXfaceTestApp)
	public:
	virtual BOOL	InitInstance();
	virtual int		ExitInstance();
	virtual void	WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	//}}AFX_VIRTUAL
#ifdef _DEBUG
BOOL OnIdle(LONG lCount);
#endif

// Implementation
public:
	//{{AFX_MSG(CXfaceTestApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CXfaceTestApp theApp;

//
// Registry
//

extern const TCHAR	gcszCopyright[];
extern const TCHAR	gcszWindowName[];
extern const TCHAR	gcszWindowClass[];

//
// Utility Functions
//

extern BOOL LoadString(CString& str, UINT nID);
//extern void Replace(CString& strBuffer, LPCTSTR pszFind, LPCTSTR pszReplace);
extern LPCTSTR _tcsistr(LPCTSTR pszString, LPCTSTR pszPattern);
extern LPCTSTR _tcsnistr(LPCTSTR pszString, LPCTSTR pszPattern, DWORD plen);
//extern CString ToHex(BYTE* pBuffer, DWORD nLength);
extern CString ToASCII(BYTE* pBuffer, DWORD nLength);

//
// User Messages
//

#ifdef _DEBUG
#define MLOG(x) theApp.Message( MSG_DEBUG, x )
#else
#define MLOG(x)
#endif

#define MSG_DEFAULT		0
#define MSG_SYSTEM		1
#define MSG_ERROR		2
#define MSG_DEBUG		3
#define MSG_TEMP		4

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFXXFACETEST_H__0E9E774A_836F_409F_B01B_8CA215E1F411__INCLUDED_)
