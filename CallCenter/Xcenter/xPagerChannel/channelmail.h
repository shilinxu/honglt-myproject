//
// ChannelMail.h
//

#if !defined(AFX_CHANNELMAIL_H__C368B333_AA85_455F_8127_0D36525990FB__INCLUDED_)
#define AFX_CHANNELMAIL_H__C368B333_AA85_455F_8127_0D36525990FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"

//
// definitions of file name
//

#define	FILE_MAIL_WELCOME		"mail.welcome.pcm"
#define	FILE_MAIL_RCVOPER		"mail.rcvoper.pcm"
#define	FILE_MAIL_WTFXTEL		"mail.wtfxtel.pcm"
#define	FILE_MAIL_RCVFAX		"mail.rcvfax.pcm"
#define	FILE_MAIL_WTUSRTEL		"mail.wtusrtel.pcm"
#define	FILE_MAIL_WTUSRPAS		"mail.wtusrpas.pcm"
#define	FILE_MAIL_GETFAX		"mail.getfax.pcm"
//#define FILE_MAIL_PLYUFAX		"mail.plyufax.pcm"

class CNetworkWnd;

class CMailChannel : public CChannel  
{
// Construction
public:
	CMailChannel(int nChan, LPCTSTR xFeeCode = "1010");
	virtual ~CMailChannel();

// Attributes
protected:
	CHAR*			FaxNumber;
	CHAR*			FaxWord;
	CString			UsrLvFax;
	CNetworkWnd*	m_pHomeWnd;
	_RecordsetPtr	m_pRecordset;
protected:
	struct state	FwdPlay;
	struct state	RcvFnOper;
	
	struct state	RcvFxTele;
	struct state	RcvFxByte;
	struct state	GetFxTele;
	struct state	GetFxPass;
	struct state	GetFxByte;
	
// Operations
protected:
	virtual void	Setup();
	struct state*	FwdCaller(LPCTSTR xLocalId);
	int				ShExitCode_init(EVTBLK *evtblk, int nExitCode = EXIT_GOODBYE);
protected:
	int				GetFxTeleCount();
	BOOL			OpenUsrRecord(LPCTSTR xPhone);
	BOOL			RepackFile(LPCTSTR xFaxFile);
	CString			MakeUsrFaxFile(LPTSTR sExtFile = "tif");
	BOOL			PutUsrRecord(LPCTSTR xFaxFile);
	BOOL			ChkUserState(LPCTSTR xUsrNumber, LPCTSTR xUsrWord = NULL);
	BOOL			PutUsrEmail(LPCTSTR xUsrNumber, LPCTSTR xCallerId, LPCTSTR xFaxFile);
protected:
	int				bkIdle_init(EVTBLK *evtblk);
	int				FeeLogin_init(EVTBLK *evtblk);
	int				FeeRebind_init(EVTBLK *evtblk);
	struct state*	FeeLogin_cmplt(EVTBLK *evtblk);
	int				RcvTalk_init(EVTBLK *evtblk);

	int				FwdPlay_init(EVTBLK *evtblk);
	struct state*	FwdPlay_cmplt(EVTBLK *evtblk);
	int				RcvFnOper_init(EVTBLK *evtblk);
	struct state*	RcvFnOper_cmplt(EVTBLK *evtblk);

	int				RcvFxTele_init(EVTBLK *evtblk);
	struct state*	RcvFxTele_cmplt(EVTBLK *evtblk);
	int				RcvFxByte_init(EVTBLK *evtblk);
	struct state*	RcvFxByte_cmplt(EVTBLK *evtblk);

	int				GetFxTele_init(EVTBLK *evtblk);
	struct state*	GetFxTele_cmplt(EVTBLK *evtblk);
	int				GetFxPass_init(EVTBLK *evtblk);
	struct state*	GetFxPass_cmplt(EVTBLK *evtblk);
	int				GetFxByte_init(EVTBLK *evtblk);
	struct state*	GetFxByte_cmplt(EVTBLK *evtblk);
};

#endif // !defined(AFX_CHANNELMAIL_H__C368B333_AA85_455F_8127_0D36525990FB__INCLUDED_)
