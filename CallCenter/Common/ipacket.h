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

//////////////////////////////////////////////////////////////////////
// IPacket Common

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
public:
	IErrPacket(int nErrCode = 0);
	virtual ~IErrPacket();

public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
	
public:
	int			ErrCode;
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

class IHaltPacket : public IPacket
{
public:
	IHaltPacket(int nChannel = -1);
	virtual ~IHaltPacket();

public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);

public:
	int			Channel;
};

//////////////////////////////////////////////////////////////////////
// IPacket Xcenter

class IHandPacket : public IPacket
{
public:
	IHandPacket();
	virtual ~IHandPacket();

public:
	inline int		GetClientId(LPTSTR xClientId) const;
	inline void		SetClientId(LPCTSTR xClientId);
public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);

protected:
	CHAR		ClientId[8];
};

class IRingPacket : public IPacket
{
public:
	IRingPacket();
	virtual ~IRingPacket();

protected:
	CHAR		CallerId[21];
	CHAR		PhonNumber[21];
	INT			ChNumber;

public:
	inline int		GetCallerId(LPTSTR xCallerId) const;
	inline int		GetPhonNumber(LPTSTR xPhonNumber) const;
	inline int		GetChNumber(INT& nChNumber) const;
	inline void		SetCallerId(LPCTSTR xCallerId);
	inline void		SetPhonNumber(LPCTSTR xPhonNumber);
	inline void		SetChNumber(INT nChNumber);
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

class IDialPacket : public IPacket
{
public:
	IDialPacket();
	virtual ~IDialPacket();

protected:
	CHAR		RemoteId[21];
	CHAR		LocalId[21];
	CHAR		CallerId[21];
	CHAR		FeeNumber[21];

public:
	inline int		GetLocalId(LPTSTR xLocalId) const;
	inline int		GetRemoteId(LPTSTR xRemoteId) const;
	inline int		GetCallerId(LPTSTR xCallerId) const;
	inline int		GetFeeNumber(LPTSTR xFeeNumber) const;
	inline void		SetLocalId(LPCTSTR xLocalId);
	inline void		SetRemoteId(LPCTSTR xRemoteId);
	inline void		SetCallerId(LPCTSTR xCallerId);
	inline void		SetFeeNumber(LPCTSTR xFeeNumber);
public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

class IRoutPacket : public IPacket
{
public:
	IRoutPacket();
	virtual ~IRoutPacket();

public:
	CHAR		RemoteId[21];
	CHAR		PakParam[800];

public:
	inline int		GetRemoteId(LPTSTR xRemoteId) const;
	inline void		SetRemoteId(LPCTSTR xRemoteId);
public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

class ICtocPacket : public IPacket
{
public:
	ICtocPacket(int nClientId = 0);
	virtual ~ICtocPacket();

public:
	int			ClientId;

public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

class IRecdPacket : public IPacket
{
public:
	IRecdPacket(int nChannel = -1);
	virtual ~IRecdPacket();

public:
	int			Channel;
	CHAR		PakParam[16];

public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

class ILisnPacket : public IPacket
{
public:
	ILisnPacket(int nChannel = -1);
	virtual ~ILisnPacket();

public:
	int			Channel;
	CHAR		PakParam[16];

public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

class IWalkPacket : public IPacket
{
public:
	IWalkPacket(LPCTSTR xWalkId);
	virtual ~IWalkPacket();

protected:
	CHAR		WalkId[21];

public:
	inline void		SetWalkId(LPTSTR xRemoteId);
public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

class IWaitPacket : public IPacket
{
public:
	IWaitPacket(LPCTSTR xParam = NULL);
	virtual ~IWaitPacket();

public:
	CHAR		PakParam[16];

public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

//////////////////////////////////////////////////////////////////////
// IPacket Xmaster

class IBindPacket : public IPacket
{
// Construction
public:
	IBindPacket();
	virtual ~IBindPacket();

// Attributes
protected:
	union{
		// ����
		struct {
			CHAR	CallType[4];
			
