//
// Settings.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Settings.h"
#include "Registry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CSettings Settings;

//////////////////////////////////////////////////////////////////////
// CSettings setup

void CSettings::Setup()
{
	Add( _T(".Path"), &General.Path, General.Path );
	Add( _T(".Debug"), &General.Debug, FALSE );
	Add( _T(".DebugLog"), &General.DebugLog, FALSE );
	Add( _T(".PathUsr"), &General.PathUsr, "." );
	Add( _T(".PathFax"), &General.PathFax, "." );
	Add( _T(".PathMuc"), &General.PathMuc, "." );
	Add( _T(".PathRec"), &General.PathRec, "." );
	
	Add( _T("Database.Host"), &Database.Host, "127.0.0.1" );
	Add( _T("Database.Database"), &Database.Database, "CallCenter" );
	Add( _T("Database.UserID"), &Database.UserID, "sa" );
	Add( _T("Database.Password"), &Database.Password, "" );
	
	Add( _T("Centric.Local"), &Centric.Local, "\\\\.\\Pipe\\Xcenter" );
	Add( _T("Centric.Remote"), &Centric.Remote, "\\\\.\\Pipe\\Xgateway" );
	Add( _T("Centric.UserId"), &Centric.UserId, "Guest" );
	Add( _T("Centric.Password"), &Centric.Password, "" );
	
	Add( _T("Master.Host"), &Master.Host, "127.0.0.1" );
	Add( _T("Master.Port"), &Master.Port, 9901 );
	Add( _T("Master.TimeOut"), &Master.TimeOut, 5 );
	
	Add( _T("Monitor.Host"), &Monitor.Host, "127.0.0.1" );
	Add( _T("Monitor.Port"), &Monitor.Port, 9904 );
	Add( _T("Monitor.IpAddr"), &Monitor.IpAddr, "*" );
	
	Add( _T("Meeting.MaxOutTime"), &Meeting.MaxOutTime, 3 * 60 );
	Add( _T("Meeting.CallSpace"), &Meeting.CallSpace, 30 );
	Add( _T("Meeting.RecFile"), &Meeting.RecFile, "<REMOTE>-<DATE><TIME>.pcm" );
	Add( _T("Meeting.RecPath"), &Meeting.RecPath, "." );
	
	Add( _T("User.RecType"), &User.RecType, FALSE );
	Add( _T("User.RecTrunk"), &User.RecTrunk, "任意" );
	Add( _T("User.RecFile"), &User.RecFile, "<CHAN>\\<REMOTE>-<DATE><TIME>.pcm" );
	Add( _T("User.MaxRing"), &User.MaxRing, 15 );
	Add( _T("User.NoUser"), &User.NoUser, FALSE );
	Add( _T("User.UserBusy"), &User.UserBusy, FALSE );
	Add( _T("User.UserTime"), &User.UserTime, "0800-2200" );
	Add( _T("User.Host"), &User.Host, "127.0.0.1" );
	Add( _T("User.Port"), &User.Port, 9902 );
	
	Add( _T("Trunk.TimeOut"), &Trunk.TimeOut, 30 );
	Add( _T("Trunk.InLimit"), &Trunk.InLimit, FALSE );
	Add( _T("Trunk.InLimited"), &Trunk.InLimited, "" );
	Add( _T("Trunk.LookOut"), &Trunk.LookOut, "0" );
	Add( _T("Trunk.OutPhone"), &Trunk.OutPhone, "" );
	Add( _T("Trunk.OutLimit"), &Trunk.OutLimit, FALSE );
	Add( _T("Trunk.OutLimited"), &Trunk.OutLimited, 0 );
	Add( _T("Trunk.OutReserved"), &Trunk.OutReserved, "" );
	Add( _T("Trunk.RecFile"), &Trunk.RecFile, "<CHAN>\\<REMOTE>-<DATE><TIME>.pcm" );
	
	Add( _T("Clerk.Interval"), &Clerk.Interval, 60 );
	Add( _T("Clerk.TxCallerId"), &Clerk.TxCallerId, "057496031" );
	
	Add( _T("P2job.PagStart"), &P2job.PagStart, "08:00" );
	Add( _T("P2job.PagClose"), &P2job.PagClose, "20:00" );
	
	Add( _T("Test.Enable"), &Test.Enable, FALSE );
	Add( _T("Test.Caller"), &Test.Caller, "" );
	Add( _T("Test.Phone"), &Test.Phone, "" );
	
	Add( _T("Profit.PlyNumb"), &Profit.PlyNumb, TRUE );
	Add( _T("Profit.SeekNum"), &Profit.SeekNum, "0-0" );
	Add( _T("Profit.MinGivSec"), &Profit.MinGivSec, 30 );
	Add( _T("Profit.MaxGivLong"), &Profit.MaxGivLong, 30 );
	
	Add( _T("Play.OnlyUser"), &Play.OnlyUser, FALSE );
	Add( _T("Play.OnlyUserId"), &Play.OnlyUserId, "" );
	
	Add( _T("Pager.NavPlay"), &Pager.NavPlay, TRUE );
	Add( _T("Pager.NavOffset"), &Pager.NavOffset, "0-0" );
	Add( _T("Pager.PageExpire"), &Pager.PageExpire, 24 * 3600 );
	Add( _T("Pager.PageRetry"), &Pager.PageRetry, 3 );
	Add( _T("Pager.PageInterval"), &Pager.PageInterval, 3 * 60 );
	
	Add( _T("Tricker.Enable"), &Tricker.Enable, FALSE );
	Add( _T("Tricker.PhoneNum"), &Tricker.PhoneNum, "" );
	Add( _T("Tricker.TxCaller"), &Tricker.TxCaller, "" );
	Add( _T("Tricker.ExceptNum"), &Tricker.ExceptNum, "" );
}

