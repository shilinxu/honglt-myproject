//
// ChannelCaller.h
//

#if !defined(AFX_CHANNELCALLER_H__FC4B9EC0_9506_4ECE_8618_C45B2D3FD427__INCLUDED_)
#define AFX_CHANNELCALLER_H__FC4B9EC0_9506_4ECE_8618_C45B2D3FD427__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"

//
// definitions of file name
//

#define	FILE_CALLER_WELCOME		"caller.welcome.pcm"
#define	FILE_CALLER_FINDUSR		"caller.findusr.pcm"

class CUserWnd;

class CCallerChannel : public CChannel  
{
// Construction
public:
	CCallerChannel(int nChan, LPCTSTR xFeeCode = "0");
	virtual ~CCallerChannel();

// Attributes
protected:
	CUserWnd*		m_pUserWnd;
	void	(CCallerChannel::*m_fnKiller)();
protected:
	struct state	FwdCaller;
	struct state	FindUser;

// Operations
public:
	BOOL			IsNotUserTime();
protected:
	int				ReadUserId(LPCTSTR xPhone, CString& xUserId);
	BOOL			ReadOperInfo(LPCTSTR xPhone, CString& xOperId, CString& xUserId);
	CChannel*		ReadOperUser(LPCTSTR xLocalId, LPCTSTR xOper);
	CChannel*		ReadOperUser(LPCTSTR xLocalId);
	struct state*	CallRemoteUser(LPCTSTR xRemoteId, LPCTSTR xCallerId, struct state* lpszDefaultState);
	BOOL			PutUserRing(LPCTSTR xLocalId, LPCTSTR xRemoteId, int nChan);
	BOOL			PutNetworkRing(LPCTSTR xLocalId, LPCTSTR xRemoteId, int nChan);
	BOOL			PutForwardRing(LPCTSTR xLocalId, LPCTSTR xRemoteId, int nChan);
	BOOL			PutForwardRing(CXMLElement* pXML, CChannel*& pChannel);
protected:
	virtual void	Setup();
	virtual void	KillLinker();
	virtual void	SetLinker(int nChan);
	virtual void	KillNetwork();
	struct state*	FwdNotify(LPCTSTR xLocalId, struct state* lpszDefaultState);
protected:
	int				bkIdle_init(EVTBLK *evtblk);
	int				FeeLogin_init(EVTBLK *evtblk);
	struct state*	FeeLogin_cmplt(EVTBLK *evtblk);
	int				rcvMFsig_init(EVTBLK *evtblk);
	struct state*	WaitDial_cmplt(EVTBLK *evtblk);
	int				RcvTalk_init(EVTBLK *evtblk);
	struct state*	RcvTalk_cmplt(EVTBLK *evtblk);
	
	int				FwdCaller_init(EVTBLK *evtblk);
	struct state*	FwdCaller_cmplt(EVTBLK *evtblk);
	int				FindUser_init(EVTBLK *evtblk);
	struct state*	FindUser_cmplt(EVTBLK *evtblk);
	
	struct state*	FindVip_cmplt(EVTBLK *evtblk);
};

#endif // !defined(AFX_CHANNELCALLER_H__FC4B9EC0_9506_4ECE_8618_C45B2D3FD427__INCLUDED_)
