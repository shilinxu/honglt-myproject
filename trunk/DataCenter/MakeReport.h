//
// MakeReport.h
//

#if !defined(AFX_MAKEREPORT_H__9C50190B_BD15_4456_817A_C6D9E63ED55A__INCLUDED_)
#define AFX_MAKEREPORT_H__9C50190B_BD15_4456_817A_C6D9E63ED55A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "Resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMakeReportApp:

class CMakeReportApp : public CWinApp
{
// Construction
public:
	CMakeReportApp();
	
// Attributes
public:
	CMutex				m_pSection;
	WORD				m_nVersion[4];

	_ConnectionPtr		m_pConnection;
	BOOL				m_bInitialized;
	
// Operations
public:
	CString		GetErrorString();
protected:
	BOOL		SetFirstInstance();
	void		GetVersionNumber();
protected:
	BOOL		OpenDatabase();
	BOOL		CloseDatabase();

// Overrides
public:
	//{{AFX_VIRTUAL(CMakeReportApp)
	public:
	virtual BOOL	InitInstance();
	virtual int		ExitInstance();
	virtual void	WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	//}}AFX_VIRTUAL

// Implementation
public:
	//{{AFX_MSG(CMakeReportApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CMakeReportApp theApp;

//
// Registry
//

extern const TCHAR	gcszCopyright[];
extern const TCHAR	gcszWindowName[];
extern const TCHAR	gcszWindowClass[];

//
// Utility Functions
//

BOOL LoadString(CString& str, UINT nID);
LPCTSTR _tcsistr(LPCTSTR pszString, LPCTSTR pszPattern);
LPCTSTR _tcsnistr(LPCTSTR pszString, LPCTSTR pszPattern, DWORD plen);
void Replace(CString& strBuffer, LPCTSTR pszFind, LPCTSTR pszReplace);
CString ToHex(BYTE* pBuffer, DWORD nLength);
CString ToASCII(BYTE* pBuffer, DWORD nLength);

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

#define WM_LOG			(WM_USER+109)

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAKEREPORT_H__9C50190B_BD15_4456_817A_C6D9E63ED55A__INCLUDED_)
