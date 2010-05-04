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

#define	FILE_PLAY_WELCOME		"play.welcome.pcm"
#define	FILE_PLAY_RCVOPER		"play.rcvoper.pcm"
#define	FILE_PAGE_WTLVTELE		"page.wtlvtele.pcm"
#define	FILE_PAGE_RCVLVWRD		"page.rcvlvwrd.pcm"
#define	FILE_PAGE_BJMUSIC		"page.bjmusic.pcm"
#define	FILE_PAGE_PLYLVWRD		"page.plylvwrd.pcm"

class CPlayChannel : public CChannel  
{
// Construction
public:
//	CPlayChannel(CChannel* pParent);
	CPlayChannel(CChannel* pParent, LPCTSTR xMusic, DWORD nReserved);
	CPlayChannel(int nChan, LPCTSTR xFeeCode = "1222");
	CPlayChannel(CChannel* pParent, LPCTSTR xRemoteId);
	CPlayChannel(CChannel* pParent, LPCTSTR xPCMFile, LPCTSTR xDefaultRemote);
	virtual ~CPlayChannel();

// Attributes
protected:
	_RecordsetPtr	m_pRecordset;
	CHAR*			PageNumber;
	CString			UsrLvFile;
	CString			BjMusic;
	COleDateTime	UsrLvTime;
	BOOL			IsBjMusic;
	BOOL			BjPlaying;
protected:
	struct state	FwdPlay;
	struct state	RcvFnOper;
	
	struct state	RcvLvTele;
	struct state	RcvLvWord;
	
	struct state	PlyLvFile;
	struct state	PlyLvWord;
	
	struct state	PlyMusic;
	CStringArray	LstMusic;
	int				PosMusic;
	
// Operations
public:
	static
	CChannel*		ConvertMusic(CChannel* pChannel, LPCTSTR xMusic);
	static
	CChannel*		ConvertFrom(CChannel* pChannel, LPCTSTR xString = NULL);
	static
	CChannel*		ConvertPager(CChannel* pParent, LPCTSTR xRemoteId);
	static
	CChannel*		ConvertPager(CChannel* pParent, LPCTSTR xRemoteId, BOOL bBjMusic);
	static
	CChannel*		ConvertForward(CChannel* pParent, LPCTSTR xPCMFile);
	BOOL			FormatObject(LPCTSTR xRemoteId, LPCTSTR xBjMusic);
	BOOL			FormatMusic(LPCTSTR xString);
protected:
	virtual void	Setup();
	struct state*	FwdNotify(LPCTSTR xLocalId, struct state* lpszDefaultState);
	void			FwdNotify(LPCTSTR xLocalId, LPCTSTR xCallType);
	int				ShPlyNavFile(int nChan, LPCTSTR xNavFile);
	virtual BOOL	OnBindPacket(CBindPacket* pPacket);
	int				ShRcvTalk_init(EVTBLK *evtblk, struct state* lpszDefaultState);
protected:
	int				GetUsrLvCount();
	BOOL			IsOnlyToSelf() const;
	BOOL			OpenUsrRecord(LPCTSTR xPhone);
	BOOL			PutUsrRecord(LPCTSTR xUsrFile);
	BOOL			PutUsrRecord(LPCTSTR xUsrFile, DWORD nReserved);
	CString			MakeUsrLvFile(LPTSTR sExtFile = "pcm");
	BOOL			TryPagerToGroup(int xGroupId, LPCTSTR xUsrFile);
	BOOL			PutUsrMusic(LPCTSTR xUsrFile, LPCTSTR xMusicFile);
	BOOL			PutUsrMusic(LPCTSTR xUsrMusic, DWORD nReserved);
	BOOL			TryMusicToGroup(int xGroupId, LPCTSTR xUsrMusic);
	BOOL			ReadUserShortMobile(LPCTSTR xRemoted, LPTSTR xPageNumber);
	BOOL			OpenFavRecord(LPCTSTR xPhone);
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
	
	int				PlyFavFile_init(EVTBLK *evtblk);
	int				PlyFavWord_init(EVTBLK *evtblk);
	
	int				RcvTalkLvWord_init(EVTBLK *evtblk);
	int				RcvTalkLvPlay_init(EVTBLK *evtblk);
	int				RcvTalkLvResp_init(EVTBLK *evtblk);
	int				RcvTalkLvBack_init(EVTBLK *evtblk);
	struct state*	RcvLvWordOk_cmplt(EVTBLK *evtblk);
	struct state*	RcvLvTeleOk_cmplt(EVTBLK *evtblk);
	
	int				RcvTalkMusic_init(EVTBLK *evtblk);	// ×ÔÌý¸èÇú
	int				PlyMusic_init(EVTBLK *evtblk);
	struct state*	PlyMusic_cmplt(EVTBLK *evtblk);
};

#endif // !defined(AFX_CHANNELPLAY_H__71DA1205_0347_471B_9790_9CD3DEB1D5C7__INCLUDED_)
