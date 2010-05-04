//
// Neighbour.h
//

#if !defined(AFX_NEIGHBOUR_H__34869C27_B106_4FDA_AD6C_60B5BBCB0F7F__INCLUDED_)
#define AFX_NEIGHBOUR_H__34869C27_B106_4FDA_AD6C_60B5BBCB0F7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Transfer.h"
class CNetworkWnd;

class CNeighbour : public CTransfer  
{
// Construction
public:
	CNeighbour(SOCKET hSocket, SOCKADDR_IN* pHost);
	virtual ~CNeighbour();

// Attributes
public:
	CNetworkWnd*	m_pHomeWnd;
protected:
	BOOL			m_bNavigated;
	DWORD			m_tNavigated;
	double			m_nSyncMoney;
	int				m_nSyncRepeat;
	DWORD			m_tExpensed;
protected:
	_RecordsetPtr	m_pRecordfee;
	_RecordsetPtr	m_pRecordset;
	_RecordsetPtr	m_pRecordusr;

// Operations
public:
	BOOL			IsNavigated() const;
public:
	virtual void	Setup();
	virtual void	Disconnect();
	virtual BOOL	OnRun();
//	virtual void	OnDropped(BOOL bError);
protected:
	virtual BOOL	OnPacket(IPacket* pPacket);
	virtual BOOL	OnBindPacket(IBindPacket* pPacket);
	virtual BOOL	OnExitPacket(IExitPacket* pPacket);
	virtual BOOL	OnTalkPacket(ITalkPacket* pPacket);
	virtual BOOL	OnSyncPacket(ISyncPacket* pPacket);
	virtual BOOL	OnNavigate(DWORD tNow);
	virtual BOOL	OnNavigate(DWORD tNow, DWORD* pnLength, double* pnExpense);
protected:
	BOOL			AddSession(LPCTSTR xLocaldId, LPCTSTR xUsrNumber, LPCTSTR xFeeNumber = NULL);
	int				ReadUsrState(LPCTSTR xTalkType);
	BOOL			ReadCallBack(double* pnExpense = NULL);
	BOOL			ReadUsrType(LPCTSTR xFeeCode, LPCTSTR xFeeType, double* pnExpense = NULL);
	BOOL			ReadFeeEntry(LPCTSTR xPhonNum, LPCTSTR xTalkType);
	BOOL			ReckonBill(DWORD* pnLength = NULL, double* pnExpense = NULL);
protected:
	BOOL			ChkUserState(LPCTSTR xUsrNumber, LPCTSTR xCallType);
	void			AddExpense(LPCTSTR xThirdId = NULL, LPCTSTR xTalkType = _T("CATK"), BOOL bUpdate = TRUE);
	void			WriteExpense(DWORD nLength, double nExpense, BOOL bUpdate = TRUE);
	void			WriteUserInfo(double nExpense);
	double			CalcUserBlance();
	double			CalcExpense(DWORD nLength, int nFeeRule);
	BOOL			CalcExpense(DWORD nLength, double& nExpense);
	int				CalcExpense(DWORD nLength, DWORD tExpense, double& nExpense);
	double			InfFixedFee(double nExpense);
	BOOL			CheckGivenTry();
	BOOL			ChockRedPay(double xBala);
	BOOL			ChockTalkRemote(LPCTSTR xPhone, LPCTSTR xCallType, LPCTSTR xFeeCode);
private:
	BOOL			ReadFeeType(LPCTSTR xFeeType, int* pnElapse, double* pnExpense) const;
	BOOL			IsUsrRegisted();

	virtual CPacket*	ReadBuffer(CBuffer* pBuffer, BOOL bTemp) {return NULL;}
};

#endif // !defined(AFX_NEIGHBOUR_H__34869C27_B106_4FDA_AD6C_60B5BBCB0F7F__INCLUDED_)
