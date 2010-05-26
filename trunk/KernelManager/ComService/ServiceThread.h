//
// ServiceThread.h
//

#if !defined(AFX_SERVICETHREAD_H__F8730A3D_293C_4A47_9DD8_F2F7EF976410__INCLUDED_)
#define AFX_SERVICETHREAD_H__F8730A3D_293C_4A47_9DD8_F2F7EF976410__INCLUDED_

#pragma once
AKM_BEGIN_VERSIONED_NAMESPACE_DECL

///////////////////////////////////////////////////////////////////////////////
// interface

interface IServiceThread
{
	virtual BOOL StartThread() = NULL;
	virtual BOOL StopThread(DWORD dwTimeOut) = NULL;
	virtual BOOL TerminateThread(DWORD dwExitCode) = NULL;
	virtual BOOL IsActive() const = NULL;
};

///////////////////////////////////////////////////////////////////////////////
// CServiceThread

class AKM_Export CServiceThread : public IServiceThread
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
	virtual	BOOL	IsActive() const;
protected:
	static	UINT __stdcall ThreadStart(LPVOID pParam);
	virtual	BOOL	DoRun() = NULL;
	virtual	BOOL	OnRun();
};

AKM_END_VERSIONED_NAMESPACE_DECL

#endif // !defined(AFX_SERVICETHREAD_H__F8730A3D_293C_4A47_9DD8_F2F7EF976410__INCLUDED_)
