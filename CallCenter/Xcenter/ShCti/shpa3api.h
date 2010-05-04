  //                               �����ȨЭ��
//1��	����������Ϣ�������޹�˾�����¼�Ʊ���˾��ӵ��"����������и�����Ʒ���ļ�������ĵ�"�����¼�Ʊ���Ʒ������ȫ��Ȩ���κε�λ�͸�����ʹ��ǰ���ñ���˾����Ȩ��
//2��	����˾��Ȩ��������ȫ�������ĵ�λ�͸������ʹ�ñ���Ʒ��
//A��	����Ʒ��ͨ���Ϸ�������������ı���˾Ӳ����Ʒ���ʹ��ʱ��
//B��	��˽�����������������Ʒ�ͱ���Ʒ���κ�һ���֡�
//3��	�����ϵ�2�������������λ�͸�����Ҫʹ�ñ���Ʒʱ�������ñ���˾��������Ȩ��
//4��	�����Ȩ�ĵ�λ�͸��˲���ת�����õ���Ȩ��
//5��	ʹ�ñ���Ʒ����������ȫ��ⱾЭ����������ȫ�����ܡ�

#ifndef WIN32
	#ifndef OS_LINUX
		#ifndef TYPEDEF_H
			#define TYPEDEF_H

			typedef char         	CHAR;
			typedef float           FLOAT;
			typedef int				BOOL;
			typedef int             INT;
			typedef long			LONG;
			typedef void            VOID;

			typedef unsigned char	BYTE;
			typedef unsigned char   UCHAR;
			typedef unsigned int    UINT;
			typedef unsigned long   DWORD;
			typedef unsigned long   ULONG;
			typedef unsigned short  USHORT;
			typedef unsigned short	WORD;

			typedef BOOL            *LPBOOL;
			typedef BYTE            *LPBYTE;
			typedef BYTE            *PBYTE;
			typedef CHAR            *LPSTR;
			typedef CHAR            *LPTSTR;
			typedef CHAR            *PCHAR;
			typedef DWORD           *LPDWORD;
			typedef DWORD           *PDWORD;
			typedef WORD            *LPWORD;
			typedef WORD            *PWORD;
			typedef UCHAR           *PUCHAR;
			typedef void            *PVOID;
			typedef long			*LPLONG;
			typedef long			*PLONG;
			typedef void            *LPVOID;

			typedef const CHAR      *LPCSTR;
			typedef const CHAR      *LPCTSTR;
			typedef const void      *LPCVOID;

		#endif // end of #ifndef TYPEDEF_H
	#endif // end of #ifndef OS_LINUX

	#ifndef WINAPI
		#define WINAPI
	#endif

#endif // end of #ifndef WIN32

#ifndef SHCTI_CONST
	#define SHCTI_CONST

//-----------------------------------------------------------------------
// definition of channel type, which can be retrieved by invoking
// function "SsmGetFlag()" and set by invoking "SsmSetFlag()"
//-----------------------------------------------------------------------
enum{
	F_RCVDTMFSENS=1,					//����DTMF���ж�
	F_TXDTMFAMP=2,						//����DTMF�ź�ǿ��
	F_RCVPHONUMHOLDUP=3,				//���к������ر��
	F_RELATIVEENGYHOOKDETECT=4,			//�Ƿ�����ģ��绰�߱���ժ��������㷨
	F_RXR2FILTERTIME=5,					//R2�����˲�ʱ��
	F_RECTOFILEA_CALLBACKTIME = 6,
	F_CALLERIDSTYLE=7,
	F_InVoiceToBus=8,
	F_ClearInVoiceOnRcvDtmf=9,
	F_MixerResToBus=10,
	F_HighAndLowFreqEnScale=11,
	F_DualAndAllFreqEnScale=12,
	F_EchoCancelInFsk=13,				//����Fsk�շ������еĻز�����
	F_ChToRingingOnRingCnt=14,
	F_ISDNNet_WaitRemotePickup=15,
	F_SetAdjustCtl=16,
	F_ClearInVoiceOnRcv450Hz = 17,
	F_FSKMinGate = 18,
};

//-----------------------------------------------------------------------
// definition of blocking reason, which can be retrieved by invoking
// function "SsmGetBlockReason()"
//-----------------------------------------------------------------------
enum{
	BLOCKBY_NULL,
	BLOCKBY_TupRcvMGB,
	BLOCKBY_TupRcvHGB,
	BLOCKBY_TupRcvSGB,
	BLOCKBY_TupWaitRLGTimeout,
	BLOCKBY_TupBlockByApp,
};

//-----------------------------------------------------------------------
// Definition of status on blocking remote circuit or circuit groups,
// which might be used by following functions
//		1. "SsmChkBlockRemoteXXX()"
//		2. "SsmChkBlockRemoteXXX()"
//-----------------------------------------------------------------------
#define BLOCKREMOTE_Unblocked			0	// no block-signal is issued
#define BLOCKREMOTE_Blocked				1	// is in blocked state now
#define BLOCKREMOTE_WaitBlockAck		2	// waiting for acknowledgement signal after sending block-signal
#define BLOCKREMOTE_WaitUnblockAck		3	// waiting for acknowledgement signal after sending unblock-signal

//-----------------------------------------------------------------------
// Definition of local blocking status on circuit or circuit groups,
// which might be used by following functions
//		 "SsmQueryLocalXXXBlockState()"
//-----------------------------------------------------------------------
#define BLOCK_AppBlockCic	0x00000001	// channel is blocked by invoking SsmBlockLocalCh()
#define BLOCK_TupRcvBLO		0x00000002	// channel is blocked by received BLO
#define BLOCK_TupRcvSGB		0x00000004	// channel is blocked by received SGB
#define BLOCK_TupRcvHGB		0x00000008	// channel is blocked by received HGB
#define BLOCK_TupRcvMGB		0x00000010	// channel is blocked by received MGB
#define BLOCK_AppBlockPCM	0x00000020	// channel is blocked by invoking SsmBlockLocalPCM()


//-----------------------------------------------------------------------
// Definition of channel unavailable reason
//-----------------------------------------------------------------------
#define UNAVAILABLE_PcmSyncLos		0x00000001	
#define UNAVAILABLE_Mtp3Unusuable	0x00000002	

