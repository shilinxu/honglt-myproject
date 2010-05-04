//
// ChannelNotify.h
//

#if !defined(AFX_CHANNELNOTIFY_H__771B289D_4128_429E_BC16_025C8CB738D3__INCLUDED_)
#define AFX_CHANNELNOTIFY_H__771B289D_4128_429E_BC16_025C8CB738D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"

//
// definitions of file name
//

#define	FILE_NTFY_WELCOME		"ntfy.welcome.pcm"
#define	FILE_NTFY_WIZARD		"ntfy.wizard.pcm"
#define	FILE_NTFY_RCVOPER		"ntfy.rcvoper.pcm"
#ifdef _SHARE_95950
#define	FILE_NTFY_RCVOPER_IF	"ntfy.rcvoper_if.pcm"
#endif
#define	FILE_NTFY_RCVLVWRD		"ntfy.rcvlvwrd.pcm"
#define	FILE_NTFY_BJMUSIC		"ntfy.bjmusic.pcm"
#define	FILE_NTFY_GOODBYE		"ntfy.goodbye.pcm"
#define	FILE_PAGE_PLYLVWRD		"page.plylvwrd.pcm"
#define	FILE_NTFY_NAVEMIT		"ntfy.navemit.pcm"

//#define	FILE_CHAT_WELCOME		"chat.welcome.pcm"
//#define	FILE_CHAT_RCVOPER		"chat.rcvoper.pcm"
//#define	FILE_CHAT_RCVLVWRD		"chat.rcvlvwrd.pcm"
//#define	FILE_CHAT_INJOIN		"chat.injoin.pcm"
//#define	FILE_CHAT_LEAVEOUT		"chat.leaveout.pcm"
#define	FILE_MEET_MEMBSIT		"meet.membsit.pcm"
#define	FILE_MEET_MEMBEXT		"meet.membext.pcm"

class CNotifyChannel : public CChannel  
{
// Construction
public:
	static CChannel* Create(int nChan, LPCTSTR xFeeCode);
	CNotifyChannel(int nChan, LPCTSTR xFeeCode = "1111");
	virtual ~CNotifyChannel();

// Attributes
protected:
	_RecordsetPtr	m_pRecordset;
	CString			UsrLvFile;
	CXMLElement*	m_pXML;
protected:
	struct state	FwdNotify;
	struct state	RcvFnOper;
	struct state	PlyNotify;
	
	struct state	PlyUlWord;
	struct state	RcvLvWord;
	struct state	PlyLvFile;
	
