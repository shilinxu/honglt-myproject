//
// ChannelMesger.h
//

#if !defined(AFX_CHANNELMESGER_H__71DA1205_0347_471B_9790_9CD3DEB1D5C7__INCLUDED_)
#define AFX_CHANNELMESGER_H__71DA1205_0347_471B_9790_9CD3DEB1D5C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"

//
// definitions of file name
//

#define	FILE_MSGER_WELCOME		"msger.welcome.pcm"

#define	FILE_PAGE_PLYLVWRD		"page.plylvwrd.pcm"

class CMesgerChannel : public CChannel  
{
// Construction
public:
	CMesgerChannel(int nChan, LPCTSTR xFeeCode = "MSGR");
	virtual ~CMesgerChannel();

// Attributes
protected:
	_RecordsetPtr	m_pRecordset;
protected:
	struct state	FwdPlay;
	
	struct state	PlyLvFile;
	struct state	PlyLvWord;
	
// Operations
protected:
	virtual void	Setup();
	int				ShRcvTalk_init(EVTBLK *evtblk, struct state* lpszDefaultState);
	int				ShExitPlay_init(EVTBLK *evtblk, int nExitCode);
	struct state*	ShExitPlay_cmplt(EVTBLK *evtblk, int nExitCode);
protected:
	void			RecUsrBack(CHAR cDtmfKey);
	BOOL			OpenMailbox(int nMaxTopic = 100);
	int				ShPlyNavFile(int nChan, LPCTSTR xNavFile);
protected:
	int				bkIdle_init(EVTBLK *evtblk);
	int				FeeLogin_init(EVTBLK *evtblk);
	struct state*	FeeLogin_cmplt(EVTBLK *evtblk);
	int				rcvMFsig_init(EVTBLK *evtblk);
	int				RcvTalk_init(EVTBLK *evtblk);

	int				FwdPlay_init(EVTBLK *evtblk);
	struct state*	FwdPlay_cmplt(EVTBLK *evtblk);
	
	int				PlyLvFile_init(EVTBLK *evtblk);
	struct state*	PlyLvFile_cmplt(EVTBLK *evtblk);
	int				PlyLvWord_init(EVTBLK *evtblk);
	struct state*	PlyLvWord_cmplt(EVTBLK *evtblk);

};

#endif // !defined(AFX_CHANNELMESGER_H__71DA1205_0347_471B_9790_9CD3DEB1D5C7__INCLUDED_)
