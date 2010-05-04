//
// Settings.h
//

#if !defined(AFX_SETTINGS_H__5E273589_33B1_4782_987E_8378CDF31B68__INCLUDED_)
#define AFX_SETTINGS_H__5E273589_33B1_4782_987E_8378CDF31B68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AFX_EXT_CLASS CSettings  
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
		CString		PathUsr;
		CString		PathFax;
		CString		PathMuc;
		CString		PathRec;
	} General;
	struct sDatabase
	{
		CString		Host;
		CString		Database;
		CString		UserID;
		CString		Password;
	} Database;
	struct sCentric
	{
		CString		Local;
		CString		Remote;
		CString		UserId;
		CString		Password;
	} Centric;
	struct sMaster
	{
		CString		Host;
		DWORD		Port;
		INT			TimeOut;
	} Master;
	struct sMonitor
	{
		CString		Host;
		DWORD		Port;
		CString		IpAddr;
	} Monitor;
	struct sNeighbour
	{
		CString		In;
		CString		Out;
	} Neighbour;
	struct sMeeting
	{
		INT			MaxOutTime;
		INT			CallSpace;
		CString		RecFile;
		CString		RecPath;
	} Meeting;
	struct sUser
	{
		BOOL		RecType;
		CString		RecFile;
		CString		RecTrunk;
		BOOL		UserBusy;
		BOOL		MaxRing;
		BOOL		NoUser;
		CString		UserTime;
		CString		Host;
		DWORD		Port;
	} User;
	struct sTrunk
	{
		INT			TimeOut;
		BOOL		InLimit;
		CString		InLimited;
		CString		LookOut;
		CString		OutPhone;
		BOOL		OutLimit;
		INT			OutLimited;
		CString		OutReserved;
		CString		RecFile;
	} Trunk;
	struct sClerk
	{
		INT			Interval;
		CString		TxCallerId;
	} Clerk;	// default...
	struct sP2job
	{
		CString		PagStart;
		CString		PagClose;
	} P2job;	// default...
	struct sTest
	{
		BOOL		Enable;
		CString		Caller;
		CString		Phone;
	} Test;
	struct sProfit
	{
		BOOL		PlyNumb;
		CString		SeekNum;
		INT			MinGivSec;
		INT			MaxGivLong;
	} Profit;
	struct sPlay
	{
		BOOL		OnlyUser;
		CString		OnlyUserId;
	} Play;
	struct sPager
	{
		BOOL		NavPlay;
		CString		NavOffset;
		INT			PageExpire;
		INT			PageRetry;
		INT			PageInterval;
	} Pager;
	struct sTricker
	{
		BOOL		Enable;
		CString		TxCaller;
		CString		PhoneNum;
		CString		ExceptNum;
	} Tricker;

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

extern AFX_EXT_CLASS CSettings Settings;

#endif // !defined(AFX_SETTINGS_H__5E273589_33B1_4782_987E_8378CDF31B68__INCLUDED_)