//-----------------------------------------------------------------------
// Definition of DST D-Channel Event
//-----------------------------------------------------------------------
#define    DST_OFFHOOK					0x8								
#define    DST_ONHOOK					0xe
#define    DST_LT_ON					0x1001
#define    DST_LT_OFF					0x1002
#define    DST_LT_FLASHING				0x1003
#define    DST_DGT_PRS					0x1006
#define    DST_DGT_RLS					0x1007
#define    DST_MSG_CHG					0x1008
#define    DST_STARTSTOP_ON				0x1009
#define    DST_STARTSTOP_OFF				0x100a
#define    DST_LT_FASTFLASHING				0x100b
#define    DST_DOWNLOAD_STATUS				0x100c
#define    DST_FINISHED_PLAY				0x100d
#define    DST_FUNC_BTN_PRS				0x100e
#define    DST_FUNC_BTN_RLS				0x100f
#define    DST_HOLD_BTN_PRS				0x1010
#define    DST_HOLD_BTN_RLS				0x1011
#define    DST_RELEASE_BTN_PRS				0x1012
#define    DST_RELEASE_BTN_RLS				0x1013
#define    DST_TRANSFER_BTN_PRS				0x1014
#define    DST_ANSWER_BTN_PRS				0x1015
#define    DST_SPEAKER_BTN_PRS				0x1016
#define    DST_REDIAL_BTN_PRS				0x1017
#define    DST_CONF_BTN_PRS				0x1018
#define    DST_RECALL_BTN_PRS				0x1019
#define    DST_FEATURE_BTN_PRS				0x101a
#define    DST_UP_DOWN					0x101b
#define    DST_EXIT_BTN_PRS				0x101c
#define    DST_HELP_BTN_PRS				0x101d
#define    DST_SOFT_BTN_PRS				0x101e
#define    DST_RING_ON					0x101f
#define    DST_RING_OFF					0x1020
#define    DST_LINE_BTN_PRS				0x1021
#define    DST_MENU_BTN_PRS				0x1022
#define    DST_PREVIOUS_BTN_PRS				0x1023
#define    DST_NEXT_BTN_PRS				0x1024
#define    DST_LT_QUICKFLASH				0x1025
#define    DST_AUDIO_ON					0x1026
#define    DST_AUDIO_OFF				0x1027
#define    DST_DISPLAY_CLOCK				0x1028
#define    DST_DISPLAY_TIMER				0x1029
#define    DST_DISPLAY_CLEAR				0x102a
#define    DST_CFWD					0x102b
#define    DST_CFWD_CANCELED				0x102c
#define    DST_AUTO_ANSWER				0x102d
#define    DST_AUTO_ANSWER_CANCELED			0x102e
#define    DST_SET_BUSY					0x102f
#define    DST_SET_BUSY_CANCELED			0x1030
#define    DST_DESTINATION_BUSY				0x1031
#define    DST_REORDER					0x1032
#define    DST_LT_VERY_FASTFLASHING			0x1033
#define    DST_SPEAKER_BTN_RLS				0x1034
#define    DST_REDIAL_BTN_RLS				0x1035
#define    DST_TRANSFER_BTN_RLS				0x1036
#define    DST_CONF_BTN_RLS				0x1037
#define    DST_DISCONNECTED				0x1038
#define    DST_CONNECTED				0x1039
#define    DST_ABANDONED				0x103a
#define    DST_SUSPENDED				0x103b
#define    DST_RESUMED					0x103c
#define    DST_HELD					0x103d
#define    DST_RETRIEVED				0x103e
#define    DST_REJECTED					0x103f
#define    DST_MSG_BTN_PRS				0x1040
#define    DST_MSG_BTN_RLS				0x1041
#define    DST_SUPERVISOR_BTN_PRS			0x1042
#define    DST_SUPERVISOR_BTN_RLS			0x1043
#define    DST_WRAPUP_BTN_PRS				0x1044
#define    DST_WRAPUP_BTN_RLS				0x1045
#define    DST_READY_BTN_PRS				0x1046
#define    DST_READY_BTN_RLS				0x1047
#define    DST_LOGON_BTN_PRS				0x1048
#define    DST_BREAK_BTN_PRS				0x1049
#define    DST_AUDIO_CHG				0x104a
#define    DST_DISPLAY_MSG				0x104b
#define    DST_WORK_BTN_PRS				0x104c
#define    DST_TALLY_BTN_PRS				0x104d
#define    DST_PROGRAM_BTN_PRS				0x104e
#define    DST_MUTE_BTN_PRS				0x104f
#define    DST_ALERTING_AUTO_ANSWER			0x1050	
#define    DST_MENU_BTN_RLS				0x1051
#define    DST_EXIT_BTN_RLS				0x1052
#define    DST_NEXT_BTN_RLS				0x1053
#define    DST_PREVIOUS_BTN_RLS				0x1054
#define    DST_SHIFT_BTN_PRS				0x1055
#define    DST_SHIFT_BTN_RLS				0x1056
#define    DST_PAGE_BTN_PRS				0x1057
#define    DST_PAGE_BTN_RLS				0x1058
#define    DST_SOFT_BTN_RLS				0x1059
#define    DST_LINE_LT_OFF				0x1060
#define    DST_LINE_LT_ON				0x1061
#define    DST_LINE_LT_FLASHING				0x1062
#define    DST_LINE_LT_FASTFLASHING			0x1063
#define    DST_LINE_LT_VERY_FASTFLASHING		0x1064
#define    DST_LINE_LT_QUICKFLASH			0x1065
#define    DST_LINE_LT_WINK				0x1066
#define    DST_LINE_LT_SLOW_WINK			0x1067
#define    DST_FEATURE_LT_OFF				0x1068
#define    DST_FEATURE_LT_ON				0x1069
#define    DST_FEATURE_LT_FLASHING			0x106A
#define    DST_FEATURE_LT_FASTFLASHING			0x106B
#define    DST_FEATURE_LT_VERY_FASTFLASHING		0x106C
#define    DST_FEATURE_LT_QUICKFLASH			0x106D
#define    DST_FEATURE_LT_WINK				0x106E
#define    DST_FEATURE_LT_SLOW_WINK			0x106F
#define    DST_SPEAKER_LT_OFF				0x1070
#define    DST_SPEAKER_LT_ON				0x1071
#define    DST_SPEAKER_LT_FLASHING			0x1072
#define    DST_SPEAKER_LT_FASTFLASHING			0x1073
#define    DST_SPEAKER_LT_VERY_FASTFLASHING		0x1074
#define    DST_SPEAKER_LT_QUICKFLASH			0x1075
#define    DST_SPEAKER_LT_WINK				0x1076
#define    DST_SPEAKER_LT_SLOW_WINK			0x1077
#define    DST_MIC_LT_OFF				0x1078
#define    DST_MIC_LT_ON				0x1079
#define    DST_MIC_LT_FLASHING				0x107A
#define    DST_MIC_LT_FASTFLASHING			0x107B
#define    DST_MIC_LT_VERY_FASTFLASHING			0x107C
#define    DST_MIC_LT_QUICKFLASH			0x107D
#define    DST_MIC_LT_WINK				0x107E
#define    DST_MIC_LT_SLOW_WINK				0x107F
#define    DST_HOLD_LT_OFF				0x1080
#define    DST_HOLD_LT_ON				0x1081
#define    DST_HOLD_LT_FLASHING				0x1082
#define    DST_HOLD_LT_FASTFLASHING			0x1083
#define    DST_HOLD_LT_VERY_FASTFLASHING		0x1084
#define    DST_HOLD_LT_QUICKFLASH			0x1085
#define    DST_HOLD_LT_WINK				0x1086
#define    DST_HOLD_LT_SLOW_WINK			0x1087
#define    DST_RELEASE_LT_OFF				0x1088
#define    DST_RELEASE_LT_ON				0x1089
#define    DST_RELEASE_LT_FLASHING			0x108A
#define    DST_RELEASE_LT_FASTFLASHING			0x108B
#define    DST_RELEASE_LT_VERY_FASTFLASHING		0x108C
#define    DST_RELEASE_LT_QUICKFLASH			0x108D
#define    DST_RELEASE_LT_WINK				0x108E
#define    DST_RELEASE_LT_SLOW_WINK			0x108F
#define    DST_HELP_LT_OFF				0x1090
#define    DST_HELP_LT_ON				0x1091
#define    DST_HELP_LT_FLASHING				0x1092
#define    DST_HELP_LT_FASTFLASHING			0x1093
#define    DST_HELP_LT_VERY_FASTFLASHING		0x1094
#define    DST_HELP_LT_QUICKFLASH			0x1095
#define    DST_HELP_LT_WINK				0x1096
#define    DST_HELP_LT_SLOW_WINK			0x1097 
#define    DST_SUPERVISOR_LT_OFF			0x1098
#define    DST_SUPERVISOR_LT_ON				0x1099
#define    DST_SUPERVISOR_LT_FLASHING			0x109A
#define    DST_SUPERVISOR_LT_FASTFLASHING		0x109B
#define    DST_SUPERVISOR_LT_VERY_FASTFLASHING		0x109C
#define    DST_SUPERVISOR_LT_QUICKFLASH			0x109D
#define    DST_SUPERVISOR_LT_WINK			0x109E
#define    DST_SUPERVISOR_LT_SLOW_WINK			0x109F
#define    DST_READY_LT_OFF				0x10A0
#define    DST_READY_LT_ON				0x10A1
#define    DST_READY_LT_FLASHING			0x10A2
#define    DST_READY_LT_FASTFLASHING			0x10A3
#define    DST_READY_LT_VERY_FASTFLASHING		0x10A4
#define    DST_READY_LT_QUICKFLASH			0x10A5
#define    DST_READY_LT_WINK				0x10A6
#define    DST_READY_LT_SLOW_WINK			0x10A7
#define    DST_LOGON_LT_OFF				0x10A8
#define    DST_LOGON_LT_ON				0x10A9
#define    DST_LOGON_LT_FLASHING			0x10AA
#define    DST_LOGON_LT_FASTFLASHING			0x10AB
#define    DST_LOGON_LT_VERY_FASTFLASHING		0x10AC
#define    DST_LOGON_LT_QUICKFLASH			0x10AD
#define    DST_LOGON_LT_WINK				0x10AE
#define    DST_LOGON_LT_SLOW_WINK			0x10AF
#define    DST_WRAPUP_LT_OFF				0x10B0
#define    DST_WRAPUP_LT_ON				0x10B1
#define    DST_WRAPUP_LT_FLASHING			0x10B2
#define    DST_WRAPUP_LT_FASTFLASHING			0x10B3
#define    DST_WRAPUP_LT_VERY_FASTFLASHING		0x10B4
#define    DST_WRAPUP_LT_QUICKFLASH			0x10B5
#define    DST_WRAPUP_LT_WINK				0x10B6
#define    DST_WRAPUP_LT_SLOW_WINK			0x10B7
#define    DST_RING_LT_OFF				0x10B8
#define    DST_RING_LT_ON				0x10B9
#define    DST_RING_LT_FLASHING				0x10BA
#define    DST_RING_LT_FASTFLASHING			0x10BB
#define    DST_RING_LT_VERY_FASTFLASHING		0x10BC
#define    DST_RING_LT_QUICKFLASH			0x10BD
#define    DST_RING_LT_WINK				0x10BE
#define    DST_RING_LT_SLOW_WINK			0x10BF 
#define    DST_ANSWER_LT_OFF				0x10C0
#define    DST_ANSWER_LT_ON				0x10C1
#define    DST_ANSWER_LT_FLASHING			0x10C2
#define    DST_ANSWER_LT_FASTFLASHING			0x10C3
#define    DST_ANSWER_LT_VERY_FASTFLASHING		0x10C4
#define    DST_ANSWER_LT_QUICKFLASH			0x10C5
#define    DST_ANSWER_LT_WINK				0x10C6
#define    DST_ANSWER_LT_SLOW_WINK			0x10C7
#define    DST_PROGRAM_LT_OFF				0x10C8
#define    DST_PROGRAM_LT_ON				0x10C9
#define    DST_PROGRAM_LT_FLASHING			0x10CA
#define    DST_PROGRAM_LT_FASTFLASHING			0x10CB
#define    DST_PROGRAM_LT_VERY_FASTFLASHING		0x10CC
#define    DST_PROGRAM_LT_QUICKFLASH			0x10CD
#define    DST_PROGRAM_LT_WINK				0x10CE
#define    DST_PROGRAM_LT_MEDIUM_WINK			0x10CF
#define    DST_MSG_LT_OFF				0x10D0
#define    DST_MSG_LT_ON				0x10D1
#define    DST_MSG_LT_FLASHING				0x10D2
#define    DST_MSG_LT_FASTFLASHING			0x10D3
#define    DST_MSG_LT_VERY_FASTFLASHING			0x10D4
#define    DST_MSG_LT_QUICKFLASH			0x10D5
#define    DST_MSG_LT_WINK				0x10D6
#define    DST_MSG_LT_SLOW_WINK				0x10D7
#define    DST_TRANSFER_LT_OFF				0x10D8
#define    DST_TRANSFER_LT_ON				0x10D9
#define    DST_TRANSFER_LT_FLASHING			0x10DA
#define    DST_TRANSFER_LT_FASTFLASHING			0x10DB
#define    DST_TRANSFER_LT_VERY_FASTFLASHING		0x10DC
#define    DST_TRANSFER_LT_QUICKFLASH			0x10DD
#define    DST_TRANSFER_LT_WINK				0x10DE
#define    DST_TRANSFER_LT_MEDIUM_WINK			0x10DF
#define    DST_CONFERENCE_LT_OFF			0x10E0
#define    DST_CONFERENCE_LT_ON				0x10E1
#define    DST_CONFERENCE_LT_FLASHING			0x10E2
#define    DST_CONFERENCE_LT_FASTFLASHING		0x10E3
#define    DST_CONFERENCE_LT_VERY_FASTFLASHING		0x10E4
#define    DST_CONFERENCE_LT_QUICKFLASH			0x10E5
#define    DST_CONFERENCE_LT_WINK			0x10E6
#define    DST_CONFERENCE_LT_MEDIUM_WINK		0x10E7
#define    DST_SOFT_LT_OFF				0x10E8
#define    DST_SOFT_LT_ON				0x10E9
#define    DST_SOFT_LT_FLASHING				0x10EA
#define    DST_SOFT_LT_FASTFLASHING			0x10EB
#define    DST_SOFT_LT_VERY_FASTFLASHING		0x10EC
#define    DST_SOFT_LT_QUICKFLASH			0x10ED
#define    DST_SOFT_LT_WINK				0x10EE
#define    DST_SOFT_LT_SLOW_WINK			0x10EF
#define    DST_MENU_LT_OFF				0x10F0
#define    DST_MENU_LT_ON				0x10F1
#define    DST_MENU_LT_FLASHING				0x10F2
#define    DST_MENU_LT_FASTFLASHING			0x10F3
#define    DST_MENU_LT_VERY_FASTFLASHING		0x10F4
#define    DST_MENU_LT_QUICKFLASH			0x10F5
#define    DST_MENU_LT_WINK				0x10F6 
#define    DST_MENU_LT_SLOW_WINK			0x10F7
#define    DST_CALLWAITING_LT_OFF			0x10F8
#define    DST_CALLWAITING_LT_ON			0x10F9
#define    DST_CALLWAITING_LT_FLASHING			0x10FA
#define    DST_CALLWAITING_LT_FASTFLASHING		0x10FB
#define    DST_CALLWAITING_LT_VERY_FASTFLASHING		0x10FC
#define    DST_CALLWAITING_LT_QUICKFLASH		0x10FD
#define    DST_CALLWAITING_LT_WINK			0x10FE
#define    DST_CALLWAITING_LT_SLOW_WINK			0x10FF
#define    DST_REDIAL_LT_OFF				0x1100
#define    DST_REDIAL_LT_ON				0x1101
#define    DST_REDIAL_LT_FLASHING			0x1102
#define    DST_REDIAL_LT_FASTFLASHING			0x1103
#define    DST_REDIAL_LT_VERY_FASTFLASHING		0x1104
#define    DST_REDIAL_LT_QUICKFLASH			0x1105
#define    DST_REDIAL_LT_WINK				0x1106
#define    DST_REDIAL_LT_SLOW_WINK			0x1107
#define    DST_PAGE_LT_OFF				0x1108
#define    DST_PAGE_LT_ON				0x1109
#define    DST_PAGE_LT_FLASHING				0x110A
#define    DST_PAGE_LT_FASTFLASHING			0x110B
#define    DST_PAGE_LT_VERY_FASTFLASHING		0x110C
#define    DST_PAGE_LT_QUICKFLASH			0x110D
#define    DST_CTRL_BTN_PRS				0x110E
#define    DST_CTRL_BTN_RLS				0x110F
#define    DST_CANCEL_BTN_PRS				0x1110
#define    DST_CANCEL_BTN_RLS				0x1111
#define    DST_MIC_BTN_PRS				0x1112
#define    DST_MIC_BTN_RLS				0x1113
#define    DST_FLASH_BTN_PRS				0x1114
#define    DST_FLASH_BTN_RLS				0x1115
#define    DST_DIRECTORY_BTN_PRS			0x1116
#define    DST_DIRECTORY_BTN_RLS			0x1117
#define    DST_HANDSFREE_BTN_PRS			0x1118
#define    DST_HANDSFREE_BTN_RLS			0x1119
#define    DST_RINGTONE_BTN_PRS				0x111A
#define    DST_RINGTONE_BTN_RLS				0x111B
#define    DST_SAVE_BTN_PRS				0x111C
#define    DST_SAVE_BTN_RLS				0x111D
#define    DST_MUTE_LT_OFF				0x111E
#define    DST_MUTE_LT_ON				0x111F
#define    DST_MUTE_LT_FLASHING				0x1120
#define    DST_MUTE_LT_FASTFLASHING			0x1121
#define    DST_MUTE_LT_VERY_FASTFLASHING		0x1122
#define    DST_MUTE_LT_QUICKFLASH			0x1123
#define    DST_MUTE_LT_WINK				0x1124
#define    DST_MUTE_LT_SLOW_WINK			0x1125
#define    DST_MUTE_LT_MEDIUM_WINK			0x1126
#define    DST_HANDSFREE_LT_OFF				0x1127
#define    DST_HANDSFREE_LT_ON				0x1128
#define    DST_HANDSFREE_LT_FLASHING			0x1129
#define    DST_HANDSFREE_LT_FASTFLASHING		0x112A
#define    DST_HANDSFREE_LT_VERY_FASTFLASHING		0x112B
#define    DST_HANDSFREE_LT_QUICKFLASH			0x112C
#define    DST_HANDSFREE_LT_WINK			0x112D
#define    DST_HANDSFREE_LT_SLOW_WINK			0x112E
#define    DST_HANDSFREE_LT_MEDIUM_WINK			0x112F
#define    DST_DIRECTORY_LT_OFF				0x1130
#define    DST_DIRECTORY_LT_ON				0x1131
#define    DST_DIRECTORY_LT_FLASHING			0x1132
#define    DST_DIRECTORY_LT_FASTFLASHING		0x1133
#define    DST_DIRECTORY_LT_VERY_FASTFLASHING		0x1134
#define    DST_DIRECTORY_LT_QUICKFLASH			0x1135
#define    DST_DIRECTORY_LT_WINK			0x1136
#define    DST_DIRECTORY_LT_SLOW_WINK			0x1137
#define    DST_DIRECTORY_LT_MEDIUM_WINK			0x1138
#define    DST_RINGTONE_LT_OFF				0x1139
#define    DST_RINGTONE_LT_ON				0x113A
#define    DST_RINGTONE_LT_FLASHING			0x113B
#define    DST_RINGTONE_LT_FASTFLASHING			0x113C
#define    DST_RINGTONE_LT_VERY_FASTFLASHING		0x113D
#define    DST_RINGTONE_LT_QUICKFLASH			0x113E
#define    DST_RINGTONE_LT_WINK				0x113F
#define    DST_RINGTONE_LT_SLOW_WINK			0x1140
#define    DST_RINGTONE_LT_MEDIUM_WINK			0x1141
#define    DST_SAVE_LT_OFF				0x1142
#define    DST_SAVE_LT_ON				0x1143
#define    DST_SAVE_LT_FLASHING				0x1144
#define    DST_SAVE_LT_FASTFLASHING			0x1145
#define    DST_SAVE_LT_VERY_FASTFLASHING		0x1146
#define    DST_SAVE_LT_QUICKFLASH			0x1147
#define    DST_SAVE_LT_WINK				0x1148
#define    DST_SAVE_LT_SLOW_WINK			0x1149
#define    DST_SAVE_LT_MEDIUM_WINK			0x114A 
#define    DST_FUNC_LT_WINK				0x114B
#define    DST_FUNC_LT_SLOW_WINK			0x114C
#define    DST_FUNC_LT_MEDIUM_WINK			0x114D
#define    DST_CALLWAITING_BTN_PRS			0x114E
#define    DST_CALLWAITING_BTN_RLS			0x114F
#define    DST_PARK_BTN_PRS				0x1150
#define    DST_PARK_BTN_RLS				0x1151
#define    DST_NEWCALL_BTN_PRS				0x1152
#define    DST_NEWCALL_BTN_RLS				0x1153
#define    DST_PARK_LT_OFF				0x1154
#define    DST_PARK_LT_ON				0x1155
#define    DST_PARK_LT_FLASHING				0x1156
#define    DST_PARK_LT_FASTFLASHING			0x1157
#define    DST_PARK_LT_VERY_FASTFLASHING		0x1158
#define    DST_PARK_LT_QUICKFLASH			0x1159
#define    DST_PARK_LT_WINK				0x115A
#define    DST_PARK_LT_SLOW_WINK			0x115B
#define    DST_PARK_LT_MEDIUM_WINK			0x115C
#define    DST_SCROLL_BTN_PRS				0x115D
#define    DST_SCROLL_BTN_RLS				0x115E
#define    DST_DIVERT_BTN_PRS				0x115F
#define    DST_DIVERT_BTN_RLS				0x1160
#define    DST_GROUP_BTN_PRS				0x1161
#define    DST_GROUP_BTN_RLS				0x1162
#define    DST_SPEEDDIAL_BTN_PRS			0x1163
#define    DST_SPEEDDIAL_BTN_RLS			0x1164
#define    DST_DND_BTN_PRS				0x1165
#define    DST_DND_BTN_RLS				0x1166
#define    DST_ENTER_BTN_PRS				0x1167
#define    DST_ENTER_BTN_RLS				0x1168
#define    DST_CLEAR_BTN_PRS				0x1169
#define    DST_CLEAR_BTN_RLS				0x116A
#define    DST_DESTINATION_BTN_PRS			0x116B
#define    DST_DESTINATION_BTN_RLS			0x116C
#define    DST_DND_LT_OFF				0x116D
#define    DST_DND_LT_ON				0x116E
#define    DST_DND_LT_FLASHING				0x116F
#define    DST_DND_LT_FASTFLASHING			0x1170
#define    DST_DND_LT_VERY_FASTFLASHING			0x1171
#define    DST_DND_LT_QUICKFLASH			0x1172
#define    DST_DND_LT_WINK				0x1173
#define    DST_DND_LT_SLOW_WINK				0x1174
#define    DST_DND_LT_MEDIUM_WINK			0x1175
#define    DST_GROUP_LT_OFF				0x1176
#define    DST_GROUP_LT_ON				0x1177
#define    DST_GROUP_LT_FLASHING			0x1178
#define    DST_GROUP_LT_FASTFLASHING			0x1179
#define    DST_GROUP_LT_VERY_FASTFLASHING		0x117A
#define    DST_GROUP_LT_QUICKFLASH			0x117B
#define    DST_GROUP_LT_WINK				0x117C
#define    DST_GROUP_LT_SLOW_WINK			0x117D
#define    DST_GROUP_LT_MEDIUM_WINK			0x117E
#define    DST_DIVERT_LT_OFF				0x117F
#define    DST_DIVERT_LT_ON				0x1180
#define    DST_DIVERT_LT_FLASHING			0x1181
#define    DST_DIVERT_LT_FASTFLASHING			0x1182
#define    DST_DIVERT_LT_VERY_FASTFLASHING		0x1183
#define    DST_DIVERT_LT_QUICKFLASH			0x1184
#define    DST_DIVERT_LT_WINK				0x1185 
#define    DST_DIVERT_LT_SLOW_WINK			0x1186
#define    DST_DIVERT_LT_MEDIUM_WINK			0x1187
#define    DST_SCROLL_LT_OFF				0x1188
#define    DST_SCROLL_LT_ON				0x1189
#define    DST_SCROLL_LT_FLASHING			0x118A
#define    DST_SCROLL_LT_FASTFLASHING			0x118B
#define    DST_SCROLL_LT_VERY_FASTFLASHING		0x118C
#define    DST_SCROLL_LT_QUICKFLASH			0x118D
#define    DST_SCROLL_LT_WINK				0x118E
#define    DST_SCROLL_LT_SLOW_WINK			0x118F
#define    DST_SCROLL_LT_MEDIUM_WINK			0x1190
#define    DST_CALLBACK_BTN_PRS				0x1191
#define    DST_CALLBACK_BTN_RLS				0x1192
#define    DST_FLASH_LT_OFF				0x1193
#define    DST_FLASH_LT_ON				0x1194
#define    DST_FLASH_LT_FLASHING			0x1195
#define    DST_FLASH_LT_FASTFLASHING			0x1196
#define    DST_FLASH_LT_VERY_FASTFLASHING		0x1197
#define    DST_FLASH_LT_QUICKFLASH			0x1198
#define    DST_FLASH_LT_WINK				0x1199
#define    DST_FLASH_LT_SLOW_WINK			0x119A
#define    DST_FLASH_LT_MEDIUM_WINK			0x119B
#define    DST_MODE_BTN_PRS				0x119C
#define    DST_MODE_BTN_RLS				0x119D
#define    DST_SPEAKER_LT_MEDIUM_WINK			0x119E
#define    DST_MSG_LT_MEDIUM_WINK			0x119F
#define    DST_SPEEDDIAL_LT_OFF				0x11A0
#define    DST_SPEEDDIAL_LT_ON				0x11A1
#define    DST_SPEEDDIAL_LT_FLASHING			0x11A2
#define    DST_SPEEDDIAL_LT_FASTFLASHING		0x11A3
#define    DST_SPEEDDIAL_LT_VERY_FASTFLASHING		0x11A4
#define    DST_SPEEDDIAL_LT_QUICKFLASH			0x11A5
#define    DST_SPEEDDIAL_LT_WINK			0x11A6
#define    DST_SPEEDDIAL_LT_SLOW_WINK			0x11A7
#define    DST_SPEEDDIAL_LT_MEDIUM_WINK			0x11A8
#define    DST_SELECT_BTN_PRS				0x11A9
#define    DST_SELECT_BTN_RLS				0x11AA
#define    DST_PAUSE_BTN_PRS				0x11AB
#define    DST_PAUSE_BTN_RLS				0x11AC
#define    DST_INTERCOM_BTN_PRS				0x11AD
#define    DST_INTERCOM_BTN_RLS				0x11AE
#define    DST_INTERCOM_LT_OFF				0x11AF
#define    DST_INTERCOM_LT_ON				0x11B0
#define    DST_INTERCOM_LT_FLASHING			0x11B1
#define    DST_INTERCOM_LT_FASTFLASHING			0x11B2
#define    DST_INTERCOM_LT_VERY_FASTFLASHING		0x11B3
#define    DST_INTERCOM_LT_QUICKFLASH			0x11B4
#define    DST_INTERCOM_LT_WINK				0x11B5
#define    DST_INTERCOM_LT_SLOW_WINK			0x11B6
#define    DST_INTERCOM_LT_MEDIUM_WINK			0x11B7
#define    DST_CFWD_LT_OFF				0x11B8
#define    DST_CFWD_LT_ON				0x11B9
#define    DST_CFWD_LT_FLASHING				0x11BA
#define    DST_CFWD_LT_FASTFLASHING			0x11BB
#define    DST_CFWD_LT_VERY_FASTFLASHING		0x11BC
#define    DST_CFWD_LT_QUICKFLASH			0x11BD
#define    DST_CFWD_LT_WINK				0x11BE
#define    DST_CFWD_LT_SLOW_WINK			0x11BF
#define    DST_CFWD_LT_MEDIUM_WINK			0x11C0
#define    DST_CFWD_BTN_PRS				0x11C1
#define    DST_CFWD_BTN_RLS				0x11C2
#define    DST_SPECIAL_LT_OFF				0x11C3
#define    DST_SPECIAL_LT_ON				0x11C4
#define    DST_SPECIAL_LT_FLASHING			0x11C5
#define    DST_SPECIAL_LT_FASTFLASHING			0x11C6
#define    DST_SPECIAL_LT_VERY_FASTFLASHING		0x11C7
#define    DST_SPECIAL_LT_QUICKFLASH			0x11C8
#define    DST_SPECIAL_LT_WINK				0x11C9
#define    DST_SPECIAL_LT_SLOW_WINK			0x11CA
#define    DST_SPECIAL_LT_MEDIUM_WINK			0x11CB
#define    DST_SPECIAL_BTN_PRS				0x11CC
#define    DST_SPECIAL_BTN_RLS				0x11CD
#define    DST_FORWARD_LT_OFF				0x11CE
#define    DST_FORWARD_LT_ON				0x11CF
#define    DST_FORWARD_LT_FLASHING			0x11D0
#define    DST_FORWARD_LT_FASTFLASHING			0x11D1
#define    DST_FORWARD_LT_VERY_FASTFLASHING		0x11D2
#define    DST_FORWARD_LT_QUICKFLASH			0x11D3
#define    DST_FORWARD_LT_WINK				0x11D4
#define    DST_FORWARD_LT_SLOW_WINK			0x11D5
#define    DST_FORWARD_LT_MEDIUM_WINK			0x11D6
#define    DST_FORWARD_BTN_PRS				0x11D7
#define    DST_FORWARD_BTN_RLS				0x11D8
#define    DST_OUTGOING_LT_OFF				0x11D9
#define    DST_OUTGOING_LT_ON				0x11DA
#define    DST_OUTGOING_LT_FLASHING			0x11DB
#define    DST_OUTGOING_LT_FASTFLASHING			0x11DC
#define    DST_OUTGOING_LT_VERY_FASTFLASHING		0x11DD
#define    DST_OUTGOING_LT_QUICKFLASH			0x11DE
#define    DST_OUTGOING_LT_WINK				0x11DF
#define    DST_OUTGOING_LT_SLOW_WINK			0x11E0
#define    DST_OUTGOING_LT_MEDIUM_WINK			0x11E1
#define    DST_OUTGOING_BTN_PRS				0x11E2
#define    DST_OUTGOING_BTN_RLS				0x11E3
#define    DST_BACKSPACE_LT_OFF				0x11E4
#define    DST_BACKSPACE_LT_ON				0x11E5
#define    DST_BACKSPACE_LT_FLASHING			0x11E6
#define    DST_BACKSPACE_LT_FASTFLASHING		0x11E7
#define    DST_BACKSPACE_LT_VERY_FASTFLASHING		0x11E8
#define    DST_BACKSPACE_LT_QUICKFLASH			0x11E9
#define    DST_BACKSPACE_LT_WINK			0x11EA
#define    DST_BACKSPACE_LT_SLOW_WINK			0x11EB
#define    DST_BACKSPACE_LT_MEDIUM_WINK			0x11EC
#define    DST_BACKSPACE_BTN_PRS			0x11ED
#define    DST_BACKSPACE_BTN_RLS			0x11EE
#define    DST_START_TONE				0x11EF
#define    DST_STOP_TONE				0x11F0
#define    DST_FLASHHOOK				0x11F1
#define    DST_LINE_BTN_RLS				0x11F2
#define    DST_FEATURE_BTN_RLS				0x11F3
#define    DST_MUTE_BTN_RLS				0x11F4
#define    DST_HELP_BTN_RLS				0x11F5
#define    DST_LOGON_BTN_RLS				0x11F6
#define    DST_ANSWER_BTN_RLS				0x11F7
#define    DST_PROGRAM_BTN_RLS				0x11F8
#define    DST_CONFERENCE_BTN_RLS			0x11F9
#define    DST_RECALL_BTN_RLS				0x11FA
#define    DST_BREAK_BTN_RLS				0x11FB
#define    DST_WORK_BTN_RLS				0x11FC
#define    DST_TALLY_BTN_RLS				0x11FD
#define    DST_EXPAND_LT_OFF				0x1200
#define    DST_EXPAND_LT_ON				0x1201
#define    DST_EXPAND_LT_FLASHING			0x1202
#define    DST_EXPAND_LT_FASTFLASHING			0x1203
#define    DST_EXPAND_LT_VERY_FASTFLASHING		0x1204
#define    DST_EXPAND_LT_QUICKFLASH			0x1205
#define    DST_EXPAND_LT_WINK				0x1206
#define    DST_EXPAND_LT_SLOW_WINK			0x1207
#define    DST_EXPAND_LT_MEDIUM_WINK			0x1208
#define    DST_EXPAND_BTN_PRS				0x1209
#define    DST_EXPAND_BTN_RLS				0x120A
#define    DST_SERVICES_LT_OFF				0x1210
#define    DST_SERVICES_LT_ON				0x1211
#define    DST_SERVICES_LT_FLASHING			0x1212
#define    DST_SERVICES_LT_FASTFLASHING			0x1213
#define    DST_SERVICES_LT_VERY_FASTFLASHING		0x1214
#define    DST_SERVICES_LT_QUICKFLASH			0x1215
#define    DST_SERVICES_LT_WINK				0x1216
#define    DST_SERVICES_LT_SLOW_WINK			0x1217
#define    DST_SERVICES_LT_MEDIUM_WINK			0x1218
#define    DST_SERVICES_BTN_PRS				0x1219
#define    DST_SERVICES_BTN_RLS				0x121A
#define    DST_HEADSET_LT_OFF				0x1220
#define    DST_HEADSET_LT_ON				0x1221
#define    DST_HEADSET_LT_FLASHING			0x1222
#define    DST_HEADSET_LT_FASTFLASHING			0x1223
#define    DST_HEADSET_LT_VERY_FASTFLASHING		0x1224
#define    DST_HEADSET_LT_QUICKFLASH			0x1225
#define    DST_HEADSET_LT_WINK				0x1226
#define    DST_HEADSET_LT_SLOW_WINK			0x1227
#define    DST_HEADSET_LT_MEDIUM_WINK			0x1228
#define    DST_HEADSET_BTN_PRS				0x1229
#define    DST_HEADSET_BTN_RLS				0x122A
#define    DST_NAVIGATION_BTN_PRS			0x1239
#define    DST_NAVIGATION_BTN_RLS			0x123A
#define    DST_COPY_LT_OFF				0x1240
#define    DST_COPY_LT_ON				0x1241
#define    DST_COPY_LT_FLASHING				0x1242
#define    DST_COPY_LT_FASTFLASHING			0x1243
#define    DST_COPY_LT_VERY_FASTFLASHING		0x1244
#define    DST_COPY_LT_QUICKFLASH			0x1245
#define    DST_COPY_LT_WINK				0x1246
#define    DST_COPY_LT_SLOW_WINK			0x1247
#define    DST_COPY_LT_MEDIUM_WINK			0x1248
#define    DST_COPY_BTN_PRS				0x1249
#define    DST_COPY_BTN_RLS				0x124A
#define    DST_LINE_LT_MEDIUM_WINK			0x1250
#define    DST_MIC_LT_MEDIUM_WINK			0x1251 
#define    DST_HOLD_LT_MEDIUM_WINK			0x1252
#define    DST_RELEASE_LT_MEDIUM_WINK			0x1253
#define    DST_HELP_LT_MEDIUM_WINK			0x1254
#define    DST_SUPERVISOR_LT_MEDIUM_WINK		0x1255
#define    DST_READY_LT_MEDIUM_WINK			0x1256
#define    DST_LOGON_LT_MEDIUM_WINK			0x1257
#define    DST_WRAPUP_LT_MEDIUM_WINK			0x1258
#define    DST_RING_LT_MEDIUM_WINK			0x1259
#define    DST_ANSWER_LT_MEDIUM_WINK			0x125A
#define    DST_PROGRAM_LT_SLOW_WINK			0x125B
#define    DST_TRANSFER_LT_SLOW_WINK			0x125C
#define    DST_CONFERENCE_LT_SLOW_WINK			0x125D
#define    DST_SOFT_LT_MEDIUM_WINK			0x125E 
#define    DST_MENU_LT_MEDIUM_WINK			0x125F
#define    DST_CALLWAITING_LT_MEDIUM_WINK		0x1260 
#define    DST_REDIAL_LT_MEDIUM_WINK			0x1261
#define    DST_PAGE_LT_MEDIUM_WINK			0x1262
#define    DST_FEATURE_LT_MEDIUM_WINK			0x1263
#define    DST_PAGE_LT_WINK				0x1264
#define    DST_PAGE_LT_SLOW_WINK			0x1265
#define	   DST_CALLBACK_LT_ON				0x1267
#define    DST_CALLBACK_LT_FLASHING			0x1268
#define    DST_CALLBACK_LT_WINK				0x1269
#define    DST_CALLBACK_LT_FASTFLASHING			0x126a
#define    DST_ICM_LT_OFF				0x126b
#define    DST_ICM_LT_ON				0x126c
#define    DST_ICM_LT_FLASHING				0x126d
#define    DST_ICM_LT_WINK				0x126e
#define    DST_ICM_LT_FASTFLASHING			0x126f
#define    DST_ICM_BTN_PRS				0x1270
#define    DST_ICM_BTN_RLS				0x1271
#define    DST_CISCO_SCCP_CALL_INFO			0x1280
#define    DST_CALLBACK_LT_OFF	            		0x1266   
#define    DST_CONFERENCE_BTN_PRS           		DST_CONF_BTN_PRS           
#define    DST_FUNC_LT_FASTFLASHING			DST_LT_FASTFLASHING        
#define    DST_FUNC_LT_FLASHING				DST_LT_FLASHING            
#define    DST_FUNC_LT_OFF				DST_LT_OFF                 
#define    DST_FUNC_LT_ON				DST_LT_ON                  
#define    DST_FUNC_LT_QUICKFLASH			DST_LT_QUICKFLASH          
#define    DST_FUNC_LT_VERY_FASTFLASHING		DST_LT_VERY_FASTFLASHING

