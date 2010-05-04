//
// Settings.cpp
//

#include "stdafx.h"
#include "Xgateway.h"
#include "Settings.h"
#include "Registry.h"

#include "RenderGraph.h"

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
	Add( _T(".Debug"), &General.Debug, FALSE );
	Add( _T(".DebugLog"), &General.DebugLog, FALSE );
	Add( _T(".TimeOut"), &General.TimeOut, 30 );
	
	Add( _T("Database.Host"), &Database.Host, "." );
	Add( _T("Database.Database"), &Database.Database, "CallCenter" );
	Add( _T("Database.UserID"), &Database.UserID, "sa" );
	Add( _T("Database.Password"), &Database.Password, "" );
	
	Add( _T("Network.Host"), &Network.Host, "自动" );
	Add( _T("Network.Port"), &Network.Port, 9960 );
	
	Add( _T("Centric.Local"), &Centric.Local, "\\\\.\\Pipe\\Xgateway" );
	Add( _T("Centric.Remote"), &Centric.Remote, "\\\\.\\Pipe\\Xcenter" );
	Add( _T("Centric.UserId"), &Centric.UserId, "Guest" );
	Add( _T("Centric.Password"), &Centric.Password, "" );
	
	Add( _T("Audio.Input"), &Audio.Input, NULL );
	Add( _T("Audio.Render"), &Audio.Render, NULL );
	Add( _T("Audio.Port"), &Audio.Port, 6640 );
	
	Add( _T("Video.Input"), &Video.Input, NULL );
	Add( _T("Video.Port"), &Video.Port, 6641 );
	
	Add( _T("Record.PathRec"), &Record.PathRec, "." );
	Add( _T("Record.RecFile"), &Record.RecFile, "<USER>\\<DATE>\\<TIME>-<REMOTE>.AVI" );
	Add( _T("Record.RecMask"), &Record.RecMask, RecordPayload(PayloadMask_Audio) );
	
//	Add( _T("Monitor.Port"), &Monitor.Port, 6642 );
	
#ifdef _BEST
	Add( _T("Network.PortDebug"), &Network.Port, 9980 );
	Add( _T("Centric.LocalDebug"), &Centric.Local, "\\\\.\\Pipe\\Xgateway" );
	Add( _T("Centric.RemoteDebug"), &Centric.Remote, "\\\\.\\Pipe\\Xcenter" );
	
	Add( _T("Audio.PortDebug"), &Audio.Port, 8640 );
	Add( _T("Video.PortDebug"), &Video.Port, 8641 );
//	Add( _T("Monitor.PortDebug"), &Monitor.Port, 8642 );
	
	// Network.Port
	
	Item* pItem = GetSetting( _T("Network.Port") );
	POSITION pos = m_pItems.Find( pItem );
	
	if ( pos ) m_pItems.RemoveAt( pos );
	if ( pItem ) delete pItem;
	
	// Centric
	
	pItem = GetSetting( _T("Centric.Local") );
	pos = m_pItems.Find( pItem );
	
	if ( pos ) m_pItems.RemoveAt( pos );
	if ( pItem ) delete pItem;
	
	pItem = GetSetting( _T("Centric.Remote") );
	pos = m_pItems.Find( pItem );
	
	if ( pos ) m_pItems.RemoveAt( pos );
	if ( pItem ) delete pItem;
	
	// Audio.Port
	
	pItem = GetSetting( _T("Audio.Port") );
	pos = m_pItems.Find( pItem );
	
	if ( pos ) m_pItems.RemoveAt( pos );
	if ( pItem ) delete pItem;
	
	// Video.Port
	
	pItem = GetSetting( _T("Video.Port") );
	pos = m_pItems.Find( pItem );
	
	if ( pos ) m_pItems.RemoveAt( pos );
	if ( pItem ) delete pItem;
/*	
	// Monitor.Port
	
	pItem = GetSetting( _T("Monitor.Port") );
	pos = m_pItems.Find( pItem );
	
	if ( pos ) m_pItems.RemoveAt( pos );
	if ( pItem ) delete pItem;
	*/
#endif
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
	
	if ( Network.Host.CompareNoCase( _T("自动") ) == 0 )
		Network.Host.Empty();
	
	CreateDirectory( General.Path + _T("\\Data"), NULL );
	CreateDirectory( General.Path + _T("\\Guide"), NULL );
}

void CSettings::Save()
{
	if ( Network.Host.IsEmpty() ) Network.Host = _T("自动");
	
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