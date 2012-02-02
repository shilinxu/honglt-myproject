//
// ServiceThread.h
//

#if !defined(AFX_SERVICETHREAD_H__F8730A3D_293C_4A47_9DD8_F2F7EF976410__INCLUDED_)
#define AFX_SERVICETHREAD_H__F8730A3D_293C_4A47_9DD8_F2F7EF976410__INCLUDED_

#pragma once

///////////////////////////////////////////////////////////////////////////////
// interface

interface IServiceThread
{
	virtual BOOL StartThread() = NULL;
	virtual BOOL StopThread(DWORD dwTimeOut) = NULL;
	virtual BOOL TerminateThread(DWORD dwExitCode) = NULL;
	virtual BOOL IsRuning() const = NULL;
};

///////////////////////////////////////////////////////////////////////////////
// CServiceThread

class COM_SERVICE_CLASS CServiceThread : public IServiceThread
{
// Construction
protected:
	CServiceThread();
	virtual ~CServiceThread();

// Attributes
public:
	UINT			m_nThreadId;
protected:
	HANDLE			m_hThread;
	volatile BOOL	m_bThread;

// Operations
public:
	virtual	BOOL	StartThread();
	virtual	BOOL	StopThread(DWORD dwTimeOut = INFINITE);
	virtual BOOL	TerminateThread(DWORD dwExitCode);
	virtual	BOOL	IsRuning() const;
protected:
	static	UINT __stdcall ThreadStart(LPVOID pParam);
	virtual	BOOL	OnThreadStart() { return TRUE; }
	virtual	BOOL	OnThreadConclude() { return TRUE; }
	virtual	BOOL	DoRun() = NULL;
private:
	BOOL	OnRun();
};

///////////////////////////////////////////////////////////////////////////////
// CThreadGuard

class CThreadGuard
{
// Construction
public:
	CThreadGuard() { InitializeCriticalSection(&m_cs); }
	~CThreadGuard() { DeleteCriticalSection(&m_cs); }

// Operations
public:
	void Lock() { EnterCriticalSection(&m_cs); }
	void Unlock() { LeaveCriticalSection(&m_cs); }
private:
	CRITICAL_SECTION m_cs;
};

template <typename T>
class CAutoLock
{
// Construction
public:
	CAutoLock(T& lockable) : m_lockable(lockable) { m_lockable.Lock(); }
	~CAutoLock() { m_lockable.Unlock(); }
private:
	T& m_lockable;
};

#endif // !defined(AFX_SERVICETHREAD_H__F8730A3D_293C_4A47_9DD8_F2F7EF976410__INCLUDED_)
