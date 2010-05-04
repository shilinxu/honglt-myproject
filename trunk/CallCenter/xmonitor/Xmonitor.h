//
// Xmonitor.h
//

#if !defined(AFXXMONITOR_H__0E9E774A_836F_409F_B01B_8CA215E1F411__INCLUDED_)
#define AFXXMONITOR_H__0E9E774A_836F_409F_B01B_8CA215E1F411__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "Resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CXmonitorApp:

class CXmonitorApp : public CWinApp
{
// Construction
public:
	CXmonitorApp();

// Attributes
public:
	CMutex				m_pMutex;
	CMutex				m_pSection;

	_ConnectionPtr		m_pConnection;
	BOOL				m_bInitialized;
protected:
	CCriticalSection	m_csMessage;
	static TCHAR		szMessageBuffer[16384];

// Operations
public:
	void		Message(int nType, UINT nID, ...);
	void		Message(int nType, LPCTSTR pszFormat, ...);
public:
	void		GetVersionNumber(WORD nVersion[]);
	CString		GetErrorString();
protected:
	BOOL		OpenDatabase();
	BOOL		CloseDatabase();
protected:
	void		PrintMessage(int nType, LPCTSTR pszLog);
	void		LogMessage(LPCTSTR pszLog, BOOL bShowTime = TRUE);

// Overrides
public:
	//{{AFX_VIRTUAL(CXmonitorApp)
	public:
	virtual BOOL	InitInstance();
	virtual int		ExitInstance();
	virtual void	WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	//}}AFX_VIRTUAL

// Implementation
public:
	//{{AFX_MSG(CXmonitorApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class CConnectDlg;
};

extern CXmonitorApp theApp;

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
void Replace(CString& strBuffer, LPCTSTR pszFind, LPCTSTR pszReplace);
LPCTSTR _tcsistr(LPCTSTR pszString, LPCTSTR pszPattern);
LPCTSTR _tcsnistr(LPCTSTR pszString, LPCTSTR pszPattern, DWORD plen);

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

//
// User Messages
//

#define WM_PACKETREQ		(WM_USER+104)

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFXXMINITOR_H__0E9E774A_836F_409F_B01B_8CA215E1F411__INCLUDED_)
