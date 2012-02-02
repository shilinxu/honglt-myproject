//
// TransferManager.h
//

#if !defined(AFX_TRANSFERMANAGER_H__1F5AEBEA_1F73_4B1B_9783_2C2EC3559BCE__INCLUDED_)
#define AFX_TRANSFERMANAGER_H__1F5AEBEA_1F73_4B1B_9783_2C2EC3559BCE__INCLUDED_

#pragma once

class CTransferThread;

typedef CAtlArray<ITransferable *> CTransferablePtrArray;
typedef CAtlArray<CTransferThread *> CTransferThreadPtrArray;

class KERNEL_MANAGER_CLASS CTransferManager	: protected CServiceThread
{
// Construction
public:
	CTransferManager();
	virtual ~CTransferManager();

// Attributes
protected:
	HANDLE			m_hCompletionPort;
	CThreadGuard	m_pSection;
	HANDLE			m_hWakeup;
protected:
	CTransferablePtrArray	m_pTransferArray;
	CTransferThreadPtrArray	m_pTransferThreadArray;
private:
	LONG			m_nReference;

// Operations
public:
	virtual BOOL	StartService();
	virtual BOOL	StopService();
public:
// 	virtual	DWORD	GetIdentifier() const;
	virtual BOOL	Attach(ITransferable* pTransfer, HANDLE hHandle, LONG lEvent = FD_CONNECT);
	virtual BOOL	Detach(ITransferable* pTransfer);
protected:
	virtual	BOOL	DoRun();
protected:
	void			PostCleanup();
};

class CTransferThread : public CServiceThread
{
// Construction
public:
	CTransferThread(HANDLE hCompletionPort);
	virtual ~CTransferThread();
protected:
	CTransferThread();
	
// Attributes
public:
	HANDLE			m_hCompletionPort;
	
// Operations
protected:
	virtual	BOOL	DoRun();
	virtual	BOOL	OnThreadStart();
};
// 
// extern KERNEL_MANAGER_CLASS CTransferManager TransferManager;

#endif // !defined(AFX_TRANSFERMANAGER_H__1F5AEBEA_1F73_4B1B_9783_2C2EC3559BCE__INCLUDED_)
