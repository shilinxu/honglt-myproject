//
// EventService.h
//

#if !defined(EVENT_SERVICE_H__INCLUDED_)
#define EVENT_SERVICE_H__INCLUDED_

#pragma once
#include "KernelManager.h"

typedef	void (CALLBACK* LPONEVENTBLOCK)(WORD dwEventCode, DWORD dwReference, DWORD dwEndReason, DWORD dwParam, LPVOID lv);

//
// class CEventService
//

class CEventService	: public CServiceThread
{
// Definitions
public:
	typedef struct tagEventBlock
	{
		WORD			wEventCode;
		DWORD			dwReference;
		DWORD			dwTimestamp;
		LPONEVENTBLOCK	fnModuleProc;
		DWORD			dwEndReason;
		DWORD			dwParam;
	} EVENTBLOCK, *LPEVENTBLOCK;
protected:
	typedef CAtlArray<LPEVENTBLOCK> CEventBlockPtrArray;

// Construction
public:
	CEventService();
	virtual ~CEventService();

// Attributes
public:
	IApplication*				m_pApplication;
protected:
	CEventBlockPtrArray			m_pEventBlockPtrActive;
	CEventBlockPtrArray			m_pEventBlockPtrRelease;
protected:
	HANDLE						m_hEvent;
	CThreadGuard				m_pSection;
	static TCHAR				szMessageBuffer[16384];

// Operations
public:
	KERNEL_MANAGER_CLASS static BOOL	StartService(IApplication* pApplication);
	KERNEL_MANAGER_CLASS static BOOL	StopService();
public:
	KERNEL_MANAGER_CLASS static	void	Message(int nType, LPCTSTR pszFormat, ...);
	KERNEL_MANAGER_CLASS static void	CreateEvent(WORD wEventCode, DWORD dwReference, LPONEVENTBLOCK fnModuleProc, DWORD dwTimestamp, DWORD dwParam);
	KERNEL_MANAGER_CLASS static void	ResetEvent(WORD wEventCode, DWORD dwReference, DWORD dwEndReason, DWORD dwParam = 0);
	KERNEL_MANAGER_CLASS static void	ResetEvent(WORD wEventCode, DWORD dwReference);
protected:
	virtual void	Print(int nType, LPCTSTR pszFormat, ...);
	virtual void	Print(int nType, LPCTSTR pszFormat, va_list pArgs);
protected:
	virtual	WORD	_AddEvent_(WORD wEventCode, DWORD dwReference, LPONEVENTBLOCK fnModuleProc, DWORD dwTimestamp, DWORD dwParam);
	virtual	BOOL	_ResetEvent_(WORD dwEventCode, DWORD dwReference, DWORD dwEndReason, DWORD dwParam);
	virtual	BOOL	_RemoveEvent_(WORD dwEventCode, DWORD dwReference);
protected:
	static CEventService*	Default();
protected:
	virtual BOOL	PrintMessage(int nType, LPCTSTR pszMsg);
	virtual	void	OnEventBlock(LPEVENTBLOCK pEventBlock, DWORD dwEndReason, DWORD dwParam);
	virtual	BOOL	DoRun();
protected:
	INT_PTR			_FindEvent_(WORD wEventCode, DWORD dwReference);
	INT_PTR			PostEventRun(LPEVENTBLOCK* ppEventBlock = NULL);
	void			PostEventBlock(DWORD dwEndReason = E_SYS_NoError);
	BOOL			GetEventBlock(LPEVENTBLOCK& pEventBlock);
	void			CleanEvent();
};

#endif // TRACE_SERVICE_H__INCLUDED_