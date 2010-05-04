//
// ChannelInter.h
//

#if !defined(AFX_CHANNELINTER_H__E8DEEE17_AE9D_41FC_925E_F1E04246DD35__INCLUDED_)
#define AFX_CHANNELINTER_H__E8DEEE17_AE9D_41FC_925E_F1E04246DD35__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"

//
// definitions of file name
//

#define	FILE_INTR_WELCOME		"intr.welcome.pcm"
#define	FILE_INTR_FINDUSR		"intr.findusr.pcm"

class CInterChannel : public CChannel  
{
// Construction
public:
	CInterChannel(int nChan, LPCTSTR xFeeCode = "9900");
	virtual ~CInterChannel();

// Attributes
protected:
	void	(CInterChannel::*m_fnKiller)();
protected:
	struct state	FwdPlay;
	struct state	FindUser;

// Operations
public:
	static
	CChannel*		ConvertFrom(CChannel* pChannel, LPCTSTR xString = NULL);
	BOOL			PutUserRing(LPCTSTR xLocalId, LPCTSTR xRemoteId);
	BOOL			PutNetworkRing(LPCTSTR xLocalId, LPCTSTR xRemoteId);
protected:
	void			Setup();
	virtual void	KillLinker();
	virtual void	KillNetwork();
protected:
	int				bkIdle_init(EVTBLK *evtblk);
	int				FeeLogin_init(EVTBLK *evtblk);
	struct state*	FeeLogin_cmplt(EVTBLK *evtblk);
	int				rcvMFsig_init(EVTBLK *evtblk);
	int				RcvTalk_init(EVTBLK *evtblk);
	struct state*	RcvTalk_cmplt(EVTBLK *evtblk);
	
	int				FwdPlay_init(EVTBLK *evtblk);
	struct state*	FwdPlay_cmplt(EVTBLK *evtblk);
	int				FindUser_init(EVTBLK *evtblk);
	struct state*	FindUser_cmplt(EVTBLK *evtblk);
};

#endif // !defined(AFX_CHANNELINTER_H__E8DEEE17_AE9D_41FC_925E_F1E04246DD35__INCLUDED_)
