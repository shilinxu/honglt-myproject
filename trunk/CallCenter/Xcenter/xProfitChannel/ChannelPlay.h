//
// ChannelPlay.h
//

#if !defined(AFX_CHANNELPLAY_H__C2F789D8_1C7F_424C_B71F_D86DE21C03B9__INCLUDED_)
#define AFX_CHANNELPLAY_H__C2F789D8_1C7F_424C_B71F_D86DE21C03B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"

//
// definitions of file name
//
#define	FILE_PROT_WELCOME		"profit.welcome.pcm"
//#define	FILE_PROT_RCVOPER		"profit.rcvoper.pcm"
//#define	FILE_PROT_NORECD		"profit.norecd.pcm"
#define	FILE_PROT_NOTUSER		"profit.notuser.pcm"
#define	FILE_PROT_OKANSWER		"profit.okanswer.pcm"
#define	FILE_TALK_RCVTELE		"talk.rcvtele.pcm"
#define	FILE_PROT_CALLBACK		"profit.callback.pcm"
#define	FILE_PROT_NOLONG		"profit.nolong.pcm"

//
// definitions of exit code
//

#define EXIT_NOLONG			10

class CPlayChannel : public CChannel  
{
// Construction
public:
	CPlayChannel(int nChan, LPCTSTR xFeeCode = "1999");
	virtual ~CPlayChannel();

// Attributes
protected:
	_RecordsetPtr	m_pRecordset;
	LPVOID			BufferPtr;
	DWORD			BufferSize;
protected:
	struct state	FwdPlay;
	struct state	RcvFnOper;
	struct state	PlyUlWord;
	struct state	OkAnswer;
	struct state	RcvLvTel;
	
// Operations
protected:
	void			Setup();
	virtual void	KillLinker();
	virtual LPVOID	NewBuffer(DWORD nBufSize);
	virtual void	SetBuffer(LPVOID pBuf, DWORD nBufSize);
	virtual void	FreeBuffer();
protected:
	BOOL			ReadProfit();
	int				PlayBlob(int nChan, FieldPtr xPlyBlob, BOOL bTermDtmf = TRUE);
	struct state*	ShExitCode_cmplt(EVTBLK *evtblk, int nExitCode = EXIT_GOODBYE);
protected:
	CXMLElement*	CheckBlacklist(CXMLElement* pParent, LPCTSTR xRemoteId);
	BOOL			CheckBlacklist(LPCTSTR xRemoteId);
	BOOL			SaveXML(LPCTSTR lpszFile, CXMLElement* pXML);
private:
	BOOL			InTimeProfit(LPCTSTR lpszHour) const;
	BOOL			InWeekProfit(LPCTSTR lpszWeek) const;
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
	
	int				PlyUlWord_init(EVTBLK *evtblk);
	struct state*	PlyUlWord_cmplt(EVTBLK *evtblk);
	int				OkAnswer_init(EVTBLK *evtblk);
	struct state*	OkAnswer_cmplt(EVTBLK *evtblk);
	
	int				RcvLvTel_init(EVTBLK *evtblk);
	struct state*	RcvLvTel_cmplt(EVTBLK *evtblk);
	int				RcvUlWord_init(EVTBLK *evtblk);
	struct state*	RcvUlWord_cmplt(EVTBLK *evtblk);
	struct state*	RcvUlTalk_cmplt(EVTBLK *evtblk);

	int				ExitPlay_init(EVTBLK *evtblk);
//	struct state*	ExitPlay_cmplt(EVTBLK *evtblk);
};

#endif // !defined(AFX_CHANNELPLAY_H__C2F789D8_1C7F_424C_B71F_D86DE21C03B9__INCLUDED_)
