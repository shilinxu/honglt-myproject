//
// ChannelCaller.h
//

#if !defined(AFX_CHANNELCALLER_H__FC4B9EC0_9506_4ECE_8618_C45B2D3FD427__INCLUDED_)
#define AFX_CHANNELCALLER_H__FC4B9EC0_9506_4ECE_8618_C45B2D3FD427__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"

//
// definitions of file name
//

#define	FILE_KINGB_WELCOME		"kingb.welcome.pcm"
#define	FILE_KINGB_FINDUSR		"kingb.findusr.pcm"
#define	FILE_KINGB_NOTUSER		"kingb.notuser.pcm"

class CUserWnd;

class CCallerChannel : public CChannel  
{
// Construction
public:
	CCallerChannel(int nChan, LPCTSTR xFeeCode);
	virtual ~CCallerChannel();

// Attributes
protected:
	CUserWnd*		m_pUserWnd;
protected:
	struct state	FwdCaller;
	struct state	FindUser;

// Operations
public:
	BOOL			IsOnlyUserId(LPCTSTR xRemoteId);
	BOOL			IsNotUserTime();
protected:
	virtual void	Setup();
	virtual void	KillLinker();
	virtual void	SetLinker(int nChan);
protected:
	int				bkIdle_init(EVTBLK *evtblk);
	int				FeeLogin_init(EVTBLK *evtblk);
	struct state*	FeeLogin_cmplt(EVTBLK *evtblk);
	int				rcvMFsig_init(EVTBLK *evtblk);
	struct state*	WaitDial_cmplt(EVTBLK *evtblk);
	int				RcvTalk_init(EVTBLK *evtblk);
	struct state*	RcvTalk_cmplt(EVTBLK *evtblk);
	
	int				FwdCaller_init(EVTBLK *evtblk);
	struct state*	FwdCaller_cmplt(EVTBLK *evtblk);
	int				FindUser_init(EVTBLK *evtblk);
	struct state*	FindUser_cmplt(EVTBLK *evtblk);
	
	int				ExitPlay_init(EVTBLK *evtblk);
};

#endif // !defined(AFX_CHANNELCALLER_H__FC4B9EC0_9506_4ECE_8618_C45B2D3FD427__INCLUDED_)
