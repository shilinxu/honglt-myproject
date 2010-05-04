//
// Pipeline.h
//

#if !defined(AFX_PIPELINE_H__8FD482C2_3D03_4B21_8FAC_BD6644F6B4BD__INCLUDED_)
#define AFX_PIPELINE_H__8FD482C2_3D03_4B21_8FAC_BD6644F6B4BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Buffer.h"

class CPipeline  
{
// Construction
public:
	CPipeline();
	CPipeline(HANDLE hPipe);
	virtual ~CPipeline();

// Attributes
public:
	BOOL		m_bConnected;
	DWORD		m_tConnected;
protected:
	HANDLE		m_hPipe;
	CBuffer*	m_pInput;
	CBuffer*	m_pOutput;
	OVERLAPPED*	m_pReadIoOver;
	OVERLAPPED*	m_pWriteIoOver;
private:
	BYTE*		m_pBuffer;
	DWORD		m_nBuffer;

// Operations
public:
	BOOL			IsConnected() const;
public:
	CString			ToHex(BYTE* pBuffer, DWORD nLength);
	CString			ToASCII(BYTE* pBuffer, DWORD nLength);
public:
	virtual	BOOL	Connect(LPCTSTR lpPipeName, DWORD nMaxInstances = PIPE_UNLIMITED_INSTANCES, DWORD nTimeOut = NMPWAIT_WAIT_FOREVER);
	virtual BOOL	ConnectTo(LPCTSTR lpPipeName, DWORD nTimeOut = NMPWAIT_WAIT_FOREVER);
	virtual void	AcceptFrom(HANDLE hPipe);
	virtual void	AttachTo(CPipeline* pConnection);
	virtual void	Close();
protected:
	virtual BOOL	OnRun();
	virtual BOOL	OnRead();
	virtual BOOL	OnWrite();
	virtual BOOL	OnConnected();
	virtual BOOL	OnDropped();
	virtual BOOL	OnReadCompleted();
	virtual BOOL	OnWriteCompleted();
	virtual void	DoPrint(BYTE* pBuffer, DWORD nLength, LPCTSTR lpszHeader);
};

#endif // !defined(AFX_PIPELINE_H__8FD482C2_3D03_4B21_8FAC_BD6644F6B4BD__INCLUDED_)