	struct state	TryLvFile;
	struct state	RcvLvChat;
	struct state	RcvLvMedia;
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
	BOOL			LoadXML(LPCTSTR pszPath);
protected:
	struct state*	RenewPlay(LPCTSTR xLocalId);
	virtual BOOL	OnBindPacket(CBindPacket* pPacket);
	virtual BOOL	OnErrPacket(CErrPacket* pPacket);
	protected:
	int				ShPlyNavFile(int nChan, LPCTSTR xNavFile);
	int				ShPlyLvWord(int nChan, LPCTSTR xLvFile);
	int				ShPlyLvWord(int nChan, LPCTSTR xNavFile, int nCount);
	struct state*	ShRcvFnOper_cmplt(EVTBLK *evtblk, struct state* lpstDefaultState);
	struct state*	Next_RcvFnOper(PROCESS cmplt, CHAR nDtmfKey, struct state* lpstDefaultState);
	struct state*	Prev_RcvFnOper(struct keyRcvFnOper*& ShRcvFnOper, struct state* lpstDefaultState);
	struct state*	Next_RcvFnOper(PROCESS cmplt, CHAR nDtmfKey, struct state* lpstDefaultState, DWORD nReserved);
protected:
	struct state*	RcvFnOper_cmplt(CXMLElement* pXML, CHAR nDtmfKey);
	BOOL			IsDeniedHost(CXMLElement* pXML, LPCTSTR pszHost) const;
	CXMLElement*	FindPlay(struct keyRcvFnOper* ShRcvFnOper, CHAR nDtmfKey);
protected:
	int				GetUsrLvCount(BOOL bForce = FALSE);
	BOOL			OpenUsrRecord(LPCTSTR xPhone, int nIndex = -1);
	BOOL			PutUsrRecord(LPCTSTR xUsrFile, LPCTSTR xPhone, int nKey = 0);
	BOOL			PutUsrRecord(LPCTSTR xUsrFile, LPCTSTR xPhone, DWORD nReserved);
	BOOL			IsAdministrator(LPCTSTR xRemoteId) const;
	BOOL			IsPlayManager(CXMLElement* pXML, LPCTSTR xRemoteId) const;
	int				WriteUserClick(LPCTSTR pszCatalog);
protected:
	int				PlayMemberJoined(int nChan, int nRoomId, int nJoinMode);
	int				PlayMemberJoined(int nChan, int nMmbrId);
	int				PlayMemberExited(int nChan);
	int				PlayMemberExited(int nChan, int nMmbrId);
	int				KickOutMember(int nRoomId, int nMmbrId);
//	int				ExcludeMember(int nRoomId, int nChan2);
	int				ExcludeMember(int nRoomId, int nMmbrId, BOOL bExclude);
	int				FindConfSpeaker(int nRoomId, int nFirstMmbr, int* pnNextMmbr = NULL);
	int				ParseOrderChat(LPCTSTR lpszOrder, int nRoomId, int nMmbrId);
protected:
	int				VerifyRoomId(int nRoomId);
	int				VerifyRoomId(int nRoomId, CXMLElement* pXML);
	int				VerifyRoomUser(CXMLElement* pXML, LPCTSTR xRemoteId, int nRoomId);
	BOOL			VerifyRoomOwner(LPCTSTR lpszText, LPCTSTR xRemoteId);
	BOOL			VerifyRoomGroup(LPCTSTR xOwnerId, int nGroupId, LPCTSTR xRemoteId);
	BOOL			VerifyRoomGroup(CXMLElement* pXML, LPCTSTR xRemoteId);
	BOOL			CheckBlacklist(LPCTSTR xRemoteId, LPCTSTR xLocalId = "96031111");
	BOOL			WriteBlacklist(LPCTSTR xRemoteId, LPCTSTR xLocalId, int nHour = 24);
protected:
	int				GetMaxTextId(LPCTSTR hKey);
	BOOL			OnRecordFile(BOOL bRecMixer, int nVolMixer, CString* pstrFile = NULL);
protected:
	int				bkIdle_init(EVTBLK *evtblk);
	int				FeeLogin_init(EVTBLK *evtblk);
	int				FeeRebind_init(EVTBLK *evtblk);
	struct state*	FeeLogin_cmplt(EVTBLK *evtblk);
	int				rcvMFsig_init(EVTBLK *evtblk);
	int				RcvTalk_init(EVTBLK *evtblk);

	int				FwdNotify_init(EVTBLK *evtblk);
	struct state*	FwdNotify_cmplt(EVTBLK *evtblk);
	int				RcvFnOper_init(EVTBLK *evtblk);
	struct state*	RcvFnOper_cmplt(EVTBLK *evtblk);
	int				PlyNotify_init(EVTBLK *evtblk);
	struct state*	PlyNotify_cmplt(EVTBLK *evtblk);
	
	int				PlyUlWord_init(EVTBLK *evtblk);
	struct state*	PlyUlWord_cmplt(EVTBLK *evtblk);
	
	int				RcvLvOper_init(EVTBLK *evtblk);
	int				PlyLvFile_init(EVTBLK *evtblk);
	struct state*	PlyLvFile_cmplt(EVTBLK *evtblk);
	struct state*	PlyLvWord_cmplt(EVTBLK *evtblk);
	int				RcvLvWord_init(EVTBLK *evtblk);
	struct state*	RcvLvWord_cmplt(EVTBLK *evtblk);
	
	struct state*	RcvTryOper_cmplt(EVTBLK *evtblk);
	int				TryLvFile_init(EVTBLK *evtblk);
	struct state*	TryLvFile_cmplt(EVTBLK *evtblk);
	int				ExitPlay_init(EVTBLK *evtblk);

	int				RcvLvChat_init(EVTBLK *evtblk);
	struct state*	RcvLvChat_cmplt(EVTBLK *evtblk);
	int				bkOffHk_init(EVTBLK *evtblk);
	
	int				RcvLvMedia_init(EVTBLK *evtblk);
	struct state*	RcvLvMedia_cmplt(EVTBLK *evtblk);
};

#endif // !defined(AFX_CHANNELNOTIFY_H__771B289D_4128_429E_BC16_025C8CB738D3__INCLUDED_)