//////////////////////////////////////////////////////////////////////
// CSettings construction

CSettings::CSettings()
{
	TCHAR szPath[128];
	GetModuleFileName( NULL, szPath, 128 );
	
	General.Path = szPath;
	if ( General.Path.ReverseFind( '\\' ) >= 0 )
		General.Path = General.Path.Left( General.Path.ReverseFind( '\\' ) );
	
	Setup();
}

CSettings::~CSettings()
{
	for ( POSITION pos = m_pItems.GetHeadPosition() ; pos ; )
	{
		delete (Item*)m_pItems.GetNext( pos );
	}
}

//////////////////////////////////////////////////////////////////////
// CSettings add items

void CSettings::Add(LPCTSTR pszName, DWORD* pDword, DWORD nDefault)
{
	m_pItems.AddTail( new Item( pszName, pDword, NULL, NULL ) );
	*pDword = nDefault;
}

void CSettings::Add(LPCTSTR pszName, int* pDword, DWORD nDefault)
{
	m_pItems.AddTail( new Item( pszName, (DWORD*)pDword, NULL, NULL ) );
	*pDword = nDefault;
}

void CSettings::Add(LPCTSTR pszName, DOUBLE* pFloat, DOUBLE nDefault)
{
	m_pItems.AddTail( new Item( pszName, NULL, pFloat, NULL ) );
	*pFloat = nDefault;
}

void CSettings::Add(LPCTSTR pszName, CString* pString, LPCTSTR pszDefault)
{
	m_pItems.AddTail( new Item( pszName, NULL, NULL, pString ) );
	if ( pszDefault ) *pString = pszDefault;
}

//////////////////////////////////////////////////////////////////////
// CSettings load