//-----------------------------------------------------------------------
// ��������ʧ��ԭ��ĳ������壬���ں���SsmGetLastErrCode()����ֵ
//-----------------------------------------------------------------------
enum{
	C_ERROR_INIT_FAILURE      =0,
	C_ERROR_SSMAPI_UNOPENED   =1,
	C_ERROR_INVALID_APPCH     =2,
	C_ERROR_UNSUPPORTED_OP    =3,
	C_ERROR_INDEX_UNOPENED    =4,
	C_ERROR_INVALID_BUSCH     =5,
	C_ERROR_OP_UNOPENED       =6,
	C_ERROR_INVALID_FORMAT    =7,
	C_ERROR_INVALID_PARAMETER =8,
	C_ERROR_FILEOP_FAILURE    =9,
	C_ERROR_MEMORY_FAILURE    =10,
	C_ERROR_RESOURCE_USEUP    =11,
	C_ERROR_SYSTEM            =12,
	C_ERROR_IdleChNotFound    =13,
	C_ERROR_OP_FAILURE		  =14,
	C_ERROR_INVALID_APPSPYCIC =15,
	C_ERROR_FAX_NOFILE		  =16,
};

//-----------------------------------------------------------------------
// �Զ���������ʧ�ܵĳ������壬���ں���SsmGetAutoDialFailureReason()����ֵ
//-----------------------------------------------------------------------
enum{
	ATDL_NULL=0,								// û�к�������
	ATDL_Cancel=1,								// Ӧ�ó�������ȡ��
	ATDL_WaitDialAnsTimeout=2,					// �ȴ�����Ӧ��ʱ
	ATDL_WaitRemotePickupTimeout=3,				// �ȴ�����ժ����ʱ
	ATDL_PcmSyncLos=4,							// PCM��·������ʧ������100ms

	ATDL_Mtp3Unusable=10,						// No.7����������
	ATDL_RcvSSB=11,								// No.7����յ�SSB
	ATDL_RcvSLB=12,								// No.7����յ�SLB
	ATDL_RcvSTB=13,								// No.7����յ�STB
	ATDL_RcvUNN=14,								// No.7����յ�UNN
	ATDL_RcvSEC=15,								// No.7����յ�SEC
	ATDL_RcvCGC=16,								// No.7����յ�CGC
	ATDL_RcvNNC=17,								// No.7����յ�NNC
	ATDL_RcvCFL=18,								// No.7����յ�CFL
	ATDL_RcvLOS=19,								// No.7����յ�LOS
	ATDL_RcvSST=20,								// No.7����յ�SST
	ATDL_RcvACB=21,								// No.7����յ�ACB
	ATDL_RcvDPN=22,								// No.7����յ�DPN
	ATDL_RcvEUM=23,								// No.7����յ�EUM
	ATDL_RcvADI=24,								// No.7����յ�ADI
	ATDL_RcvBLO=25,								// No.7����յ�BLO
	ATDL_DoubleOccupy=26,						// No.7������ͬ��
	ATDL_CircuitReset=27,						// No.7����յ���·/Ⱥ��ԭ�ź�
	ATDL_BlockedByRemote=28,					// No.7�����·���Զ˽���������

	ATDL_SS1WaitOccupyAckTimeout=40,			// No.1����ȴ�ռ��Ӧ���źų�ʱ
	ATDL_SS1RcvCAS_HANGUP=41,					// No.1����յ���������ź�
	ATDL_SS1RcvA4=42,							// No.1����յ�A4�źţ�����ӵ����
	ATDL_SS1RcvA5=43,							// No.1����յ�A5�źţ��պţ�
	ATDL_SS1RcvUndefinedAx=44,					// No.1����յ�δ����ĺ���A���ź�
	ATDL_SS1RcvUndefinedAxOnTxCallerId=45,		// No.1���������ʱ�յ�δ����ĺ���A���ź�
	ATDL_SS1WaitAxTimeout=46,					// No.1����Ⱥ���պ���A���źų�ʱ
	ATDL_SS1WaitAxStopTimeout=47,				// No.1����Ⱥ����A���ź�ͣ����ʱ
	ATDL_SS1WaitAxTimeoutOnTxCallerId=48,		// No.1���������ʱ�Ⱥ����A���źų�ʱ
	ATDL_SS1WaitAxStopTimeoutOnTxCallerId=49,	// No.1���������ʱ�Ⱥ����A���ź�ͣ����ʱ
	ATDL_SS1RcvKB2=50,							// No.1����յ�KB2�ź�(�û�����æ��)
	ATDL_SS1RcvKB3=51,							// No.1����յ�KB3�ź�(�û�����æ��)
	ATDL_SS1RcvKB4=52,							// No.1����յ�KB4�źţ�����ӵ����
	ATDL_SS1RcvKB5=53,							// No.1����յ�KB5�źţ��պţ�
	ATDL_SS1RcvUndefinedKB=54,					// No.1����յ�δ�����KB�ź�
	ATDL_SS1WaitKBTimeout=55,					// No.1������պ���KB�źų�ʱ
	ATDL_SS1WaitKBStopTimeout=56,				// No.1����Ⱥ򱻽з�ͣ������KB�źų�ʱ

