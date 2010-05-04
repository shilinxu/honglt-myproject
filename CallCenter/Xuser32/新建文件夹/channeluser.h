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
	CUserChannel();
	virtual ~CUserChannel();

// Attributes
protected:
	
// Operations
public:
	virtual void	Setup();
	virtual void	Release();
protected:
	virtual BOOL	OnPacket(CPacket* pPacket);
	virtual	BOOL	OnSyncOrderRelationReq(CSyncOrderRelation* pPacket);
	
	friend class CHomeWnd;
	enum
	{
		htsNull, htsReady,
		htsHeader,
		htsContent,
		htsComplete,
	};
};

#endif // !defined(AFX_CHANNELUSER_H__B55F8BDC_4162_4DDB_8596_153F57A34582__INCLUDED_)
