//
// ChannelUser.h
//

#if !defined(AFX_CHANNELUSER_H__B55F8BDC_4162_4DDB_8596_153F57A34582__INCLUDED_)
#define AFX_CHANNELUSER_H__B55F8BDC_4162_4DDB_8596_153F57A34582__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"

class CUserChannel	: public CChannel
{
// Construction
public:
	CUserChannel(SOCKET hSocket, SOCKADDR_IN* pHost);
	CUserChannel(LPCTSTR xUserId = NULL);
	virtual ~CUserChannel();

// Attributes
protected:
	
// Operations
protected:
	virtual void	Setup();
protected:
	virtual BOOL	OnPacket(CPacket* pPacket);
	virtual BOOL	OnSyncOrderRelation(CXMLElement* pXML);
	virtual BOOL	OnSyncDataView(CXMLElement* pXML);
public:
	virtual void	JoinMonitor(BOOL bJoined = TRUE);
	
	friend class CJoinVisitorDlg;
};

#endif // !defined(AFX_CHANNELUSER_H__B55F8BDC_4162_4DDB_8596_153F57A34582__INCLUDED_)
