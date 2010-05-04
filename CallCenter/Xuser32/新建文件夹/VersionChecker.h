//
// VersionChecker.h
//

#if !defined(AFX_VERSIONCHECKER_H__3BF79544_0C71_4C49_BDBB_F60F27B5BEC4__INCLUDED_)
#define AFX_VERSIONCHECKER_H__3BF79544_0C71_4C49_BDBB_F60F27B5BEC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSplashDlg;
class CVersionUpgrader;

class CVersionChecker  
{
// Construction
public:
	CVersionChecker();
	virtual ~CVersionChecker();
	
// Attributes
public:
	CSplashDlg*			m_pWndNotify;
	CVersionUpgrader*	m_pUpgrader;
#if 0
	CEvent				m_pWakeup;
protected:
	HANDLE				m_hThread;
	volatile BOOL		m_bThread;
	CCriticalSection	m_pSection;
#endif

// Operations
public:
	BOOL		Setup(CSplashDlg* pWndNotify);
	void		Release();
	void		Restart(CWinApp* pApp);
protected:
	BOOL		OnNextFile(LPCTSTR xFile);
	BOOL		OnParseElement(LPCTSTR xElement);
	void		GetVersionNumber(LPCTSTR xFile, WORD nVersion[]);
#if 0
protected:
	void		StartThread();
	void		StopThread();
	BOOL		HitUpgrader();
protected:
	static UINT	ThreadStart(LPVOID pParam);
	void		OnRun();
#endif
};

extern CVersionChecker VersionChecker;

#endif // !defined(AFX_VERSIONCHECKER_H__3BF79544_0C71_4C49_BDBB_F60F27B5BEC4__INCLUDED_)
