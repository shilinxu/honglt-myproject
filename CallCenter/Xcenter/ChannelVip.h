//
// ChannelVip.h
//

#if !defined(AFX_CHANNELVIP_H__A6B8B5CF_9A47_40AE_807A_F519212F32B7__INCLUDED_)
#define AFX_CHANNELVIP_H__A6B8B5CF_9A47_40AE_807A_F519212F32B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Interlink.h"

class CVipChannel : public CInterlink
{
// Construction
public:
	CVipChannel(LPCTSTR xUserId, int nChan);
	virtual ~CVipChannel();
#ifdef _DEBUG
//static BOOL ReadBlock(EVTBLK* evtblk);
#endif

// Attributes
public:
	LPTSTR			m_xUserId;
	const	int		m_nChan;

// Operations
protected:
};

#endif // !defined(AFX_CHANNELVIP_H__A6B8B5CF_9A47_40AE_807A_F519212F32B7__INCLUDED_)
