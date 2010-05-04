//
// Settings.cpp
//

#include "stdafx.h"
#include "Xmaster.h"
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
	Add( _T(".Debug"), &General.Debug, FALSE );
	Add( _T(".MaxReadyOut"), &General.MaxReadyOut, 100 );
	
	Add( _T("Database.Host"), &Database.Host, "127.0.0.1" );
	Add( _T("Database.Database"), &Database.Database, "CallCenter" );
	Add( _T("Database.UserID"), &Database.UserID, "sa" );
	Add( _T("Database.Password"), &Database.Password, "" );
	
	Add( _T("Network.Host"), &Network.Host, "127.0.0.1" );
	Add( _T("Network.Port"), &Network.Port, 9901 );
	Add( _T("Network.MaxLength"), &Network.MaxLength, 3 * 3600 );
	Add( _T("Network.ShortFree"), &Network.ShortFree, 0 );
	
	Add( _T("FreeTry.Enable"), &FreeTry.Enable, TRUE );
	Add( _T("FreeTry.GivenValue"), &FreeTry.GivenValue, 500 );
	Add( _T("FreeTry.UserHeader"), &FreeTry.UserHeader, "911" );
	Add( _T("FreeTry.UserLength"), &FreeTry.UserLength, 8 );
	
	Add( _T("Block.UsrTime"), &Block.UsrTime, "00:00" );
	Add( _T("Block.UsrDate"), &Block.UsrDate, 1 );
	Add( _T("Block.UsrCycle"), &Block.UsrCycle, 31 );
	
	Add( _T("GivenTry.Enable"), &GivenTry.Enable, TRUE );
	Add( _T("GivenTry.LocalId"), &GivenTry.LocalId, "96031" );
	Add( _T("GivenTry.MaxLength"), &GivenTry.MaxLength, 90 );
	
	Add( _T("Payer.PayStart"), &Payer.PayStart, "08:30" );
	Add( _T("Payer.PayClose"), &Payer.PayClose, "20:00" );
	Add( _T("Payer.PayBalan"), &Payer.PayBalan, 10 );
	Add( _T("Payer.PcmPager"), &Payer.PcmPager, "prompt.nomorepay.pcm" );
	Add( _T("Payer.RedEnable"), &Payer.RedEnable, TRUE );
	Add( _T("Payer.RedBalan"), &Payer.RedBalan, 0.60 );
	
	Add( _T("SubSm.UsrTime"), &SubSm.UsrTime, "00:00" );
	Add( _T("SubSm.UsrDate"), &SubSm.UsrDate, 1 );
	Add( _T("SubSm.UsrRule"), &SubSm.UsrRule, "" );
	Add( _T("SubSm.ReBack"), &SubSm.ReBack, TRUE );
	Add( _T("SubSm.Cookie"), &SubSm.Cookie, 200801L );
	
	Add( _T("Record.Enable"), &Record.Enable, FALSE );
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

//	CreateDirectory( General.Path + _T("\\Data"), NULL );
}

void CSettings::Save()
{
	CRegistry pRegistry;

	if ( Network.Host.IsEmpty() ) Settings.Network.Host = _T("自动");
	
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