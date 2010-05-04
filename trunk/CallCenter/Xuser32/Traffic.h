//
// Traffic.h
//

#if !defined(AFX_TRAFFIC_H__54895494_ABFB_4B29_B096_FB3204AFC5B4__INCLUDED_)
#define AFX_TRAFFIC_H__54895494_ABFB_4B29_B096_FB3204AFC5B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Buffer.h"
#include "RTPSession.h"
class RTPUDPv4Transmitter;
class RTPSessionLayer;
struct RTPPayload;
class CXMLElement;

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
	CMapStringToPtr	m_pList;

// Operations
public:
	virtual int		RenderFile(LPCTSTR pszName, WORD nPort, DWORD nSamplesPer);
	virtual void	ReleaseFile(LPCTSTR pszName);
public:
	RTPSessionLayer* FindChannel(LPCTSTR xUserId) const;
	RTPSessionLayer* SetChannel(RTPSessionLayer* pChannel, LPCTSTR xUserId);
protected:
	void			Clear();
};

class RTPSessionLayer : public RTPSession 
{
// Construction
public:
	RTPSessionLayer();
	virtual ~RTPSessionLayer();

// Attributes
protected:
	RTPUDPv4Transmitter*	m_pTransmitter;
	CCriticalSection m_pSection;
protected:
	CMap<CString, LPCTSTR, UINT, UINT>	m_pList;
	CMap<UINT, UINT, RTPPayload*, RTPPayload*>	m_pDatagrams;

// Operations
public:
	virtual int		RenderFile(WORD nPort, DWORD nSamplesPer);
	virtual BOOL	JoinedMirror(LPCTSTR xUserId, UINT ssrc);
	virtual BOOL	LeavedMirror(LPCTSTR xUserId, UINT ssrc);
public:
	int				SendPayload(const void *pBuffer, size_t nBuffer, BYTE nPayloadType, DWORD nTimestampinc);
	virtual BOOL	TravelChild(LPCTSTR xUserId);
public:
	virtual int		JoinMirror(LPCTSTR xUserId, const RTPAddress& pReceiver, BOOL bLeave = FALSE);
	virtual int		JoinMirror(LPCTSTR xUserId, CXMLElement* pXML, BOOL bJoined = TRUE);
	virtual int		JoinRecord(BOOL bJoined = TRUE);
public:
	virtual HRESULT RenderBuffer(LPCTSTR xUserId, HRESULT (CALLBACK* lpfnRender)(LPVOID, RTPPayload*), LPVOID lpHandle);
	virtual HRESULT PurgeBuffer(LPCTSTR xUserId);
protected:
	virtual void	OnPollThreadStep();
	virtual void	OnRTPPacket(RTPPacket *pack, const RTPTime &receivetime, const RTPAddress *senderaddress);
	virtual void	OnAPPPacket(RTCPAPPPacket *apppacket, const RTPTime &receivetime, const RTPAddress *senderaddress);
	virtual BOOL	ParseXML(CXMLElement* pXML, RTPSourceData *srcdat);
	virtual HRESULT DeliverBuffer(RTPPacket *pack, RTPPayload*& pPayload);
	virtual BOOL	OnSyncMirrorView(CXMLElement* pXML, UINT ssrc);
protected:
	BOOL			PostMessageToOwner(CXMLElement* pXML, UINT ssrc);
private:
	BOOL			RTPAddressFrom(CXMLElement* pXML, RTPAddress* addr);
	void			Clear();
	
	friend class CTraffic;
};

struct RTPPayload : RTPHeader
{
	CBuffer		PayloadBuffer;
	RTPPayload*	PayloadNext;

	RTPPayload(RTPPayload* pPayload = NULL)
	{
		PayloadNext = pPayload;
	}
	
	virtual ~RTPPayload()
	{
		if ( PayloadNext ) delete PayloadNext;
	}
};

#define RTP_Audio	"RTPAudio"
#define RTP_Video	"RTPVideo"
#define RTP_File	"RTPFile"

extern AFX_EXT_CLASS CTraffic Traffic;

#endif // !defined(AFX_TRAFFIC_H__54895494_ABFB_4B29_B096_FB3204AFC5B4__INCLUDED_)