			CHAR	LocalId[21];
			CHAR	RemoteId[21];
			CHAR	FeeNumber[21];
		};
		// Ӧ��
		struct {
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

class IRedPacket : public IPacket
{
public:
	IRedPacket(int nMinutes = 0);
	virtual ~IRedPacket();

public:
	inline int		GetMinutes() const;
	inline void		SetMinutes(int nMinutes);
public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);

protected:
	WORD			Minutes;
};

class ISyncPacket : public IPacket
{
public:
	ISyncPacket(double nMoney = 0, int nRepeat = 1);
	virtual ~ISyncPacket();

public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);

public:
	double		SyncMoney;
	int			SyncRepeat;
};

//////////////////////////////////////////////////////////////////////
// IPacket Xmoniter

class IAchnPacket : public IPacket
{
public:
	IAchnPacket();
	IAchnPacket(int nChType, int nChFirst, int nChLength);
	virtual ~IAchnPacket();

public:
	int			ChType;
	int			ChFirst;
	int			ChLength;

public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

class IEchnPacket : public IPacket
{
public:
	IEchnPacket();
	IEchnPacket(int nChan, int nChState);
	virtual ~IEchnPacket();

public:
	int			ChNum;
	int			ChState;

public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

//////////////////////////////////////////////////////////////////////
// IPacket Xgateway

class IUserPacket : public IPacket
{
public:
	IUserPacket(LPCTSTR xUserId, LPCTSTR xPass = NULL);
	IUserPacket();
	virtual ~IUserPacket();
	
protected:
	CHAR		UserId[21];
	CHAR		MD5Hashing[32];
	
public:
	inline int		GetUserId(LPTSTR xUserId) const;
	inline BOOL		IsHashUsrCompatible(LPCTSTR xUserId, LPCTSTR xPass);
public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

#include "IPacket.inl"

//
// �����붨��
//

//////////////////////////////////////////////////////////////////////
//  	����			�ֹ�	����	�Ʒ�	����	�ڵ�	����//
//////////////////////////////////////////////////////////////////////
#define	ICP_OK			"OK"	//++	++		++
#define ICP_ERR			"ERR"	//++	++		++
#define ICP_EXIT		"BYE"	//++	++		++				����
#define ICP_HALT		"HAL"	//++							��λ
//////////////////////////////////////////////////////////////////////
#define ICP_HAND		"HND"	//++							��ϯ��
#define ICP_RING		"RNG"	//++							������ʾ
#define ICP_DIAL		"DIA"	//++							��ϯ���
#define ICP_ROUT		"SVR"	//++							ҵ��ת��
#define ICP_CTOC		"CTC"	//++							ת����ϯ
#define ICP_RECD		"REC"	//++							¼��
#define ICP_LISN		"LIS"	//++							����
#define ICP_WALK		"WAR"	//++							�������
#define ICP_WAIT		"WAT"	//++							�ȴ�����
//////////////////////////////////////////////////////////////////////
#define ICP_SYNC		"SYN"	//		++						ͬ���۷�
#define ICP_BIND		"BND"	//		++						�ƷѰ�
#define ICP_TALK		"TAK"	//++	++						ͨ����ʼ
#define ICP_RED			"RED"	//		++						�ͷѸ澯
//////////////////////////////////////////////////////////////////////
#define ICP_USER		"USR"	//				++				ע�������ϯ

//////////////////////////////////////////////////////////////////////
#define ICP_ACHN		"ACH"	// ���ӿɿ�ͨ��
#define ICP_ECHN		"ECH"	// ����ͨ��״̬

//
// �����붨��
//

#define ICP_SUCCESS			0
#define ICP_ERROR			4000
#define ICP_NOUSER			4001
#define	ICP_NOPAY			4002
#define ICP_CALLBACK		8888

#endif // !defined(AFX_IPACKET_H__C068F946_E633_493B_AE26_1037276AFC8C__INCLUDED_)
