//
// ChannelMeet.h
//

#if !defined(AFX_CHANNELMEET_H__C368B333_AA85_455F_8127_0D36525990FB__INCLUDED_)
#define AFX_CHANNELMEET_H__C368B333_AA85_455F_8127_0D36525990FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"

//
// definitions of file name
//

#define	FILE_MEET_WELCOME		"meet.welcome.pcm"
#define	FILE_MEET_WTMTTELE		"meet.wtmttel.pcm"
#define	FILE_MEET_PLYCALL		"meet.plycall.pcm"
#define	FILE_MEET_PLYUTAK		"meet.plyutak.pcm"
#define	FILE_MEET_MEMBSIT		"meet.membsit.pcm"
#define	FILE_MEET_MEMBEXT		"meet.membext.pcm"

#define	FILE_TALK_REDPAY1		"talk.redpay1.pcm"
#define	FILE_TALK_REDPAY2		"talk.redpay2.pcm"
#define	FILE_TALK_REDPAY3		"talk.redpay3.pcm"

class CMeetChannel : public CChannel  
{
// Construction
public:
	CMeetChannel(int nChan, LPCTSTR xFeeCode);
	CMeetChannel(CChannel* pParent);
	virtual ~CMeetChannel();

// Attributes
protected:
	CHAR*			MeetNumber;
	DWORD			RoomNumber;
protected:
	struct state	FwdPlay;
	struct state	RcvMtTele;
	struct state	RegMember;
	struct state	MsgMember;
	struct state	JoinRoom;
	
// Operations
public:
	static
	CChannel*		ConvertFrom(CChannel* pChannel, LPCTSTR xWalkType);
	BOOL			FormatObject(LPCTSTR xWalkType);
	static	int		GetMemberIndex(int nRoomId, int nIndex, int* pnChan);
protected:
	virtual void	Setup();
	BOOL			GetFeeNumber(int nRoomId, LPSTR xFeeNumber);
	static	int		PlayMemberJoined(CChannel* pChannel, int nMmbrId);
	static	int		PlayMemberExited(CChannel* pChannel, int nMmbrId);
	static	int		PlayMemberSpeaker(int nRoomId, int nChan);
	static	int		PrefixIPSpeaker(LPCTSTR lpszHost, int nChan);
	friend	class	CPagerChannel;
protected:
	virtual BOOL	OnPacket(CPacket* pPacket);
	virtual BOOL	OnRedPacket(CRedPacket* pPacket);
	virtual BOOL	OnRecordPacket(CRecordPacket* pPacket);
	virtual BOOL	OnBindPacket(CBindPacket* pPacket);
protected:
	int				CreateRoom(LPCTSTR xPhone);
	BOOL			ClearRoom(int nRoomId);
	BOOL			PutIntoRoom(int nRoomId, LPCTSTR xRoomOwner, LPCTSTR xGroup);
	BOOL			PutIntoRoom(int nRoomId, LPCTSTR xPhone, BYTE nFlag, BYTE nState);
	BOOL			PutIntoHistory(LPCTSTR xFeeNumber, LPCTSTR xRecFile);
	void			SuspendMeet(int nRoomId);
	void			ResumeMeet(int nRoomId);
	BOOL			ReadUserShortMobile(LPCTSTR xRemoted, LPTSTR xPageNumber);
protected:
protected:
	int				bkIdle_init(EVTBLK *evtblk);
	int				bkOffHk_init(EVTBLK *evtblk);
	int				FeeLogin_init(EVTBLK *evtblk);
	int				FeeRebind_init(EVTBLK *evtblk);
	struct state*	FeeLogin_cmplt(EVTBLK *evtblk);
	int				rcvMFsig_init(EVTBLK *evtblk);
	int				RcvTalk_init(EVTBLK *evtblk);
	int				RcvTalkWaitJoin_init(EVTBLK *evtblk);

	int				FwdPlay_init(EVTBLK *evtblk);
	struct state*	FwdPlay_cmplt(EVTBLK *evtblk);
	
	int				RcvMtTele_init(EVTBLK *evtblk);
	struct state*	RcvMtTele_cmplt(EVTBLK *evtblk);
	int				RegMember_init(EVTBLK *evtblk);
	struct state*	RegMember_cmplt(EVTBLK *evtblk);
	
	int				MsgMember_init(EVTBLK *evtblk);
	struct state*	MsgMember_cmplt(EVTBLK *evtblk);
	int				JoinRoom_init(EVTBLK *evtblk);
	struct state*	JoinRoom_cmplt(EVTBLK *evtblk);
};

enum
{
	// 对话
	S_TAK_Speaker		= 1003,		// 移交话筒
};

#endif // !defined(AFX_CHANNELMEET_H__C368B333_AA85_455F_8127_0D36525990FB__INCLUDED_)
