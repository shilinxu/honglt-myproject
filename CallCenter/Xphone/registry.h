//
// Registry.h
//

#if !defined(AFX_REGISTRY_H__1E30C9FF_8F5A_4C30_A1CF_B61126690128__INCLUDED_)
#define AFX_REGISTRY_H__1E30C9FF_8F5A_4C30_A1CF_B61126690128__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRegistry 
{
// Construction
public:
	CRegistry();
	~CRegistry();
	
// Operations
public:
	CString GetString(LPCTSTR pszSection, LPCTSTR pszName, LPCTSTR pszDefault = NULL);
	int		GetInt(LPCTSTR pszSection, LPCTSTR pszName, int nDefault = 0);
	DWORD	GetDword(LPCTSTR pszSection, LPCTSTR pszName, DWORD dwDefault = 0);
	double	GetFloat(LPCTSTR pszSection, LPCTSTR pszName, double fDefault = 0.0f);
public:
	BOOL	SetString(LPCTSTR pszSection, LPCTSTR pszName, LPCTSTR pszValue);
	BOOL	SetInt(LPCTSTR pszSection, LPCTSTR pszName, int nValue);
	BOOL	RemoveValue(LPCTSTR pszSection, LPCTSTR pszName);
	
// Implementation
protected:
	void	DisplayErrorMessageBox(DWORD nErrorCode);
};

#endif // !defined(AFX_REGISTRY_H__1E30C9FF_8F5A_4C30_A1CF_B61126690128__INCLUDED_)
