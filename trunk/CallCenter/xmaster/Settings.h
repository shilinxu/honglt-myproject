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
		DWORD		MaxReadyOut;
	} General;
	struct sDatabase
	{
		CString		Host;
		CString		Database;
		CString		UserID;
		CString		Password;
	} Database;
	struct sNetwork
	{
		CString		Host;
		DWORD		Port;
		DWORD		MaxLength;	// 计费最大时长
		DWORD		ShortFree;	// 超短话单免费时长
	} Network;
	struct sFreeTry
	{
		BOOL		Enable;
		int			GivenValue;
		CString		UserHeader;
		int			UserLength;
	} FreeTry;
	struct sBlock
	{
		CString		UsrTime;
		INT			UsrDate;
		INT			UsrCycle;
	} Block;
	struct sSubSm
	{
		CString		UsrTime;
		INT			UsrDate;
		CString		UsrRule;
		BOOL		ReBack;
		DWORD		Cookie;
	} SubSm;
	struct sGivenTry
	{
		BOOL		Enable;
		CString		LocalId;
		INT			MaxLength;
	} GivenTry;
	struct sPayer
	{
		CString		PayStart;
		CString		PayClose;
		INT			PayBalan;
		CString		PcmPager;
		BOOL		RedEnable;
		DOUBLE		RedBalan;
	} Payer;
	struct sRecord
	{
		BOOL		Enable;
	} Record;

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
