//
// Channel.h
//

#if !defined(AFX_CHANNEL_H__5522AB2D_025E_45A9_BCC4_2A355609B824__INCLUDED_)
#define AFX_CHANNEL_H__5522AB2D_025E_45A9_BCC4_2A355609B824__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//
// Utility
//

#define EVTBLK				MESSAGE_INFO
#define MAKEVENT(a, b)		((LONG)(((DWORD)((WORD)(a))) << 16) | ((WORD)(b)))

//
// Configuration
//

#define MAX_PHONE			21
#define MAX_BUFFER			256
#define MAX_FILENAME		128
#define MAX_PASSWORD		10
#define MAX_CALLTYPE		4

//
// definitions of call types
//

#define CA_TALK				"CATK"	// 主叫
#define CA_HOOK				"CAHK"	// 被叫
#define CA_BACK				"CABK"	// 回呼
#define CA_DEBG				"DEBG"	// 内部测试(保留)

#define TR_OFF				0
#define TR_ON       		1

//
// definitions of file name
//

#define FT_PROMPT			"Media"

#define	FILE_PROMPT_CALLBACK	"prompt.callback.pcm"
#define	FILE_PROMPT_GOODBYE	"prompt.goodbye.pcm"
#define	FILE_PROMPT_NORECD	"prompt.norecd.pcm"
#define	FILE_PROMPT_NOTUSER	"prompt.notuser.pcm"
#define	FILE_PROMPT_NOPAY	"prompt.nopay.pcm"
#define	FILE_PROMPT_BUSY	"prompt.busy.pcm"
#define	FILE_PROMPT_QUERY	"prompt.query.pcm"

//
// definitions of exit code
//

#define EXIT_NORM			0
#define EXIT_GOODBYE		1
#define EXIT_NOTUSER		2
#define EXIT_NOPAY			3
#define EXIT_BUSY			4
#define EXIT_NORECD			5
#define EXIT_CALLBACK		6

#include "Transfer.h"
#include "PacketCore.h"

class CChannel;
class CNetworkChild;

typedef int (CChannel::*INITIATE)(EVTBLK *evtblk);			// pointer to initiating functions
typedef struct state *(CChannel::*PROCESS)(EVTBLK *evtblk);	// pointer to processing functions

struct state
{
	INITIATE	initiate;	// pointer to initiating functions
	PROCESS		process;	// pointer to processing functions
};

#pragma pack(1)

class CChannel : public CTransfer
{
	struct TIMER { BYTE state;     DWORD expire; };

// Construction
public:
	CChannel(int nChan, LPCTSTR xFeeCode = NULL);
	virtual ~CChannel();

// Attributes
public:
	int				m_nChan;
	LPTSTR			m_xFeeCode;
	DWORD			m_nTaskId;
	CNetworkChild*	m_pOwner;
public:
	struct state*	fcnp;					// pointr for CompletionFunc
	CHAR			LocalId[MAX_PHONE+1];	// buffer for local telnum
	CHAR			RemoteId[MAX_PHONE+1];	// buffer for remote telnum
	CHAR			FeeNumber[MAX_PHONE+1];	// buffer for fee telnum
	CHAR			CallType[MAX_CALLTYPE+1];//buffer for call type
public:
	HINSTANCE		m_hInstance;			// Library Channel
	HINSTANCE		m_hDllClosing;			// Library Channel Of Old
protected:
	struct TIMER	ExpTimer;				// timer for time out
	BYTE			ExitCode : 4;			// code of exit channel
	BYTE			Reserved : 4;			// reserved for future use
protected:
	struct state	bkIdle;
	struct state	bkFree;
	struct state	bkOnHk;
	struct state	bkOffHk;
	struct state	ResetPcm;
	
	struct state	seiConfm;
	struct state	rcvMFsig;
	struct state	RcvTalk;
	
	struct state	WaitDial;

