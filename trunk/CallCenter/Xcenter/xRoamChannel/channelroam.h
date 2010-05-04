//
// ChannelRoam.h
//

#if !defined(AFX_CHANNELROAM_H__6F3D5FE7_E67D_4549_939B_587E1E43FCED__INCLUDED_)
#define AFX_CHANNELROAM_H__6F3D5FE7_E67D_4549_939B_587E1E43FCED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"

//
// definitions of file name
//

#define	FILE_TALK_WELCOME		"talk.welcome.pcm"

class CRoamChannel : public CChannel  
{
// Construction
public:
	CRoamChannel(int nChan, LPCTSTR xFeeCode);
	virtual ~CRoamChannel();

// Attributes
protected:
	CHAR*			PageNumber;
	CHAR*			UserNumber;
protected:
	struct state	FwdPlay;
	
// Operations
protected:
	void			Setup();
	virtual void	KillLinker();
protected:
	BOOL			ReadUserAgent(LPCTSTR xUserId, LPCTSTR xLocalId, CString* pxAgentId);
	struct state*	ShExitCode_cmplt(EVTBLK *evtblk, int nExitCode = EXIT_GOODBYE);
protected:
	int				bkIdle_init(EVTBLK *evtblk);
	struct state*	FeeLogin_cmplt(EVTBLK *evtblk);
	int				RcvTalk_init(EVTBLK *evtblk);
	
	int				FwdPlay_init(EVTBLK *evtblk);
	struct state*	FwdPlay_cmplt(EVTBLK *evtblk);
	
	int				RcvUlWord_init(EVTBLK *evtblk);
	struct state*	RcvUlWord_cmplt(EVTBLK *evtblk);
};

#endif // !defined(AFX_CHANNELROAM_H__6F3D5FE7_E67D_4549_939B_587E1E43FCED__INCLUDED_)
