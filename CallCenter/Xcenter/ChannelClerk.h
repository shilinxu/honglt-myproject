//
// ChannelClerk.h
//

#if !defined(AFX_CHANNELCLERK_H__E405680E_DEAC_4132_A344_703DB6E64F98__INCLUDED_)
#define AFX_CHANNELCLERK_H__E405680E_DEAC_4132_A344_703DB6E64F98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"
class CClerkWnd;

class CClerkChannel : public CChannel  
{
	friend class CClerkWnd;
	
// Construction
public:
	CClerkChannel(int nChan, LPCTSTR xFeeCode = "9119");
	virtual ~CClerkChannel();

// Attributes
protected:
	LPVOID			LvWordPtr;
	DWORD			LvWordSize;
	CString			m_xClerkId;
	CClerkWnd*		m_pClerkWnd;
protected:
	struct state	PlyLvWord;

// Operations
public:
	static CClerkChannel* ConvertFrom(CChannel* pParent);
	virtual void	SetBuffer(LPVOID pBuf, DWORD nBufSize);
protected:
	virtual void	Setup();
	virtual void	Clone(CChannel* pParent);
protected:
	int				bkIdle_init(EVTBLK *evtblk);
	int				WaitDial_init(EVTBLK *evtblk);
	int				RcvTalk_init(EVTBLK *evtblk);
	struct state*	FeeLogin_cmplt(EVTBLK *evtblk);
	
	int				PlyLvWord_init(EVTBLK *evtblk);
	struct state*	PlyLvWord_cmplt(EVTBLK *evtblk);
};

#endif // !defined(AFX_CHANNELCLERK_H__E405680E_DEAC_4132_A344_703DB6E64F98__INCLUDED_)
