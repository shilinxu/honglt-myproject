//
// Traffic.h
//

#if !defined(AFX_TRAFFIC_H__54895494_ABFB_4B29_B096_FB3204AFC5B4__INCLUDED_)
#define AFX_TRAFFIC_H__54895494_ABFB_4B29_B096_FB3204AFC5B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPacket;
class CChannel;

class AFX_EXT_CLASS CTraffic  
{
// Construction
public:
	CTraffic();
	virtual ~CTraffic();

// Attributes
public:
	CEvent			m_pWakeup;
	CMutex			m_pSection;
protected:
	HANDLE			m_hThread;
	volatile BOOL	m_bThread;
protected:
	CMap<COleDateTime, COleDateTime&, CPacket*, CPacket*&> m_pUsrBacks;

// Operations
public:
	BOOL			Setup();
	void			Release();
	CPacket*		AddUsrBack(LPCTSTR xLocalId, LPCTSTR xRemoteId);
	BOOL			IsMusic(LPCTSTR xPath) const;
protected:
	CPacket*		AddUsrBack(CPacket* pPacket, DWORD nReserve = 0);
	CChannel*		PagerLibrary(CChannel* pParent, LPCTSTR pszName, LPCTSTR xRemoteId, LPCTSTR xFeeNumber, DWORD nReserved = 0);
protected:
	void			StartThread();
	void			StopThread();
	void			Clear();
protected:
	BOOL			OnRunUsrBack(POSITION posNext);
	BOOL			OnRunMeeting();
	static	UINT	ThreadStart(LPVOID pParam);
	virtual	BOOL	OnRun();
};

extern AFX_EXT_CLASS CTraffic Traffic;

#endif // !defined(AFX_TRAFFIC_H__54895494_ABFB_4B29_B096_FB3204AFC5B4__INCLUDED_)
