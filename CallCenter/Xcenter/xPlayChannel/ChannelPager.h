//
// ChannelPager.h
//

#if !defined(AFX_CHANNELPAGER_H__8650DF6F_7611_4671_A5D5_E3A4636E905C__INCLUDED_)
#define AFX_CHANNELPAGER_H__8650DF6F_7611_4671_A5D5_E3A4636E905C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"

//
// definitions of file name
//

#define	FILE_PAGE_RCVOPER		"page.rcvoper.pcm"

class CPagerChannel : public CChannel  
{
// Construction
protected:
	CPagerChannel(int nChan);
	virtual ~CPagerChannel();

// Attributes
protected:
	BOOL			m_bPending;
	_RecordsetPtr	m_pRecordset;
protected:
	struct state	PlyLvWord;
	struct state	RcvFnOper;

// Operations
public:
	static
	CChannel*		PagerFrom(CChannel* pParent, LPCTSTR, LPCTSTR, DWORD nReserved);
protected:
	virtual void	Setup();
	virtual HMODULE	Release();
	struct state*	FwdNotify(LPCTSTR xLocalId);
	virtual BOOL	OnBindPacket(CBindPacket* pPacket);
	virtual BOOL	OnPacket(CPacket* pPacket);
protected:
	BOOL			UpdateState(LPCTSTR xState);
	BOOL			LoadFrom(_RecordsetPtr xRecordset);
	CString			ReadPagerType(LPCTSTR xPath, LPCTSTR xDefault) const;
	double			CalcUserBlance(LPCTSTR xUsrNumber);
	void			RecUsrBack(CHAR xKey);
protected:
	int				bkIdle_init(EVTBLK *evtblk);
	int				FeeRebind_init(EVTBLK *evtblk);
	struct state*	FeeLogin_cmplt(EVTBLK *evtblk);
	struct state*	WaitDial_cmplt(EVTBLK *evtblk);
	int				RcvTalk_init(EVTBLK *evtblk);

	int				PlyLvWord_init(EVTBLK *evtblk);
	struct state*	PlyLvWord_cmplt(EVTBLK *evtblk);
	
	int				RcvFnOper_init(EVTBLK *evtblk);
	struct state*	RcvFnOper_cmplt(EVTBLK *evtblk);
};

#endif // !defined(AFX_CHANNELPAGER_H__8650DF6F_7611_4671_A5D5_E3A4636E905C__INCLUDED_)