	ATDL_ISDNNETISBUS	= 60,					//ISDN������æ
	ATDL_ISDNEMPTYNO	= 61,					//ISDN:�������ǿպ�.

	ATDL_IllegalMessage = 65,       //SS7:�Ƿ���Ϣ
	ATDL_RcvREL         = 66,       //ISUP:�յ��ͷ���Ϣ
	ATDL_RcvCBK         = 67,       //TUP: Rcv CBK Dial Failure
	ATDL_IPInvalidPhonum = 68,			//IP: ����������Ч
	ATDL_IPRemoteBusy	 = 69,				//IP: �Զ�æ
	ATDL_IPBeenRefused	 = 70,			//IP: ���ܾ�
	ATDL_IPDnsFail		 = 71,				//IP: DNS��Ч
	ATDL_IPCodecUnSupport = 72,			//IP: ��֧�ֵ�CODEC����
	ATDL_IPOutOfResources = 73,			//IP: û�п��õ���Դ
	ATDL_IPLocalNetworkErr = 74,		//IP: ����������ִ���
	ATDL_IPRemoteNetworkErr = 75		//IP: Զ��������ִ���
};



//-----------------------------------------------------------------------
// �Զ���������ִ������ĳ������壬����SsmChkAutoDial()�ķ���ֵ
//-----------------------------------------------------------------------
enum{
	DIAL_STANDBY        = 0,	// û���Զ���������
	DIAL_DIALING        = 1,	// �����Զ�����
	DIAL_ECHOTONE       = 2,	// �����걻�к�����⵽�˻�����
	DIAL_NO_DIALTONE    = 3,	// û�в��������Զ�����ʧ�ܡ�
	DIAL_BUSYTONE       = 4,	// �����û�æ���Զ����Ž���
	DIAL_ECHO_NOVOICE   = 5,	// ģ������ͨ�����Ž������յ�������������������Զ����Ž���
	DIAL_NOVOICE        = 6,	// ģ������ͨ�����Ž�������ָ��ʱ����û�м�⵽�κ��������Զ����Ž���
	DIAL_VOICE          = 7,	// �����û�ժ�����Զ����Ž���
	DIAL_VOICEF1        = 8,	// �����û�ժ�����յ�Ƶ��F1��������ģ������ͨ�������Զ����Ž���
	DIAL_VOICEF2        = 9,	// �����û�ժ�����յ�Ƶ��F2��������ģ������ͨ�������Զ����Ž���
	DIAL_NOANSWER       = 10,	// ���˽������Զ�����ʧ��
	DIAL_FAILURE        = 11,	// �Զ�����ʧ��
	DIAL_INVALID_PHONUM = 12,	// �պţ��Զ����Ž���
};

//-----------------------------------------------------------------------
// ͨ������ԭ�������壬����SsmGetPendingReason()�ķ���ֵ
//-----------------------------------------------------------------------
enum{
	ANALOGOUT_NO_DIALTONE=0,				//ģ��ͨ�����Զ�����ʱû�м�⵽���������Զ�����ʧ�ܡ�
	ANALOGOUT_BUSYTONE=1,					//ģ��ͨ�����Զ����Ž������⵽���������Զ�����ʧ�ܡ�
	ANALOGOUT_ECHO_NOVOICE=2,				//ģ��ͨ�����Զ����Ž����󲢼�⵽���������ޡ�
	ANALOGOUT_NOANSWER=3,					//ģ��ͨ�����Զ����Ž������⵽�����źŵ���ָ����ʱ����û��Ӧ��
	ANALOGOUT_TALKING_REMOTE_HANGUPED=4,	//ģ��ͨ������"ͨ��"״̬ʱ���Է��һ�
	ANALOGOUT_NOVOICE=5,					//ģ��ͨ�����Զ����Ž������⵽��·�ϳ�������

	PEND_WaitBckStpMsg=10,					//�����м�ͨ�����ȴ�Ӧ�ó������ñ����û�״̬

	SS1IN_BWD_KB5=11,						//No.1����ͨ�����ȴ����в���
	PEND_RemoteHangupOnTalking=12,			//�����м�ͨ�����Զ��������ͨ�����⵽�����û��ȹһ�

	PEND_AutoDialFailed=13,					//�����м�ͨ�����Զ�����ʧ��
	PEND_SsxUnusable=14,					//�����м�ͨ�����������
	PEND_CircuitReset=15,					//�����м�ͨ������·��ԭ
	PEND_PcmSyncLos=16,						//�����м�ͨ����������ͬ����ʧʱ�䳬��100ms

	SS1OUT_TALKING_REMOTE_HANGUPED=20,		//�����м�ͨ�����Զ���������ͨ�����⵽�����û��ȹһ�
	PEND_CalleeHangupOnTalking=20,			//�����м�ͨ�����Զ���������ͨ�����⵽�����û��ȹһ�

	SS1OUT_NOANSWER=21,						//No.1����ͨ�����ȴ������û�ժ����ʱ
	SS1OUT_NOBWDACK=22,						//No.1����ͨ�����ȴ�ռ��Ӧ���źų�ʱ
	SS1OUT_DIALING_BWD_HANGUP=23,			//No.1����ͨ�����յ���������ź�
	SS1OUT_BWD_A5=24,						//No.1����ͨ�����յ�A=5���պţ��ź�
	SS1OUT_BWD_KB5=25,						//No.1����ͨ�����յ�KB=5���պţ��ź�
	SS1OUT_BWD_KB2=26,						//No.1����ͨ�����û�����æ��
	SS1OUT_BWD_KB3=27,						//No.1����ͨ�����û�����æ��
	SS1OUT_BWD_A4=28,						//No.1����ͨ��������ӵ��
	SS1OUT_BWD_KB4=29,						//No.1����ͨ�����յ�KB=4������ӵ�����ź�
	SS1OUT_TIMEOUT_BWD_A=30,				//No.1����ͨ�����Ⱥ���պ���A���źų�ʱ
	SS1OUT_TIMEOUT_BWD_A_STOP=31,			//No.1����ͨ�����Ⱥ����A���ź�ͣ����ʱ
	SS1OUT_TIMEOUT_BWD_KB=32,				//No.1����ͨ�������պ���KB�źų�ʱ
	SS1OUT_TIMEOUT_BWD_KB_STOP=33,			//No.1����ͨ�����Ⱥ򱻽з�ͣ������KB�źų�ʱ
	SS1OUT_TIMEOUT_CALLERID_BWD_A1=34,		//No.1����ͨ�����յ�δ����ĺ���A���ź�
	SS1OUT_TIMEOUT_CALLERID_BWD_A1_STOP=35,	//No.1����ͨ��������CALLERIDʱ�Ⱥ����A���ź�ͣ����ʱ
	SS1OUT_UNDEFINED_CALLERID_BWD_A=36,		//No.1����ͨ�����������к���ʱ�յ�δ����ĺ���A���ź�
	SS1OUT_UNDEFINED_BWD_A=37,				//No.1����ͨ�����յ�δ����ĺ���A���ź�
	SS1OUT_UNDEFINED_BWD_KB=38,				//No.1����ͨ�����յ�δ�����KB�ź�
	
	ISDN_CALLOVER		= 41,				//���н���,�Է��ȹһ�.
	ISDN_WAIT_RELEASE	= 42,				//�ȴ��ͷ�
	ISDN_HANGING		= 43,				//������
	ISDN_RELEASING		= 44,				//�����ͷ�

	ISDN_UNALLOCATED_NUMBER = 45,			//ISDN��δ����ĺ���
	ISDN_NETWORK_BUSY		= 46,			//ISDN, ����æ��
	ISDN_CIRCUIT_NOT_AVAILABLE= 47,			//ISDN, ָ���ĵ�·�����á�
	PEND_CalleeHangupOnWaitRemotePickUp=48,	//�����м�ͨ�����Զ�������ȴ����йһ�ʱ��⵽�����û��ȹһ�

	ISUP_HardCircuitBlock    = 49,
	ISUP_RemoteSuspend       = 50,

	PEND_RcvHGBOrSGB		 = 51,			//���յ��Զ˽�����SGB/HGB������������

	ISDN_NO_ANSWER			=52,			//ISDN, ��Ӧ��
	ISDN_CALL_REJ			=53,			//ISDN, ���оܾ�
};

//-----------------------------------------------------------------------
// ͨ��״̬�������壬����SsmGetChState()�ķ���ֵ
//-----------------------------------------------------------------------
enum{
	S_CALL_STANDBY =0,					//�����С�״̬
	S_CALL_PICKUPED=1,					//��ժ����״̬
	S_CALL_RINGING =2,					//�����塱״̬
	S_CALL_TALKING =3,					//��ͨ����״̬

	S_CALL_ANALOG_WAITDIALTONE  =4,		// ���ȴ���������״̬  ��ģ��ͨ����
	S_CALL_ANALOG_TXPHONUM      =5,		// �����š�״̬        ��ģ��ͨ����
	S_CALL_ANALOG_WAITDIALRESULT=6,		// ���ȴ����Ž����״̬��ģ��ͨ����

	S_CALL_PENDING            =7,		// ������״̬
	S_CALL_OFFLINE            =8,		// �����ߡ�״̬        ��¼��ͨ����
	S_CALL_WAIT_REMOTE_PICKUP =9,		// ���ȴ�����ժ����״̬
	S_CALL_ANALOG_CLEAR      =10,		//  **�ڲ�״̬**       ��ģ��ͨ����
	S_CALL_UNAVAILABLE       =11,		// ��ͨ�������á�״̬
	S_CALL_LOCKED            =12,		// ������������״̬

	S_CALL_RemoteBlock	     =19,		// ���Զ˱�����״̬
	S_CALL_LocalBlock        =20,		// �����˱�����״̬

	S_CALL_Ss1InWaitPhoNum  =30,		// ���ȴ����ձ��к��롱״̬			��No.1������룩
	S_CALL_Ss1InWaitFwdStop =31,		// ���ȴ�ǰ��ͣ����״̬				��No.1������룩
	S_CALL_Ss1InWaitCallerID=32,		// ���ȴ�����CALLERID���롱״̬		��No.1������룩
	S_CALL_Ss1InWaitKD      =33,		// ���ȴ�����KD�źš�״̬			��No.1������룩
	S_CALL_Ss1InWaitKDStop  =34,		// ���ȴ�KD�ź�ͣ����״̬			��No.1������룩
	S_CALL_SS1_SAYIDLE      =35,		// ������ʾ�����״̬				��No.1���
	S_CALL_SS1WaitIdleCAS   =36,		// ���ȴ��Զ�ʾ�С�״̬				��No.1���
	S_CALL_SS1PhoNumHoldup  =37,		// ������������ء�״̬				��No.1���
	S_CALL_Ss1InWaitStopSendA3p=38,		// ���ȴ�ͣ��A3p�źš�״̬			��No.1���


	S_CALL_Ss1OutWaitBwdAck       =40,	// ���ȴ�����ռ��֤ʵ���״̬		��No.1���������
	S_CALL_Ss1OutTxPhoNum         =41,	// �����ͱ��к��롱״̬				��No.1���������
	S_CALL_Ss1OutWaitAppendPhoNum =42,	// ���ȴ�Ӧ�ó���׷�ӵ绰���롱״̬	��No.1���������
	S_CALL_Ss1OutTxCallerID       =43,	// ���������к��롱״̬				��No.1���������
	S_CALL_Ss1OutWaitKB           =44,	// ���ȴ�����KB�źš�״̬			��No.1���������
	S_CALL_Ss1OutDetectA3p        =45,	// �����A3p�ź�(����)��״̬		��No.1���������


	S_FAX_Wait    = S_CALL_STANDBY,		// �����С�״̬								������ͨ����
	S_FAX_ROUND   = 50,					// ��״̬ת�ƹ����С�״̬					������ͨ����
	S_FAX_PhaseA  = 51,					// ��������н�����״̬						������ͨ����
	S_FAX_PhaseB  = 52,					// �����汨��ǰ����״̬					������ͨ����
	S_FAX_SendDCS = 53,					// �����淢��������շ�����DCS�źš�״̬    ������ͨ����
	S_FAX_Train   = 54,					// �����汨�Ĵ���ǰ����ѵ����״̬			������ͨ����
	S_FAX_PhaseC  = 55,					// �����汨�Ĵ����С�״̬					������ͨ����
	S_FAX_PhaseD  = 56,					// �����汨�ĺ���״̬					������ͨ����
	S_FAX_NextPage= 57,					// �����汨�Ĵ�����һҳ��״̬				������ͨ����
	S_FAX_AllSent = 58,					// �����淢���б��Ĵ��������״̬			������ͨ����
	S_FAX_PhaseE  = 59,					// ����������ͷš�״̬						������ͨ����
	S_FAX_Reset   = 60,					// ����λMODEM��״̬						������ͨ����
	S_FAX_Init    = 61,					// ����ʼ��MODEM��״̬						������ͨ����
	S_FAX_RcvDCS  = 62,					// ����������н��շ�����DCS�źš�״̬		������ͨ����
	S_FAX_SendFTT = 63,					// ������������򷢷�����ѵ��ʧ���ź�FTT��״̬		������ͨ����
	S_FAX_SendCFR = 64,					// ������������򷢷����Ϳɽ��ܵ�֤ʵ�ź�CFR��״̬  ������ͨ����
	S_FAX_SendPPS		= 65,			// �����淢��ECM�������ݺ���PPS��Ϣ��		 ������ͨ����
	S_FAX_RcvPPR		= 66,			// �����淢��PPS�����PPR��״̬				 ������ͨ����
	S_FAX_RepeatECMPage = 67,			// ���������ECM�ط���״̬					 ������ͨ����
	S_FAX_CTC_CTR		= 68,			//	�������յ�4��PPR��ķ�Ӧ��				 ������ͨ����
	S_FAX_SendPPR		= 69,			//  ��������ܵ�ECMҳ����PPR��Ϣ��		 ������ͨ����
	S_FAX_EOR_ERR		= 300,			//	���������Eor����"						������ͨ����
	S_FAX_RNR_RR		= 301,			//  "RNR�ظ�RR��ȷ�ϴ�����շ��Ƿ�׼����"	������ͨ����
	S_FAX_RTN				= 302,			//	������������,RTN���ķ񶨼�����ѵ��
	
	S_TUP_WaitPcmReset    = 70,			// ���ȴ���·Ⱥ��ԭ��״̬		��No.7����TUPЭ�飩
	S_TUP_WaitSAM         = 71,			// ���ȴ�������ַ��Ϣ��״̬		��No.7����TUPЭ�飩
	S_TUP_WaitGSM         = 72,			// ���ȴ�GSM��Ϣ��״̬			��No.7����TUPЭ�飩
	S_TUP_WaitCLF         = 73,			// ���ȴ����в����źš�״̬		��No.7����TUPЭ�飩
	S_TUP_WaitPrefix      = 74,			// ���ȴ���������ֹڡ�״̬		��No.7����TUPЭ�飩
	S_TUP_WaitDialAnswer  = 75,			// ���ȴ����Ž����״̬			��No.7����TUPЭ�飩
	S_TUP_WaitRLG         = 76,			// ���ȴ��ͷż໤�źš�״̬		��No.7����TUPЭ�飩
	S_TUP_WaitSetCallerID = 77,         //  "�ȴ���������"״̬          ��No.7����TUPЭ�飩

	S_ISDN_OUT_WAIT_NET_RESPONSE	= 81,	//ISDN���ȴ�������Ӧ
	S_ISDN_OUT_PLS_APPEND_NO		= 82,	//ISDN����׷�Ӻ���
	S_ISDN_IN_CHK_CALL_IN			= 83,	//ISDN����⵽����
	S_ISDN_IN_RCVING_NO				= 84,	//ISDN�����ڽ��պ���
	S_ISDN_IN_WAIT_TALK				= 85,	//ISDN��׼������ͨ��
	S_ISDN_OUT_WAIT_ALERT			= 86,	//ISDN: �ȴ��Է��������ź�

