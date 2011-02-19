//
// Buffer.h
//

#if !defined (BUFFER_H__INCLUDED_)
#define BUFFER_H__INCLUDED_

#pragma once

//
// Tools
//

#define SWAP_SHORT(x) ( ( ( (x) & 0xFF00 ) >> 8 ) + ( ( (x) & 0x00FF ) << 8 ) )
#define SWAP_LONG(x) ( ( ( (x) & 0xFF000000 ) >> 24 ) + ( ( (x) & 0x00FF0000 ) >> 8 ) + ( ( (x) & 0x0000FF00 ) << 8 ) + ( ( (x) & 0x000000FF ) << 24 ) )
#define SWAP_64(x) ( ( SWAP_LONG( (x) & 0xFFFFFFFF ) << 32 ) | SWAP_LONG( (x) >> 32 ) )

class COM_SERVICE_CLASS CBuffer
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
#ifdef WIN32
	void		Print(LPCWSTR pszText, UINT nCodePage = CP_ACP);
#else
	void		Print(LPCWSTR pszText);
#endif // WIN32
	DWORD		AddBuffer(CBuffer* pBuffer, DWORD nLength = 0xFFFFFFFF, BOOL bPeek = FALSE);
	void		AddReversed(const void* pData, DWORD nLength);
	void		Prefix(LPCSTR pszText);
	void		EnsureBuffer(DWORD nLength);
public:
	BOOL		Read(void* pData, DWORD nLength);
//	CString		ReadString(DWORD nBytes, UINT nCodePage = CP_ACP);
//	BOOL		ReadLine(CString& strLine, UINT nCodePage = CP_ACP, BOOL bPeek = FALSE);
//	BOOL		StartsWith(LPCSTR pszString, DWORD nLength, BOOL bRemove = FALSE);
public:
	static void ReverseBuffer(const void* pInput, void* pOutput, DWORD nLength);
};

#endif // BUFFER_H__INCLUDED_
