//
// ChannelUser.h
//

#if !defined(AFX_CHANNELUSER_H__B55F8BDC_4162_4DDB_8596_153F57A34582__INCLUDED_)
#define AFX_CHANNELUSER_H__B55F8BDC_4162_4DDB_8596_153F57A34582__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"
#include "RTPIPv4Address.h"
class CJoinVisitorDlg;

class CUserChannel	: public CChannel
{
// Construction
public:
	CUserChannel(SOCKET hSocket, SOCKADDR_IN* pHost);
	CUserChannel(LPCTSTR xUserId = NULL);
	virtual ~CUserChannel();

// Attributes
protected:
	CJoinVisitorDlg*	m_pVisitorWnd;
	
// Operations
public:
	virtual void	Setup();
	virtual void	OnHandshake();
	virtual void	SetUserId(LPCTSTR xUserId);
//	virtual BOOL	IsNavigated(LPCTSTR xCallType) const;
public:
	virtual BOOL	CallRemote(LPCTSTR xRemoteId, LPCTSTR xLocalId = NULL);
	virtual BOOL	PickupRemote();
	virtual void	HangRemote();
public:
	virtual BOOL	JoinTalk(LPCTSTR xRemoteId, LPCTSTR xCallerId = NULL);
	virtual BOOL	LeaveTalk();
	virtual void	PutHangup();
protected:
	virtual void	Disconnect();
protected:
	virtual BOOL	OnPacket(CPacket* pPacket);
	virtual BOOL	OnSyncOrderRelation(CSyncOrderRelation* pPacket);
	virtual BOOL	OnSyncHostLink(CXMLElement* pXML);
	virtual BOOL	OnSyncDataView(CXMLElement* pXML);
	//virtual BOOL	OnSyncOrderRelation(CXMLElement* pXML);
//	virtual BOOL	OnSyncMirrorView(CXMLElement* pXML);
	virtual BOOL	OnSyncMonitorView(CXMLElement* pXML);
protected:	// interface
	virtual BOOL	OnSyncHostshaking(CXMLElement* pXML);
	virtual BOOL	OnSyncHostBreaking(CXMLElement* pXML);
public:
//	virtual void	JoinMirror(LPCTSTR pszRender, UINT ssrc, BOOL bJoined = TRUE);
//	virtual void	JoinMonitor(BOOL bLeave = FALSE);
protected:
//	virtual void	JoinMirror(UINT ssrc, BOOL bLeave = FALSE);
	
	friend class CJoinVisitorDlg;
};

#endif // !defined(AFX_CHANNELUSER_H__B55F8BDC_4162_4DDB_8596_153F57A34582__INCLUDED_)
