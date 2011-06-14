//
// Settings.cpp
//

#include "stdafx.h"
#include "MakeReport.h"
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
	Add( _T(".调试"), &General.Debug, FALSE );
	Add( _T(".写日志"), &General.DebugLog, FALSE );
	Add( _T(".计算栏目费用"), &General.Increment, FALSE );
	Add( _T(".自动运行联通帐务"), &General.AutoUnicom, FALSE );

	Add( _T("数据库.服务器"), &Database.Host, "." );
	Add( _T("数据库.数据库"), &Database.Database, "DataCenter" );
	
	Add( _T("中国联通.地址"), &Unicom.Host, _T("127.0.0.1") );
	Add( _T("中国联通.端口"), &Unicom.Port, 21 );
	Add( _T("中国联通.用户名"), &Unicom.UserId, _T("test") );
	Add( _T("中国联通.密码"), &Unicom.Password, _T("test") );
	Add( _T("中国联通.路径"), &Unicom.PathUsr, _T("/") );
	Add( _T("中国联通.上次出帐编号"), &Unicom.BookMark, 0 );
	Add( _T("中国联通.忽略忙的时段"), &Unicom.IgnoreHour, _T("18,19,20,21,22,23,0") );
	Add( _T("中国联通.查询间隔(秒)"), &Unicom.Interval, 60 );
	Add( _T("中国联通.通话时长临时调整"), &Unicom.TalkTemp, 0 );
	Add( _T("中国联通.测试"), &Telecom.Test, FALSE );
	Add( _T("中国联通.测试号码"), &Telecom.TestPhone, _T("") );
	
	Add( _T("中国电信.地址"), &Telecom.Host, _T("127.0.0.1") );
	Add( _T("中国电信.端口"), &Telecom.Port, 21 );
	Add( _T("中国电信.用户名"), &Telecom.UserId, _T("test") );
	Add( _T("中国电信.密码"), &Telecom.Password, _T("test") );
	Add( _T("中国电信.路径"), &Telecom.PathUsr, _T("/") );
	Add( _T("中国电信.通话时长临时调整"), &Telecom.TalkTemp, 0 );
	Add( _T("中国电信.测试"), &Telecom.Test, FALSE );
	Add( _T("中国电信.测试号码"), &Telecom.TestPhone, _T("") );
	Add( _T("中国电信.起始出帐编号"), &Telecom.BookMark, 0 );
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
	for ( POSITION pos = m_pItems.GetHeadPosition() ; pos ; )
	{
		Item* pItem = (Item*)m_pItems.GetNext( pos );
		pItem->Load();
	}
	
	CreateDirectory( General.Path + _T("\\Data"), NULL );
}

void CSettings::Save()
{
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