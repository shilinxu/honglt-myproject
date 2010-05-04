//
// Skin.cpp
//

#include "StdAfx.h"
#include "ImageFile.h"

//////////////////////////////////////////////////////////////////////
// CImageFile construction

CImageFile::CImageFile()
{
	Default();
}

CImageFile::CImageFile(LPCTSTR pszFile)
{
	Default();

	m_hFile = CreateFile( pszFile, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
}

CImageFile::~CImageFile()
{
	if ( m_hFile ) CloseHandle( m_hFile );
}

void CImageFile::Default()
{
	m_hFile		= NULL;
}

//////////////////////////////////////////////////////////////////////
// CImageFile load from file

HBITMAP CImageFile::LoadBitmap(LPCTSTR pszFile)
{
	CImageFile pImageFile(pszFile);
	return pImageFile.LoadFromFile();
}

HBITMAP CImageFile::LoadFromFile()
{
	if ( ! IsFile() ) return NULL;
	
	HANDLE hMap = CreateFileMapping( m_hFile, NULL, PAGE_READONLY, 0, 0, NULL );
	
	DWORD nPosition = SetFilePointer( m_hFile, 0, NULL, FILE_CURRENT );
	DWORD nLength = GetFileSize( m_hFile, NULL );
	HBITMAP hBitmap = NULL;
	
	if ( LPCVOID pBuffer = MapViewOfFile( hMap, FILE_MAP_READ, 0, nPosition, nLength ) )
	{
		hBitmap = LoadFromMemory( pBuffer, nLength );
		UnmapViewOfFile( pBuffer );
	}
	
	CloseHandle( hMap );

	return hBitmap;
}

//////////////////////////////////////////////////////////////////////
// CImageFile load from Resource

HBITMAP CImageFile::LoadBitmap(HINSTANCE hInstance, UINT nResourceID, LPCTSTR pszType)
{
	HMODULE hModule = (HMODULE)hInstance;
	HRSRC hRes = FindResource( hModule, MAKEINTRESOURCE( nResourceID ), pszType );
	
	if ( hRes == NULL ) return NULL;
	
	DWORD nLength		= SizeofResource( hModule, hRes );
	HGLOBAL hMemory		= ::LoadResource( hModule, hRes );
	LPCVOID pBuffer		= (LPCVOID)LockResource( hMemory );

	CImageFile pImageFile;
	return pImageFile.LoadFromMemory( pBuffer, nLength );
}

HBITMAP CImageFile::LoadFromMemory(LPCVOID pBuffer, DWORD nLength, HDC hUseDC)
{
	LPSTR				hDIB;
	LPVOID				lpDIBBits;
	BITMAPFILEHEADER	bmfHeader;
	DWORD				bmfHeaderLen;

	bmfHeaderLen = sizeof(bmfHeader);
	strncpy( (LPSTR)&bmfHeader, (LPSTR)pBuffer, bmfHeaderLen );
	if ( bmfHeader.bfType != ((WORD) ('M' << 8) | 'B') ) return NULL;

	hDIB = (LPSTR)pBuffer + bmfHeaderLen;
	BITMAPINFOHEADER &bmiHeader = *(LPBITMAPINFOHEADER)hDIB ;
	BITMAPINFO &bmInfo = *(LPBITMAPINFO)hDIB ;
	int nColors = bmiHeader.biClrUsed ? bmiHeader.biClrUsed : 1 << bmiHeader.biBitCount; 
	if ( bmInfo.bmiHeader.biBitCount > 8 )
		lpDIBBits = (LPVOID)((LPDWORD)(bmInfo.bmiColors + bmInfo.bmiHeader.biClrUsed) + 
		((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
	else
		lpDIBBits = (LPVOID)(bmInfo.bmiColors + nColors);

	HDC hDC = hUseDC ? hUseDC : GetDC( 0 );

	return CreateDIBitmap( hDC, &bmiHeader, CBM_INIT, lpDIBBits, &bmInfo, DIB_RGB_COLORS );
}
