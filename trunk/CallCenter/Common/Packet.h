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
protected:
	CPacket(LPCTSTR sCommandId);
	virtual ~CPacket();

public:
	LPCTSTR 		CommandId;
protected:
	volatile DWORD	BufferSize;

public:
	inline	BOOL	IsCommandId(LPCTSTR sCommandId) const;
	inline	DWORD	GetBufferSize() const;
	inline	void	Release();
public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
protected:
	static int		ReadField(BYTE* pBuffer, LPTSTR sField, int nField, CHAR cPart = ' ');
	static CPacket*	ReadBuffer(BYTE* pBuffer, DWORD nLength);
};

// COkPacket

class COkPacket : public CPacket
{
public:
	COkPacket();
	virtual ~COkPacket();
};

// CErrPacket

class CErrPacket : public CPacket
{
public:
	CErrPacket(int nErrCode = 0);
	virtual ~CErrPacket();
	
public:
	int				m_nErrCode;

public:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
};

// CByePacket

class CByePacket : public CPacket
{
public:
	CByePacket();
	virtual ~CByePacket();
};

// inline access

BOOL CPacket::IsCommandId(LPCTSTR xCommandId) const
{
	return _tcscmp( CommandId, xCommandId ) == 0;
}

DWORD CPacket::GetBufferSize() const
{
	return BufferSize;
}

void CPacket::Release()
{
	if ( this == NULL ) return;
	
	delete this;
}

//
// Error types
//

enum{
	EError_Success		= 0,	// ³É¹¦
	EError_Unknown		= 8000,	// Î´Öª´íÎó
};

//
// Command types
//

#define	IOK				"OK"
#define	IBYE			"BYE"
#define	IERROR			"ERR"

#endif // !defined(AFX_PACKET_H__D7F1FCE7_3975_4D37_8D35_A9C15E344215__INCLUDED_)
