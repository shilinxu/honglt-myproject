//
// ChannelSentry.h
//

#if !defined(AFX_CHANNELSENTRY_H__6E08DC92_B772_4D10_836B_7DDC9C66323B__INCLUDED_)
#define AFX_CHANNELSENTRY_H__6E08DC92_B772_4D10_836B_7DDC9C66323B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"

class CSentryChannel : public CChannel  
{
// Construction
public:
	CSentryChannel(CXMLElement* pXML);
	virtual ~CSentryChannel();

// Attributes
protected:
	CXMLElement*	m_pXML;
	int				m_nTries;
	
// Operations
protected:
	virtual void	Disconnect(int nError = 0);
	virtual BOOL	LinkRestart();
protected:
	virtual BOOL	OnRun();
	virtual BOOL	OnConnected();
protected:
	BOOL			AlarmMachine();
	BOOL			RestartMachine(LPCTSTR lpszAction);
	BOOL			AlarmToShortMessage(CXMLElement* pXML);
};

#endif // !defined(AFX_CHANNELSENTRY_H__6E08DC92_B772_4D10_836B_7DDC9C66323B__INCLUDED_)
