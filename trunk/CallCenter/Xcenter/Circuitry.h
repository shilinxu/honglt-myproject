//
// Circuitry.h
//

#if !defined(AFX_CIRCUITRY_H__CB905B66_8156_46D3_9C40_C4A9CD7A8A4C__INCLUDED_)
#define AFX_CIRCUITRY_H__CB905B66_8156_46D3_9C40_C4A9CD7A8A4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//
// Configuration
//

#define MAX_RECLST	2
#define MAX_PLAYLST	2

#include "Interlink.h"

class CCircuitry : public CInterlink  
{
// Construction
public:
	CCircuitry(LPCTSTR xUserId);
	CCircuitry(HANDLE hPipe);
	virtual ~CCircuitry();
	
// Attributes
public:
	LPTSTR			m_xUserId;
	DWORD			m_nTaskId;
protected:
	CInterlink*		m_pChain;
	BYTE*			m_pBuffer;
	BYTE(*			m_pBufferIn)[MAX_PLAYLST];
	CEvent			m_pWakeup;
private:
	OVERLAPPED*		m_pRecIoOver;
	OVERLAPPED*		m_pPlayIoOver;
	int				m_nPlyCookie;
	
// Operations
public:
	BOOL			Start(int nChan, BOOL Rebind = FALSE);
	BOOL			Reset();
protected:
	void			FreeBuffer();
	void			NewBuffer(int nChan);
public:
	virtual BOOL	OnPacket(CPacket* pPacket);
	virtual void	Disconnect();
protected:
	virtual BOOL	OnRun();
	virtual BOOL	OnDropped();
	virtual BOOL	OnConnected();
	virtual void	Clone(CCircuitry* pParent);
	virtual void	DoPrint(BYTE* pBuffer, DWORD nLength, LPCTSTR lpszHeader);
protected:
	virtual int		ReplayWith(int nChan, CInterlink* pChain);
	virtual int		RecordWith(int nChan, BYTE* pBuffer, DWORD dwStopOffset = 0);
	static	BOOL CALLBACK OnRecMemBlockDone(int ch, int nEndReason, PUCHAR pucBuf, DWORD dwStopOffset, PVOID pV);
	static	BOOL CALLBACK OnPlayMemBlockDone(int ch, int nEndReason, PUCHAR pucBuf, DWORD dwStopOffset, PVOID pV);
protected:
	virtual BOOL	OnBindPacket(CBindPacket* pPacket);
	virtual BOOL	OnDialPacket(CDialPacket* pPacket);
	virtual BOOL	OnRingPacket(CRingPacket* pPacket);
	virtual BOOL	OnTalkPacket(CTalkPacket* pPacket);
private:
	virtual CPacket*	ReadBuffer(CBuffer* pBuffer);
	int				ResetRecord(OVERLAPPED*	pRecIoOver);
	int				ResetReplay(OVERLAPPED*	pPlayIoOver);
	
	friend class CNeighbour;
};

#endif // !defined(AFX_CIRCUITRY_H__CB905B66_8156_46D3_9C40_C4A9CD7A8A4C__INCLUDED_)
