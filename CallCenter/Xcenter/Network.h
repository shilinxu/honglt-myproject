//
// Network.h
//

#if !defined(AFX_NETWORK_H__3C174927_E261_4F3E_8CFC_274E5B8E40FE__INCLUDED_)
#define AFX_NETWORK_H__3C174927_E261_4F3E_8CFC_274E5B8E40FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CChannel;

class AFX_EXT_CLASS CNetwork
{
// Construction
public:
	CNetwork();
	virtual ~CNetwork();

// Attributes
public:
	CEvent			m_pWakeup;
	CMutex			m_pSection;
	BOOL			m_bConnected;
	int				m_nMaxChan;
protected:
	HANDLE			m_hThread;
	volatile BOOL	m_bThread;
	CMap<int, int&, CChannel*, CChannel*&> m_pList;

// Operations
public:
	BOOL			Setup();
	void			Release();
	BOOL			Connect();
	void			Disconnect();
public:
	BOOL			IsConnected() const;
	int				IsWellConnected() const;
	CChannel*		FindChannel(int nChan) const;
	CChannel*		SetChannel(CChannel* pChannel);
	CChannel*		MakeChannel(int nChan, LPCTSTR xFeeCode);
public:
	CChannel*		LookOut(LPCTSTR xLinkPcm);
	CChannel*		LookOutPcm(LPCTSTR xLinkPcm);
	CChannel*		LookOutPhs(LPCTSTR xRemoteId);
	CChannel*		LookOutPhs(LPCTSTR xRemoteId, LPCTSTR xFeeNumber);
	int				BlockRemote(LPCTSTR xRemotePcm);
	BOOL			VerifyIncoming(int nLinkCh);
protected:
	int				DeflatePcm(WORD nLinkId);
	CChannel*		LookOutCh(int nChFirst, int nChTotal);
	CChannel*		LookOutPcm(WORD nLinkId, int nChCount = 30);
protected:
	void			StartThread();
	void			StopThread();
	BOOL			RunPolling();
	BOOL			RunChannel();
protected:
	CChannel*		FromHandle(int nChan, LPCTSTR xFeeCode, LPCTSTR xInstance);
	static	UINT	ThreadStart(LPVOID pParam);
	virtual	BOOL	OnRun();
private:
	BOOL			HavingText(LPCTSTR lpszText, LPCTSTR lpszValue) const;

#ifdef NPICKUP
#define EVTBLK				MESSAGE_INFO
public:
	void		PutBlock(EVTBLK* evtblk);
	BOOL		ReadBlock(EVTBLK* evtblk);
#endif
};

extern AFX_EXT_CLASS CNetwork Network;

typedef enum
{
	CHN_ANALOG		=0,
	//CHN_INTER		=1,
	CHN_USER		=2,
	//CHN_REC		=3,
	CHN_SS1			=4,
	CHN_FAX			=5,
	CHN_TUP			=6,
	CHN_ISDN_USER	=7,		
	CHN_ISDN_NET	=8,
	CHN_SOFTFAX		=9,
	//CHN_MAGNET	=10,
	CHN_ISUP		=11,
	CHN_UNKNOWN,
} EChannel;

#endif // !defined(AFX_NETWORK_H__3C174927_E261_4F3E_8CFC_274E5B8E40FE__INCLUDED_)
