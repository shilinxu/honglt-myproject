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

#define	FILE_SECY_WELCOME		"secy.welcome.pcm"
#define	FILE_TALK_RCVTELE		"talk.rcvtele.pcm"
#define	FILE_TALK_RCVUSER		"talk.rcvuser.pcm"

class CTalkChannel : public CChannel  
{
// Construction
public:
	CTalkChannel(int nChan, LPCTSTR xFeeCode);
	virtual ~CTalkChannel();

// Attributes
protected:
	CHAR*			PageNumber;
	CHAR*			UserNumber;
protected:
	struct state	FwdPlay;
	struct state	RcvLvTel;
	
// Operations
public:
	static
	CChannel*		ConvertFrom(CChannel* pChannel, LPCTSTR xString = NULL);
protected:
	void			Setup();
	virtual void	KillLinker();
	BOOL			ReadUserShortMobile(LPCTSTR xRemoted, LPTSTR xPageNumber);
	struct state*	ShExitCode_cmplt(EVTBLK *evtblk, int nExitCode = EXIT_GOODBYE);
protected:
	int				bkIdle_init(EVTBLK *evtblk);
	struct state*	FeeLogin_cmplt(EVTBLK *evtblk);
	int				rcvMFsig_init(EVTBLK *evtblk);
	int				RcvTalk_init(EVTBLK *evtblk);
	
	int				FwdPlay_init(EVTBLK *evtblk);
	struct state*	FwdPlay_cmplt(EVTBLK *evtblk);
	int				RcvLvTel_init(EVTBLK *evtblk);
	struct state*	RcvLvTel_cmplt(EVTBLK *evtblk);
	
	int				RcvUlWord_init(EVTBLK *evtblk);
	struct state*	RcvUlWord_cmplt(EVTBLK *evtblk, LPCTSTR xFeeNumber, LPCTSTR xLocalId);
	struct state*	RcvUlTalk_cmplt(EVTBLK *evtblk);
};

#endif // !defined(AFX_CHANNELTALK_H__6F3D5FE7_E67D_4549_939B_587E1E43FCED__INCLUDED_)
