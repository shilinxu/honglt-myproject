//
// Neighbour.h
//

#if !defined(AFX_NEIGHBOUR_H__64583D81_A0AD_4988_9E63_35B373122B2A__INCLUDED_)
#define AFX_NEIGHBOUR_H__64583D81_A0AD_4988_9E63_35B373122B2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Transfer.h"
class CHomeWnd;
class CChannel;

class CNeighbour : public CTransfer  
{
// Construction
public:
	CNeighbour();
	virtual ~CNeighbour();

// Attributes
protected:
	CHomeWnd*		m_pHomeWnd;
protected:
	//CMap<int, int&, CChannel*, CChannel*&> m_pList;
	CPtrList		m_pList;

// Operations
public:
	virtual BOOL	Connect();
	virtual void	Disconnect();
public:
	void			Clear();
	CChannel*		FindChannel(int nChan) const;
	CChannel*		SetChannel(CChannel* pChannel);
	void			AddClient(int nChType, int nChFirst, int nChLength);
	int				GetChString(int nChType, CString& xChType) const;
protected:
	virtual BOOL	Setup();
	virtual BOOL	OnRun();
	virtual BOOL	OnConnected();
	virtual void	OnPacket(IPacket* pPacket);
	virtual void	OnAchnPacket(IAchnPacket* pPacket);
	virtual void	OnEchnPacket(IEchnPacket* pPacket);
#ifdef _DEBUG
//	virtual IPacket*		ReadBuffer(CBuffer* pBuffer);
#endif

// List Access
public:
	inline POSITION GetIterator() const
	{
		return m_pList.GetHeadPosition();
	}

	inline CChannel* GetNext(POSITION& pos) const
	{
		return (CChannel*)m_pList.GetNext( pos );
	}

	inline int GetCount() const
	{
		return m_pList.GetCount();
	}
	
// Channel Suffixes
protected:
	static LPCTSTR m_pszChSuffix[];
};

extern CNeighbour Neighbour;

#endif // !defined(AFX_NEIGHBOUR_H__64583D81_A0AD_4988_9E63_35B373122B2A__INCLUDED_)
