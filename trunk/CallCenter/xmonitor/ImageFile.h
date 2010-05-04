//
// Skin.h
//

#if !defined(AFX_IMAGEFILE_H__E4027F91_1EB1_4B0D_82E2_C8DD1E7F054F__INCLUDED_)
#define AFX_IMAGEFILE_H__E4027F91_1EB1_4B0D_82E2_C8DD1E7F054F__INCLUDED_

#pragma once

class CImageFile
{
// Construction
public:
	CImageFile();
	CImageFile(LPCTSTR pszFile);
	virtual ~CImageFile(void);

// Attributes
protected:
	HANDLE	m_hFile;

// Operations
public:
	static HBITMAP LoadBitmap(LPCTSTR pszFile);
	static HBITMAP LoadBitmap(HINSTANCE hInstance, UINT nResourceID, LPCTSTR pszType);
protected:
	void	Default();
	HBITMAP	LoadFromFile();
	HBITMAP LoadFromMemory(LPCVOID pBuffer, DWORD nLength, HDC hUseDC = 0);

// inline
public:
	inline BOOL IsFile() const
	{
		return ( m_hFile != NULL );
	}
};

#endif // !defined(AFX_SKIN_H__E4027F91_1EB1_4B0D_82E2_C8DD1E7F054F__INCLUDED_)
