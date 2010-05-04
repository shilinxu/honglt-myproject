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
	} General;
	struct sDatabase
	{
		CString		Host;
		CString		Database;
	} Database;
	struct sNetwork
	{
		CString		Host;
		int			Port;
	} Network;
	struct sMonitor
	{
		BOOL		Sentry;
	} Monitor;

// Attributes : Item List
protected:
	CPtrList	m_pItems;
public:
	class Item
	{
	public:
		Item(LPCTSTR pszName, DWORD* pDword, DOUBLE* pFloat, CString* pString);
		void	Load();
		void	Save();
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
public:
protected:
	void	Setup();
	void	Add(LPCTSTR pszName, DWORD* pDword, DWORD nDefault);
	void	Add(LPCTSTR pszName, int* pDword, DWORD nDefault);
	void	Add(LPCTSTR pszName, DOUBLE* pFloat, DOUBLE nDefault);
	void	Add(LPCTSTR pszName, CString* pString, LPCTSTR pszDefault);
};

extern CSettings Settings;

#endif // !defined(AFX_SETTINGS_H__5E273589_33B1_4782_987E_8378CDF31B68__INCLUDED_)
