//
// ChannelInter.h
//

#if !defined(AFX_CHANNELINTER_H__E8DEEE17_AE9D_41FC_925E_F1E04246DD35__INCLUDED_)
#define AFX_CHANNELINTER_H__E8DEEE17_AE9D_41FC_925E_F1E04246DD35__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"
#define	FILE_CALLER_FINDUSR		"caller.findusr.pcm"

class CInterChannel : public CChannel  
{
	friend class CUserChannel;
	
// Construction
public:
	CInterChannel(int nChan, LPCTSTR xFeeCode = "9900");
	virtual ~CInterChannel();

// Attributes
protected:
	struct state	FindUser;

// Operations
protected:
	virtual void	Setup();
	virtual void	SetLinker(int nChan);
protected:
	int				bkIdle_init(EVTBLK *evtblk);
	int				bkOffHk_init(EVTBLK *evtblk);
	int				rcvMFsig_init(EVTBLK *evtblk);
	int				RcvTalk_init(EVTBLK *evtblk);
	struct state*	RcvTalk_cmplt(EVTBLK *evtblk);
	struct state*	FeeLogin_cmplt(EVTBLK *evtblk);
	struct state*	WaitDial_cmplt(EVTBLK *evtblk);
	
	int				FindUser_init(EVTBLK *evtblk);
	struct state*	FindUser_cmplt(EVTBLK *evtblk);
	int				TalkUser_init(EVTBLK *evtblk);
	struct state*	TalkUser_cmplt(EVTBLK *evtblk);
};

#endif // !defined(AFX_CHANNELINTER_H__E8DEEE17_AE9D_41FC_925E_F1E04246DD35__INCLUDED_)
