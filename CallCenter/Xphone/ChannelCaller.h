//
// ChannelCaller.h
//

#if !defined(AFX_CHANNELCALLER_H__B55F8BDC_4162_4DDB_8596_153F57A34582__INCLUDED_)
#define AFX_CHANNELCALLER_H__B55F8BDC_4162_4DDB_8596_153F57A34582__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"

class CCallerChannel	: public CChannel
{
// Construction
public:
	CCallerChannel(LPCTSTR xUserId = NULL);
	virtual ~CCallerChannel();

// Attributes
public:
	LPTSTR			m_xUserId;
	
// Operations
public:
	virtual void	Setup();
	virtual void	OnHandshake();
	virtual BOOL	IsNavigated(LPCTSTR xCallType) const;
public:
	virtual void	SetUserId(LPCTSTR xUserId);
	virtual BOOL	ConnectTo(LPCTSTR xUserId, LPCTSTR xPassword);
public:
};

#endif // !defined(AFX_CHANNELCALLER_H__B55F8BDC_4162_4DDB_8596_153F57A34582__INCLUDED_)
