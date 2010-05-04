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
#define	FILE_MAIL_WIZARD		"mail.wizard.pcm"

#define	FILE_MAIL_WTFXTEL		"mail.wtfxtel.pcm"
#define	FILE_MAIL_RCVFAX		"mail.rcvfax.pcm"
#define	FILE_MAIL_WTUSRTEL		"mail.wtusrtel.pcm"
#define	FILE_MAIL_WTUSRPAS		"mail.wtusrpas.pcm"
#define	FILE_MAIL_GETFAX		"mail.getfax.pcm"
//#define FILE_MAIL_PLYUFAX		"mail.plyufax.pcm"

#define	FILE_MAIL_REPLYOK		"mail.replyok.pcm"
#define	FILE_MAIL_CHGPASOK		"mail.chgpasok.pcm"
#define	FILE_MAIL_ORDEROK		"mail.orderok.pcm"
#define	FILE_MAIL_UNORDEROK		"mail.unordok.pcm"
#define	FILE_MAIL_NOTORDER		"mail.notorder.pcm"

//
// definitions of exit code
//

#define EXIT_BASEID			10
#if !(EXIT_BASEID >= 10)
#error EXIT_BASEID must >= CChannel::EXITCODE[10]
#endif

#define MAIL_REPLYOK		(EXIT_BASEID + 1)
#define MAIL_CHANGEPASSOK	(EXIT_BASEID + 2)
#define MAIL_ORDEROK		(EXIT_BASEID + 3)
#define MAIL_UNORDEROK		(EXIT_BASEID + 4)
#define MAIL_NOTORDER		(EXIT_BASEID + 5)

class CNetworkWnd;

class CMailChannel : public CChannel  
{
// Construction
public:
	CMailChannel(int nChan, LPCTSTR xFeeCode = "MAIL");
	CMailChannel(CChannel* pParent);
	virtual ~CMailChannel();

// Attributes
protected:
	CNetworkWnd*	m_pHomeWnd;
	_RecordsetPtr	m_pRecordset;
	CXMLElement*	m_pXML;
protected:
	struct state	FwdPlay;
	struct state	RcvFnOper;
	struct state	PlyNotify;
	
	struct state	RcvFxTele;
	struct state	RcvFxByte;
	struct state	GetFxTele;
	struct state	GetFxPass;
	struct state	GetFxByte;
	
	struct state	RcvUsrPass;
	struct state	PlyNotOrder;
private:
	CHAR			m_pBuffer[MAX_BUFFER + 1];
	_RecordsetPtr	m_pRecordusr;
private:
	struct keyRcvFnOper
	{
		BYTE		hKey;
		DWORD		Reserved;
		PROCESS		process;
		keyRcvFnOper*	previous;
		
		keyRcvFnOper(keyRcvFnOper* prev, PROCESS cmplt)
		{
			previous = prev; process = cmplt;
			hKey = '\0'; Reserved = 0;
		}
		virtual ~keyRcvFnOper()
		{
			if ( previous ) delete previous;
		}
		BYTE* RenderKey(BYTE* hBuf)
		{
			if ( previous ) hBuf =previous->RenderKey( hBuf++ );
			*hBuf = hKey; return hKey ? ++hBuf : hBuf;
		}
	} *ShRcvFnOper;
	
// Operations
public:
	static
	CChannel*		ConvertFrom(CChannel* pChannel, LPCTSTR xString = NULL);
	BOOL			FormatObject(LPCTSTR xString);
protected:
	virtual void	Setup();
	struct state*	FwdCaller(LPCTSTR xLocalId, struct state* lpszDefaultState);
	int				ShExitCode_init(EVTBLK *evtblk, int nExitCode = EXIT_GOODBYE);
	int				ShRcvTalk_init(EVTBLK *evtblk, struct state* lpszDefaultState);
	CXMLElement*	FindPlay(struct keyRcvFnOper* ShRcvFnOper, CHAR nDtmfKey);
	struct state*	ShRcvFxDtmf_cmplt(EVTBLK *evtblk, struct state* lpstDefaultState, int nMax = MAX_BUFFER);
protected:
	struct state*	ShRcvFnOper_cmplt(EVTBLK *evtblk, struct state* lpstDefaultState);
	struct state*	Next_RcvFnOper(PROCESS cmplt, CHAR nDtmfKey, struct state* lpstDefaultState);
	struct state*	Prev_RcvFnOper(struct keyRcvFnOper*& ShRcvFnOper, struct state* lpstDefaultState);
	struct state*	Next_RcvFnOper(PROCESS cmplt, CHAR nDtmfKey, struct state* lpstDefaultState, DWORD nReserved);
	struct state*	RcvFnOper_cmplt(CXMLElement* pXML, CHAR nDtmfKey);
protected:
	BOOL			OpenUsrRecord(LPCTSTR xPhone, int nTopic = 100);
	BOOL			RepackFile(LPCTSTR xFaxFile, LPCTSTR xFaxNumber);
	CString			MakeUsrFaxFile(LPCTSTR sBuffer, LPTSTR sExtFile = "tif");
	BOOL			PutUsrRecord(LPCTSTR xFaxFile, LPCTSTR xFaxNumber);
	BOOL			ChkUserState(LPCTSTR xUsrNumber, LPCTSTR xUsrWord = NULL);
	BOOL			PutUsrEmail(LPCTSTR xUsrNumber, LPCTSTR xCallerId, LPCTSTR xFaxFile);
	BOOL			ChkUserState(LPCTSTR xUsrWord, DWORD);
protected:
	virtual BOOL	OnBindPacket(CBindPacket* pPacket);
	BOOL			PutUsrRecord(LPCTSTR xFaxFile, LPCTSTR xFaxNumber, LPCTSTR xFeeType, LPCTSTR xFeeNumber);
	BOOL			TryPagerToGroup(int xGroupId, LPCTSTR xUsrFile);
protected:
	int				bkIdle_init(EVTBLK *evtblk);
	int				FeeLogin_init(EVTBLK *evtblk);
	int				FeeRebind_init(EVTBLK *evtblk);
	struct state*	FeeLogin_cmplt(EVTBLK *evtblk);
	int				rcvMFsig_init(EVTBLK *evtblk);
	int				RcvTalk_init(EVTBLK *evtblk);

	int				FwdPlay_init(EVTBLK *evtblk);
	struct state*	FwdPlay_cmplt(EVTBLK *evtblk);
	int				RcvFnOper_init(EVTBLK *evtblk);
	struct state*	RcvFnOper_cmplt(EVTBLK *evtblk);
	
	int				PlyNotify_init(EVTBLK *evtblk);
	struct state*	PlyNotify_cmplt(EVTBLK *evtblk);

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
	
	struct state*	ReplyTele_cmplt(EVTBLK *evtblk);
	struct state*	RcvOrder_cmplt(EVTBLK *evtblk);
	struct state*	RcvUnorder_cmplt(EVTBLK *evtblk);
	
	int				RcvUsrPass_init(EVTBLK *evtblk);
	struct state*	RcvUsrPass_cmplt(EVTBLK *evtblk);
	
	int				ExitPlay_init(EVTBLK *evtblk);
	
	int				PlyNotOrder_init(EVTBLK *evtblk);
	struct state*	PlyNotOrder_cmplt(EVTBLK *evtblk);
};

#endif // !defined(AFX_CHANNELMAIL_H__C368B333_AA85_455F_8127_0D36525990FB__INCLUDED_)
