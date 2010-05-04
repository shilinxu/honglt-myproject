//
// ChannelXeno.h
//

#if !defined(AFX_CHANNELXENO_H__EB7FA8A5_D5AF_43C2_BFF4_751F0CFF973D__INCLUDED_)
#define AFX_CHANNELXENO_H__EB7FA8A5_D5AF_43C2_BFF4_751F0CFF973D__INCLUDED_

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
#define	FILE_XENO_VFXNTEL		"xeno.vfxntel.pcm"
#define	FILE_XENO_RCVMONY		"xeno.rcvmony.pcm"
#define	FILE_XENO_WTUSRCM		"xeno.wtusrcm.pcm"

class CXenoChannel : public CChannel  
{
// Construction
public:
	CXenoChannel(int nChan, LPCTSTR xFeeCode = "1234");
	virtual ~CXenoChannel();

// Attributes
protected:
	CHAR			XenoNumber[MAX_BUFFER+1];	// buffer for caller telnum
	CHAR			XenoCard[MAX_CARD_LEN+1];	// buffer for card number
	CHAR			XenoWord[MAX_PASSWORD+1];	// buffer for card password
	BYTE			XenoError;					// CARD err or expired
protected:
	_RecordsetPtr	m_pRecordusr;
	_RecordsetPtr	m_pRecordcar;
protected:
	struct state	FwdPlay;
	struct state	RcvFnOper;
	struct state	RcvXnTele;
	struct state	RcvXnCard;
	struct state	RcvXnWord;
	struct state	ReputCard;
	struct state	PlayXnOk;
	struct state	ReadBalan;
	struct state	VfXnTele;
	struct state	RcvXnMony;
	
	struct state	XnTeleNext;
	struct state	XnTelePrev;
	
	struct state	RcvUsrList;
	struct state	RcvUsrPass;
	
// Operations
public:
	static
	CChannel*		ConvertFrom(CChannel* pChannel, LPCTSTR xString = NULL);
protected:
	void			Setup();
	double			CalcUserBlance(BOOL bPeekPre = FALSE);
	BOOL			ChkUserState(LPCTSTR xUsrNumber, BOOL bAutoAdd = TRUE);
	int				ShPlyXnTele(int nChan, LPCTSTR xNavFile);
	int				ShPlyXnMoney(int nChan, double nBalance);
	int				ShPlyXnBala(int nChan, double nBalance, LPCTSTR xNavFile, DWORD nOffset1, DWORD nOffset2);
	BOOL			PutUsrRecord(_bstr_t xjsjbh, LPCTSTR xUsrList);
	int				ShExitPlay_init(EVTBLK *evtblk, int nExitCode);
protected:
	struct state*	FwdCaller(LPCTSTR xLocalId, struct state* lpszDefaultState);
	struct state*	XenoUserMoney(struct state* lpszDefaultState);
	struct state*	MoveUserMoney(double nBalance, LPCTSTR xUsrNumber, struct state* lpszDefaultState);
	struct state*	XenoUserPass(LPCTSTR xNewPass, struct state* lpszDefaultState);
protected:
	int				bkIdle_init(EVTBLK *evtblk);
	int				FeeLogin_init(EVTBLK *evtblk);
	struct state*	FeeLogin_cmplt(EVTBLK *evtblk);
	int				rcvMFsig_init(EVTBLK *evtblk);
	int				RcvTalk_init(EVTBLK *evtblk);
	
	int				FwdPlay_init(EVTBLK *evtblk);
	struct state*	FwdPlay_cmplt(EVTBLK *evtblk);
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
	int				ReadBalan_init(EVTBLK *evtblk);
	struct state*	ReadBalan_cmplt(EVTBLK *evtblk);
	
	int				RcvXnMony_init(EVTBLK *evtblk);
	struct state*	RcvXnMony_cmplt(EVTBLK *evtblk);
	
	int				VfXnTele_init(EVTBLK *evtblk);
	struct state*	VfXnTele_cmplt(EVTBLK *evtblk);
	
	int				RcvUsrList_init(EVTBLK *evtblk);
	struct state*	RcvUsrList_cmplt(EVTBLK *evtblk);
	
	int				RcvUsrPass_init(EVTBLK *evtblk);
	struct state*	RcvUsrPass_cmplt(EVTBLK *evtblk);
};

#endif // !defined(AFX_CHANNELXENO_H__EB7FA8A5_D5AF_43C2_BFF4_751F0CFF973D__INCLUDED_)
