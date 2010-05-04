//
// ChannelFax.h
//

#if !defined(AFX_CHANNELFAX_H__D7AE7C00_1DC1_4935_A184_5B3149E0D298__INCLUDED_)
#define AFX_CHANNELFAX_H__D7AE7C00_1DC1_4935_A184_5B3149E0D298__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"

class CFaxChannel : public CChannel  
{
// Construction
public:
	CFaxChannel(int nChan);
	virtual ~CFaxChannel();
	
// Attributes
protected:
	struct state	FaxPage;

// Operations
protected:
	struct state*	bkIdle_cmplt(EVTBLK* evtblk);
	
	int				FaxPage_init(EVTBLK *evtblk);
	struct state*	FaxPage_cmplt(EVTBLK *evtblk);
};

#endif // !defined(AFX_CHANNELFAX_H__D7AE7C00_1DC1_4935_A184_5B3149E0D298__INCLUDED_)
