//
// Xphone.h
//

#if !defined(AFXXPHONE_H__0E9E774A_836F_409F_B01B_8CA215E1F411__INCLUDED_)
#define AFXXPHONE_H__0E9E774A_836F_409F_B01B_8CA215E1F411__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "Resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CXphoneApp:

class CXphoneApp : public CWinApp
{
// Construction
public:
	CXphoneApp();

// Attributes
public:
	CMutex				m_pMutex;
	CMutex				m_pSection;
	WORD				m_nVersion[4];
protected:
	BOOL				m_bInitialized;

// Operations
public:
	CString		GetErrorString();
	BOOL		CheckUsage(CString& xUserId, CString& xPassword);
protected:
	BOOL		SetFirstInstance();
	void		GetVersionNumber();

// Overrides
public:
	//{{AFX_VIRTUAL(CXphoneApp)
	public:
	virtual BOOL	InitInstance();
	virtual int		ExitInstance();
	virtual void	WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	//}}AFX_VIRTUAL

// Implementation
public:
	//{{AFX_MSG(CXphoneApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CXphoneApp theApp;

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

#define WM_TRAY			(WM_USER+101)
#define WM_PACKETREQ	(WM_USER+104)
#define WM_STARTPREVIEW	(WM_USER+105)
#define WM_BINDUSER		(WM_USER+101)
#define WM_LOG			(WM_USER+109)

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFXXPHONE_H__0E9E774A_836F_409F_B01B_8CA215E1F411__INCLUDED_)
