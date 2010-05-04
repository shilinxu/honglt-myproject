//
// Xuser32.h
//

#if !defined(AFXXUSER32_H__0E9E774A_836F_409F_B01B_8CA215E1F411__INCLUDED_)
#define AFXXUSER32_H__0E9E774A_836F_409F_B01B_8CA215E1F411__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "Resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CXuser32App:

class CXuser32App : public CWinApp
{
// Construction
public:
	CXuser32App();

// Attributes
public:
	CMutex				m_pMutex;
	CMutex				m_pSection;
	volatile BOOL		m_bInitialized;
	BOOL				m_bTestor;
protected:
	CCriticalSection	m_csMessage;
	static TCHAR		szMessageBuffer[16384];

// Operations
public:
	AFX_EXT_CLASS void	Message(int nType, UINT nID, ...);
	AFX_EXT_CLASS void	Message(int nType, LPCTSTR pszFormat, ...);
public:
	AFX_EXT_CLASS void	GetVersionNumber(WORD nVersion[]);
	CString		GetErrorString();
protected:
	void		PrintMessage(int nType, LPCTSTR pszLog);
	void		LogMessage(LPCTSTR pszLog, BOOL bShowTime = TRUE);
	BOOL		CheckUsage(CString& xUserId, CString& xPassword, BOOL& bTestor);
	BOOL		SetFirstInstance();

// Overrides
public:
	//{{AFX_VIRTUAL(CXuser32App)
	public:
	virtual BOOL	InitInstance();
	virtual int		ExitInstance();
	virtual void	WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	//}}AFX_VIRTUAL

// Implementation
public:
	//{{AFX_MSG(CXuser32App)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern AFX_EXT_CLASS CXuser32App theApp;

//
// Registry
//

extern const TCHAR	gcszCopyright[];
extern const TCHAR	gcszWindowName[];
extern const TCHAR	gcszWindowClass[];

//
// Utility Functions
//

extern AFX_EXT_CLASS BOOL LoadString(CString& str, UINT nID);
extern AFX_EXT_CLASS void Replace(CString& strBuffer, LPCTSTR pszFind, LPCTSTR pszReplace);

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

#define WM_UPDATESERVICE	(WM_USER+102)
#define WM_REQUESTRESP		(WM_USER+103)
#define WM_PACKETREQ		(WM_USER+104)
#define WM_STARTPREVIEW		(WM_USER+105)
#define WM_WINDOWCLEANUP	(WM_USER+106)
#define WM_SYNCDELIVER		(WM_USER+107)
#define WM_JOINTALK			(WM_USER+108)
#define WM_STOPRENDER		(WM_USER+109)
#define WM_RENDERBUFFER		(WM_USER+110)
#define WM_SYNCNOTIFY		(WM_USER+111)

#define NM_SHUTDOWN			101

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFXXUSER32_H__0E9E774A_836F_409F_B01B_8CA215E1F411__INCLUDED_)
