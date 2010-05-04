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
	CPagerChannel(int nChan, LPCTSTR xFeeCode = "2222");
	virtual ~CPagerChannel();

// Attributes
protected:
	DWORD			m_nPagerId;
	BOOL			m_bPending;
	_RecordsetPtr	m_pRecordset;
protected:
	struct state	PlyLvWord;
	struct state	RcvFnOper;

// Operations
public:
	static
	CChannel*		ConvertFrom(CChannel* pChannel, DWORD nTaskId);
	static
	CChannel*		ConvertFrom(CChannel* pChannel, _RecordsetPtr xRecordset);
	BOOL			LoadFrom(DWORD nTaskId);
	BOOL			LoadFrom(_RecordsetPtr xRecordset);
protected:
	virtual void	Setup();
	virtual HMODULE	Release();
	virtual BOOL	OnBindPacket(CBindPacket* pPacket);
protected:
//	CChannel*		LookOut(int nChType);
	BOOL			UpdateState(LPCTSTR xState);
	CString			ReadPagerType(LPCTSTR xPath, LPCTSTR xDefault) const;
	double			CalcUserBlance(LPCTSTR xUsrNumber);
	void			RecUsrBack(CHAR xKey);
protected:
	int				bkIdle_init(EVTBLK *evtblk);
	int				FeeLogin_init(EVTBLK *evtblk);
	struct state*	FeeLogin_cmplt(EVTBLK *evtblk);
	struct state*	WaitDial_cmplt(EVTBLK *evtblk);
	int				RcvTalk_init(EVTBLK *evtblk);

	int				PlyLvWord_init(EVTBLK *evtblk);
	struct state*	PlyLvWord_cmplt(EVTBLK *evtblk);
	
	int				RcvFnOper_init(EVTBLK *evtblk);
	struct state*	RcvFnOper_cmplt(EVTBLK *evtblk);
//virtual void	OnDialPacket(IDialPacket* pPacket);
};

#endif // !defined(AFX_CHANNELPAGER_H__8650DF6F_7611_4671_A5D5_E3A4636E905C__INCLUDED_)
