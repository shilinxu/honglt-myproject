//
// sysdef.h
//


#if !defined(__SYSDEF_H__)
#define __SYSDEF_H__

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

#define CA_TALK				"CATK"	// ����
#define CA_HOOK				"CAHK"	// ����
#define CA_BACK				"CABK"	// �غ�
#define CA_DEBG				"DEBG"	// �ڲ�����(����)

#define	FILE_CALLER_WELCOME		"caller.welcome.pcm"
#define	FILE_CALLER_FINDUSR		"caller.findusr.pcm"
#ifdef _SHARE_95950
#define	FILE_CALLER_WELCOME_LT	"caller.welcome_lt.pcm"
#endif

#define	FILE_FRND_WELCOME		"frnd.welcome.pcm"
#define	FILE_FRND_RCVOPER		"frnd.rcvoper.pcm"
#define	FILE_FRND_WTLVMEMB		"frnd.wtlvmemb.pcm"
#define	FILE_FRND_RCVLVOPER		"frnd.rcvlvoper.pcm"
#define	FILE_FRND_RCVULOPER		"frnd.rcvuloper.pcm"
#define	FILE_FRND_RCVULWORD		"frnd.rcvulword.pcm"
#define	FILE_FRND_BJMUSIC		"frnd.bjmusic.pcm"
#define	FILE_FRND_RCVCOMP		"frnd.rcvcomp.pcm"
#define	FILE_FRND_NOTUSER		"frnd.notuser.pcm"
#define	FILE_FRND_PLYULWORD		"frnd.plyulword.pcm"

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

//
// definitions of timer
//

struct TIMER { BYTE state;     DWORD expire; };

#define TR_OFF				0
#define TR_ON       		1

//
// definitions of network type
//

typedef enum
{
	//CHN_ANALOG	=0,
	//CHN_INTER		=1,
	CHN_USER		=2,
	//CHN_REC		=3,
	CHN_SS1			=4,
	CHN_FAX			=5,
	CHN_TUP			=6,
	//CHN_ISDN_USER	=7,		
	//CHN_ISDN_NET	=8,
	CHN_SOFTFAX		=9,
	//CHN_MAGNET	=10,
	CHN_ISUP		=11,
	CHN_UNKNOWN,
} EChannel;

//
// �Զ����¼�
//

enum
{
	// �Ʒ�
	E_FEE_Login			= 0x1001,	// ���ӼƷ�����
	E_TAK_Connect		= 0x1002,	// �Ի�����
};

enum
{
	//�Ʒ�
	S_FEE_Connected		= 1001,		// �Ʒ�TCP�����ɹ�
	S_FEE_Disconnected	= 1002,		// �Ʒ�TCP����ʧ��
//	S_FEE_Binded		= 1003,		// �󶨲����ɹ�
};

enum
{
	//�Ի�
	S_TAK_Connected		= 1001,		// �Ự���ӳɹ�
	S_TAK_Disconnected	= 1002,		// �Ự����ʧ��
};


#endif // !defined(__SYSDEF_H__)