void CSettings::Load()
{
	CRegistry pRegistry;
	
	for ( POSITION pos = m_pItems.GetHeadPosition() ; pos ; )
	{
		Item* pItem = (Item*)m_pItems.GetNext( pos );
		pItem->Load();
	}
	
	if ( Master.Host.CompareNoCase( _T("自动") ) == 0 )
		Master.Host.Empty();
	if ( Monitor.Host.CompareNoCase( _T("自动") ) == 0 )
		Monitor.Host.Empty();
	if ( User.Host.CompareNoCase( _T("自动") ) == 0 )
		User.Host.Empty();

	CreateDirectory( General.Path + _T("\\Data"), NULL );
	CreateDirectory( General.Path + _T("\\Media"), NULL );
	CreateDirectory( General.Path + _T("\\Guide"), NULL );
	
	CreateDirectory( General.PathUsr, NULL );
	CreateDirectory( General.PathFax, NULL );
	CreateDirectory( General.PathRec, NULL );
	CreateDirectory( Meeting.RecPath, NULL );
}

void CSettings::Save()
{
	CRegistry pRegistry;
	
	if ( Master.Host.IsEmpty() ) Master.Host = _T("自动");
	if ( Monitor.Host.IsEmpty() ) Monitor.Host = _T("自动");
	if ( User.Host.IsEmpty() ) User.Host = _T("自动");
	
	for ( POSITION pos = m_pItems.GetHeadPosition() ; pos ; )
	{
		Item* pItem = (Item*)m_pItems.GetNext( pos );
		if ( pItem->m_sName != _T(".Path") ) pItem->Save();
	}
}

//////////////////////////////////////////////////////////////////////
// CSettings settings lookup

CSettings::Item* CSettings::GetSetting(LPCTSTR pszName) const
{
	for ( POSITION pos = m_pItems.GetHeadPosition() ; pos ; )
	{
		Item* pItem = (Item*)m_pItems.GetNext( pos );
		if ( pItem->m_sName.CompareNoCase( pszName ) == 0 ) return pItem;
	}
	
	return NULL;
}

CSettings::Item* CSettings::GetSetting(LPVOID pValue) const
{
	for ( POSITION pos = m_pItems.GetHeadPosition() ; pos ; )
	{
		Item* pItem = (Item*)m_pItems.GetNext( pos );
		if ( pItem->m_pDword == pValue ||
			 pItem->m_pFloat == pValue ||
			 pItem->m_pString == pValue ) return pItem;
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////
// CSettings::Item construction and operations

CSettings::Item::Item(LPCTSTR pszName, DWORD* pDword, DOUBLE* pFloat, CString* pString)
{
	m_sName		= pszName;
	m_pDword	= pDword;
	m_pFloat	= pFloat;
	m_pString	= pString;
}

void CSettings::Item::Load()
{
	CRegistry pRegistry;
	
	int nPos = m_sName.Find( '.' );
	if ( nPos < 0 ) return;

	if ( m_pDword )
	{
		*m_pDword = pRegistry.GetDword( m_sName.Left( nPos ), m_sName.Mid( nPos + 1 ), *m_pDword );
	}
	else if ( m_pFloat )
	{
		*m_pFloat = pRegistry.GetFloat( m_sName.Left( nPos ), m_sName.Mid( nPos + 1 ), *m_pFloat );
	}
	else
	{
		*m_pString = pRegistry.GetString( m_sName.Left( nPos ), m_sName.Mid( nPos + 1 ), *m_pString );
	}

}

void CSettings::Item::Save()
{
	CRegistry pRegistry;
	
	int nPos = m_sName.Find( '.' );
	if ( nPos < 0 ) return;

	if ( m_pDword )
	{
		pRegistry.SetInt( m_sName.Left( nPos ), m_sName.Mid( nPos + 1 ), *m_pDword );
	}
	else if ( m_pFloat )
	{
		CString str;
		str.Format( _T("%e"), *m_pFloat );
		pRegistry.SetString( m_sName.Left( nPos ), m_sName.Mid( nPos + 1 ), str );
	}
	else
	{
		pRegistry.SetString( m_sName.Left( nPos ), m_sName.Mid( nPos + 1 ), *m_pString );
	}
}