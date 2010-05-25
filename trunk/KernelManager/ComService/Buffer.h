//
// Buffer.h
//

#if !defined (BUFFER_H__INCLUDED_)
#define BUFFER_H__INCLUDED_

#pragma once
AKM_BEGIN_VERSIONED_NAMESPACE_DECL

//
// Tools
//

#define SWAP_SHORT(x) ( ( ( (x) & 0xFF00 ) >> 8 ) + ( ( (x) & 0x00FF ) << 8 ) )
#define SWAP_LONG(x) ( ( ( (x) & 0xFF000000 ) >> 24 ) + ( ( (x) & 0x00FF0000 ) >> 8 ) + ( ( (x) & 0x0000FF00 ) << 8 ) + ( ( (x) & 0x000000FF ) << 24 ) )
#define SWAP_64(x) ( ( SWAP_LONG( (x) & 0xFFFFFFFF ) << 32 ) | SWAP_LONG( (x) >> 32 ) )

typedef std::string AKM_String;

class AKM_Export CBuffer
{
// Construction
public:
	CBuffer();
	virtual ~CBuffer();

// Attributes
public:
	BYTE*		m_pBuffer;
	DWORD		m_nLength;
	DWORD		m_nBuffer;
	
// Operations
public:
	void		Add(const void* pData, DWORD nLength);
	void		Insert(DWORD nOffset, const void* pData, DWORD nLength);
	void		Remove(DWORD nLength);
	void		Clear();
public:
	void		Print(LPCSTR pszText);
	void		Print(LPCWSTR pszText, UINT nCodePage = CP_ACP);
	DWORD		AddBuffer(CBuffer* pBuffer, DWORD nLength = 0xFFFFFFFF, BOOL bPeek = FALSE);
	void		AddReversed(const void* pData, DWORD nLength);
	void		Prefix(LPCSTR pszText);
	void		EnsureBuffer(DWORD nLength);
public:
	BOOL		Read(void* pData, DWORD nLength);
	AKM_String	ReadString(DWORD nBytes, UINT nCodePage = CP_ACP);
	BOOL    ReadLine(wstring& strLine, UINT nCodePage = CP_ACP, BOOL bPeek = FALSE);
//	BOOL    StartsWith(LPCSTR pszString, BOOL bRemove = FALSE);                      // Returns true if the buffer starts with this text
public:
	static void ReverseBuffer(const void* pInput, void* pOutput, DWORD nLength);
};

AKM_END_VERSIONED_NAMESPACE_DECL

#endif // BUFFER_H__INCLUDED_
