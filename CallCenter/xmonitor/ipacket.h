//
// IPacket.h
//

#if !defined(AFX_IPACKET_H__C068F946_E633_493B_AE26_1037276AFC8C__INCLUDED_)
#define AFX_IPACKET_H__C068F946_E633_493B_AE26_1037276AFC8C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBuffer;

class IPacket  
{
// Construction
protected:
	IPacket(LPCTSTR sCommandId);
	virtual ~IPacket();

// Attributes
protected:
	LPCTSTR 	CommandId;

// Operations
public:
	inline BOOL		IsCommandId(LPCTSTR xCommandId) const;
	inline void		Release();
public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength) = 0;
	virtual int		EncodeBody(CBuffer *pBuffer) = 0;
public:
	static IPacket*	ReadBuffer(BYTE* pBuffer, DWORD nLength);
	static int		ReadField(BYTE* pBuffer, LPTSTR sField, int nField, CHAR cPart = ' ');
};

class IOkPacket : public IPacket
{
public:
	IOkPacket();
	virtual ~IOkPacket();

public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

class IErrPacket : public IPacket
{
// Construction
public:
	IErrPacket(int nErrCode = 0);
	virtual ~IErrPacket();

// Attributes
public:
	int			ErrCode;

// Operations
public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

class IBindPacket : public IPacket
{
// Construction
public:
	IBindPacket();
	virtual ~IBindPacket();

// Attributes
protected:
	union
	{
		// 请求
		struct 
		{
			CHAR	CallType[4];
			
			CHAR	LocalId[21];
			CHAR	RemoteId[21];
			CHAR	FeeNumber[21];
		};
		// 应答
		struct 
		{
			CHAR	FeeTable[4];
		};
	};

// Operations
public:
	inline int		GetCallType(LPTSTR xCallType) const;
	inline int		GetLocalId(LPTSTR xLocalId) const;
	inline int		GetRemoteId(LPTSTR xRemoteId) const;
	inline int		GetFeeNumber(LPTSTR xFeeNumber) const;
	inline void		SetCallType(LPCTSTR xCallType);
	inline void		SetLocalId(LPCTSTR xLocalId);
	inline void		SetRemoteId(LPCTSTR xRemoteId);
	inline void		SetFeeNumber(LPCTSTR xFeeNumber);
	
