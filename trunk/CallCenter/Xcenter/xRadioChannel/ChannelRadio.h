//
// ChannelRadio.h
//

#if !defined(AFX_CHANNELRADIO_H__771B289D_4128_429E_BC16_025C8CB738D3__INCLUDED_)
#define AFX_CHANNELRADIO_H__771B289D_4128_429E_BC16_025C8CB738D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"
#include "gdat.h"

//
// definitions of file name
//

#define	FILE_RADO_WELCOME		"%s.welcome.pcm"
#define	FILE_RADO_WIZARD		"%s.wizard.pcm"
#define	FILE_RADO_RCVOPER		"%s.rcvoper.pcm"
#define	FILE_RADO_GOODBYE		"%s.goodbye.pcm"

#define	FILE_RADO_RCVLVWRD		"%s.rcvlvwrd.pcm"

#define	FILE_STOK_READDAP		"stok.readdap.pcm"	// ´óÅÌ
#define	FILE_STOK_READGEG		"stok.readgep.pcm"	// ¸ö¹É

class CRadioChannel : public CChannel  
{
// Construction
public:
	static CChannel* Create(int nChan, LPCTSTR xFeeCode);
	CRadioChannel(int nChan, LPCTSTR xFeeCode = "1333");
	virtual ~CRadioChannel();

// Attributes
protected:
	CStringList		m_pList;
	POSITION		m_pPosNext;
	CXMLElement*	m_pXML;
	_RecordsetPtr	m_pRecordset;
	CString			UsrLvFile;
protected:
	struct state	FwdPlay;
	struct state	RcvFnOper;
	struct state	PlyNotify;
	
	struct state	PlyUlWord;
	struct state	RcvLvWord;
	struct state	PlyLvFile;
	struct state	TryLvFile;
	
	struct state	PlayQReal;
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
protected:
	virtual void	Setup();
	struct state*	RenewPlay(LPCTSTR xLocalId);
	virtual BOOL	OnBindPacket(CBindPacket* pPacket);
	virtual BOOL	OnErrPacket(CErrPacket* pPacket);
	virtual BOOL	OnRecordFile(BOOL bRecMixer, int nVolMixer, CString* pstrFile = NULL);
protected:
	struct state*	ShRcvFnOper_cmplt(EVTBLK *evtblk, struct state* lpstDefaultState);
	struct state*	Next_RcvFnOper(PROCESS cmplt, CHAR nDtmfKey, struct state* lpstDefaultState);
	struct state*	Prev_RcvFnOper(struct keyRcvFnOper*& ShRcvFnOper, struct state* lpstDefaultState);
	struct state*	Next_RcvFnOper(PROCESS cmplt, CHAR nDtmfKey, struct state* lpstDefaultState, DWORD nReserved);
	struct state*	RcvFnOper_cmplt(CXMLElement* pXML, CHAR nDtmfKey);
protected:
	int				ShPlyNavFile(LPCTSTR xNavFile, GKPDAT* kp);
	int				ShPlyNavFile(LPCTSTR xNavFile, GKPDAT* kp, DWORD nReserved);
	int				ShPlyNavFile(int nChan, LPCTSTR xNavFile);
	int				ShPlyLvWord(int nChan, LPCTSTR xLvFile);
	int				ShPlyLvWord(int nChan, LPCTSTR xNavFile, int nCount);
	virtual void	FormatIntoList(LPCTSTR lpzText);
protected:
	int				GetUsrLvCount(BOOL bForce = FALSE);
	BOOL			OpenUsrRecord(LPCTSTR xPhone, int nExpired = 7*24*3600, int nIndex = -1);
	BOOL			PutUsrRecord(LPCTSTR xUsrFile, LPCTSTR xPhone, int nKey = 0);
	CXMLElement*	FindPlay(struct keyRcvFnOper* ShRcvFnOper, CHAR nDtmfKey);
	BOOL			VerifyRoomGroup(LPCTSTR xOwnerId, int nGroupId, LPCTSTR xRemoteId);
	BOOL			CheckBlacklist(LPCTSTR xRemoteId, LPCTSTR xLocalId = "96031111");
	BOOL			WriteBlacklist(LPCTSTR xRemoteId, LPCTSTR xLocalId, int nHour = 24);
	BOOL			IsPlayManager(CXMLElement* pXML, LPCTSTR xRemoteId) const;
	BOOL			IsAdministrator(LPCTSTR xRemoteId) const;
	int				WriteUserClick(LPCTSTR pszCatalog);
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
	
	int				PlyNotify_init(EVTBLK *evtblk);
	struct state*	PlyNotify_cmplt(EVTBLK *evtblk);
	
	int				PlyUlWord_init(EVTBLK *evtblk);
	struct state*	PlyUlWord_cmplt(EVTBLK *evtblk);
	int				RcvLvWord_init(EVTBLK *evtblk);
	struct state*	RcvLvWord_cmplt(EVTBLK *evtblk);
	
	int				PlyLvFile_init(EVTBLK *evtblk);
	struct state*	PlyLvFile_cmplt(EVTBLK *evtblk);
	struct state*	PlyLvWord_cmplt(EVTBLK *evtblk);
	
	struct state*	RcvTryOper_cmplt(EVTBLK *evtblk);
	int				TryLvFile_init(EVTBLK *evtblk);
	struct state*	TryLvFile_cmplt(EVTBLK *evtblk);
	
	struct state*	RcvHqOper_cmplt(EVTBLK *evtblk);
	int				PlayQReal_init(EVTBLK *evtblk);
	struct state*	PlayQReal_cmplt(EVTBLK *evtblk);
	
	int				ExitPlay_init(EVTBLK *evtblk);
};

#endif // !defined(AFX_CHANNELRADIO_H__771B289D_4128_429E_BC16_025C8CB738D3__INCLUDED_)
