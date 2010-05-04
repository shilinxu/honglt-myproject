//
// ChannelUser.h
//

#if !defined(AFX_CHANNELUSER_H__B55F8BDC_4162_4DDB_8596_153F57A34582__INCLUDED_)
#define AFX_CHANNELUSER_H__B55F8BDC_4162_4DDB_8596_153F57A34582__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"
#include "PacketCore.h"

class CUserChannel : public CChannel
{
// Construction
public:
	CUserChannel(LPCTSTR xUserId, LPCTSTR xGroupId);
	virtual ~CUserChannel();

// Attributes
protected:
	LPTSTR			m_xGroupId;
	LPTSTR			m_xLinkId;
private:
	int				m_nAlawCookie;
	int				m_nH263Cookie;

// Operations
public:
	virtual void	Setup();
	virtual void	SetLinkId(LPCTSTR xLinkId);
protected:
	virtual void	Disconnect(DWORD);
	virtual void	OnHandshake();
	virtual BOOL	OnRun();
protected:
	virtual BOOL	OnPacket(CPacket* pPacket);
	virtual BOOL	OnSyncDataView(CSyncDataView* pPacket);
	virtual BOOL	OnSyncHostFile(CSyncHostFile* pPacket);
	virtual BOOL	OnSyncOrderRelation(CXMLElement* pXML);
protected:
	virtual BOOL	OnSyncOrderRelation(CXMLElement* pXML, DWORD);
	virtual void	OnSyncMirrorView(DWORD);
	virtual BOOL	OnSyncMirrorView(CXMLElement* pXML);
	virtual BOOL	OnSyncMonitorView(CXMLElement* pXML) = 0;
	virtual BOOL	OnSyncDataView(CXMLElement* pXML);
//	virtual BOOL	OnSyncDataFileReq(CXMLElement* pXML);
//	virtual BOOL	OnSyncDataFileResp(CXMLElement* pXML, HANDLE hFile);
//	virtual BOOL	OnSyncDataFileResp(CXMLElement* pXML, UINT nErrId);
	virtual BOOL	OnSyncHostshaking(CXMLElement* pXML);
	virtual BOOL	OnSyncHostBreaking(CXMLElement* pXML);
	virtual BOOL	OnSyncHostRecording(CXMLElement* pXML) = 0;
	virtual BOOL	OnSyncHostTalking(CXMLElement* pXML);
};

//
// Configuration
//

#define GUEST_CODE		_T("xent9999999999")

#endif // !defined(AFX_CHANNELUSER_H__B55F8BDC_4162_4DDB_8596_153F57A34582__INCLUDED_)
