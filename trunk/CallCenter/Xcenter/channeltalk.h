//
// ChannelTalk.h
//

#if !defined(AFX_CHANNELTALK_H__6F3D5FE7_E67D_4549_939B_587E1E43FCED__INCLUDED_)
#define AFX_CHANNELTALK_H__6F3D5FE7_E67D_4549_939B_587E1E43FCED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"

//
// definitions of file name
//

#define	FILE_TALK_WELCOME		"talk.welcome.pcm"
#define	FILE_TALK_RCVTELE		"talk.rcvtele.pcm"
#define	FILE_TALK_RCVUSER		"talk.rcvuser.pcm"
#define	FILE_TALK_REDPAY1		"talk.redpay1.pcm"
#define	FILE_TALK_REDPAY2		"talk.redpay2.pcm"
#define	FILE_TALK_REDPAY3		"talk.redpay3.pcm"

class CTalkChannel : public CChannel  
{
// Construction
public:
	CTalkChannel(int nChan, LPCTSTR xFeeCode = "1600");
	virtual ~CTalkChannel();

// Attributes
protected:
	CHAR			PageNumber[MAX_PHONE+1];
	CHAR			UserNumber[MAX_PHONE+1];
protected:
	struct state	FwdTalk;
	struct state	RcvTele;
	struct state	RcvUser;
	struct state	TalkWith;
	
// Operations
protected:
	void			Setup();
protected:
	virtual void	OnPacket(IPacket* pPacket);
	virtual void	OnRedPacket(IRedPacket* pPacket);
protected:
	int				bkIdle_init(EVTBLK *evtblk);
	int				RcvTalk_init(EVTBLK *evtblk);
	int				bkOffHk_init(EVTBLK *evtblk);
	struct state*	seiConfm_cmplt(EVTBLK *evtblk);
	struct state*	FeeLogin_cmplt(EVTBLK *evtblk);
	int				rcvMFsig_init(EVTBLK *evtblk);
	
	int				FwdTalk_init(EVTBLK *evtblk);
	struct state*	FwdTalk_cmplt(EVTBLK *evtblk);
	int				RcvTele_init(EVTBLK *evtblk);
	struct state*	RcvTele_cmplt(EVTBLK *evtblk);
	int				RcvUser_init(EVTBLK *evtblk);
	struct state*	RcvUser_cmplt(EVTBLK *evtblk);
	int				TalkWith_init(EVTBLK *evtblk);
	struct state*	TalkWith_cmplt(EVTBLK *evtblk);
	int				TalkHost_init(EVTBLK *evtblk);
};

#endif // !defined(AFX_CHANNELTALK_H__6F3D5FE7_E67D_4549_939B_587E1E43FCED__INCLUDED_)
