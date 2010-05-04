//
// ChannelPager.h
//

#if !defined(AFX_CHANNELPAGER_H__8650DF6F_7611_4671_A5D5_E3A4636E905C__INCLUDED_)
#define AFX_CHANNELPAGER_H__8650DF6F_7611_4671_A5D5_E3A4636E905C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"

//
// definitions of file name
//

#define	FILE_MEET_PLYUTAK		"meet.plyutak.pcm"
#define	FILE_MEET_MEMBSIT		"meet.membsit.pcm"
#define	FILE_MEET_MEMBEXT		"meet.membext.pcm"

class CPagerChannel : public CChannel
{
// Construction
protected:
	CPagerChannel(int nChan, WORD nRoomId);
	virtual ~CPagerChannel();

// Attributes
public:
	const int		RoomNumber;
protected:
	struct state	MsgMember;
	struct state	JoinRoom;

// Operations
public:
	static
	CChannel*		PagerFrom(CChannel* pParent, LPCTSTR xRemoteId, LPCTSTR xFeeNumber, DWORD nRoomId);
protected:
	virtual void	Setup();
	virtual BOOL	OnBindPacket(CBindPacket* pPacket);
protected:
//	int				PlayMemberJoined(int nMmbrId);
//	int				PlayMemberExited(int nMmbrId);
//	int				PlayMemberSpeaker(int nRoomId, int nChan);
protected:
	int				bkIdle_init(EVTBLK *evtblk);
	int				bkOffHk_init(EVTBLK *evtblk);
	int				FeeLogin_init(EVTBLK *evtblk);
	struct state*	FeeLogin_cmplt(EVTBLK *evtblk);
	
	int				MsgMember_init(EVTBLK *evtblk);
	struct state*	MsgMember_cmplt(EVTBLK *evtblk);
	int				JoinRoom_init(EVTBLK *evtblk);
	struct state*	JoinRoom_cmplt(EVTBLK *evtblk);
};

#endif // !defined(AFX_CHANNELPAGER_H__8650DF6F_7611_4671_A5D5_E3A4636E905C__INCLUDED_)