	S_ISDN_CALL_BEGIN				= 87,	//ISDN������ʱΪ�շ�����У�����ʱΪ�ռ�⵽����
	S_ISDN_WAIT_HUANGUP				= 88,	//ISDN:�ȴ��ͷ����

	S_CALL_SENDRING                 = 100,  //��ʯģ�����ڷ�������

	S_SPY_STANDBY					= S_CALL_STANDBY,	//��أ�����
	S_SPY_RCVPHONUM                 = 105,				//��أ����պ���
	S_SPY_RINGING					= S_CALL_RINGING,	//��أ�����
	S_SPY_TALKING					= S_CALL_TALKING,	//��أ�ͨ��

	S_SPY_SS1RESET					= 110,	//SS1��أ���ԭ
	S_SPY_SS1WAITBWDACK				= 111,	//SS1��أ��ȴ�����֤ʵ
	S_SPY_SS1WAITKB					= 112,	//SS1��أ��ȴ�KB

	S_ISUP_WaitSAM                 = 120,// ISUP:�ȴ���̺���
	S_ISUP_WaitRLC                 = 121,// ISUP:�ȴ��ͷ������Ϣ
	S_ISUP_WaitReset               = 122,// ISUP:��·��ԭ
	S_ISUP_LocallyBlocked          = 123,// ISUP:���ر����������ر���Զ�˺���
	S_ISUP_RemotelyBlocked         = 124,// ISUP:Զ�˱�������Զ�˱������˺���
	S_ISUP_WaitDialAnswer          = 125,// ISUP:�ȴ��������
	S_ISUP_WaitINF                 = 126,// ISUP:�ȴ����к���
	S_ISUP_WaitSetCallerID         = 127,// ISUP:�ȴ���������
	S_DTRC_ACTIVE				   = 128,// DTRC:����ػ�·���ڷǿ���״̬
	
	S_ISUP_Suspend				   = 129,//	ISUP:ͨ�����յ���ͣ
	
	S_CALL_VOIP_DIALING		= 132,					//VoIP���в���״̬
	S_CALL_VOIP_WAIT_CONNECTED	= 133,		//VoIP����ժ���ȴ�����ͨ��״̬
	S_CALL_VOIP_CHANNEL_UNUSABLE	= 134,		//VoIPͨ��Ŀǰ������
	S_CALL_DISCONECT = 135				//USB����
};

//�¼�����
enum{
	E_PROC_Recognize			= 0x0000,
	E_CHG_ISDNStatus			= 0x0001,
	E_RCV_Ss7Msu				= 0x0002,
	E_CHG_Mtp3State				= 0x0003,
	E_CHG_FaxChState			= 0x0004,
	E_CHG_FaxPages				= 0x0005,
	E_PROC_FaxEnd				= 0x0006,
	E_CHG_PcmLinkStatus			= 0x0007,
	E_CHG_LineVoltage			= 0x0008,
	E_RCV_CAS					= 0x0009,
	E_RCV_R2					= 0x000a,
	E_PROC_WaitDTMF				= 0x000b,
	E_CHG_RcvDTMF				= 0x000c,
	E_PROC_SendDTMF				= 0x000d,
	E_PROC_SendFlash			= 0x000e,
	E_PROC_PlayEnd				= 0x000f,
	E_PROC_PlayFile				= 0x0010,
	E_PROC_PlayFileList			= 0x0011,
	E_PROC_PlayMem				= 0x0012,
	E_PROC_RecordEnd			= 0x0013,
	E_PROC_RecordFile			= 0x0014,
	E_PROC_RecordMem			= 0x0015,
	E_PROC_SendFSK				= 0x0016,
	E_PROC_RcvFSK				= 0x0017,
	E_CHG_ChState				= 0x0018,
	E_PROC_AutoDial				= 0x0019,
	E_CHG_RemoteChBlock			= 0x001a,
	E_CHG_RemotePCMBlock		= 0x001b,
	E_SYS_ActualPickup			= 0x001c,
	E_CHG_RingFlag				= 0x001d,
	E_CHG_RingCount				= 0x001e,
	E_CHG_CIDExBuf				= 0x001f,
	E_CHG_RxPhoNumBuf			= 0x0020,
	E_CHG_PolarRvrsCount		= 0x0021,
	E_SYS_RemotePickup			= 0x0022,
	E_CHG_FlashCount			= 0x0023,
	E_CHG_HookState				= 0x0024,
	E_CHG_ToneAnalyze			= 0x0025,
	E_OverallEnergy				= 0x0026,
	E_CHG_OvrlEnrgLevel			= 0x0027,
	E_CHG_BusyTone				= 0x0028,
	E_CHG_BusyToneEx			= 0x0029,
	E_CHG_VocFxFlag				= 0x002a,
	E_CHG_ToneValue				= 0x002b,
	E_CHG_RingEchoToneTime		= 0x002c,
	E_CHG_PeakFrq				= 0x002d,
	E_SYS_BargeIn				= 0x002e,
	E_SYS_NoSound				= 0x002f,
	E_SYS_TIMEOUT				= 0x0030,
	E_CHG_SpyState				= 0x0031,
	E_CHG_SpyLinkStatus			= 0x0032,
	E_RCV_DTR_AUDIO				= 0x0033,
	E_RCV_DTR_HOOK				= 0x0034,
	E_RCV_DTR_LAMP				= 0x0035,
	E_RCV_DTR_FKEY				= 0x0036,
	E_RCV_DTR_DKEY				= 0x0037,
	E_RCV_DTR_VOX				= 0x0038,
	E_RCV_DTR_DISPLAY			= 0x0039,
	E_RCV_DTR_DIRECTION			= 0x003a,
	E_RCV_DTR_RING				= 0x003b,
	E_CHG_CICRxPhoNumBuf		= 0x003c,
	E_CHG_CICState				= 0x003d,
	E_PROC_CICAutoDial			= 0x003e,
	E_RCV_Ss7IsupUtuinf			= 0x003f,
	E_CHG_Mtp2Status			= 0x0040,
	E_RCV_DSTDChannel			= 0x0041,

	E_RCV_Ss7SpyMsu				= 0X0042,
	E_CHG_ToneDetector			= 0x0043,
	E_CHG_ToneDetectorItem		= 0x0044,	
	E_RCV_CALLERID			= 0x0045,
	E_PROC_FaxDcnTag		  = 0x0046,
	MAX_EVENT_SIZE
};

typedef struct  _MESSAGE_INFO
{
	WORD	wEventCode;
	int		nReference;
	DWORD	dwParam; //�������
}MESSAGE_INFO, *PMESSAGE_INFO;

typedef struct _EVENT_SET_INFO
{
	DWORD dwWorkMode;
	LPVOID lpHandlerParam;
	DWORD dwOutCondition;
	DWORD dwOutParamVal;
	DWORD dwUser;
}EVENT_SET_INFO,*PEVENT_SET_INFO;

typedef struct tagFAX_FILE_SCT
{
	char	szFileName[256];
	int	nStartPage;
	int	nEndPage;
	int	nReserve1;
	int	nReserve2;
}FAX_FILE_SCT, *PFAX_FILE_SCT;

typedef struct _SSM_EVENT{
	WORD			wEventCode;
	int				nReference;
	DWORD			dwParam;
	DWORD			dwUser;
	DWORD			dwSubReason;			//Event sub-reason
	DWORD			dwXtraInfo;				//Extra information
	PVOID           pvBuffer;				//Pointer to buffer
    DWORD           dwBufferLength;			//Length of buffer pointed by pvBuffer
    DWORD           dwDataLength;			//Length of data in buffer pointed by pvBuffer
    DWORD           dwEventFlag;            //Falgs of the following:
                                            //bit 0,	=1 - App created the event
                                            //			=0 - SHP_A3.DLL created the event
                                            //bit 1,	Reserved
											//bit 2,	=1 - data has been truncated
											//			=0 - data has not been truncated
	DWORD           dwReserved1;
	LONGLONG		llReserved2;
}SSM_EVENT, *PSSM_EVENT;

typedef struct _VERSION
{
    UCHAR   ucMajor;
    UCHAR   ucMinor;
    USHORT  usInternal;
    USHORT  usBuild;
    UCHAR   ucRelease;
    UCHAR   ucFeature;
}SSM_VERSION, *PSSM_VERSION;

enum
{
TIMER_ONE,
TIMER_PERIODIC 
};

enum
{
	NO_EVENT, // ���¼���ʽ
	EVENT_POLLING, //�¼���ѯ
	EVENT_CALLBACK, //�¼��ص�
	EVENT_MESSAGE //windows��Ϣ
};

//�ļ���������ָʾ�����������.
enum
{
	PLAYPERCENT,
	PLAYTIME,
	DATABYTESPLAYED,
	DATABYTESTOPLAY
};

//�ڴ�¼��������ָʾ�����������.
enum
{
	END_HALF_BUFFER,
	END_BUFFER,	
	MEM_OFFSET,
	MEM_BYTES,
};

//�ļ�¼������ָʾ�����������.
enum
{
	RECORD_TIME,
	RECORD_BYTES
};

//ISUP�û����ֲ����������壬����SsmSetIsupFlag��nType����
enum
{
	ISUP_CallerParam = 1,//���к������
	ISUP_PhoNumParam = 2,//���к������
	ISUP_PhoNumREL = 3,//������ķ���Ϣ��REL

	ISUP_REL_DENY_SetToOther = 100,//���б�������Ϊ�������
	DefaultIAM_OriginalCalleeParam = 101,
};

enum
{
	Tup_ANX = 1,//���У�C_TUP_ANU, C_TUP_ANC, C_TUP_ANN
};

#pragma pack(1)
typedef struct tag_ISUP_RIREL{
	WORD	wRIMsg;
	WORD	wRIPhoNumPara;
	WORD	wPhoNumLen;
	UCHAR	ucRIPhoNum[20];
}ISUP_RIREL,*PISUP_RIREL;
#pragma pack()

#define TYPE_MESSAGE_CONTACT	0x0001			// ��ȡContact�ֶ�����

#endif	// end of #ifndef SHCTI_CONST

#ifndef SHCTI_CALLBACK
	#define SHCTI_CALLBACK

	typedef	void (WINAPI *RXDTMFHANDLER) (int ch, char cDtmf, int nDTStatus, PVOID pV);
	typedef	BOOL (WINAPI *RECORDMEMBLOCKHANDLER) (int ch, int nEndReason, PUCHAR pucBuf, DWORD dwStopOffset, PVOID pV);
	typedef	BOOL (WINAPI *PLAYMEMBLOCKHANDLER) (int ch, int nEndReason, PUCHAR pucBuf, DWORD dwStopOffset, PVOID pV);
	typedef	void (CALLBACK  *LPRECTOMEM) (int ch, LPBYTE lpData, DWORD dwDataLen); //appended by xxb,2002.12.02
	typedef	void (CALLBACK  *LPRECTOMEMB) (int ch, LPBYTE lpData, DWORD dwDataLen, PVOID pV);
	typedef	int  (CALLBACK  *LPFNEVENTCALLBACK) (WORD wEvent, int nReference, DWORD dwParam, DWORD	dwUser);
	typedef	int  (CALLBACK  *LPFNEVENTCALLBACKA) (PSSM_EVENT pEvent);
#endif

