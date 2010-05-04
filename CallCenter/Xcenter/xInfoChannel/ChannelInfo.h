//
// ChannelInfo.h
//

#if !defined(AFX_CHANNELINFO_H__8884D113_13E0_4974_9C09_875E8B1CE676__INCLUDED_)
#define AFX_CHANNELINFO_H__8884D113_13E0_4974_9C09_875E8B1CE676__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"

//
// definitions of file name
//

#define	FILE_INFO_WELCOME		"info.welcome.pcm"
#define FILE_INFO_WELCOME_LT	FILE_INFO_WELCOME
//#define	FILE_INFO_WELCOME_LT	"info.welcome_lt.pcm"
#define	FILE_INFO_RCVOPER		"info.rcvoper.pcm"
#define	FILE_INFO_WTLVTEL		"info.wtlvtel.pcm"
#define	FILE_INFO_RCVLVWRD		"info.rcvlvwrd.pcm"

#define	FILE_PAGE_WTLVTELE		"page.wtlvtele.pcm"
#define	FILE_PAGE_RCVLVWRD		"page.rcvlvwrd.pcm"
#define	FILE_PAGE_BJMUSIC		"page.bjmusic.pcm"
#define	FILE_PAGE_PLYLVWRD		"page.plylvwrd.pcm"

class  CInfoChannel : public CChannel
{
// Construction
public:
	CInfoChannel(int nChan, LPCTSTR xFeeCode = "9501");
	virtual ~CInfoChannel();
	
// Attributes
protected:
	_RecordsetPtr	m_pRecordset;
	CHAR*			PageNumber;
	CString			UsrLvFile;
	BOOL			BjPlaying;
protected:
	struct state	FwdPlay;
	struct state	RcvFnOper;
	
	struct state	RcvLvTele;
	struct state	RcvLvWord;
	
	struct state	PlyLvFile;
	struct state	PlyLvWord;
	
	struct state	PlayXFile;
	
// Operations
protected:
	BOOL			OpenXRecord(LPCTSTR xPlay);
	BOOL			IsOnlyUserId(LPCTSTR xRemoteId);
	BOOL			IsUnicomUser() const;
	LPCTSTR			GetFileWelcome() const;
protected:
	int				GetUsrLvCount();
	BOOL			OpenUsrRecord(LPCTSTR xPhone);
	BOOL			PutUsrRecord(LPCTSTR xFaxFile);
	CString			MakeUsrLvFile(LPTSTR sExtFile = "pcm");
	BOOL			TryPagerToGroup(int xGroupId, LPCTSTR xUsrFile);
	int				ShPlyNavFile(int nChan, LPCTSTR xNavFile);
protected:
	virtual void	Setup();
	struct state*	FwdNotify(LPCTSTR xLocalId);
	struct state*	FwdXPlay(BYTE nDtmfKey);
	virtual BOOL	OnBindPacket(CBindPacket* pPacket);
	int				ShExitCode_init(EVTBLK *evtblk, int nExitCode = EXIT_GOODBYE);
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
	
	int				RcvLvTele_init(EVTBLK *evtblk);
	struct state*	RcvLvTele_cmplt(EVTBLK *evtblk);
	int				RcvLvWord_init(EVTBLK *evtblk);
	struct state*	RcvLvWord_cmplt(EVTBLK *evtblk);
	
	int				PlyLvFile_init(EVTBLK *evtblk);
	struct state*	PlyLvFile_cmplt(EVTBLK *evtblk);
	int				PlyLvWord_init(EVTBLK *evtblk);
	struct state*	PlyLvWord_cmplt(EVTBLK *evtblk);
	
	int				PlayXFile_init(EVTBLK *evtblk);
	struct state*	PlayXFile_cmplt(EVTBLK *evtblk);
};

#endif // !defined(AFX_CHANNELINFO_H__8884D113_13E0_4974_9C09_875E8B1CE676__INCLUDED_)
