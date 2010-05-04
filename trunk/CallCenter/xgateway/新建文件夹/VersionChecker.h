//
// VersionChecker.h
//

#if !defined(AFX_VERSIONCHECKER_H__3BF79544_0C71_4C49_BDBB_F60F27B5BEC4__INCLUDED_)
#define AFX_VERSIONCHECKER_H__3BF79544_0C71_4C49_BDBB_F60F27B5BEC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVersionUpgrader;

class CVersionChecker  
{
// Construction
public:
	CVersionChecker();
	virtual ~CVersionChecker();
	
// Attributes
public:
	SOCKET			m_hSocket;
	CEvent			m_pWakeup;
	BOOL			m_bConnected;
	CMutex			m_pSection;
protected:
	HANDLE			m_hThread;
	volatile BOOL	m_bThread;
	CPtrList		m_pList;

// Operations
public:
	BOOL			Setup();
	void			Release();
	BOOL			Connect();
	void			Disconnect();
	BOOL			IsConnected() const;
	void			Clear();
protected:
	void			StartThread();
	void			StopThread();
	BOOL			CreateNeighbour(SOCKET hSocket, SOCKADDR_IN* pHost);
protected:
	static	UINT	ThreadStart(LPVOID pParam);
	virtual	void	OnRun();
	virtual	BOOL	AcceptNeighbour();
	virtual void	RunNeighbours();
	
// List Access
public:
	inline POSITION GetIterator() const
	{
		return m_pList.GetHeadPosition();
	}

	inline CVersionUpgrader* GetNext(POSITION& pos) const
	{
		return (CVersionUpgrader*)m_pList.GetNext( pos );
	}

	inline int GetCount() const
	{
		return m_pList.GetCount();
	}
};

extern CVersionChecker VersionChecker;

#endif // !defined(AFX_VERSIONCHECKER_H__3BF79544_0C71_4C49_BDBB_F60F27B5BEC4__INCLUDED_)
