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

#define FILE_MAIL_PLYUFAX		"mail.plyufax.pcm"

class CPagerChannel : public CChannel  
{
// Construction
protected:
	CPagerChannel(int nChan, LPCTSTR xFeeCode = "9000");
	virtual ~CPagerChannel();

// Attributes
protected:
	DWORD			m_nPagerId;
	BOOL			m_bPending;
	_RecordsetPtr	m_pRecordset;
protected:
	struct state	PlyFxByte;

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
	CChannel*		LookOut(int nChType);
	BOOL			UpdateState(LPCTSTR xState);
	virtual BOOL	OnBindPacket(CBindPacket* pPacket);
protected:
	int				bkIdle_init(EVTBLK *evtblk);
	int				FeeLogin_init(EVTBLK *evtblk);
	struct state*	FeeLogin_cmplt(EVTBLK *evtblk);
	struct state*	WaitDial_cmplt(EVTBLK *evtblk);
	int				RcvTalk_init(EVTBLK *evtblk);

	int				PlyFxByte_init(EVTBLK *evtblk);
	struct state*	PlyFxByte_cmplt(EVTBLK *evtblk);
};

#endif // !defined(AFX_CHANNELPAGER_H__8650DF6F_7611_4671_A5D5_E3A4636E905C__INCLUDED_)
