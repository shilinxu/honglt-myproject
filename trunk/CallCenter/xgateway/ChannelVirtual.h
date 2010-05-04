//
// ChannelVirtual.h
//

#if !defined(AFX_CHANNELVIRTUAL_H__B1DEF5F1_E11B_42DF_A811_B2460749394F__INCLUDED_)
#define AFX_CHANNELVIRTUAL_H__B1DEF5F1_E11B_42DF_A811_B2460749394F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Pipeline.h"
#include "PacketBase.h"

class CVirtualChannel : public CPipeline  
{
// Construction
public:
	CVirtualChannel();
	virtual ~CVirtualChannel();
	
// Attributes
public:
	CPipeline*		m_pBuilder;
	
// Operations
public:
	virtual BOOL	OnRun();
protected:
	virtual BOOL	OnPacket(CPacket* pPacket);
	virtual BOOL	OnLookOutReq(CLookOutReq* pPacket);
};

#endif // !defined(AFX_CHANNELVIRTUAL_H__B1DEF5F1_E11B_42DF_A811_B2460749394F__INCLUDED_)
