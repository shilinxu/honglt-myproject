//
// Packet.h
//

#if !defined(AFX_PACKET_H__D7F1FCE7_3975_4D37_8D35_A9C15E344215__INCLUDED_)
#define AFX_PACKET_H__D7F1FCE7_3975_4D37_8D35_A9C15E344215__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBuffer;

// CPacket

class CPacket
{
// Construction
protected:
	virtual ~CPacket();

// Attributes
protected:
	volatile DWORD	m_nBuffer;

// Operations
public:
	inline	void	Release();
	inline	DWORD	GetSize() const;
public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength) = NULL;
	virtual int		EncodeBody(CBuffer* pBuffer) = NULL;
public:
	virtual CPacket*Copy(CPacket* pPacket) = NULL;
	virtual CPacket*Clone() = NULL;
public:
	virtual	CPacket*ParseMatch(BYTE* pBuffer, DWORD nLength) = NULL;
	virtual CString	ToString();
};

//////////////////////////////////////////////////////////////////////
// CPacket construction

AFX_INLINE CPacket::~CPacket()
{
}

//////////////////////////////////////////////////////////////////////
// CPacket operations

AFX_INLINE CString CPacket::ToString()
{
	return _T("");
}

void CPacket::Release()
{
	if ( this == NULL ) return;
	
	delete this;
}

DWORD CPacket::GetSize() const
{
	return m_nBuffer;
}

#endif // !defined(AFX_PACKET_H__D7F1FCE7_3975_4D37_8D35_A9C15E344215__INCLUDED_)
