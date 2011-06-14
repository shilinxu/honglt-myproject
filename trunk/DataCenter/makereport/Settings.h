//
// Settings.h
//

#if !defined(AFX_SETTINGS_H__5E273589_33B1_4782_987E_8378CDF31B68__INCLUDED_)
#define AFX_SETTINGS_H__5E273589_33B1_4782_987E_8378CDF31B68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSettings  
{
// Construction
public:
	CSettings();
	virtual ~CSettings();

// Attributes
public:
	struct sGeneral
	{
		CString		Path;
		BOOL		Debug;
		BOOL		DebugLog;
		BOOL		Increment;
		BOOL		AutoUnicom;
		BOOL		AutoTelecom;
	} General;
	struct sDatabase
	{
		CString		Host;
		CString		Database;
	} Database;
	struct sUnicom
	{
		CString		Host;
		int			Port;
		CString		UserId;
		CString		Password;
		CString		PathUsr;
		DWORD		Interval;
		int			TalkTemp;		// 变更通话时长
		DWORD		BookMark;		// 查询书签
		CString		IgnoreHour;
		BOOL		Test;
		CString		TestPhone;
	} Unicom;
	struct sTelecom
	{
		CString		Host;
		int			Port;
		CString		UserId;
		CString		Password;
		CString		PathUsr;
		int			TalkTemp;		// 变更通话时长
		BOOL		Test;
		CString		TestPhone;
		DWORD		BookMark;		// 查询书签
	} Telecom;

protected:
	CPtrList	m_pItems;

public:
	class Item
	{
	public:
		Item(LPCTSTR pszName, DWORD* pDword, DOUBLE* pFloat, CString* pString);
		void	Load();
		void	Save();
	protected:
		CString GetString(LPCTSTR pszSection, LPCTSTR pszName, LPCTSTR pszDefault = NULL);
		int		GetInt(LPCTSTR pszSection, LPCTSTR pszName, int nDefault = 0);
		DWORD	GetDword(LPCTSTR pszSection, LPCTSTR pszName, DWORD dwDefault = 0);
		double	GetFloat(LPCTSTR pszSection, LPCTSTR pszName, double fDefault = 0.0f);
		BOOL	SetString(LPCTSTR pszSection, LPCTSTR pszName, LPCTSTR pszValue);
		BOOL	SetInt(LPCTSTR pszSection, LPCTSTR pszName, int nValue);
	public:
		CString		m_sName;
		DWORD*		m_pDword;
		DOUBLE*		m_pFloat;
		CString*	m_pString;
	};

// Operations
public:
	void	Load();
	void	Save();
	Item*	GetSetting(LPCTSTR pszName) const;
	Item*	GetSetting(LPVOID pValue) const;
protected:
	void	Setup();
	void	Add(LPCTSTR pszName, DWORD* pDword, DWORD nDefault);
	void	Add(LPCTSTR pszName, int* pDword, DWORD nDefault);
	void	Add(LPCTSTR pszName, DOUBLE* pFloat, DOUBLE nDefault);
	void	Add(LPCTSTR pszName, CString* pString, LPCTSTR pszDefault);
};

extern CSettings Settings;

#endif // !defined(AFX_SETTINGS_H__5E273589_33B1_4782_987E_8378CDF31B68__INCLUDED_)
