//
// ChannelCorpDx.h
//

#if !defined(AFX_CHANNELCORPDX_H__674E573B_9B04_42E4_85A6_36D90D35E1A1__INCLUDED_)
#define AFX_CHANNELCORPDX_H__674E573B_9B04_42E4_85A6_36D90D35E1A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"

//
// definitions of file name
//

#define	FILE_PAGE_PLYWORD			"pager.plyword.pcm"
#define	FILE_PAGE_GOODBYE			"pager.goodbye.pcm"

class CCorpChannelDx : public CChannel  
{
// Construction
public:
	CCorpChannelDx(int nChan, LPCTSTR xFeeCode = "9500");
	virtual ~CCorpChannelDx();

// Attributes
protected:
	BOOL			m_bPending;
protected:
	struct state	PlyLvWord;

// Operations
protected:
	virtual void	Setup();
	virtual void	KillLinker();
	virtual void	OnRecdPacket(IRecdPacket* pPacket);
protected:
	void			UpdateState(LPCTSTR xState);
	void CCorpChannelDx::RecUsrBack(CHAR xKey);
protected:
	int				bkIdle_init(EVTBLK *evtblk);
	int				WaitDial_init(EVTBLK *evtblk);
	struct state*	WaitDial_cmplt(EVTBLK *evtblk);
	int				RcvTalk_init(EVTBLK *evtblk);
	struct state*	FeeLogin_cmplt(EVTBLK *evtblk);
	int				ExitPlay_init(EVTBLK *evtblk);
	
	int				PlyLvWord_init(EVTBLK *evtblk);
	struct state*	PlyLvWord_cmplt(EVTBLK *evtblk);
};

#endif // !defined(AFX_CHANNELCORPDX_H__674E573B_9B04_42E4_85A6_36D90D35E1A1__INCLUDED_)
