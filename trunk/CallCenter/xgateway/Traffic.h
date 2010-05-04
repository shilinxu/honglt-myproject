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

class CTraffic : public RTPSession
{
// Construction
public:
	CTraffic();
	virtual ~CTraffic();

// Attributes
public:
	CEvent			m_pWakeup;
	CMutex			m_pSection;
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
	CMutex			m_pSection;
	RTPUDPv4Transmitter*	m_pTransmitter;
protected:
	CMap<CString, LPCTSTR, UINT, UINT>	m_pList;
	CMap<UINT, UINT, RTPPayload*, RTPPayload*>	m_pDatagrams;

// Operations
public:
	virtual int		RenderFile(WORD nPort, DWORD nSamplesPer);
	virtual BOOL	JoinedMirror(LPCTSTR xUserId, UINT ssrc);
	virtual BOOL	LeavedMirror(LPCTSTR xUserId, UINT ssrc);
public:
	virtual int		SendPayload(const void *pBuffer, size_t nBuffer, BYTE nPayloadType, DWORD nTimestampinc);
	virtual HRESULT	RenderBuffer(BYTE* pBuffer, DWORD nLength, BYTE nPayloadType, LPCTSTR xUserId);
	CXMLElement*	FormatRTP(LPCTSTR xUserId, CXMLElement* pXML);
	int				JoinMirrorWith(LPCTSTR xUserId);
protected:
	BOOL			LookupKey(UINT ssrc, CString& xUserId);
	int				JoinMirror(RTPSourceData* srcdat, LPCTSTR xUserId);
protected:
	virtual void	OnPollThreadStep();
	virtual void	OnNewSource(RTPSourceData *srcdat);
	virtual void	OnRemoveSource(RTPSourceData *srcdat);
	virtual void	OnRTPPacket(RTPPacket *pack, const RTPTime &receivetime, const RTPAddress *senderaddress);
	virtual HRESULT DeliverBuffer(RTPPacket *pack, RTPPayload*& pPayload);
//	virtual HRESULT DeliverBuffer(RTPPayload*& pPayload, LPCTSTR xUserId);
	virtual HRESULT DeliverBuffer(BYTE* pBuffer, DWORD nLength, BYTE nPayloadType, DWORD nTimestamp, LPCTSTR xUserId);
	virtual HRESULT RenderBuffer(RTPPayload*& pPayload, LPCTSTR xUserId);
protected:
	CXMLElement*	FormatRTP(CXMLElement* pParent, RTPSourceData* srcdat, BOOL bRTP = TRUE);
	void			Clear();
protected:
	virtual void	OnAPPPacket(RTCPAPPPacket *apppacket, const RTPTime &receivetime, const RTPAddress *senderaddress);
	virtual BOOL	ParseXML(CXMLElement* pXML, RTPSourceData *srcdat);
	virtual BOOL	OnSyncMirrorView(CXMLElement* pXML, UINT ssrc);
	
	friend class CRtpReceiverFilter;
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

extern CTraffic Traffic;
extern RTPSessionLayer RTPAudio;
extern RTPSessionLayer RTPVideo;

#endif // !defined(AFX_TRAFFIC_H__54895494_ABFB_4B29_B096_FB3204AFC5B4__INCLUDED_)
