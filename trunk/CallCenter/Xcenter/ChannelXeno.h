//
// ChannelXeno.h
//

#if !defined(AFX_CHANNELXENO_H__B75B232F_FFF4_4A32_A44B_6C62FDA3944B__INCLUDED_)
#define AFX_CHANNELXENO_H__B75B232F_FFF4_4A32_A44B_6C62FDA3944B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"

//
// Configuration
//

#define MAX_CARD_LEN		21
#define MAX_PASSWORD		10

//
// XenoError
//

#define XENO_SUCCESS		0
#define XENO_INVALID		1	// id or pass error
#define XENO_EXPIRED		2	// card expired
#define XENO_REJECT			3	// error on reput old card

//
// definitions of file name
//

#define	FILE_XENO_WELCOME		"xeno.welcome.pcm"
#define	FILE_XENO_RCVOPER		"xeno.rcvoper.pcm"
#define	FILE_XENO_WTXNTEL		"xeno.wtxntel.pcm"
#define	FILE_XENO_RCVCARD		"xeno.rcvcard.pcm"
#define	FILE_XENO_RCVWORD		"xeno.rcvword.pcm"
#define	FILE_XENO_NOCARD		"xeno.nocard.pcm"
#define	FILE_XENO_NOTCARD		"xeno.notcard.pcm"
#define	FILE_XENO_XENOOK		"xeno.xenook.pcm"
#define	FILE_XENO_AQUERY1		"xeno.aquery1.pcm"
#define	FILE_XENO_AQUERY2		"xeno.aquery2.pcm"
#define	FILE_XENO_NOPAY			"xeno.nopay.pcm"

class CXenoChannel : public CChannel  
{
// Construction
public:
	CXenoChannel(int nChan, LPCTSTR xFeeCode = "1234");
	virtual ~CXenoChannel();

// Attributes
protected:
	CHAR			XenoNumber[MAX_PHONE+1];	// buffer for caller telnum
	CHAR			XenoCard[MAX_CARD_LEN+1];	// buffer for card number
	CHAR			XenoWord[MAX_PASSWORD+1];	// buffer for card password
	BYTE			XenoError;					// CARD err or expired
protected:
	_RecordsetPtr	m_pRecordusr;
	_RecordsetPtr	m_pRecordcar;
	BOOL			m_bFwding;
protected:
	struct state	FwdXenoer;
	struct state	RcvFnOper;
	struct state	RcvXnTele;
	struct state	RcvXnCard;
	struct state	RcvXnWord;
	struct state	ReputCard;
	struct state	PlayXnOk;
	struct state	EnqMoney;

// Operations
protected:
	virtual void	Setup();
	struct state*	XenoUserMoney();
	struct state*	FwdCaller();
	double			CalcUserBlance();
	BOOL			ChkUserState(LPCTSTR xUsrNumber, BOOL bAutoAdd = TRUE);
protected:
	int				bkIdle_init(EVTBLK *evtblk);
	int				rcvMFsig_init(EVTBLK *evtblk);
	int				RcvTalk_init(EVTBLK *evtblk);
	struct state*	FeeLogin_cmplt(EVTBLK *evtblk);

	int				FwdXenoer_init(EVTBLK *evtblk);
	struct state*	FwdXenoer_cmplt(EVTBLK *evtblk);
	int				RcvFnOper_init(EVTBLK *evtblk);
	struct state*	RcvFnOper_cmplt(EVTBLK *evtblk);
	int				RcvXnTele_init(EVTBLK *evtblk);
	struct state*	RcvXnTele_cmplt(EVTBLK *evtblk);
	int				RcvXnCard_init(EVTBLK *evtblk);
	struct state*	RcvXnCard_cmplt(EVTBLK *evtblk);
	int				RcvXnWord_init(EVTBLK *evtblk);
	struct state*	RcvXnWord_cmplt(EVTBLK *evtblk);
	
	int				ReputCard_init(EVTBLK *evtblk);
	struct state*	ReputCard_cmplt(EVTBLK *evtblk);
	int				PlayXnOk_init(EVTBLK *evtblk);
	struct state*	PlayXnOk_cmplt(EVTBLK *evtblk);
	int				EnqMoney_init(EVTBLK *evtblk);
	struct state*	EnqMoney_cmplt(EVTBLK *evtblk);
};

#endif // !defined(AFX_CHANNELXENO_H__B75B232F_FFF4_4A32_A44B_6C62FDA3944B__INCLUDED_)
