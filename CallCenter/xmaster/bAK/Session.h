//
// Session.h
//

#if !defined(AFX_SESSION_H__34869C27_B106_4FDA_AD6C_60B5BBCB0F7F__INCLUDED_)
#define AFX_SESSION_H__34869C27_B106_4FDA_AD6C_60B5BBCB0F7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Transfer.h"
class CNetworkChild;

class CSession : public CTransfer  
{
	typedef enum
	{
		EProgressNull, EProgressReady,
		EProgressNavigate, EProgressComplete,
	} EProgress;
	
// Construction
public:
	CSession(SOCKET hSocket, SOCKADDR_IN* pHost);
	virtual ~CSession();

// Attributes
protected:
	CNetworkChild*	m_pOwner;
	EProgress		m_nProgress;
	BOOL			m_bNavigated;
	DWORD			m_tNavigated;
	double			m_nSyncMoney;
	int				m_nSyncRepeat;
protected:
	_RecordsetPtr	m_pRecordfee;
	_RecordsetPtr	m_pRecordset;
	_RecordsetPtr	m_pRecordusr;

// Operations
public:
	BOOL			IsOnline() const;
	BOOL			IsNavigated() const;
	void			SetOwner(CNetworkChild* pOwner);
public:
	virtual void	Disconnect();
	virtual BOOL	OnRun();
protected:
	virtual BOOL	OnPacket(IPacket* pPacket);
	virtual BOOL	OnBindPacket(IBindPacket* pPacket);
	virtual BOOL	OnExitPacket(IExitPacket* pPacket);
	virtual BOOL	OnTalkPacket(ITalkPacket* pPacket);
	virtual BOOL	OnSyncPacket(ISyncPacket* pPacket);
	virtual BOOL	OnNavigate(DWORD tNow, DWORD* pnLength = NULL, double* pnExpense = NULL);
protected:
	int				ReadUsrState(LPCTSTR xTalkType);
	BOOL			ReadCallBack(double* pnExpense = NULL);
	BOOL			ReadUsrType(LPCTSTR xFeeCode, LPCTSTR xFeeType, double* pnExpense = NULL);
	BOOL			ReadFeeEntry(LPCTSTR xPhonNum, LPCTSTR xTalkType);
	BOOL			ChkUserState(LPCTSTR xUsrNumber, LPCTSTR xCallType);
	void			AddExpense(LPCTSTR xThirdId = NULL, BOOL bUpdate = TRUE);
	double			WriteExpense(DWORD nLength, double nExpense);
	double			CalcUserBlance();
	double			CalcExpense(DWORD nLength);
	double			CalcExpense(DWORD nLength, double nUpFlow);
	double			InfFixedFee(double nExpense);
	BOOL			AddSession(LPCTSTR xLocaldId, LPCTSTR xUsrNumber, LPCTSTR xFeeNumber = NULL);
	BOOL			ReckonBill(DWORD* pnLength = NULL, double* pnExpense = NULL);
private:
	BOOL			ReadFeeType(LPCTSTR xFeeType, int* pnElapse, double* pnExpense) const;
	BOOL			IsUsrRegisted();
};

#endif // !defined(AFX_SESSION_H__34869C27_B106_4FDA_AD6C_60B5BBCB0F7F__INCLUDED_)
