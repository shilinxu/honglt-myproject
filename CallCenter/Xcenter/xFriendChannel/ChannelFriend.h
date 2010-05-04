//
// ChannelFriend.h
//

#if !defined(AFX_CHANNELFRIEND_H__8884D113_13E0_4974_9C09_875E8B1CE676__INCLUDED_)
#define AFX_CHANNELFRIEND_H__8884D113_13E0_4974_9C09_875E8B1CE676__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"

//
// definitions of file name
//
#define	FILE_FRND_WELCOME		"frnd.welcome.pcm"
#define	FILE_FRND_RCVOPER		"frnd.rcvoper.pcm"
#define	FILE_FRND_WTLVMEMB		"frnd.wtlvmemb.pcm"
#define	FILE_FRND_RCVLVOPER		"frnd.rcvlvoper.pcm"
#define	FILE_FRND_RCVULOPER		"frnd.rcvuloper.pcm"
#define	FILE_FRND_RCVULWORD		"frnd.rcvulword.pcm"
#define	FILE_FRND_BJMUSIC		"frnd.bjmusic.pcm"
#define	FILE_FRND_RCVCOMP		"frnd.rcvcomp.pcm"
#define	FILE_FRND_NOTUSER		"frnd.notuser.pcm"
#define	FILE_FRND_PLYULWORD		"frnd.plyulword.pcm"

class  CFriendChannel : public CChannel
{
// Construction
public:
	CFriendChannel(int nChan, LPCTSTR xFeeCode = "1999");
	CFriendChannel(int nChan, LPCTSTR xFeeCode, BYTE nReserved);
	virtual ~CFriendChannel();
	
// Attributes
protected:
	_RecordsetPtr	m_pRecordset;
	_variant_t		m_pHostmark;
	LPVOID			LvWordPtr;
	DWORD			LvWordSize;
protected:
	struct state	FwdPlay;
	struct state	RcvFnOper;
	struct state	RcvUsrComp;
	struct state	PlyUsrComp;
	struct state	RcvLvMemb;
	struct state	RcvLvWord;
	struct state	ErrReject;
	struct state	RcvLvOper;
	struct state	PlyUlWord;
	struct state	RcvUlOper;
	struct state	RcvUlWord;
	
// Operations
protected:
	virtual void	Setup();
	virtual void	KillLinker();
	virtual LPVOID	NewBuffer(DWORD nBufSize);
	virtual void	SetBuffer(LPVOID pBuf, DWORD nBufSize);
	virtual void	FreeBuffer();
	BOOL			ChkUserState(LPCTSTR xUsrNumber);
	BOOL			FindUserId(LPCTSTR xUserId);
	BOOL			IsRejectTime();
	int				PlayBlob(int nChan, FieldPtr xPlyBlob, BOOL bTermDtmf = TRUE);
protected:
	struct state*	FwdCaller(LPCTSTR xLocalId);
	int				ShPlyUlWord_init(EVTBLK *evtblk);
	struct state*	ShRcvLvMemb_cmplt(EVTBLK *evtblk, struct state* lpstEntryState);
	struct state*	ShRcvFdWord_cmplt(EVTBLK *evtblk, LPTSTR lpstBuffer, struct state* lpstDefaultState, struct state* lpstEntryState);
	struct state*	ShPlyUlWord_cmplt(EVTBLK *evtblk, struct state* lpstEntryState);
	struct state*	ShPlyUlWordX_cmplt(EVTBLK *evtblk, CHAR xSex);
	struct state*	ShExitCode_cmplt(EVTBLK *evtblk, int nExitCode = EXIT_GOODBYE);
	struct state*	ShPlayEnd_cmplt(EVTBLK *evtblk, struct state* lpstDefaultState, struct state* lpstEntryState);
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
	
	int				RcvUsrComp_init(EVTBLK *evtblk);
	struct state*	RcvUsrComp_cmplt(EVTBLK *evtblk);
	int				PlyUsrComp_init(EVTBLK *evtblk);
	struct state*	PlyUsrComp_cmplt(EVTBLK *evtblk);
	
	int				RcvLvMemb_init(EVTBLK *evtblk);
	struct state*	RcvLvMemb_cmplt(EVTBLK *evtblk);
	int				RcvLvWord_init(EVTBLK *evtblk);
	struct state*	RcvLvWord_cmplt(EVTBLK *evtblk);
	int				PlyLvWord_init(EVTBLK *evtblk);
	int				ErrReject_init(EVTBLK *evtblk);
	struct state*	ErrReject_cmplt(EVTBLK *evtblk);
	
	int				RcvLvOper_init(EVTBLK *evtblk);
	struct state*	RcvLvOper_cmplt(EVTBLK *evtblk);
	struct state*	RcvUlMemb_cmplt(EVTBLK *evtblk);
	int				PlyUlWord_init(EVTBLK *evtblk);
	struct state*	PlyUlWord_cmplt(EVTBLK *evtblk);
	struct state*	PlyUlWord1_cmplt(EVTBLK *evtblk);
	struct state*	PlyUlWord2_cmplt(EVTBLK *evtblk);
	
	int				RcvUlOper_init(EVTBLK *evtblk);
	struct state*	RcvUlOper_cmplt(EVTBLK *evtblk);
	int				RcvUlWord_init(EVTBLK *evtblk);
	struct state*	RcvUlWord_cmplt(EVTBLK *evtblk);
	struct state*	PutUlWord_cmplt(struct state* lpstDefaultState);
	struct state*	DelUlWord_cmplt(struct state* lpstDefaultState);
	struct state*	xPlyUlWord_cmplt(EVTBLK *evtblk);
};

#endif // !defined(AFX_CHANNELFRIEND_H__8884D113_13E0_4974_9C09_875E8B1CE676__INCLUDED_)