#ifdef __cplusplus
	extern "C" {
#endif


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@ INITIALIZATION OPERATION  @@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int WINAPI SsmStartCti(LPSTR lpSsmCfgFileName, LPSTR lpIndexCfgFileName);
int WINAPI SsmCloseCti();
void WINAPI SsmGetLastErrMsg(LPSTR szErrMsgBuf);
char *WINAPI SsmGetLastErrMsgA(void);
int WINAPI SsmGetLastErrCode();
int WINAPI SsmGetMaxUsableBoard();
int WINAPI SsmGetMaxCfgBoard();
int WINAPI SsmGetRxDtmfBufSize();
int WINAPI SsmGetAccreditId(int nBId);
int WINAPI SsmGetMaxCh(void);
int WINAPI SsmGetChType(int ch);
int WINAPI SsmSetMaxIdxSeg(WORD wMaxIdxSeg);
int WINAPI SsmLoadIndexData(int	nSegNo, LPSTR pAlias, int nCodec, LPSTR	pVocFile, long lStartPos, long lLen);
int WINAPI SsmFreeIndexData(int nSegNo);
int WINAPI SsmGetTotalIndexSeg();
int WINAPI SsmPauseCard();
int WINAPI SsmRestartCard();

int WINAPI SsmGetAccreditIdEx(int nBId);	
int WINAPI SsmGetBoardModel(int nBId);		
void WINAPI SsmSetDV(BOOL bEnable);			
BOOL WINAPI SsmGetDV(void);

DWORD WINAPI SsmGetPciSerialNo(int nBId);

DWORD WINAPI SsmGetIntCount();
int WINAPI SsmSetListenMode(int nMode);

int WINAPI SsmStartBoard(int nBId);
int WINAPI SsmStopBoard(int nBId);
int WINAPI SsmGetChHdInfo(int ch, int * pnBId, int * pnBCh);
int WINAPI SsmGetAppChId(int * AppchId, int BrdId, int BrdChId);

int WINAPI SsmSetChState(int ch, int nState);
int	WINAPI StartTimer(int ch, WORD ClockType);
DWORD WINAPI ElapseTime(int ch, WORD ClockType);
int WINAPI SsmSetLogOutput(int nAPIDbg, int nEventStart, int nEventEnd, int nChStart, int nChEnd, DWORD dwReserve);

int WINAPI SsmLoadChIndexData(int ch, int nSegNo, LPSTR pAlias, int nCodec, LPSTR	pVocFile, long lStartPos, long lLen);
int WINAPI SsmFreeChIndexData(int ch, int nSegNo);
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ PLAY OPERATION @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int WINAPI SsmSetPlayVolume(int ch, int nVolume);
int WINAPI SsmSetDtmfStopPlay(int ch, BOOL bDspf);
int WINAPI SsmGetDtmfStopPlayFlag(int ch);
int WINAPI SsmSetBargeinStopPlay(int ch, BOOL bBispf);
int WINAPI SsmGetBargeinStopPlayFlag(int ch);
int	WINAPI SsmStopPlay(int ch);
int WINAPI SsmPlayFile(int ch, LPSTR pszFileName, int nFormat, DWORD dwStartPos, DWORD dwLen);
int WINAPI SsmStopPlayFile(int ch);
int WINAPI SsmPausePlay(int ch);
int WINAPI SsmRestartPlay(int ch);
int WINAPI SsmFastFwdPlay(int ch);
int WINAPI SsmFastBwdPlay(int ch);
int WINAPI SsmSetPlayTime(int ch, DWORD dwTime);
long WINAPI SsmGetPlayedTimeEx(int ch);
int  WINAPI SsmGetPlayingFileInfo(int ch , int *pnFormat , long *pnTotalTime);
int WINAPI SsmSetPlayPrct(int ch, DWORD dwPercentage);
long WINAPI SsmGetPlayedTime(int ch);
int WINAPI SsmGetPlayedPercentage(int ch);
long WINAPI SsmGetDataBytesToPlay(int ch);
int WINAPI SsmCheckPlay(int ch);
int WINAPI SsmGetPlayType(int ch);
int WINAPI SsmPlayIndexString(int ch, LPSTR pszIdxStr);
int WINAPI SsmPlayIndexList(int ch, WORD wIdxListLen, PWORD pwIdxList);
int WINAPI SsmStopPlayIndex(int ch);
int WINAPI SsmClearFileList(int ch);
int WINAPI SsmAddToFileList(int ch, LPSTR pszFileName, int nFormat, DWORD dwStartPos, DWORD dwLen);
int WINAPI SsmPlayFileList(int ch);
int WINAPI SsmStopPlayFileList(int ch);
int WINAPI SsmPlayMem(int ch, int nFormat, LPBYTE pBuf, DWORD dwBufSize, DWORD dwStartOffset, DWORD dwStopOffset);
int WINAPI SsmGetPlayOffset(int ch, DWORD* pdwPlayOffset);
int WINAPI SsmSetStopPlayOffset(int ch, DWORD dwStopPlayOffset);
int WINAPI SsmStopPlayMem(int ch);
int WINAPI SsmClearPlayMemList(void);
int WINAPI SsmAddToPlayMemList(LPBYTE pBuf, DWORD dwDataLen, int nFormat);
int WINAPI SsmPlayMemList(int ch, PWORD pMemList, WORD wMemListLen);
int WINAPI SsmStopPlayMemList(int ch);

int WINAPI SsmSetDTMFStopPlayCharSet(int ch, LPSTR lpstrDtmfCharSet);
int WINAPI SsmGetDTMFStopPlayCharSet(int ch, LPSTR lpstrDtmfCharSet);
int WINAPI SsmSetHangupStopPlayFlag(int ch, BOOL bHangupStopRecFlag);
int WINAPI SsmSetHangupStopPlayFlag(int ch, BOOL bHangupStopPlayFlag);
int WINAPI SsmPlayMemBlock(int ch, int nFormat, LPBYTE pBuf, DWORD dwBufSize, PLAYMEMBLOCKHANDLER OnPlayMemBlockDone, PVOID pV);

int WINAPI SsmStopPlayMemBlock(int ch);
long WINAPI SsmGetDataBytesPlayed(int ch);
int	WINAPI SsmSetPlayGain(int ch, WORD wGainLevel);
int WINAPI SsmSetLine0OutTo(BOOL bEnable);

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@    RECORD OPERATION   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int WINAPI SsmSetRecVolume(int ch, int nVolume);
int WINAPI SsmGetRecType(int ch);
int WINAPI SsmRecToFile(int ch, LPSTR pszFileName, int nFormat, DWORD dwStartPos, DWORD dwBytes, DWORD dwTime, int nMask);
int WINAPI SsmRecToFileA(int ch, LPSTR pszFileName, int nFormat,
						DWORD dwStartPos, DWORD dwBytes, DWORD dwTime, int nMask,
						LPRECTOMEM	pfnRecToMem);

int WINAPI SsmRecToFileB(int ch, LPSTR pszFileName, int nFormat,
						DWORD dwStartPos, DWORD dwBytes, DWORD dwTime, int nMask,
						LPRECTOMEMB	pfnRecToMem, PVOID pV);


int WINAPI SsmRecToFileEx(int ch, LPSTR pszFileName, int nFormat,
						DWORD dwStartPos, DWORD dwBytes, DWORD dwTime, int nMask,
						BOOL bSaveToFileOnBargin, DWORD dwRollbackTime);

int WINAPI SsmPauseRecToFile(int ch);
int WINAPI SsmRestartRecToFile(int ch);
long WINAPI SsmGetRecTime(int ch);
long WINAPI SsmGetDataBytesToRecord(int ch);
int WINAPI SsmStopRecToFile(int ch);
int WINAPI SsmChkRecToFile(int ch);
int WINAPI SsmRecToMem(int ch, int nFormat, LPBYTE pBuf, DWORD dwBufSize, DWORD dwStartOffset);
int WINAPI SsmStopRecToMem(int ch);
long WINAPI SsmGetRecOffset(int ch);

int WINAPI SsmGetRecAGCSwitch(int ch);
int WINAPI SsmSetRecAGC(int ch, BOOL bEnable);

int WINAPI SsmQueryOpRecMixer(int ch);
int WINAPI SsmSetRecMixer(int ch, BOOL bEnRecMixer, int nMixerVolume);
int WINAPI SsmGetRecMixerState(int ch, int* pnEnRecMixer, int* pnMixerVolume);

int WINAPI SsmSetPrerecord(int ch, BOOL bEnable, int nMode, WORD wInsertTime, int nFormat);
int WINAPI SsmSetTruncateTail(int ch, DWORD	dwTime);
long WINAPI SsmGetTruncateTailTime(int ch);
int WINAPI SsmGetPrerecordState(int ch, int* pnMode, PWORD pwInsertTime, int* pnFormat);

int WINAPI SsmSetDTMFStopRecCharSet(int ch, LPSTR lpstrDtmfCharSet);
int WINAPI SsmGetDTMFStopRecCharSet(int ch, LPSTR lpstrDtmfCharSet);
int WINAPI SsmSetHangupStopRecFlag(int ch, BOOL bHangupStopRecFlag );
int WINAPI SsmCheckRecord(int ch);
int WINAPI SsmRecordMemBlock(int ch, int nFormat, LPBYTE pBuf,
					 DWORD dwBufSize, RECORDMEMBLOCKHANDLER OnRecMemBlockDone, PVOID pV);
int WINAPI SsmStopRecordMemBlock(int ch);

int WINAPI SsmSetNoModuleChBusRec(int ch, int bBusRec);

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@   RxDTMF OPERATION   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int WINAPI SsmEnableRxDtmf(int ch, BOOL bRun);
int WINAPI SsmClearRxDtmfBuf(int ch);
int WINAPI SsmGetDtmfStr(int ch, LPSTR pszDtmf);
char* WINAPI SsmGetDtmfStrA(int ch);
int WINAPI SsmGetRxDtmfLen(int ch);
int WINAPI SsmGet1stDtmf(int ch, char* pcDtmf);
int WINAPI SsmGet1stDtmfClr(int ch, char* pcDtmf);
int WINAPI SsmGetLastDtmf(int ch, char* pcDtmf);
int WINAPI SsmSetWaitDtmf(int ch, WORD wTimeOut, WORD wMaxLen, char cEndChar, BOOL bWithEndChar);
int WINAPI SsmChkWaitDtmf(int ch, LPSTR	pszDtmf);

int WINAPI SsmCancelWaitDtmf(int ch);	
int WINAPI SsmSetWaitDtmfEx(int ch, WORD wTimeOut, WORD wMaxLen, char cEndChar, BOOL bWithEndChar);

// ���ý���DTMF�ַ��ص�������
int WINAPI SsmSetRxDtmfHandler(int ch, RXDTMFHANDLER OnRcvDtmf, PVOID pV);
int WINAPI SsmSetWaitDtmfExA(int ch, WORD wTimeOut, WORD wMaxLen, char* szEndChar, BOOL bWithEndChar);

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@ TXDTMF OPERATION  @@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int WINAPI SsmQueryTxDtmf(int ch, LPSTR pszDtmf);
int WINAPI SsmQueryTxFlash(int ch, LPSTR pszDtmf);
int WINAPI SsmSetTxDtmfPara(int ch, WORD wOnTime, WORD wOffTime);
int WINAPI SsmGetTxDtmfPara(int ch, PWORD pwOnTime, PWORD pwOffTime);
int WINAPI SsmTxDtmf(int ch, LPSTR pszDtmf);
int WINAPI SsmStopTxDtmf(int ch);
int WINAPI SsmChkTxDtmf(int ch);
int WINAPI SsmTxFlash(int ch, WORD time);
int WINAPI SsmTxFlashEx(int ch, WORD time, int nChState, BOOL bIgnoreDlTn);
int WINAPI SsmChkTxFlash(int ch);
int WINAPI SsmSetTxFlashCharTime(int ch, WORD time);
int WINAPI SsmGetTxFlashCharTime(int ch);

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@ INTER-CH OPERATION  @@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int WINAPI SsmSetLocalFlashTime(int nFlashTime);
int WINAPI SsmSetASDT(int ch, BOOL bEnAutoSendDialTone);
int WINAPI SsmGetASDT(int ch);
int WINAPI SsmSetASTOD(int ch, BOOL bEnAutoSendDialTone);
int WINAPI SsmGetASTOD(int ch);
int WINAPI SsmGetFlashCount(int ch);
int WINAPI SsmClearFlashCount(int ch);
int WINAPI SsmGetHookState(int ch);
int WINAPI SsmStartRing(int ch);
int WINAPI SsmStopRing(int ch);
int WINAPI SsmStartRingWithFskCID(int ch, LPSTR pBuf, DWORD dwMaxBit, DWORD dw1stRingOffDelay);
int WINAPI SsmStartRingWithCIDStr(int ch, LPSTR pBuf, DWORD dwLen, DWORD dw1stRingOffDelay);
int WINAPI SsmCheckSendRing(int ch, int* pnCnt);

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@ SEND TONE OPERATION  @@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int WINAPI SsmQueryOpSendTone(int ch);
int WINAPI SsmSendTone(int ch, int nToneType);
int WINAPI SsmSendToneEx(int ch, DWORD dwOnTime, DWORD dwOffTime);
int WINAPI SsmStopSendTone(int ch);
int WINAPI SsmSetTxTonePara(int ch, int nFreq1, int nVolume1, int nFreq2, int nVolume2);
int WINAPI SsmGetTxTonePara(int ch, int* pnFreq1, int* pnVolume1, int* pnFreq2, int* pnVolume2);
int WINAPI SsmChkSendTone(int ch, int* pnToneType);

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@ TONE ANALYZE OPERATION  @@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int WINAPI SsmQueryOpToneAnalyze(int ch);
int WINAPI SsmClearToneAnalyzeResult(int ch);
int WINAPI SsmStartToneAnalyze(int ch);
int WINAPI SsmCloseToneAnalyze(int ch);
long WINAPI SsmGetOverallEnergy(int ch);
int WINAPI SsmGetVocFxFlag(int ch, int nSelFx, BOOL bClear);
int WINAPI SsmGetToneAnalyzeResult(int ch);
int WINAPI SsmGetBusyToneLen(int ch);
int WINAPI SsmGetBusyToneCount(int ch);
int WINAPI SsmGetRingEchoToneTime(int ch);
int WINAPI SsmGetBusyToneEx(int ch);

int WINAPI SsmSetTonePara(int ch, WORD wToneFreq1,WORD wToneBW1,WORD wToneFreq2,WORD wToneBW2,DWORD dwIsToneRatio);
int WINAPI SsmSetIsDialToneDtrTime(int ch, WORD wIsDialToneDtrTime);
int WINAPI SsmSetBusyTonePeriod(int ch, WORD wBusyTonePeriod);
int WINAPI SsmSetIsBusyToneDtrCnt(int ch, WORD wIsBusyToneDtrCnt);
int WINAPI SsmSetRingEchoTonePara(int ch, WORD wRingEchoOnTime,WORD wRingEchoOffTime);
int WINAPI SsmSetVoiceFxPara(int ch, WORD wSelFx, WORD wFx,WORD wFxBW, DWORD dwIsVocFxRatio, WORD wIsVocFxDtrTime);
int WINAPI SsmSetVoiceOnDetermineTime(int ch, WORD wIsVocDtrTime);
int WINAPI SsmSetMinVocDtrEnergy(int ch, DWORD dwMinVocDtrEnergy);
int WINAPI SsmGetTonePara(int ch, PWORD pwToneFreq1, PWORD pwToneBW1,PWORD pwToneFreq2,PWORD pwToneBW2,PDWORD pdwIsToneRatio);
int WINAPI SsmGetIsDialToneDtrTime(int ch, PWORD pwIsDialToneDtrTime);
int WINAPI SsmGetBusyTonePeriod(int ch, PWORD pwBusyTonePeriod);
int WINAPI SsmGetIsBusyToneDtrCnt(int ch, PWORD pwIsBusyToneDtrCnt);
int WINAPI SsmGetRingEchoTonePara(int ch, PWORD pwRingEchoOnTime, PWORD pwRingEchoOffTime);
int WINAPI SsmGetIsNoVocDtrmTime(int ch, PWORD pwIsNoVocDtrmTime);
int WINAPI SsmGetVoiceFxPara(int ch, WORD wSelFx, PWORD pwFx,PWORD pwFxBW, PDWORD pdwIsVocFxRatio, PWORD pwIsVocFxDtrTime);
int WINAPI SsmGetVoiceOnDetermineTime(int ch, PWORD pwIsVocDtrTime);
int WINAPI SsmGetMinVocDtrEnergy(int ch, PDWORD pdwMinVocDtrEnergy);

int WINAPI SsmQueryOpPeakFrqDetect(int ch);
int WINAPI SsmSetPeakFrqDetectBW(int ch, WORD nPeakBW);
int WINAPI SsmGetPeakFrqDetectBW(int ch);
long WINAPI SsmGetPeakFrqEnergy(int ch);
int WINAPI SsmGetPeakFrq(int ch);
DWORD WINAPI SsmGetRecPlayEnergy(int ch, DWORD dwMask);

int WINAPI SsmStart2ndToneAnalyzer(int ch, BOOL bEn);
int WINAPI SsmGet2ndToneAnalyzerState(int ch);
int WINAPI SsmSet2ndTonePara(int ch, WORD wToneFreq1,WORD wToneBW1,WORD wToneFreq2,WORD wToneBW2,DWORD dwIsToneRatio);
int WINAPI SsmGet2ndTonePara(int ch, PWORD pwToneFreq1, PWORD pwToneBW1,PWORD pwToneFreq2,PWORD pwToneBW2,PDWORD pdwIsToneRatio);
int WINAPI SsmGet2ndToneAnalyzeResult(int ch);
int WINAPI SsmClear2ndToneAnalyzeResult(int ch);
int WINAPI SsmGet2ndBusyToneLen(int ch);
int WINAPI SsmGet2ndBusyToneCount(int ch);
int WINAPI SsmSet2ndBusyTonePeriod(int ch, WORD wBusyTonePeriod);
int WINAPI SsmGet2ndBusyTonePeriod(int ch, PWORD pwBusyTonePeriod);
int WINAPI SsmSet2ndIsBusyToneDtrCnt(int ch, WORD wIsBusyToneDtrCnt);
int WINAPI SsmGet2ndIsBusyToneDtrCnt(int ch, PWORD pwIsBusyToneDtrCnt);
int WINAPI SsmSet2ndIsDialToneDtrTime(int ch, WORD wIsDialToneDtrTime);
int WINAPI SsmGet2ndIsDialToneDtrTime(int ch, PWORD pwIsDialToneDtrTime);
int WINAPI SsmSet2ndRingEchoTonePara(int ch, WORD wRingEchoOnTime,WORD wRingEchoOffTime);
int WINAPI SsmGet2ndRingEchoTonePara(int ch, PWORD pwRingEchoOnTime, PWORD pwRingEchoOffTime);
int WINAPI SsmSetBusyTonePeriodEx(int ch, int nType, WORD wMax, PWORD pwPeriod);
int WINAPI SsmGetBusyTonePeriodEx(int ch, int nType, PWORD pwPeriod);
WORD WINAPI SsmGetToneValue(int ch);
int WINAPI SsmSetToneValue(int ch,WORD value);

int WINAPI SsmGetOverallEnergyAllCh(int nBeginCh, int nChNum, PDWORD pdwEnergyTable);

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@ BARGEIN OPERATION  @@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int WINAPI SsmSetNoSoundDtrmTime(int ch, DWORD dwIsNoSoundDtrTime);
int WINAPI SsmSetIsBargeInDtrmTime(int ch, WORD wIsBargeInDtrmTime);
int WINAPI SsmSetBargeInSens(int ch, int nBargeInSens);
long WINAPI SsmGetNoSoundTime(int ch);
long WINAPI SsmGetNoSoundDtrmTime(int ch);
int WINAPI SsmGetIsBargeInDtrmTime(int ch);
int WINAPI SsmGetBargeInSens(int ch);
int WINAPI SsmDetectBargeIn(int ch);
int WINAPI SsmDetectNoSound(int ch);

//����ģ��绰�������������ժ��������
//����ֵ 0:�����ɹ�, -1:����ʧ��
int WINAPI SsmSetCalleeHookDetectP(int ch, // ͨ����
							 WORD wMulti, //Ԥ������
							 WORD wValidTime); //��Чʱ��
//��ȡģ��绰�������������ժ��������
//����ֵ 0:�����ɹ�, -1:����ʧ��
int WINAPI SsmGetCalleeHookDetectP(int ch, // ͨ����
							 PWORD pwMulti, //Ԥ������
							 PWORD pwValidTime); //��Чʱ��


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@ RING DETECT OPERATION  @@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int WINAPI SsmQueryOpRingDetect(int ch);
int WINAPI SsmGetRingFlag(int ch);
int WINAPI SsmGetRingCount(int ch);
int WINAPI SsmClearRingCount(int ch);
int WINAPI SsmChkOpCallerId(int ch);
int WINAPI SsmGetCallerId(int ch, LPSTR szCallerId);
char* WINAPI SsmGetCallerIdA(int ch);
int WINAPI SsmGetCallerIdEx(int ch, LPSTR szCallerIdEx);
int WINAPI SsmClearCallerId(int ch);
int WINAPI SsmClearCallerIdEx(int ch);
int WINAPI SsmGetCallerName(int ch, LPSTR szCallerName);

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@ CALL OPERATION  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int WINAPI SsmPickup(int ch);
int WINAPI SsmPickupANX(int ch, int nANX);
int WINAPI SsmCheckActualPickup(int ch);
int WINAPI SsmSetPickupNow(int ch, BOOL bFlag);
int WINAPI SsmHangup(int ch);
int WINAPI SsmHangupEx(int ch, UCHAR ucCauseVal);
int WINAPI SsmAutoDial(int ch, LPSTR szPhoNum);
int WINAPI SsmChkAutoDial(int ch);
int WINAPI SsmGetAutoDialFailureReason(int ch);
int WINAPI SsmGetBlockReason(int ch);
int WINAPI SsmGetChState(int ch);

int WINAPI SsmSetPlayDest(int ch, int nSelDest);
int WINAPI SsmSetRecBack(int ch, int nRecBack);

int WINAPI SsmSearchIdleCallOutCh(WORD wSearchMode, DWORD dwPrecedence);
int WINAPI SsmSetAutoCallDirection(int ch, BOOL bEnAutoCall, int nDirection);
int WINAPI SsmGetAutoCallDirection(int ch, int* pnDirection);
int WINAPI SsmAppendPhoNum(int ch, LPSTR szPhoNum);
int WINAPI SsmGetPendingReason(int ch);
long WINAPI SsmGetChStateKeepTime(int ch);
int WINAPI SsmGetPhoNumStr(int ch, LPSTR pszPhoNum);
char* WINAPI SsmGetPhoNumStrA(int ch);
int WINAPI SsmGetPhoNumLen(int ch);

int WINAPI SsmGet1stPhoNumStr(int ch, LPSTR pszPhoNum);
char* WINAPI SsmGet1stPhoNumStrA(int ch);
int WINAPI SsmGet1stPhoNumLen(int ch);

int WINAPI SsmEnableAutoSendKB(int ch, BOOL bEnable);
int WINAPI SsmGetAutoSendKBFlag(int ch);
int WINAPI SsmSetKB(int ch, BYTE btSigKB);
int WINAPI SsmSetKD(int ch, BYTE btSigKD);
int WINAPI SsmSetKA(int ch, BYTE btSigKA);
int WINAPI SsmSetTxCallerId(int ch, LPSTR pszTxCallerId);
int WINAPI SsmGetTxCallerId(int ch, LPSTR pszTxCallerId);
int WINAPI SsmGetKA(int ch);
int WINAPI SsmGetKB(int ch);
int WINAPI SsmGetKD(int ch);
int WINAPI SsmAutoDialEx(int ch, LPSTR szPhoNum,WORD wParam);
int WINAPI SsmIsHaveCpg(int ch);                  
int WINAPI SsmGetCpg(int ch,char* szmsg,int* msglen);
int WINAPI SsmSetCalleeHoldFlag(int ch, BOOL bFlag);
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	Following functions for applications using SS7
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int WINAPI SsmBlockLocalCh(int ch);
int WINAPI SsmUnblockLocalCh(int ch);
int WINAPI SsmQueryLocalChBlockState(int ch, PDWORD pdwBlockState);

int WINAPI SsmBlockLocalPCM(int nLocalPcmNo);
int WINAPI SsmUnblockLocalPCM(int nLocalPcmNo);
int WINAPI SsmQueryLocalPCMBlockState(int nLocalPcmNo, PDWORD pdwBlockState);

int WINAPI SsmQueryOpBlockRemoteCh(int ch);
int WINAPI SsmBlockRemoteCh(int ch);
int WINAPI SsmUnblockRemoteCh(int ch);
int WINAPI SsmGetRemoteChBlockStatus(int ch);

int WINAPI SsmBlockRemotePCM(int nLocalPcmNo, DWORD dwBlockMode);
int WINAPI SsmUnblockRemotePCM(int nLocalPcmNo, DWORD dwUnblockMode);
int WINAPI SsmGetRemotePCMBlockStatus(int nLocalPcmNo, DWORD dwBlockMode);

BOOL WINAPI SsmGetWaitAutoDialAnswerTime(WORD * pwSeconds);
BOOL WINAPI SsmSetWaitAutoDialAnswerTime(WORD wSeconds);

int WINAPI SsmStartPickupAnalyze(int ch);
int WINAPI SsmGetPickup(int ch);

WORD WINAPI SsmGetReleaseReason(int ch);


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@ ADAPTIVE FILTER OPERATION  @@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int WINAPI SsmQueryOpEchoCanceller(int ch);
int WINAPI SsmSetEchoCanceller(int ch, BOOL bRun);
int WINAPI SsmGetEchoCancellerState(int ch);
int WINAPI SsmSetEchoCancellerStudy(int ch, BOOL bRun);
int WINAPI SsmGetEchoCancellerStudyState(int ch);
int WINAPI SsmGetEchoCancellerRatio(int ch);
int WINAPI SsmSaveEchoCancellerPara(int ch);
int WINAPI SsmSetEchoCancelDelaySize(int ch,WORD wSize);
WORD WINAPI SsmGetEchoCancelDelaySize(int ch);

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@ CT-BUS OPERATION  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int WINAPI SsmListenTo(int ch1,int ch2);
int WINAPI SsmListenToEx(int ch1, int nVolume1, int ch2);
int WINAPI SsmStopListenTo(int ch1,int ch2);
int WINAPI SsmTalkWith(int ch1,int ch2);
int WINAPI SsmTalkWithEx(int ch1, int nVlm1, int ch2, int nVlm2);
int WINAPI SsmStopTalkWith(int ch1,int ch2);

void WINAPI PlayListen(DWORD dwBId, DWORD ch);
void WINAPI PlayListenNew(DWORD dwSpeakerCh, DWORD dwMonitorCh);
void WINAPI StopListen(DWORD dwBId);
void WINAPI StopListenNew(DWORD dwSpeakerCh);

int	WINAPI SsmLinkFrom(int ch1,int ch2);
int WINAPI SsmStopLinkFrom(int ch1,int ch2);
int WINAPI SsmLinkFromEx(int ch1, int nVolume1, int ch2);
int	WINAPI SsmLinkFromAllCh(int ch, int nVolume, int*nListenerTable,int nListenerNum);
int	WINAPI SsmUnLinkFromAllCh(int ch, int*nListenerTable,int nListenerNum);

int WINAPI	SsmLinkToBus(int ch,int ts);
int	WINAPI  SsmUnLinkToBus(int ch,int ts);
int WINAPI	SsmLinkFromBus(int ts,int ch);
int WINAPI	SsmLinkFromBusEx(int ts, int ch, int vlm);
int WINAPI	SsmUnLinkFromBus(int ts,int ch);
int WINAPI	SsmListenToPlay(int ch1, int vlm1, int ch2);
int WINAPI	SsmUnListenToPlay(int ch1, int ch2);
int WINAPI SsmClearChBusLink(int nCh);

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@ CONFERENCE OPERATION  @@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int WINAPI SsmCreateConfGroup(int nMaxMember, int nMaxSpeaker, int nMaxSpeaking, int nMaxSilenceTime);
int WINAPI SsmFreeConfGroup(int nGrpId);
int WINAPI SsmJoinConfGroup(int nGrpId, int ch, WORD wJoinMode, int nMixerVolume, BOOL bCreateAlways, BOOL bExitGrpAlways);
int WINAPI SsmExitConfGroup(int ch, BOOL bFreeGrpAlways);
int WINAPI SsmSetContactInConf(int nGrpID, int chFrom, int chTo, BOOL bFlag);
int WINAPI SsmGetConfCfgInfo(PWORD pwMaxMember, PWORD pwMaxSpeaker, PWORD pwMaxSpeaking, PWORD pwMaxSilenceTime);
int WINAPI SsmGetTotalConfGroup();
int WINAPI SsmGetConfGrpId(int* pnGrpId);
int WINAPI SsmGetConfGrpCfgInfo(int nGrpId, PWORD pwMaxMember, PWORD pwMaxSpeaker, PWORD pwMaxSpeaking, PWORD pwMaxSilenceTime);
int WINAPI SsmGetConfGrpInfo(int nGrpId, PWORD pwTotalMember, PWORD pwTotalSpeaker, PWORD pwTotalSpeaking);
int WINAPI SsmGetConfGrpMmbrId(int nGrpId, int* pnMmbrId);
int WINAPI SsmGetConfGrpMmbrInfo(int nGrpId, int nMmbrId, int* pnAppCh, PWORD pwJoinMode, PWORD pwIsSpeaking, PDWORD pdwSilenceTime);
int WINAPI SsmGetConfChInfo(int ch, int * pnGrpId, int * pnMmbrId, PWORD pwJoinMode, PWORD pwIsSpeaking, PDWORD pdwSilenceTime);
int WINAPI SsmValidateGrpId(int nGrpId);
int WINAPI SsmSetListenVlmInConf(int ch, int nVlm);

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@ DRIVER  Ver. 1.x COMPATIBLE FUNCTIONS  @@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
DWORD WINAPI InitCard(DWORD add1,DWORD add2,DWORD add3,DWORD add4,DWORD add5,
				DWORD add6,DWORD add7,DWORD add8,DWORD intno);
void WINAPI ShutCard(void);

DWORD WINAPI StartRecordFile(DWORD ch,char * fliename);
void WINAPI StopRecordFile(DWORD ch);
void WINAPI PauseRecord(DWORD ch);
void WINAPI RestartRecord(DWORD ch);
DWORD WINAPI GetRecordTime(DWORD ch);

DWORD WINAPI StartPlayFile(DWORD ch,char * filename);
void WINAPI StopPlayFile(DWORD ch);
DWORD WINAPI CheckPlayingEnd(DWORD ch);
void WINAPI PausePlay(DWORD ch);
void WINAPI RestartPlay(DWORD ch);
void WINAPI FastPlay(DWORD ch);
void WINAPI BackPlay(DWORD ch);
DWORD WINAPI GetPlayTime(DWORD ch);
DWORD WINAPI GetPlayPercent(DWORD ch);
void WINAPI SetPlayTime(DWORD ch,DWORD time);
void WINAPI SetPlayPercent(DWORD ch,DWORD percent);

DWORD WINAPI LoadIndexFile(DWORD segno,LPSTR filename,DWORD startadr,DWORD length);
void WINAPI FreeIndexMem(DWORD segno);
void WINAPI PlayIndex(DWORD ch,LPSTR segstring);
void WINAPI StopIndex(DWORD ch);
void WINAPI DTMFStop(DWORD ch,DWORD if_stop);

DWORD WINAPI GetDTMF(DWORD ch);
DWORD WINAPI GetDTMFStr(DWORD ch,char * buf);
void WINAPI ClearDTMFBuf(DWORD ch);
DWORD WINAPI DetectRing(DWORD ch);
void WINAPI ClearRing(DWORD ch);
void WINAPI HangUp(DWORD ch);
void WINAPI PickUp(DWORD ch);
DWORD WINAPI DetectInter(DWORD ch);
DWORD WINAPI GetFlash(DWORD ch);
void WINAPI SetFlashTime(int time);
DWORD WINAPI ToneCheck(DWORD ch);
DWORD WINAPI GetBusyLen(DWORD ch);
void WINAPI DTMFDial(DWORD ch,LPSTR dialstring);
DWORD WINAPI DTMFDialEnd(DWORD ch);
void WINAPI Flash(DWORD ch,DWORD time);
void WINAPI SetPlayVolume(DWORD ch,LONG volume);
void WINAPI SetRecVolume(DWORD ch,LONG volume);

DWORD WINAPI GetCallerId(DWORD ch,LPSTR buf);

void WINAPI SetPickSens(DWORD piont);
DWORD WINAPI GetPickUp(DWORD ch);
DWORD WINAPI GetNum(DWORD ch,LPSTR buf,DWORD time,DWORD len,int end_char);
void WINAPI SetWorkMode(int Mode);
void WINAPI SetUserCard(DWORD addr1,DWORD addr2,DWORD addr3,DWORD addr4,DWORD addr5,DWORD addr6,DWORD addr7,DWORD addr8);
void WINAPI SendBusyTone(int ch);
void WINAPI SendRingEchoTone(int ch);
void WINAPI SendDialTone(int ch);
void WINAPI StopSendTone(int ch);
void WINAPI StartSendRing(int ch);
void WINAPI StopSendRing(int ch);
int WINAPI Link2Ch(int ch1,int ch2);
void WINAPI UnLink2Ch(int ch1,int ch2);
int WINAPI Link3Ch(int ch1,int ch2,int ch3);
void WINAPI UnLink3Ch(int ch1,int ch2,int ch3);
int WINAPI ListenFromCh(int ch1,int ch2);
void WINAPI StopListenFromCh(int ch1,int ch2);
void WINAPI SetDelay(DWORD ch,DWORD delay);
void WINAPI SetLevel(DWORD ch,DWORD level);
DWORD WINAPI DetectSound(DWORD ch);
DWORD WINAPI GetFax11(DWORD ch);
DWORD WINAPI GetFax21(DWORD ch);
DWORD  WINAPI GetRing(DWORD ch);
void WINAPI SetDialSpeed (DWORD point );
void WINAPI GetErrorMsg(char *buf);
DWORD WINAPI SetMaxSeg(DWORD inmaxsegment);

void WINAPI PauseCard(void);
void WINAPI RestartCard(void);

BOOL WINAPI SetIRQPriority(int nPriorityClass);

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@ RECORD MODULE OPERATION  @@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
DWORD WINAPI GetLevel(DWORD ch);
void WINAPI SetJudge(DWORD ch, DWORD Judge);
int WINAPI SsmQueryOpADC(int ch);
int WINAPI SsmGetLineVoltage(int ch);
int WINAPI SsmSetDtrmLineVoltage(int ch, WORD wDtrmValtage);
int WINAPI SsmGetDtrmLineVoltage(int ch);
DWORD WINAPI DetectPickUp(DWORD ch);
DWORD WINAPI DetectEmpty(DWORD ch);
void WINAPI SetSoundJudgeLevel(DWORD ch, DWORD level);
DWORD WINAPI GetSoundJudgeLevel(DWORD ch);

int WINAPI SsmQueryOpMicGain(int ch);
int WINAPI SsmSetMicGain(int ch, int nGain);
int WINAPI SsmGetMicGain(int ch);

int WINAPI SsmSetIgnoreLineVoltage(int ch, BOOL bEnable);
int WINAPI SsmGetIgnoreLineVoltage(int ch);

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@ POWER-AMPLIFIER OPERATION  @@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void WINAPI SetVolume(DWORD dwBoardId, DWORD dwVolume);
int WINAPI SsmQueryOpPowerAmp(int ch);
int WINAPI SsmSetPowerAmpVlm(int ch, int nVolume);

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@ POLAR REVERSE OPERATION  @@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
DWORD WINAPI GetFZCount(DWORD ch);
int WINAPI SsmQueryOpPolarRvrs(int ch);
int WINAPI SsmGetPolarRvrsCount(int ch);
int WINAPI SsmGetPolarState(int ch);
int WINAPI SsmSetPolarState(int ch, int nPolar);
int WINAPI SsmSetUnimoduleState(int ch, int nLink);
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ SS1 OPERATION  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int WINAPI SsmQueryOpSS1(int ch);
int WINAPI SsmSendCAS(int ch, BYTE btCas);
int WINAPI SsmGetCAS(int ch);
int WINAPI SsmSetRxR2Mode(int ch, int nMode, BOOL bEnable);
int WINAPI SsmGetR2(int ch);
int WINAPI SsmSendR2(int ch, int nMode, BYTE btR2);
int WINAPI SsmSendR2Ex (int ch, int nMode, BYTE btR2, DWORD dwKeepTime);//added by lgr for 070314-new-�Ϻ�����������SsmSendR2Ex(ch,ms)�������ﵽ��ȷ���Ʒ���R2�����ź� 2007.6.7
int WINAPI SsmStopSendR2(int ch);

int WINAPI SsmGetSendingCAS(int ch);
int WINAPI SsmGetSendingR2(int ch, int* pnMode, BYTE* pbtR2);

//add by JK for һ�����������֧�ַ���CAS�ź� begin
int WINAPI SsmSetSendCASFlag(int ch, int nCASFlag);
int WINAPI SsmGetSendCASFlag(int ch, int* pCASFlag);
//add by JK for һ�����������֧�ַ���CAS�ź� end

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ PCM LINK OPERATION  @@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int WINAPI SsmGetMaxPcm();
int WINAPI SsmGetPcmInfo(int nPcmNo, int* pnSSxMode, int* pnBoardId, int* pnBoardPcmNo, int* pnUsePcmTS16,
				 PDWORD pdwRcvrMode, PDWORD pdwEnableAutoCall, PDWORD pdwAutoCallDirection);
int WINAPI SsmSetPcmClockMode(int nPcmNo, int nClockMode);
int WINAPI SsmGetPcmLinkStatus(int nPcmNo, PWORD pwPcmLinkStatus);
int WINAPI SsmPcmTsToCh(int nLocalPcmNo, int nTs);
int WINAPI SsmChToPcmTs(int ch, int* pnLocalPcmNo, int* pnTs);


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ SEND FSK OPERATION  @@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int WINAPI SsmSetFskPara(int nFreqBit0, int nFreqBit1, int nBaudrate, int nMdlAmp);
int WINAPI SsmGetFskPara(int* pnFreqBit0, int* pnFreqBit1, int* pnBaudrate, int* pnMdlAmp);
int WINAPI SsmTransFskData(unsigned char* pS, int	nSrcLen,int nSyncLen,int nSyncOffLen,unsigned char* pD);
int WINAPI SsmStartSendFSK(int ch, LPSTR pBuf, DWORD dwMaxBit);
int WINAPI SsmCheckSendFsk(int ch);
int WINAPI SsmStopSendFsk(int ch);

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ RECEIVE FSK OPERATION  @@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int WINAPI SsmStartRcvFSK_III(int ch, WORD wTimeOut, WORD wMaxLen, PUCHAR pucMarkCodeBuf, UCHAR ucMarkCodeCount);
int WINAPI SsmStartRcvFSK_II(int ch, WORD wTimeOut, WORD wMaxLen, PUCHAR pucMarkCodeBuf, UCHAR ucMarkCodeCount);
int WINAPI SsmStartRcvFSK(int ch, WORD wTimeOut, WORD wMaxLen, UCHAR ucEndCode, WORD wEndCodeCount);
int WINAPI SsmClearRcvFSKBuf(int ch);
int WINAPI SsmCheckRcvFSK(int ch);
int WINAPI SsmGetRcvFSK(int ch, PUCHAR pucBuf);
int WINAPI SsmStopRcvFSK(int ch);

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ SS7 OPERATION  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int WINAPI SsmSendSs7Msu(WORD wMsuLength, PUCHAR pucMsuBuf);
int WINAPI SsmGetSs7Msu(PUCHAR* ppucMsuBuf);
int WINAPI SsmGetSs7Mtp2Msu(PUCHAR pucPara, PUCHAR* ppucMsuBuf);
int WINAPI SsmGetMtp3State();
int WINAPI SsmGetMtp3StateEx(int nDpcNo);
int WINAPI SsmGetMtp2Status(int nLinkNum);
int WINAPI SsmSetISUPCAT(int nch,  UCHAR ucCallerCAT);

int WINAPI SsmGetIsupUPPara(int nBCh, WORD wEventType, LPWORD pwLength, PUCHAR pucContent);
int WINAPI SsmSetIsupUPPara(int nBCh, WORD wEventType, LPWORD pwLength, PUCHAR pucContent);
int WINAPI SsmSendIsupMsg(int nBCh, WORD wEventType);

int	WINAPI SsmGetInboundLinkSet(int nBCh, LPWORD pwLinkSetNo, LPSTR pszOpc, LPSTR pszDpc);
int	WINAPI SsmGetRedirectionInfReason(int ch);

int WINAPI SsmSetIsupFlag(int ch, int nType, DWORD dwValue, PVOID pV);
int	WINAPI SsmGetIsupFlag(int ch, int nType, DWORD *pd);

int	WINAPI SsmGetTupFlag(int ch, int nType, DWORD *pd);

int WINAPI SsmSetIsupParameter(int nBCh, UCHAR ucMsgTypeCode, UCHAR ucParamTypeCode, WORD wLength, PUCHAR pucContent);
int WINAPI SsmGetIsupParameter(int nBCh, UCHAR ucMsgTypeCode, UCHAR ucParamTypeCode, PUCHAR pucContent, WORD wNumberOfBytesToWrite, LPWORD lpNumberOfBytesWritten);
BOOL WINAPI SsmIsupGetUsr(int *ch, PUCHAR pucData, PUCHAR ucLen);
BOOL WINAPI SsmIsupSendUsr(int ch, PUCHAR pucData, UCHAR ucLen);
int WINAPI SsmSetTupParameter (int nBCh, UCHAR ucMsgTypeCode, UCHAR ucParamTypeCode, WORD wLength, PUCHAR pucContent);

int WINAPI SsmSendSs7MsuEx(int ch, int nNewStep, WORD wMsuLength, PUCHAR pucMsuBuf);
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@ FAX OPERATION API @@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int WINAPI SsmFaxGetChStateMsg(int ch ,char *buf);
int WINAPI SsmFaxGetPages(int ch);
int WINAPI SsmFaxStartReceive(int ch,char *filename);
int WINAPI SsmFaxStartSend(int ch,char *filename);
int WINAPI SsmFaxStop(int ch);
void WINAPI SsmFaxSetMaxSpeed(int speed);
int WINAPI SsmFaxCheckEnd(int ch);
int WINAPI SsmFaxSetID(int ch,char *myid);
int WINAPI SsmFaxGetID(int ch,char *myid);
int WINAPI SsmFaxSetSendMode(int ch, int mode );
int WINAPI SsmFaxGetRcvBytes(int ch);
int WINAPI SsmFaxGetSendBytes(int ch);
int WINAPI SsmFaxAppendSend(int ch,char *filename);
int WINAPI SsmFaxSetHangup(int ch, bool flag);
int WINAPI SsmFaxStartSendEx(int ch, char * filename, int nStartPage, int nEndPage);
int WINAPI SsmFaxSendMultiFile(int ch, char * szFilePath, char * szFileName);
int WINAPI SsmFaxSendMultiFileEx(int ch, FAX_FILE_SCT * pV, int nNum);
int WINAPI SsmFaxGetAllBytes(int ch);
int WINAPI SsmFaxGetSpeed(int ch);
int WINAPI SsmFaxGetMode(int ch, int * pnDir, int *pnResMOde, int * pnTransMode);
int WINAPI SsmFaxGetFailReason(int ch);
int WINAPI SsmFaxGetDcnTag(int ch);
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@ MODEM OPERATION API  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int WINAPI SsmEnableCh(int ch );
int WINAPI SsmDisableCh(int ch );
int WINAPI SsmResetCh(int ch );
int WINAPI SsmCheckResetCh(int ch );
int WINAPI SsmGetLSR(int ch,unsigned char *retu);
int WINAPI SsmGetMSR(int ch ,unsigned char *retu);
int WINAPI SsmGetRTS(int ch );
int WINAPI SsmGetCTS(int ch );
int WINAPI SsmGetOH(int ch );
int WINAPI SsmDetectCarry(int ch );

int WINAPI SsmFaxGetChState(int ch ,unsigned short *buf);


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@ SERIAL PORT IO OPERATION API @@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int WINAPI SsmReadRxBuf(int ch , int nLen, char *lpcRxBuf);
int WINAPI SsmGetRxBufLen(int ch );
void WINAPI SsmClearRxBuf(int ch );
int WINAPI SsmWriteTxBuf(int ch, int nLen, char * lpcTxBuf);
int WINAPI SsmWriteTxBuf_S(int ch, char *s);
int WINAPI SsmWriteTxBuf_C(int ch , char buf);
int WINAPI SsmGetTxBufRemain(int ch);
int WINAPI SsmGetTxBufLen(int ch );
void WINAPI SsmClearTxBuf(int ch );

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@ ISDN API @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int WINAPI SsmISDNSetDialSubAddr(int ch, LPSTR lpSubAddress);
int WINAPI SsmISDNSetTxSubAddr(int ch, LPSTR lpSubAddress);
int WINAPI SsmISDNGetTxCallerSubAddr(int ch, LPSTR lpSubAddress);
int WINAPI SsmISDNGetSubAddr(int ch, LPSTR lpSubAddress);
int WINAPI SsmISDNGetCallerSubAddr(int ch, LPSTR lpSubAddress);
int WINAPI SsmISDNGetDisplayMsg(int ch, LPSTR lpDispMsg);
int WINAPI SsmISDNSetHangupRzn(int ch, int nReason);
int WINAPI SsmISDNGetStatus(int nPcmNo, int *pL3Start, 
							int *pL2DStatus, int *pL2D_L3Atom, 
							int * pL3_L2DAtom, int *pRef_ind);
int WINAPI SsmGetUserCallerId(int ch, LPSTR szCallerId);
int WINAPI SsmISDNSetCallerIdPresent(int ch, UCHAR ucPresentation);
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@ ViaVoice recognize OPERATION API @@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int WINAPI SsmStartRecognize(int ch,int grammarid,int max_time);
int WINAPI SsmGetResultCount(int ch);
int WINAPI SsmGetCurSens(int ch);
int WINAPI SsmGetResult(LPSTR buf,LPSTR index,LPSTR score,int ch);
int WINAPI SsmGetNResult(int id,LPSTR buf, LPSTR index, LPSTR score, int ch);
void WINAPI	SsmSetSil(int nValue);
void WINAPI	SsmSetRecoSound(int nBeginLen,int nEndLen,int nSoundLen);
void WINAPI	SsmSetRecoTime(int nRecoTime,int nMaxWait);

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@ Set Hardware Flags OPERATION API @@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int WINAPI SsmSetFlag(int ch, int nType, long lValue);
int WINAPI SsmGetFlag(int ch, int nType, long* plValue);

int WINAPI SsmSetOvrlEnrgLevel(int ch,WORD wOvrlEnrgLevel);
WORD WINAPI SsmGetOvrlEnrgLevel(int ch);
int WINAPI SsmSetOvrlEnrgDtrmTime(int ch,WORD wMinTime,WORD wMaxTime);

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@ SPY  API @@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int  WINAPI SpyGetState(int nCic);
int  WINAPI SpyGetCallerId(int nCic,char *pcCid);
int  WINAPI SpyGetCalleeId(int nCic,char *pcCid);
int  WINAPI SpyGetCallInCh(int nCic);
int  WINAPI SpyGetCallOutCh(int nCic);
int  WINAPI SpyGetMaxCic();
int  WINAPI SpyStopRecToFile(int  nCic);
int  WINAPI SpyRecToFile(int nCic, WORD wDirection,
     						LPSTR pszFileName, int nCodecFormat,
	    					DWORD dwStartPos, DWORD dwBytes, 
							DWORD dwTime,  int nMask);
int  WINAPI SpyGetLinkStatus(int nSpyPcmNo,UCHAR ucFlag);

int WINAPI SsmSetTxOriginalCallerID(int ch, BYTE* pszTxCallerId);

int WINAPI SsmGetUserCallerId(int ch, LPSTR szCallerId);

int WINAPI SsmSetNumType(int ch, int nNumClass, int nNumType);
int WINAPI SsmGetNumType(int ch, int nNumClass, int* pNumType);

int WINAPI SsmSetCharge(int ch, int ChargeFlag);

int WINAPI SpyChToCic(int ch);

int WINAPI SsmGetSs7SpyMsu(PUCHAR* ppucMsuBuf);

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@ EVENT  API @@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int	WINAPI SsmSetEvent (WORD wEvent, int nReference,BOOL bEnable, PEVENT_SET_INFO pEventSet);
int WINAPI SsmGetEventMode(WORD wEvent, int nReference, PWORD pwEnable, PEVENT_SET_INFO pEventSet);
int	WINAPI SsmWaitForEvent(DWORD dwTimeOut, PMESSAGE_INFO pEvent);
int	WINAPI SsmGetEvent(PMESSAGE_INFO pEvent);
int	WINAPI SsmStartTimer(WORD wDelay, WORD fuEvent);
int	WINAPI SsmStopTimer(int nTimer);
int WINAPI SsmWaitForEventA(DWORD dwTimeOut, PSSM_EVENT pEvent);
int WINAPI SsmGetEventA(PSSM_EVENT pEvent);
int WINAPI SsmPutUserEventA(PSSM_EVENT pEvent);

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@   DTR OPERATION   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int WINAPI DTRGetLCDStr(int ch, LPSTR pszLCDStr);
int WINAPI DTRClearDKeyStr(int ch);
int WINAPI DTRSetMixerVolume(int ch, int nGroup, int nDownVolume, int nUpVolume);
int WINAPI DTRGetMixerVolume(int ch, int nGroup, int* pnDownVolume, int* pnUpVolume);
int WINAPI DTRGetDKeyStr(int ch, LPSTR pszDKeyStr);
int WINAPI DTRSendRawData2A3(int ch, UCHAR ucCmd, WORD wLen, PUCHAR pucData);
char*  WINAPI DTRGetLCDStrA(int ch);
int WINAPI SsmGetIsdnMsu(int nPcmId, PUCHAR pucMsuBuf);
int WINAPI SsmSendIsdnMsu(int nPcmId, int nMsgLen, PUCHAR pucMsuBuf);
int WINAPI SsmCheckIsdnMsu(int nPcmId);
int WINAPI SsmGetDllVersion(PSSM_VERSION pDLLVersion);
int WINAPI SsmGetInterEventType();
int WINAPI SsmSetInterEventType(int nType);
int WINAPI SsmPutUserEvent(	WORD	wEventCode,
							int		nReference,
							DWORD	dwParam //�������
							);

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@ Tcap and Sccp API  @@@@@@@@@@@@@@@@@@@@@@@@@@@ 
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int WINAPI SccpSaveReceivedMessage(int nLen,BYTE *pucMsg);
int WINAPI SccpGetReceivedMessage(int *pnLen,BYTE *pucMsg);
int WINAPI MtpGetSccpMessage(int *pnLen,BYTE *pucMsg);
int WINAPI SccpInit();
int WINAPI SccpConfig();

int WINAPI SsmUserSendMessageToTcap(int nLen, BYTE *pucMsg);//����Ϣ ��Ϣ���ȣ� ��Ϣ��
int WINAPI SsmUserGetTcapDlgMessage(int *pLen, BYTE *pucDlgInd);//���ȶԻ���Ϣ��
int WINAPI SsmUserGetTcapCmpMessage(int *pLen, int nCurDlgID, int nMsgStyle, BYTE *pucCmpInd);
int WINAPI SsmUserGetLocalMessage(int *pLen, BYTE *pucMsgInd);//���L_Cancel �� L_Reject
BYTE* WINAPI SsmUserGetISMState(int nID, BYTE *pucState);//����ID״̬
BOOL WINAPI SsmStartTcap();
void WINAPI SsmTcapGetErrorMsg(char temp[100][50]);

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@   Query Format   ��ѯͨ���Ƿ�֧��ָ����¼������ʽ@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int WINAPI SsmQueryPlayFormat(int ch,int nFormat);
int WINAPI SsmQueryRecFormat(int ch, int nFormat);

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@   Board Eeprom OPERATION   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int WINAPI SsmReadBoardEepromShareSection(int nBId, PVOID pV, int nLen);
int WINAPI SsmWriteBoardEepromShareSection(int nBId, PVOID pV, int nLen);

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@ ������Դ����API @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//	�ͼ�������������������忨���ʹ��
int WINAPI ShvGetLinkToBus(int iVCh);
int WINAPI ShvLinkToBus(int iVCh, int iTs);
int WINAPI ShvUnLinkToBus(int iVCh, int iTs);
int WINAPI ShvLinkFromBus(int iVCh, int iTs);
int WINAPI ShvUnLinkFromBus(int iVCh, int iTs);
int WINAPI ShvSetVoiceEffect(int iVCh, int iValue);
int WINAPI ShvGetVoiceEffect(int iVCh);
//	�߼����������������������ʹ��
int WINAPI SsmGetMaxVCh();
int WINAPI SsmGetMaxFreeVCh();
int WINAPI SsmBindVCh(int iCh);
int WINAPI SsmUnBindVCh(int iCh);
int WINAPI SsmSetVoiceEffect(int iCh, int iValue);
int WINAPI SsmGetVoiceEffect(int iCh);

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@   VOIP Board Operation     @@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
int WINAPI		SsmIpGetSessionCodecType(int ch);
int WINAPI		SsmIpSetForwardNum(int ch, LPSTR pszForwardNum);
int WINAPI		SsmIpInitiateTransfer(int ch, LPSTR pszTransferTo);
int WINAPI		SsmIpGetMessageField(int ch, int type, LPSTR szBuffer, int *pSize);

#ifdef __cplusplus
	}
#endif
