//
// PacketCore.h
//

#if !defined(AFX_PACKETCORE_H__704EE319_64A8_4398_A593_F422E8B4D9E0__INCLUDED_)
#define AFX_PACKETCORE_H__704EE319_64A8_4398_A593_F422E8B4D9E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Packet.h"

class CXMLElement;
class CBuffer;

// CPacketCore

class CPacketCore : public CPacket
{
public:
	CPacketCore(LPCTSTR sCommandId);
	virtual ~CPacketCore();

public:
	static CPacket*	ReadBuffer(BYTE* pBuffer, DWORD nLength);
};

// CBindPacket

class CBindPacket : public CPacketCore
{
public:
	CBindPacket(LPCTSTR xUserId, LPCTSTR xPassword = NULL);
	CBindPacket(LPCTSTR xFeeCode = NULL);
	virtual ~CBindPacket();
	
protected:
	union
	{
		struct // 计费请求
		{
			CHAR	CallType[16];
			CHAR	LocalId[21];
			CHAR	RemoteId[21];
			CHAR	FeeNumber[21];
		};
		struct // 计费应答
		{
			CHAR	FeeCode[8];
		};
		struct // 坐席请求
		{
			CHAR	UserId[16];
			CHAR	Password[10];
		};
	};
	
public:
	inline int		GetCallType(LPTSTR xCallType) const;
	inline int		GetLocalId(LPTSTR xLocalId) const;
	inline int		GetRemoteId(LPTSTR xRemoteId) const;
	inline int		GetFeeNumber(LPTSTR xFeeNumber) const;
	inline void		SetCallType(LPCTSTR xCallType);
	inline void		SetLocalId(LPCTSTR xLocalId);
	inline void		SetRemoteId(LPCTSTR xRemoteId);
	inline void		SetFeeNumber(LPCTSTR xFeeNumber);
	
	inline int		GetFeeCode(LPTSTR xFeeTable) const;
	inline void		SetFeeCode(LPCTSTR xFeeCode);
	
