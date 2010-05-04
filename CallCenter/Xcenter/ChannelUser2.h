//
// ChannelUser2.h
//

#if !defined(AFX_CHANNELUSER2_H__C4C1BB07_1995_4BFA_AFBD_E6DF6DCAB947__INCLUDED_)
#define AFX_CHANNELUSER2_H__C4C1BB07_1995_4BFA_AFBD_E6DF6DCAB947__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChannelUser.h"

//
// definitions of file name
//

#define	FILE_USER_WAITING		"user.waiting.pcm"

class CUserWnd;

class CUserChannel2 : public CUserChannel  
{
// Construction
public:
	CUserChannel2(int nChan, LPCTSTR xUserId);
	virtual ~CUserChannel2();

// Attributes
protected:
	CUserWnd*		m_pUserWnd;
	int				m_nThirdId;
	BOOL			m_bDialing;
	
// Operations
protected:
	virtual void	Setup();
	CChannel*		ConvertFrom(LPCTSTR xInstance, CChannel* pParent, LPCTSTR xPacket);
	virtual void	PrintChState(EVTBLK* evtblk);
	virtual void	Disconnect();
	virtual void	OnHandshake();
protected:
	virtual BOOL	OnPacket(CPacket* pPacket);
	virtual BOOL	OnBindPacket(CBindPacket* pPacket);
	virtual BOOL	OnWaitPacket(CWaitPacket* pPacket);
	virtual BOOL	OnDeliverPacket(CDeliverPacket* pPacket);
	virtual BOOL	OnDialPacket(CDialPacket* pPacket);
	virtual BOOL	OnRingPacket(CRingPacket* pPacket);
protected:
	int				bkOffHk_init(EVTBLK *evtblk);
	int				rcvMFsig_init(EVTBLK* evtblk);
	struct state*	rcvMFsig_cmplt(EVTBLK *evtblk);
	struct state*	RcvTalk_cmplt(EVTBLK *evtblk);
};

#endif // !defined(AFX_CHANNELUSER2_H__C4C1BB07_1995_4BFA_AFBD_E6DF6DCAB947__INCLUDED_)
