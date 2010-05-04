//
// Circuitry.h
//

#if !defined(AFX_CIRCUITRY_H__CB905B66_8156_46D3_9C40_C4A9CD7A8A4C__INCLUDED_)
#define AFX_CIRCUITRY_H__CB905B66_8156_46D3_9C40_C4A9CD7A8A4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Interlink.h"
#include "PacketCore.h"

class CDatagramOut;

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
protected:
	CInterlink*		m_pChain;
private:
	int				m_nAlawCookie;
	
// Operations
public:
	virtual BOOL	RenderBuffer(BYTE* pBuffer, DWORD nBuffer, BYTE nType);
	static	HRESULT CALLBACK RenderBuffer(BYTE* pBuffer, DWORD nBuffer, BYTE nType, DWORD, LPVOID lv);
public:
	virtual void	Disconnect();
	virtual void	ResetChain();
	virtual BOOL	TimerOut(DWORD nTimer);
protected:
	virtual BOOL	OnRun();
	virtual BOOL	OnConnected();
	virtual void	DoPrint(BYTE* pBuffer, DWORD nLength, LPCTSTR lpszHeader);
protected:
	virtual void	ResetNetwork();
	virtual void	Clone(CCircuitry* pParent);
	virtual BOOL	RunDatagrams(CBuffer* pBuffer, CDatagramOut* pDG);
protected:
	virtual BOOL	OnPacket(CPacket* pPacket);
	virtual BOOL	OnBindPacket(CBindPacket* pPacket);
	virtual BOOL	OnDialPacket(CDialPacket* pPacket);
	virtual BOOL	OnRingPacket(CRingPacket* pPacket);
protected:
	virtual CPacket*	ReadBuffer(CBuffer* pBuffer);
	
	friend class CNeighbour;
	friend class CUserChannel;
};

#endif // !defined(AFX_CIRCUITRY_H__CB905B66_8156_46D3_9C40_C4A9CD7A8A4C__INCLUDED_)
