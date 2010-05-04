//
// ChannelPager1.h
//

#if !defined(AFX_CHANNELPAGER1_H__771B289D_4128_429E_BC16_025C8CB738D3__INCLUDED_)
#define AFX_CHANNELPAGER1_H__771B289D_4128_429E_BC16_025C8CB738D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"

//
// definitions of file name
//

#define	FILE_PAGE_WELCOME		"pager.welcome.pcm"
#define	FILE_PAGE_WTLVTEL		"pager.wtlvtel.pcm"
#define	FILE_PAGE_RCVLVWRD		"pager.rcvlvwrd.pcm"
#define	FILE_PAGE_BJMUSIC		"pager.bjmusic.pcm"

class CPagerChannel1 : public CChannel  
{
// Construction
public:
	CPagerChannel1(int nChan, LPCTSTR xFeeCode = "1021");
	virtual ~CPagerChannel1();

// Attributes
protected:
	CHAR*			PageNumber;
	CString			UsrLvFile;
	CString			BjMusic;
protected:
	struct state	FwdPager;
	struct state	RcvLvTele;
	struct state	RcvLvWord;
	
// Operations
public:
	static CChannel* Create(int nChan, LPCTSTR xFeeCode);
	static CPagerChannel1*	ConvertFrom(CChannel* pChannel, LPCTSTR xString = NULL);
	BOOL			FormatObject(LPCTSTR xString);
protected:
	virtual void	Setup();
	virtual void	Clone(CChannel* pParent);
	BOOL			PutLvRecord(LPCTSTR xUsrFile);
	CString			MakeLvWordFile(LPCTSTR sExtTitle = "pcm");
	BOOL			TryPagerToGroup(int xGroupId, LPCTSTR xUsrFile);
protected:
	int				bkIdle_init(EVTBLK *evtblk);
	int				FeeLogin_init(EVTBLK *evtblk);
	struct state*	FeeLogin_cmplt(EVTBLK *evtblk);
	int				RcvTalk_init(EVTBLK *evtblk);

	int				FwdPager_init(EVTBLK *evtblk);
	struct state*	FwdPager_cmplt(EVTBLK *evtblk);
	int				RcvLvTele_init(EVTBLK *evtblk);
	struct state*	RcvLvTele_cmplt(EVTBLK *evtblk);
	int				RcvLvWord_init(EVTBLK *evtblk);
	struct state*	RcvLvWord_cmplt(EVTBLK *evtblk);
};

#endif // !defined(AFX_CHANNELPAGER1_H__771B289D_4128_429E_BC16_025C8CB738D3__INCLUDED_)
