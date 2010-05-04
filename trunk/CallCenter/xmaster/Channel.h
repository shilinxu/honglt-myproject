//
// Channel.h
//

#if !defined(AFX_CHANNEL_H__1F006043_3BA6_4C63_8C05_BECA198384C8__INCLUDED_)
#define AFX_CHANNEL_H__1F006043_3BA6_4C63_8C05_BECA198384C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Transfer.h"
#include "PacketCore.h"
class CNetworkChild;

class CChannel : public CTransfer  
{
// Construction
public:
	CChannel(SOCKET hSocket, SOCKADDR_IN* pHost);
	CChannel(CChannel* pParent);
	virtual ~CChannel();

// Attributes
protected:
	BOOL			m_bNavigated;
	DWORD			m_tNavigated;
	CNetworkChild*	m_pOwner;
protected:
	_RecordsetPtr	m_pRecordfee;
	_RecordsetPtr	m_pRecordset;
	_RecordsetPtr	m_pRecordusr;

// Operations
public:
	virtual void	Setup();
	virtual void	Disconnect();
	virtual void	SetOwner(CNetworkChild* pOwner);
	virtual int		AttachTo(LPCTSTR xLocalId, LPCTSTR xUsrNumber, LPCTSTR xFeeNumber = NULL);
public:
	BOOL			IsNavigated() const;
protected:
	virtual BOOL	OnRun();
	virtual BOOL	OnHandshake();
	virtual BOOL	OnPacket(CPacket* pPacket);
	virtual BOOL	OnBindPacket(CBindPacket* pPacket);
	virtual BOOL	OnTalkPacket(CTalkPacket* pPacket);
	virtual BOOL	OnNavigated();
protected:
	virtual BOOL	OnBindPacketResp(int nErrCode);
	virtual BOOL	OnBindPacketResp(LPCTSTR xFeeCode);
protected:
	int				ReadUsrState(LPCTSTR xTalkType);
	BOOL			ReadCallBack(double* pnExpense = NULL);
	BOOL			ReadUsrType(LPCTSTR xFeeCode);
	BOOL			ReadFeeEntry(LPCTSTR xPhonNum, LPCTSTR xTalkType);
protected:
	BOOL			AddSession(LPCTSTR xLocalId, LPCTSTR xUsrNumber, LPCTSTR xFeeNumber = NULL);
	void			AddExpense(LPCTSTR xThirdId = NULL, BOOL bUpdate = TRUE);
	BOOL			ChkUserState(LPCTSTR xUsrNumber);
	BOOL			WriteExpense(DWORD nLength, double nExpense, BOOL bUpdate = TRUE);
	void			WriteUserInfo(double nExpense);
	double			CalcUserBlance();
	double			CalcExpense(DWORD nLength, int nFeeRule);
	BOOL			CalcExpense(DWORD nLength, double& nExpense);
	int				CalcExpense(DWORD nLength, DWORD tExpense, double& nExpense);
	double			InfFixedFee(double nExpense);
	BOOL			CheckGivenTry();
	BOOL			ChockRedPay(double xBala);
	BOOL			ChockFeeRemote(LPCTSTR xPhone, int nSkip = 1);
	BOOL			ChockFeeMonth(LPCTSTR xPhone, int nSkip = 1);
private:
	BOOL			ReadFeeType(LPCTSTR xFeeType, int* pnElapse, double* pnExpense) const;
protected:
	virtual CPacket*	ReadBuffer(CBuffer* pBuffer);
	HRESULT			GetSeqNumber(DWORD& xSeqNumber) const;
	BOOL			CompleteTask();
	
	friend class CNetworkChild;
};

#endif // !defined(AFX_CHANNEL_H__1F006043_3BA6_4C63_8C05_BECA198384C8__INCLUDED_)
