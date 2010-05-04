//
// Interlink.h
//

#if !defined(AFX_INTERLINK_H__B1DEF5F1_E11B_42DF_A811_B2460749394F__INCLUDED_)
#define AFX_INTERLINK_H__B1DEF5F1_E11B_42DF_A811_B2460749394F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Pipeline.h"
#include "Packet.h"

class CInterlink : public CPipeline  
{
// Construction
public:
	CInterlink();
	CInterlink(HANDLE hPipe);
	virtual ~CInterlink();
	
// Attributes
public:
	
// Operations
public:
	inline	void	Release();
	virtual void	Disconnect();
	virtual BOOL	TimerOut(DWORD nTimer);
protected:
	virtual BOOL	OnRun();
	virtual BOOL	OnDropped();
	virtual BOOL	OnConnected();
	virtual void	DoPrint(BYTE* pBuffer, DWORD nLength, LPCTSTR lpszHeader);
protected:
	virtual BOOL	OnPacket(CPacket* pPacket);
	virtual BOOL	OnOkPacket(COkPacket* pPacket);
	virtual BOOL	OnErrPacket(CErrPacket* pPacket);
	virtual BOOL	OnByePacket(CByePacket* pPacket);
protected:
	virtual CPacket*	ReadBuffer(CBuffer* pBuffer);
	virtual	BOOL		SendPacket(const void* pData, DWORD nLength);
	virtual BOOL		SendPacket(CPacket* pPacket, BOOL bPeek = FALSE);
	
	friend class CCircuitry;
};

// inline access

void CInterlink::Release()
{
	if ( this ) delete this;
}

#endif // !defined(AFX_INTERLINK_H__B1DEF5F1_E11B_42DF_A811_B2460749394F__INCLUDED_)
