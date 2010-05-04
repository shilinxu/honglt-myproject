//
// ChannelPlay.h
//

#if !defined(AFX_CHANNELPLAY_H__71DA1205_0347_471B_9790_9CD3DEB1D5C7__INCLUDED_)
#define AFX_CHANNELPLAY_H__71DA1205_0347_471B_9790_9CD3DEB1D5C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"

//
// definitions of file name
//

#define	FILE_VIST_WELCOME		"vist.welcome.pcm"
#define	FILE_VIST_RCVOPER		"vist.rcvoper.pcm"

class CPlayChannel : public CChannel  
{
// Construction
public:
	CPlayChannel(int nChan, LPCTSTR xFeeCode = "1333");
	virtual ~CPlayChannel();

// Attributes
protected:
	_RecordsetPtr	m_pRecordset;
protected:
	struct state	FwdPlay;
	struct state	RcvFnOper;
	
	struct state	RcvVista;
	
// Operations
protected:
	virtual void	Setup();
	int				ShRcvTalk_init(EVTBLK *evtblk, struct state* lpszDefaultState);
	int				ShExitPlay_init(EVTBLK *evtblk, int nExitCode);
	struct state*	ShExitPlay_cmplt(EVTBLK *evtblk, int nExitCode);
	struct state*	FwdCaller(LPCTSTR xLocalId, struct state* lpszDefaultState);
protected:
	BOOL			OpenRecord(LPCTSTR xPhone);
	struct state*	KeyNotify(CHAR nDtmfKey);
protected:
	int				bkIdle_init(EVTBLK *evtblk);
	int				FeeLogin_init(EVTBLK *evtblk);
	int				FeeRebind_init(EVTBLK *evtblk);
	struct state*	FeeLogin_cmplt(EVTBLK *evtblk);
	int				rcvMFsig_init(EVTBLK *evtblk);
	int				RcvTalk_init(EVTBLK *evtblk);

	int				FwdPlay_init(EVTBLK *evtblk);
	struct state*	FwdPlay_cmplt(EVTBLK *evtblk);
	int				RcvFnOper_init(EVTBLK *evtblk);
	struct state*	RcvFnOper_cmplt(EVTBLK *evtblk);
	
	int				RcvVista_init(EVTBLK *evtblk);
};

#endif // !defined(AFX_CHANNELPLAY_H__71DA1205_0347_471B_9790_9CD3DEB1D5C7__INCLUDED_)
