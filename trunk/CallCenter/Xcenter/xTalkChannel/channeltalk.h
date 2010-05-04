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
	CTalkChannel(int nChan, LPCTSTR xFeeCode);
	CTalkChannel(int nChan, LPCTSTR xFeeCode, BYTE nReserved);
	virtual ~CTalkChannel();

// Attributes
protected:
	CHAR*			PageNumber;
	CHAR*			UserNumber;
protected:
	struct state	FwdPlay;
	struct state	RcvLvTel;
	struct state	RcvUlTel;
	
// Operations
public:
	static
	CChannel*		ConvertFrom(CChannel* pChannel, LPCTSTR xString = NULL);
protected:
	void			Setup();
	virtual void	KillLinker();
	BOOL			RefuseNotLocalMobileForPager(LPCTSTR xRemoted, LPCTSTR xPageNumber);
	BOOL			ReadUserShortMobile(LPCTSTR xRemoted, LPTSTR xPageNumber);
	BOOL			JoinUserMobile(LPCTSTR xUserId, LPTSTR xPageNumber);
protected:
	virtual BOOL	OnPacket(CPacket* pPacket);
	virtual BOOL	OnRedPacket(CRedPacket* pPacket);
	virtual BOOL	OnRecordPacket(CString* pxRecFile);
	BOOL			PutUsrRecord(LPCTSTR xUsrFile);
	BOOL			PutUsrRecord(LPCTSTR xUsrFile, DWORD nReserved);
protected:
	struct state*	ShExitCode_cmplt(EVTBLK *evtblk, int nExitCode = EXIT_GOODBYE);
	BOOL			IsAdministrator(LPCTSTR xRemoteId) const;
protected:
	int				bkIdle_init(EVTBLK *evtblk);
	struct state*	FeeLogin_cmplt(EVTBLK *evtblk);
	int				rcvMFsig_init(EVTBLK *evtblk);
	int				RcvTalk_init(EVTBLK *evtblk);
	
	int				FwdPlay_init(EVTBLK *evtblk);
	struct state*	FwdPlay_cmplt(EVTBLK *evtblk);
	int				RcvLvTel_init(EVTBLK *evtblk);
	struct state*	RcvLvTel_cmplt(EVTBLK *evtblk);
	int				RcvUlTel_init(EVTBLK *evtblk);
	struct state*	RcvUlTel_cmplt(EVTBLK *evtblk);
	
	int				RcvUlWord_init(EVTBLK *evtblk);
	struct state*	RcvUlWord_cmplt(EVTBLK *evtblk);
	struct state*	RcvUlTalk_cmplt(EVTBLK *evtblk);
};

#endif // !defined(AFX_CHANNELTALK_H__6F3D5FE7_E67D_4549_939B_587E1E43FCED__INCLUDED_)
