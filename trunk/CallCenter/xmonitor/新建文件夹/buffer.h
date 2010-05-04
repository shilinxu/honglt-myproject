//
// Buffer.h
//

#if !defined(AFX_BUFFER_H__B04E373F_79C2_4B66_98BE_E322D79F5D9E__INCLUDED_)
#define AFX_BUFFER_H__B04E373F_79C2_4B66_98BE_E322D79F5D9E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//
// Tools
//

#define SWAP_SHORT(x) ( ( ( (x) & 0xFF00 ) >> 8 ) + ( ( (x) & 0x00FF ) << 8 ) )
#define SWAP_LONG(x) ( ( ( (x) & 0xFF000000 ) >> 24 ) + ( ( (x) & 0x00FF0000 ) >> 8 ) + ( ( (x) & 0x0000FF00 ) << 8 ) + ( ( (x) & 0x000000FF ) << 24 ) )
#define SWAP_64(x) ( ( SWAP_LONG( (x) & 0xFFFFFFFF ) << 32 ) | SWAP_LONG( (x) >> 32 ) )

class CBuffer  
{
// Construction
public:
	CBuffer(DWORD* pLimit = NULL);
	virtual ~CBuffer();

// Attributes
public:
	BYTE*		m_pBuffer;
	DWORD		m_nLength;
	DWORD		m_nBuffer;
	
// Operations
public:
	void	Add(const void* pData, DWORD nLength);
	void	Insert(DWORD nOffset, const void* pData, DWORD nLength);
	void	Remove(DWORD nLength);
	void	Clear();
	void	Print(LPCSTR pszText);
	DWORD	AddBuffer(CBuffer* pBuffer, DWORD nLength = 0xFFFFFFFF, BOOL bPeek = FALSE);
	void	AddReversed(const void* pData, DWORD nLength);
	void	Prefix(LPCSTR pszText);
	void	EnsureBuffer(DWORD nLength);
public:
	CString	ReadString(DWORD nBytes, UINT nCodePage = CP_ACP);
	BOOL	ReadString(CString& strLine, BOOL bPeek = FALSE);
};

#endif // !defined(AFX_BUFFER_H__B04E373F_79C2_4B66_98BE_E322D79F5D9E__INCLUDED_)
