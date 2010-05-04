//
// ChannelProfit.h
//

#if !defined(AFX_CHANNELPROFIT_H__D36475EA_296B_4079_B18D_96C2171656F0__INCLUDED_)
#define AFX_CHANNELPROFIT_H__D36475EA_296B_4079_B18D_96C2171656F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"

//
// definitions of file name
//
#define	FILE_PROT_WELCOME		"profit.welcome.pcm"
#define	FILE_PROT_RCVOPER		"profit.rcvoper.pcm"
#define	FILE_PROT_PLYULWORD		"profit.plyulword.pcm"

class CProfitChannel : public CChannel  
{
// Construction
public:
	CProfitChannel(int nChan, LPCTSTR xFeeCode = "1888");
	CProfitChannel(int nChan, LPCTSTR xFeeCode, BYTE nReserved);
	virtual ~CProfitChannel();

// Attributes
protected:
	LPVOID			BufferPtr;
	DWORD			BufferSize;
	_RecordsetPtr	m_pRecordset;
	_RecordsetPtr	m_pRecordbst;
protected:
	struct state	FwdPlay;
	struct state	RcvFnOper;
	struct state	PlyUlWord;
	
// Operations
protected:
	virtual void	Setup();
	virtual LPVOID	NewBuffer(DWORD nBufSize);
	virtual void	SetBuffer(LPVOID pBuf, DWORD nBufSize);
	virtual void	FreeBuffer();
	virtual void	KillLinker();
	int				PlayBlob(int nChan, FieldPtr xPlyBlob, BOOL bTermDtmf = TRUE);
	int				PlayNavFile(LPCTSTR xRemoteId, LPCTSTR xNavFile, BOOL bTermDtmf = TRUE);
protected:
	struct state*	FwdCaller(LPCTSTR xLocalId);
	struct state*	ShRcvFnOper_cmplt(EVTBLK *evtblk, CHAR* pcDtmfKey);
	struct state*	RedexRecordset(struct state* lpszDefaultState);
	struct state*	ShExitCode_cmplt(EVTBLK *evtblk, int nExitCode = EXIT_GOODBYE);
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
	struct state*	RcvFnOperX_cmplt(EVTBLK *evtblk);
	
	int				PlyUlWord_init(EVTBLK *evtblk);
	struct state*	PlyUlWord_cmplt(EVTBLK *evtblk);
	int				PlyUlWordX_init(EVTBLK *evtblk);
	struct state*	PlyUlWordX_cmplt(EVTBLK *evtblk);
	
//	int				RcvLvWord_init(EVTBLK *evtblk);
	struct state*	RcvLvWord_cmplt(EVTBLK *evtblk);
	int				PlyLvWord_init(EVTBLK *evtblk);
};

#endif // !defined(AFX_CHANNELPROFIT_H__D36475EA_296B_4079_B18D_96C2171656F0__INCLUDED_)