	inline int		GetFeeTable(LPTSTR xFeeTable) const;
	inline void		SetFeeTable(LPCTSTR xFeeTable);
public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

class IExitPacket : public IPacket
{
public:
	IExitPacket();
	virtual ~IExitPacket();

public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

class ITalkPacket : public IPacket
{
public:
	ITalkPacket();
	virtual ~ITalkPacket();

public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

class IHaltPacket : public IPacket
{
// Construction
public:
	IHaltPacket(int nChannel = -1);
	virtual ~IHaltPacket();

// Attributes
public:
	int			Channel;

// Operations
public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

class ISyncPacket : public IPacket
{
// Construction
public:
	ISyncPacket(double nMoney = 0, int nRepeat = 1);
	virtual ~ISyncPacket();

// Attributes
public:
	double		SyncMoney;
	int			SyncRepeat;

// Operations
public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

class IHandPacket : public IPacket
{
// Construction
public:
	IHandPacket();
	virtual ~IHandPacket();

// Attributes
protected:
	CHAR		ClientId[8];

// Operations
public:
	inline int		GetClientId(LPTSTR xClientId) const;
	inline void		SetClientId(LPCTSTR xClientId);
public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

class IRingPacket : public IPacket
{
// Construction
public:
	IRingPacket();
	virtual ~IRingPacket();

// Attributes
protected:
	CHAR		CallerId[21];
	CHAR		PhonNumber[21];

// Operations
public:
	inline int		GetCallerId(LPTSTR xCallerId) const;
	inline int		GetPhonNumber(LPTSTR xPhonNumber) const;
	inline void		SetCallerId(LPCTSTR xCallerId);
	inline void		SetPhonNumber(LPCTSTR xPhonNumber);
public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

class IDialPacket : public IPacket
{
// Construction
public:
	IDialPacket();
	virtual ~IDialPacket();

// Attributes
protected:
	CHAR		RemoteId[21];
	CHAR		LocalId[21];
	CHAR		CallerId[21];
	CHAR		FeeNumber[21];
// Reserved
public:
	DWORD		Trunkset;

// Operations
public:
	inline int		GetLocalId(LPTSTR xLocalId) const;
	inline int		GetRemoteId(LPTSTR xRemoteId) const;
	inline int		GetCallerId(LPTSTR xCallerId) const;
	inline int		GetFeeNumber(LPTSTR xFeeNumber) const;
	inline void		SetLocalId(LPCTSTR xLocalId);
	inline void		SetRemoteId(LPCTSTR xRemoteId);
	inline void		SetCallerId(LPCTSTR xCallerId);
	inline void		SetFeeNumber(LPCTSTR xFeeNumber);
	inline void		SetTrunkset(WORD nTrunkId, WORD nLinkId = 0);
public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

class IRoutPacket : public IPacket
{
// Construction
public:
	IRoutPacket();
	virtual ~IRoutPacket();

// Attributes
public:
	CHAR		RemoteId[21];
	CHAR		PakParam[800];

// Operations
public:
	inline int		GetRemoteId(LPTSTR xRemoteId) const;
	inline void		SetRemoteId(LPCTSTR xRemoteId);
public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

class ICtocPacket : public IPacket
{
// Construction
public:
	ICtocPacket(int nClientId = 0);
	virtual ~ICtocPacket();

// Attributes
public:
	int			ClientId;

// Operations
public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

class IRecdPacket : public IPacket
{
// Construction
public:
	IRecdPacket(int nChannel = -1);
	virtual ~IRecdPacket();

// Attributes
public:
	int			Channel;
	CHAR		PakParam[16];

// Operations
public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

class ILisnPacket : public IPacket
{
// Construction
public:
	ILisnPacket(int nChannel = -1);
	virtual ~ILisnPacket();

// Attributes
public:
	int			Channel;
	CHAR		PakParam[16];

// Operations
public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

class IWalkPacket : public IPacket
{
// Construction
public:
	IWalkPacket(LPCTSTR xWalkId);
	virtual ~IWalkPacket();

// Attributes
protected:
	CHAR		WalkId[21];

// Operations
public:
	inline void		SetWalkId(LPTSTR xRemoteId);
public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

class IWaitPacket : public IPacket
{
// Construction
public:
	IWaitPacket(LPCTSTR xParam = NULL);
	virtual ~IWaitPacket();

// Attributes
public:
	CHAR		PakParam[16];

// Operations
public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

class IAchnPacket : public IPacket
{
// Construction
public:
	IAchnPacket();
	IAchnPacket(int nChType, int nChFirst, int nChLength);
	virtual ~IAchnPacket();

// Attributes
public:
	int			ChType;
	int			ChFirst;
	int			ChLength;

// Operations
public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

class IEchnPacket : public IPacket
{
// Construction
public:
	IEchnPacket();
	IEchnPacket(int nChan, int nChState);
	virtual ~IEchnPacket();

// Attributes
public:
	int			ChNum;
	int			ChState;

// Operations
public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

#include "IPacket.inl"

//
// 命令码定义
//

#define ICP_OK				"OK"
#define ICP_ERR				"ERR"

#define ICP_SYNC			"SYN"	// 同步扣费
#define ICP_BIND			"BND"	// 计费绑定
#define ICP_EXIT			"BYE"	// 拆除绑定
#define ICP_TALK			"TAK"	// 通话开始
#define ICP_HALT			"HAL"	// 通话结束
#define ICP_HAND			"HND"	// 坐席绑定
#define ICP_RING			"RNG"	// 来电提示
#define ICP_DIAL			"DIA"	// 坐席外呼
#define ICP_ROUT			"SVR"	// 人工转自动业务
#define ICP_CTOC			"CTC"	// 转入坐席
#define ICP_RECD			"REC"	// 录音坐席, 也适用于任何通道
#define ICP_LISN			"LIS"	// 监听坐席, 也适用于任何通道
#define ICP_WALK			"WAR"	// 秘书服务
#define ICP_WAIT			"WAT"	// 等待服务

#define ICP_ACHN			"ACH"	// 增加可控通道
#define ICP_ECHN			"ECH"	// 设置通道状态

//
// 错误码定义
//

#define ICP_SUCCESS			0
#define ICP_ERROR			4000
#define ICP_NOUSER			4001
#define	ICP_NOPAY			4002
#define ICP_CALLBACK		8888

#endif // !defined(AFX_IPACKET_H__C068F946_E633_493B_AE26_1037276AFC8C__INCLUDED_)
