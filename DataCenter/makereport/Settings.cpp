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
	Add( _T(".����"), &General.Debug, FALSE );
	Add( _T(".д��־"), &General.DebugLog, FALSE );
	Add( _T(".������Ŀ����"), &General.Increment, FALSE );
	Add( _T(".�Զ�������ͨ����"), &General.AutoUnicom, FALSE );

	Add( _T("���ݿ�.������"), &Database.Host, "." );
	Add( _T("���ݿ�.���ݿ�"), &Database.Database, "DataCenter" );
	
	Add( _T("�й���ͨ.��ַ"), &Unicom.Host, _T("127.0.0.1") );
	Add( _T("�й���ͨ.�˿�"), &Unicom.Port, 21 );
	Add( _T("�й���ͨ.�û���"), &Unicom.UserId, _T("test") );
	Add( _T("�й���ͨ.����"), &Unicom.Password, _T("test") );
	Add( _T("�й���ͨ.·��"), &Unicom.PathUsr, _T("/") );
	Add( _T("�й���ͨ.�ϴγ��ʱ��"), &Unicom.BookMark, 0 );
	Add( _T("�й���ͨ.����æ��ʱ��"), &Unicom.IgnoreHour, _T("18,19,20,21,22,23,0") );
	Add( _T("�й���ͨ.��ѯ���(��)"), &Unicom.Interval, 60 );
	Add( _T("�й���ͨ.ͨ��ʱ����ʱ����"), &Unicom.TalkTemp, 0 );
	Add( _T("�й���ͨ.����"), &Telecom.Test, FALSE );
	Add( _T("�й���ͨ.���Ժ���"), &Telecom.TestPhone, _T("") );
	
	Add( _T("�й�����.��ַ"), &Telecom.Host, _T("127.0.0.1") );
	Add( _T("�й�����.�˿�"), &Telecom.Port, 21 );
	Add( _T("�й�����.�û���"), &Telecom.UserId, _T("test") );
	Add( _T("�й�����.����"), &Telecom.Password, _T("test") );
	Add( _T("�й�����.·��"), &Telecom.PathUsr, _T("/") );
	Add( _T("�й�����.ͨ��ʱ����ʱ����"), &Telecom.TalkTemp, 0 );
	Add( _T("�й�����.����"), &Telecom.Test, FALSE );
	Add( _T("�й�����.���Ժ���"), &Telecom.TestPhone, _T("") );
	Add( _T("�й�����.��ʼ���ʱ��"), &Telecom.BookMark, 0 );
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