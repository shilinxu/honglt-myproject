//
// CtrlNetworkMonitor.h
//

#if !defined(AFX_CTRLNETWORKMONITOR_H__A9034156_723E_4F2D_BCF3_73C2A3D9456B__INCLUDED_)
#define AFX_CTRLNETWORKMONITOR_H__A9034156_723E_4F2D_BCF3_73C2A3D9456B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CtrlText.h"

/////////////////////////////////////////////////////////////////////////////
// CNetworkChild window

class CNetworkMonitor : public CTextCtrl  
{
// Construction
public:
	CNetworkMonitor();
	virtual ~CNetworkMonitor();

// Attributes
public:

// Overrides
public:
	//{{AFX_VIRTUAL(CNetworkMonitor)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CNetworkMonitor)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_CTRLNETWORKMONITOR_H__A9034156_723E_4F2D_BCF3_73C2A3D9456B__INCLUDED_)
