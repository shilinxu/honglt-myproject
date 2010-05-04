//
// VersionChecker.h
//

#if !defined(AFX_VERSIONCHECKER_H__3BF79544_0C71_4C49_BDBB_F60F27B5BEC4__INCLUDED_)
#define AFX_VERSIONCHECKER_H__3BF79544_0C71_4C49_BDBB_F60F27B5BEC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVersionChecker
{
// Construction
public:
	CVersionChecker();
	virtual ~CVersionChecker();
	
// Attributes
public:
	HANDLE				m_hThread;
	volatile BOOL		m_bThread;
protected:
	HWND				m_hNotify;
	HINTERNET			m_hInternet;
	HINTERNET			m_hSession;
protected:
	CCriticalSection	m_pSection;

// Operations
public:
	BOOL		Setup();
	void		Release();
	BOOL		Connect();
	void		Disconnect();
public:
	void		SetOwner(HWND hNotify = NULL);
	static void	GetVersionNumber(LPCTSTR xFile, WORD nVersion[]);
protected:
	BOOL		ConnectTo(LPCTSTR xHost);
	BOOL		GetFile(LPCTSTR xFile);
	LRESULT		SendMessageNotify(UINT nMsgId, WPARAM wParam = NULL, LPARAM lParam = NULL);
	BOOL		OnNextFile(LPCTSTR xFile, int* pNumFile = NULL);
	BOOL		OnParseElement(LPCTSTR xElement, BOOL bOnlyCheck = FALSE);
protected:
	static UINT	ThreadStart(LPVOID pParam);
	void		OnRun();
};

extern CVersionChecker VersionChecker;

#define WM_NEXTPAGE		(WM_USER + 101)
#define WM_CHECKOUT		(WM_USER + 102)

#endif // !defined(AFX_VERSIONCHECKER_H__3BF79544_0C71_4C49_BDBB_F60F27B5BEC4__INCLUDED_)
