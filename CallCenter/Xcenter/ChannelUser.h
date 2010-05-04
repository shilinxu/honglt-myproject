//
// ChannelUser.h
//

#if !defined(AFX_CHANNELUSER_H__48D1584A_E9AB_4720_A870_E55D40FC7FA9__INCLUDED_)
#define AFX_CHANNELUSER_H__48D1584A_E9AB_4720_A870_E55D40FC7FA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"

class CUserChannel : public CChannel  
{
// Construction
public:
	CUserChannel(int nChan);
	virtual ~CUserChannel();

// Attributes
protected:
	struct state	DebugOut;
	
// Operations
public:
	inline LPTSTR&	GetUserId();
protected:
	BOOL			IsRecordChan(int nChan) const;
	LPTSTR			GetRingNumber(LPTSTR xRemoteId, LPTSTR xLocalId = NULL) const;
	struct state*	ShRcvFdTele_cmplt(EVTBLK *evtblk, struct state* lpstEntryState);
	int				ShExitCode_init(EVTBLK *evtblk, int nExitCode);
	int				ShDebugOut_init(EVTBLK *evtblk);
protected:
	virtual void	Setup();
	virtual void	StopChan();
	virtual void	SetLinker(int nChan);
	virtual void	AttachFrom(CChannel* pChannel);
	struct state*	ChkDTSignal(EVTBLK *evtblk,struct state* lpstDefaultState);
protected:
	virtual BOOL	OnRun();
	virtual BOOL	OnPacket(CPacket* pPacket);
	virtual BOOL	OnBindPacket(CBindPacket* pPacket);
	virtual BOOL	OnDialPacket(CDialPacket* pPacket);
	virtual BOOL	OnRecordPacket(CRecordPacket* pPacket);
	virtual BOOL	OnListenPacket(CListenPacket* pPacket);
	virtual BOOL	OnHaltPacket(CHaltPacket* pPacket);
protected:
	int				bkIdle_init(EVTBLK* evtblk);
	struct state*	bkIdle_cmplt(EVTBLK* evtblk);
	int				bkOffHk_init(EVTBLK *evtblk);
	int				bkOnHk_init(EVTBLK *evtblk);
	struct state*	bkOnHk_cmplt(EVTBLK *evtblk);
	int				rcvMFsig_init(EVTBLK* evtblk);
	struct state*	rcvMFsig_cmplt(EVTBLK *evtblk);
	int				WaitDial_init(EVTBLK *evtblk);
	int				WaitDialX_init(EVTBLK *evtblk);
	struct state*	WaitDial_cmplt(EVTBLK *evtblk);
	int				RcvTalk_init(EVTBLK *evtblk);
	struct state*	RcvTalk_cmplt(EVTBLK *evtblk);
};

LPTSTR& CUserChannel::GetUserId()
{
	return CChannel::m_xFeeCode;
}

#endif // !defined(AFX_CHANNELUSER_H__48D1584A_E9AB_4720_A870_E55D40FC7FA9__INCLUDED_)
