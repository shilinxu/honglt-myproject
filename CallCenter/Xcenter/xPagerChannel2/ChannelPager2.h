//
// ChannelPager2.h
//

#if !defined(AFX_CHANNELPAGER2_H__771B289D_4128_429E_BC16_025C8CB738D3__INCLUDED_)
#define AFX_CHANNELPAGER2_H__771B289D_4128_429E_BC16_025C8CB738D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"

//
// definitions of file name
//

#define	FILE_PAGE_PLYLVWRD	"page.plylvwrd.pcm"

class CPagerChannel2 : public CChannel  
{
// Construction
public:
	static CChannel* Create(int nChan, LPCTSTR xFeeCode);
	CPagerChannel2(int nChan, LPCTSTR xFeeCode = "1022");
	virtual ~CPagerChannel2();

// Attributes
protected:
	_RecordsetPtr	m_pRecordset;
protected:
	struct state	FwdPager;
	struct state	PlyLvWord;
	
// Operations
protected:
	virtual void	Setup();
	int				PlayNavFile(int nChan, LPCTSTR xNavFile, BOOL bTermDtmf = TRUE);
protected:
	int				bkIdle_init(EVTBLK *evtblk);
	int				FeeLogin_init(EVTBLK *evtblk);
	struct state*	FeeLogin_cmplt(EVTBLK *evtblk);
	int				RcvTalk_init(EVTBLK *evtblk);

	int				FwdPager_init(EVTBLK *evtblk);
	struct state*	FwdPager_cmplt(EVTBLK *evtblk);
	int				PlyLvWord_init(EVTBLK *evtblk);
	struct state*	PlyLvWord_cmplt(EVTBLK *evtblk);
};

#endif // !defined(AFX_CHANNELPAGER2_H__771B289D_4128_429E_BC16_025C8CB738D3__INCLUDED_)
