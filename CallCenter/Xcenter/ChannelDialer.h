//
// ChannelDialer.h
//

#if !defined(AFX_CHANNELDIALER_H__C4ACE12D_5F6D_444A_BC96_F52FEA3B6E8D__INCLUDED_)
#define AFX_CHANNELDIALER_H__C4ACE12D_5F6D_444A_BC96_F52FEA3B6E8D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"

class CDialerChannel : public CChannel  
{
// Construction
public:
	CDialerChannel(int nChan, LPCTSTR xFeeCode = "2000");
	virtual ~CDialerChannel();

// Attributes
protected:
	LPVOID			BufferPtr;
	DWORD			BufferSize;
	
// Operations
protected:
	virtual void	Setup();
	virtual LPVOID	NewBuffer(DWORD nBufSize);
	virtual void	SetBuffer(LPVOID pBuf, DWORD nBufSize);
	virtual void	FreeBuffer();
	int				PlayBlob(int nChan, FieldPtr xPlyBlob, BOOL bTermDtmf = TRUE);
protected:
	BOOL			PlayEnterVox(LPCTSTR xPhone, int nChan);
protected:
	int				bkIdle_init(EVTBLK *evtblk);
	int				bkOffHk_init(EVTBLK *evtblk);
	struct state*	FeeLogin_cmplt(EVTBLK *evtblk);
	struct state*	WaitDial_cmplt(EVTBLK *evtblk);
	int				RcvTalk_init(EVTBLK *evtblk);
	struct state*	RcvTalk_cmplt(EVTBLK *evtblk);
};

#endif // !defined(AFX_CHANNELDIALER_H__C4ACE12D_5F6D_444A_BC96_F52FEA3B6E8D__INCLUDED_)