	inline int		GetUserId(LPTSTR xUserId) const;
	inline int		GetPassword(LPTSTR xPassword) const;
	inline int		GetSrvCode(LPTSTR xSrvCode);
protected:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

// CDialPacket

class CDialPacket : public CPacketCore
{
public:
	CDialPacket();
	virtual ~CDialPacket();
	
protected:
	union
	{
		struct // 拨号请求
		{
			CHAR	RemoteId[21];
			CHAR	LocalId[21];
			CHAR	CallerId[21];
			CHAR	FeeNumber[21];
		};
	};
	
public:
	inline int		GetLocalId(LPTSTR xLocalId) const;
	inline int		GetRemoteId(LPTSTR xRemoteId) const;
	inline int		GetFeeNumber(LPTSTR xFeeNumber) const;
	inline int		GetCallerId(LPTSTR xCallerId) const;
	inline void		SetLocalId(LPCTSTR xLocalId);
	inline void		SetRemoteId(LPCTSTR xRemoteId);
	inline void		SetFeeNumber(LPCTSTR xFeeNumber);
	inline void		SetCallerId(LPCTSTR xCallerId);
protected:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

// CRingPacket

class CRingPacket : public CPacketCore
{
public:
	CRingPacket();
	virtual ~CRingPacket();
	CRingPacket(LPCTSTR xLocalId, LPCTSTR xRemoteId, int nLineId = -1);
	
protected:
	union
	{
		struct // 振铃请求
		{
			CHAR	LocalId[21];
			CHAR	RemoteId[21];
			int		LineId;
		};
	};
	
public:
	inline int		GetLocalId(LPTSTR xLocalId) const;
	inline int		GetRemoteId(LPTSTR xRemoteId) const;
	inline int		GetLineId() const;
protected:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

// CTalkPacket

class CTalkPacket : public CPacketCore
{
public:
	CTalkPacket();
	virtual ~CTalkPacket();
};

// CRedPacket

class CRedPacket : public CPacketCore
{
public:
	CRedPacket(LPCTSTR xMsgType = _T("MIN"), int nMinutes = 0);
	virtual ~CRedPacket();

protected:
	CHAR	MsgType[16];
	union
	{
		struct // 剩余分钟
		{
			WORD	Minutes;
		};
	};
	
public:
	inline int		GetMsgType(LPTSTR xMsgType) const;
	inline int		GetMinutes() const;
protected:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

// CDeliverPacket

class CDeliverPacket : public CPacketCore
{
public:
	virtual ~CDeliverPacket();
	CDeliverPacket(LPCTSTR xMsgType, LPCTSTR xContent = NULL);

protected:
	union
	{
		struct // 业务转移
		{
			CHAR	MsgType[16];
			CHAR	MsgContent[1024];
		};
	};
	
public:
	inline int		GetMsgType(LPTSTR xMsgType) const;
	inline LPTSTR	GetContentPtr(LPTSTR& xContent) const;
protected:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

// CRecordPacket

class CRecordPacket : public CPacketCore
{
public:
	CRecordPacket(int nChan, LPCTSTR xMsgType = NULL);
	virtual ~CRecordPacket();
	
protected:
	union
	{
		struct // 通道录音
		{
			int		Channel;
			CHAR	MsgType[16];
		};
	};
	
public:
	inline int		GetChannel() const;
	inline int		GetMsgType(LPTSTR xMsgType) const;
protected:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

// CListenPacket

class CListenPacket : public CPacketCore
{
public:
	CListenPacket(int nChan, LPCTSTR xMsgType = NULL);
	virtual ~CListenPacket();
	
protected:
	union
	{
		struct // 通道监听
		{
			int		Channel;
			CHAR	MsgType[16];
		};
	};
	
public:
	inline int		GetChannel() const;
	inline int		GetMsgType(LPTSTR xMsgType) const;
protected:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

// CWaitPacket

class CWaitPacket : public CPacketCore
{
public:
	CWaitPacket(LPCTSTR xMsgContent = NULL);
	virtual ~CWaitPacket();

protected:
	union
	{
		struct // 放音等待
		{
			CHAR	MsgType[16];
		};
	};
	
public:
	inline int		GetMsgType(LPTSTR xMsgType) const;
protected:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

// CWalkPacket

class CWalkPacket : public CPacketCore
{
public:
	CWalkPacket(LPCTSTR xWalkId);
	virtual ~CWalkPacket();

protected:
	union
	{
		struct // 唤醒服务
		{
			CHAR	WalkId[21];
		};
	};

public:
	inline int		GetWalkId(LPTSTR xWalkId) const;
protected:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

// CHaltPacket

class CHaltPacket : public CPacketCore
{
public:
	CHaltPacket(int nLinkId = 0);
	virtual ~CHaltPacket();
	
public:
	const int		LinkId;

protected:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

// CAchnPacket

class CAchnPacket : public CPacketCore
{
public:
	CAchnPacket();
	virtual ~CAchnPacket();
	CAchnPacket(int nChType, int nChFirst, int nChLength);

public:
	union
	{
		struct // 通道分组
		{
			int		ChType;
			int		ChFirst;
			int		ChLength;
		};
	};

public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

// CEchnPacket

class CEchnPacket : public CPacketCore
{
public:
	CEchnPacket(int nChan = 0, int nChState = -1);
	virtual ~CEchnPacket();

public:
	union
	{
		struct // 通道状态
		{
			int		Channel;
			int		ChState;
		};
	};

public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

// CHandskPacket

class CHandskPacket : public CPacketCore
{
public:
	CHandskPacket();
	virtual ~CHandskPacket();
};

#if 0
// CSyncTrunkView

class CSyncTrunkView : public CPacketCore
{
public:
	CSyncTrunkView(CXMLElement* pXML = NULL);
	virtual ~CSyncTrunkView();
	
public:
	CXMLElement*	m_pXML;

public:
	BOOL		ReadContent(CXMLElement* pXML);
	BOOL		FormatContent(CXMLElement*& pXML);
protected:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

// CSyncLineState

class CSyncLineState : public CPacketCore
{
public:
	CSyncLineState(int nChan = 0, int nChState = -1);
	virtual ~CSyncLineState();

public:
	int		m_nChannel;
	int		m_nChState;

protected:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};
#endif
//////////////////////////////////////////////////////////////////////
// CBindPacket fields access

int CBindPacket::GetCallType(LPTSTR xCallType) const
{
	int nLength = _tcslen( CallType ); nLength = min( nLength, 4 );
	CopyMemory( xCallType, CallType, nLength ); return nLength;
}

int CBindPacket::GetLocalId(LPTSTR xLocalId) const
{
	int nLength = _tcslen( LocalId ); nLength = min( nLength, 21 );
	CopyMemory( xLocalId, LocalId, nLength ); return nLength;
}

int CBindPacket::GetRemoteId(LPTSTR xRemoteId) const
{
	int nLength = _tcslen( RemoteId ); nLength = min( nLength, 21 );
	CopyMemory( xRemoteId, RemoteId, nLength ); return nLength;
}

int CBindPacket::GetFeeNumber(LPTSTR xFeeNumber) const
{
	int nLength = _tcslen( FeeNumber ); nLength = min( nLength, 21 );
	CopyMemory( xFeeNumber, FeeNumber, nLength ); return nLength;
}

void CBindPacket::SetCallType(LPCTSTR xCallType)
{
	int nLength = _tcslen( xCallType ); nLength = min( nLength, 4 );
	CopyMemory( CallType, xCallType, nLength );
	ZeroMemory( CallType + nLength, 4 - nLength );
}

void CBindPacket::SetLocalId(LPCTSTR xLocalId)
{
	int nLength = _tcslen( xLocalId ); nLength = min( nLength, 21 );
	CopyMemory( LocalId, xLocalId, nLength );
	ZeroMemory( LocalId + nLength, 21 - nLength );
}

void CBindPacket::SetRemoteId(LPCTSTR xRemoteId)
{
	int nLength = _tcslen( xRemoteId ); nLength = min( nLength, 21 );
	CopyMemory( RemoteId, xRemoteId, nLength );
	ZeroMemory( RemoteId + nLength, 21 - nLength );
}

void CBindPacket::SetFeeNumber(LPCTSTR xFeeNumber)
{
	int nLength = _tcslen( xFeeNumber ); nLength = min( nLength, 21 );
	CopyMemory( FeeNumber, xFeeNumber, nLength );
	ZeroMemory( FeeNumber + nLength, 21 - nLength );
}

int CBindPacket::GetFeeCode(LPTSTR xFeeCode) const
{
	int nLength = _tcslen( FeeCode ); nLength = min( nLength, 4 );
	CopyMemory( xFeeCode, FeeCode, nLength ); return nLength;
}

void CBindPacket::SetFeeCode(LPCTSTR xFeeCode)
{
	int nLength = _tcslen( xFeeCode ); nLength = min( nLength, 4 );
	CopyMemory( FeeCode, xFeeCode, nLength );
	ZeroMemory( FeeCode + nLength, 4 - nLength );
}

int CBindPacket::GetUserId(LPTSTR xUserId) const
{
	int nLength = _tcslen( UserId ); nLength = min( nLength, 21 );
	CopyMemory( xUserId, UserId, nLength ); return nLength;
}

int CBindPacket::GetPassword(LPTSTR xPassword) const
{
	int nLength = _tcslen( Password ); nLength = min( nLength, 16 );
	CopyMemory( xPassword, Password, nLength ); return nLength;
}

int CBindPacket::GetSrvCode(LPTSTR xSrvCode)
{
	return GetPassword( xSrvCode );
}

//////////////////////////////////////////////////////////////////////
// CDialPacket fields access

int CDialPacket::GetLocalId(LPTSTR xLocalId) const
{
	int nLength = _tcslen( LocalId ); nLength = min( nLength, 21 );
	if ( xLocalId ) CopyMemory( xLocalId, LocalId, nLength ); return nLength;
}

int CDialPacket::GetRemoteId(LPTSTR xRemoteId) const
{
	int nLength = _tcslen( RemoteId ); nLength = min( nLength, 21 );
	CopyMemory( xRemoteId, RemoteId, nLength ); return nLength;
}

int CDialPacket::GetFeeNumber(LPTSTR xFeeNumber) const
{
	int nLength = _tcslen( FeeNumber ); nLength = min( nLength, 21 );
	CopyMemory( xFeeNumber, FeeNumber, nLength ); return nLength;
}

int CDialPacket::GetCallerId(LPTSTR xCallerId) const
{
	int nLength = _tcslen( CallerId ); nLength = min( nLength, 21 );
	if ( nLength == 0 ) return GetLocalId( xCallerId );
	CopyMemory( xCallerId, CallerId, nLength ); return nLength;
}

void CDialPacket::SetLocalId(LPCTSTR xLocalId)
{
	int nLength = _tcslen( xLocalId ); nLength = min( nLength, 21 );
	CopyMemory( LocalId, xLocalId, nLength );
	ZeroMemory( LocalId + nLength, 21 - nLength );
}

void CDialPacket::SetRemoteId(LPCTSTR xRemoteId)
{
	int nLength = _tcslen( xRemoteId ); nLength = min( nLength, 21 );
	CopyMemory( RemoteId, xRemoteId, nLength );
	ZeroMemory( RemoteId + nLength, 21 - nLength );
}

void CDialPacket::SetFeeNumber(LPCTSTR xFeeNumber)
{
	int nLength = _tcslen( xFeeNumber ); nLength = min( nLength, 21 );
	CopyMemory( FeeNumber, xFeeNumber, nLength );
	ZeroMemory( FeeNumber + nLength, 21 - nLength );
}

void CDialPacket::SetCallerId(LPCTSTR xCallerId)
{
	int nLength = _tcslen( xCallerId ); nLength = min( nLength, 21 );
	CopyMemory( CallerId, xCallerId, nLength );
	ZeroMemory( CallerId + nLength, 21 - nLength );
}

//////////////////////////////////////////////////////////////////////
// CRingPacket fields access

int CRingPacket::GetLocalId(LPTSTR xLocalId) const
{
	int nLength = _tcslen( LocalId ); nLength = min( nLength, 21 );
	CopyMemory( xLocalId, LocalId, nLength ); return nLength;
}

int CRingPacket::GetRemoteId(LPTSTR xRemoteId) const
{
	int nLength = _tcslen( RemoteId ); nLength = min( nLength, 21 );
	CopyMemory( xRemoteId, RemoteId, nLength ); return nLength;
}

int CRingPacket::GetLineId() const
{
	return LineId;
}

//////////////////////////////////////////////////////////////////////
// CRedPacket fields access

int CRedPacket::GetMinutes() const
{
	return Minutes;
}

//////////////////////////////////////////////////////////////////////
// CDeliverReation fields access

int CDeliverPacket::GetMsgType(LPTSTR xMsgType) const
{
	int nLength = _tcslen( MsgType ); nLength = min( nLength, 16 );
	CopyMemory( xMsgType, MsgType, nLength ); return nLength;
}

LPTSTR CDeliverPacket::GetContentPtr(LPTSTR& xContent) const
{
	return (xContent = (LPTSTR)MsgContent);
}

//////////////////////////////////////////////////////////////////////
// CRecordPacket fields access

int CRecordPacket::GetMsgType(LPTSTR xMsgType) const
{
	int nLength = _tcslen( MsgType ); nLength = min( nLength, 16 );
	CopyMemory( xMsgType, MsgType, nLength ); return nLength;
}

int CRecordPacket::GetChannel() const
{
	return Channel;
}

//////////////////////////////////////////////////////////////////////
// CWaitPacket fields access

int CWaitPacket::GetMsgType(LPTSTR xMsgType) const
{
	int nLength = _tcslen( MsgType ); nLength = min( nLength, 16 );
	CopyMemory( xMsgType, MsgType, nLength ); return nLength;
}

//////////////////////////////////////////////////////////////////////
// CListenPacket fields access

int CListenPacket::GetMsgType(LPTSTR xMsgType) const
{
	int nLength = _tcslen( MsgType ); nLength = min( nLength, 16 );
	CopyMemory( xMsgType, MsgType, nLength ); return nLength;
}

int CListenPacket::GetChannel() const
{
	return Channel;
}

//////////////////////////////////////////////////////////////////////
// CWalkPacket fields access

int CWalkPacket::GetWalkId(LPTSTR xWalkId) const
{
	int nLength = _tcslen( WalkId ); nLength = min( nLength, 21 );
	CopyMemory( xWalkId, WalkId, nLength ); return nLength;
}

//
// Error types
//

enum{
	ECore_Success		= 0,	// 成功
	ECore_NoUser		= 4001,	// 查无此人
	ECore_NoPay			= 4002,	// 欠费
	ECore_Busy			= 4003,	// 占线、忙
	ECore_Unknown		= 8000,	// 未知错误
	ECore_Rejected		= 8001,	// 拒绝存取
	ECore_CallBack		= 8888,	// 回拨
};

//
// 命令码定义
//

#define	ICore_BindLink			"BND"	// 链路请求
#define ICore_DialRemote		"DIA"	// 外呼请求
#define	ICore_TalkBegin			"TAK"	// 通话开始
#define	ICore_Ringing			"RNG"	// 来电提示

#define	ICore_RedLink			"RED"	// 链路告警
#define	ICore_Listen			"LIS"	// 监听链路
#define	ICore_Deliver			"SVR"	// 业务转移
#define ICore_RecordFile		"REC"	// 录音
#define ICore_Waiting			"WAT"	// 等待服务
#define ICore_Walker			"WAR"	// 秘书服务
#define ICore_HaltLink			"HAL"	// 链路复位

#define ICore_AddLink			"ACH"	// 增加可控通道
#define ICore_SyncLink			"ECH"	// 设置通道状态
#define ICore_Handshake			"HSK"	// 链路握手

#define	ICore_SyncTrunkView		"STV"	// 同步可控通道
#define	ICore_SyncLineState		"SLS"	// 同步通道状态

#endif // !defined(AFX_PACKETCORE_H__704EE319_64A8_4398_A593_F422E8B4D9E0__INCLUDED_)