	struct state	FeeLogin;
	struct state	ExitPlay;

// Operations
public:
	virtual void	Setup();
	virtual HMODULE	Release();
	virtual void	Clone(CChannel* pParent);
	virtual void	SetOwner(CNetworkChild* pOwner);
	virtual void	PrintChState(EVTBLK *evtblk);
	virtual void	StopChan();
	virtual BOOL	OnRun();
public:
	virtual void	SetTimer(DWORD nTimer);
	virtual void	KillTimer();
	virtual BOOL	TimerOut();
	virtual void	SetLinker(int nChan);
	virtual void	KillLinker();
protected:
	int				LoadCallerId();
	int				LoadPhonNumber();
	int				VerifyPhoneNum(CHAR* xPhone);
	BOOL			IsPoseNumber(LPCTSTR xPhone, LPCTSTR xCaller, CString* pxCaller);
	BOOL			IsTestNumber(LPCTSTR xPhone, CString* pxCaller = NULL);
	struct state*	ChkDTSignal(EVTBLK *evtblk,struct state* lpstDefaultState);
public:
	int				PlayFile(LPCTSTR xFileName, LPCTSTR xFilePath, BOOL bTermDtmf = TRUE);
	int				RecordFile(LPCTSTR xFileName, LPCTSTR xFilePath, LPCTSTR xTermDtmf = "");
	int				RecordFax(LPCTSTR xFileName, LPCTSTR xFilePath);
	int				SendFax(LPCTSTR xFileName, LPCTSTR xFilePath, BOOL bAppend = FALSE);
	int				CompDigital(LPCTSTR xDigital, BOOL bUnit);
	int				ReadDtmf(LPTSTR pBuffer, CHAR cp = '#');
	int				GetNextBuffer(LPCTSTR xSrc, LPCTSTR& xDst, CHAR cp = '*');
public:
	virtual BOOL	OnPacket(CPacket* pPacket);
	virtual BOOL	OnBindPacket(CBindPacket* pPacket);
	virtual BOOL	OnDialPacket(CDialPacket* pPacket);
	virtual BOOL	OnRecordPacket(CRecordPacket* pPacket);
	virtual BOOL	OnListenPacket(CListenPacket* pPacket);
	virtual BOOL	OnErrPacket(CErrPacket* pPacket);
	virtual BOOL	OnHaltPacket(CHaltPacket* pPacket);
	virtual BOOL	OnConnected();
	virtual void	Disconnect();
protected:
	virtual CPacket*	ReadBuffer(CBuffer* pBuffer);

protected:
	int				bkIdle_init(EVTBLK* evtblk);
	struct state*	bkIdle_cmplt(EVTBLK* evtblk);
	int				bkFree_init(EVTBLK* evtblk);
	struct state*	bkFree_cmplt(EVTBLK* evtblk);
	int				bkOnHk_init(EVTBLK *evtblk);
	struct state*	bkOnHk_cmplt(EVTBLK *evtblk);
	int				bkOffHk_init(EVTBLK *evtblk);
	struct state*	bkOffHk_cmplt(EVTBLK *evtblk);
	int				ResetPcm_init(EVTBLK* evtblk);
	struct state*	ResetPcm_cmplt(EVTBLK *evtblk);
	
	int				WaitDial_init(EVTBLK* evtblk);
	struct state*	WaitDial_cmplt(EVTBLK *evtblk);
	
	int				seiConfm_init(EVTBLK *evtblk);
	struct state*	seiConfm_cmplt(EVTBLK *evtblk);
	int				rcvMFsig_init(EVTBLK* evtblk);
	struct state*	rcvMFsig_cmplt(EVTBLK *evtblk);
	int				RcvTalk_init(EVTBLK* evtblk);
	struct state*	RcvTalk_cmplt(EVTBLK *evtblk);
	
	int				FeeLogin_init(EVTBLK *evtblk);
	struct state*	FeeLogin_cmplt(EVTBLK *evtblk);

	int				ExitPlay_init(EVTBLK *evtblk);
	struct state*	ExitPlay_cmplt(EVTBLK *evtblk);
};

#pragma pack()

//
// 自定义事件
//

enum
{
	// 计费
	E_FEE_Login			= 0x1001,	// 连接计费中心
	E_TAK_Connect		= 0x1002,	// 对话连接
};

enum
{
	//计费
	S_FEE_Connected		= 1001,		// 计费TCP建链成功
	S_FEE_Disconnected	= 1002,		// 计费TCP建链失败
//	S_FEE_Binded		= 1003,		// 绑定操作成功
};

enum
{
	//对话
	S_TAK_Connected		= 1001,		// 会话连接成功
	S_TAK_Disconnected	= 1002,		// 会话连接失败
};

#endif // !defined(AFX_CHANNEL_H__5522AB2D_025E_45A9_BCC4_2A355609B824__INCLUDED_)
