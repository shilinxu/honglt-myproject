//
// Profiles.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Profiles.h"
#include "Settings.h"
#include "XML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CProfiles Profiles;

//////////////////////////////////////////////////////////////////////
// CProfiles setup

BOOL CProfiles::Setup()
{
	Release();
	
	CSingleLock pLock( &m_pSection, TRUE );

	if ( CXMLElement* pXML = CXMLElement::FromFile( m_pszFile, TRUE ) )
	{
		this->AttachFrom( pXML );
#if 1
		CXMLElement* pXMLSub;
		
		if ( pXMLSub = pXML->GetElementByName( _T("User") ) )
		{
			m_pXMLUser = pXMLSub->Clone();
		}
		
		if ( pXMLSub = pXML->GetElementByName( _T("Corp") ) )
		{
			m_pXMLCorp = pXMLSub->Clone();
		}
		
		if ( pXMLSub = pXML->GetElementByName( _T("Inter") ) )
		{
			m_pXMLInter = pXMLSub->Clone();
		}
		
		if ( pXMLSub = pXML->GetElementByName( _T("Final") ) )
		{
			m_pXMLFinal = pXMLSub->Clone();
		}
		
		if ( pXMLSub = pXML->GetElementByName( _T("Analog") ) )
		{
			m_pXMLAnalog = pXMLSub->Clone();
		}
#endif
		delete pXML; return TRUE;
	}

	return FALSE;
}

void CProfiles::Release()
{
	CSingleLock pLock( &m_pSection, TRUE );

	this->DeleteAllAttributes();
	this->DeleteAllElements();
#if 1
	if ( m_pXMLUser != NULL )
	{
		delete m_pXMLUser; m_pXMLUser = NULL;
	}

	if ( m_pXMLCorp != NULL )
	{
		delete m_pXMLCorp; m_pXMLCorp = NULL;
	}

	if ( m_pXMLInter != NULL )
	{
		delete m_pXMLInter; m_pXMLInter = NULL;
	}

	if ( m_pXMLFinal != NULL )
	{
		delete m_pXMLFinal; m_pXMLFinal = NULL;
	}

	if ( m_pXMLAnalog != NULL )
	{
		delete m_pXMLAnalog; m_pXMLAnalog = NULL;
	}
#endif
}

//////////////////////////////////////////////////////////////////////
// CProfiles construction

CProfiles::CProfiles() : m_pszFile( _T("xProfile.xml") )
{
	m_pXMLUser	= NULL;
	m_pXMLCorp	= NULL;
	m_pXMLInter	= NULL;
	m_pXMLFinal	= NULL;
	m_pXMLAnalog= NULL;
}

CProfiles::~CProfiles()
{
	Release();
}

void CProfiles::AttachFrom(CXMLElement* pXML)
{
	this->m_sName = pXML->GetName();
	
	for ( POSITION pos = pXML->GetAttributeIterator() ; pos ; )
	{
		AddAttribute( pXML->GetNextAttribute( pos )->Clone() );
	}

	for ( pos = pXML->GetElementIterator() ; pos ; )
	{
		AddElement( pXML->GetNextElement( pos )->Clone() );
	}

	this->m_sValue = pXML->GetValue();
}

//////////////////////////////////////////////////////////////////////
// CProfiles key lookup

CXMLElement* CProfiles::operator [](LPCTSTR pszKey)
{
	return this->GetElementByName( pszKey );
}

CXMLElement* CProfiles::LookupKey(CXMLElement* pParent, LPCTSTR pszKey)
{
	return LookupKey(pParent, pszKey, _T("ID"));
}

CXMLElement* CProfiles::LookupKey(CXMLElement* pParent, LPCTSTR pszKey, LPCTSTR pszName)
{
	if ( ! pParent ) return NULL;

	CSingleLock pLock( &m_pSection, TRUE );

	for ( POSITION pos = pParent->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = pParent->GetNextElement( pos );

		if ( CXMLAttribute* pAttribute = pXML->GetAttribute( pszName ) )
		{
			if ( ! pAttribute->GetValue().CompareNoCase( pszKey ) ) return pXML;
		}
	}
	
	return NULL;
}

//////////////////////////////////////////////////////////////////////
// CProfiles attributes

BOOL CProfiles::IsPermitCalled(IN_ADDR* pAddress)
{
	CXMLElement* pParent = Profiles[ "User" ];
	
	if ( CXMLElement* pXML = Profiles.LookupKey(pParent, inet_ntoa(*pAddress), "Host") )
	{
		CXMLAttribute* pAttribute = pXML->GetAttribute( _T("Action") );
		if ( pAttribute == NULL ) return FALSE;
		
		CString strValue = pAttribute->GetValue();
		return strValue.CompareNoCase( _T("允许所有呼入") ) == 0;
	}
	
	return FALSE;
}

BOOL CProfiles::HavingText(LPCTSTR lpszText, LPCTSTR lpszValue)
{
	CHAR Ch;
	CHAR xField1[32], xField2[32];
	
	if ( _stscanf( lpszText, "(%[^)^]%c", xField1, &Ch ) == 2 )
	{
		return HavingText( xField1, lpszValue );
	}
	
	CString strValue = lpszText;
	
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		if ( _stscanf( str, "%[^-]%c%[^^]", xField1, &Ch, xField2 ) == 3 )
		{
			if ( _tcscmp( xField1, lpszValue ) <= 0 && _tcscmp( xField2, lpszValue ) >= 0 ) return TRUE;
		}
		else
		{
			if ( _tcsncmp( xField1, lpszValue, _tcslen( xField1 ) ) == 0 ) return TRUE;
			
			CHAR* sptr = _tcschr( xField1, '*' );
			if ( ! sptr && ! _tcsncmp(xField1, lpszValue, int(sptr - xField1)) ) return TRUE;
		}
	}
	
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CProfiles Corp lookup

CXMLElement* CProfiles::FindCorpId(LPCTSTR xFeeNumber)
{
	if ( ! m_pXMLCorp ) return NULL;
	
	CSingleLock pLock( &m_pSection, TRUE );
	
	for ( POSITION pos = m_pXMLCorp->GetElementIterator() ; pos ; )
	{
		CXMLElement* pMap = m_pXMLCorp->GetNextElement( pos );
		if ( ! pMap->IsNamed( _T("Map") ) ) continue;
		
		if ( pMap->GetAttributeValue( _T("Host") ) == xFeeNumber ) return pMap;
	}
	
	return NULL;
}

CXMLElement* CProfiles::FindCorpId(LPCTSTR xCorpId, INT xUserId)
{
	if ( ! _tcscmp(xCorpId, "0*") ) return FindInter( xUserId );
	
	CSingleLock pLock( &m_pSection, TRUE );
	
	if ( ! m_pXMLCorp ) return NULL;
	
	CString strValue;
	CXMLAttribute* pAttri = NULL;
	
	for ( POSITION pos = m_pXMLCorp->GetElementIterator() ; pos ; )
	{
		CXMLElement* pMap = m_pXMLCorp->GetNextElement( pos );
		if ( ! pMap->IsNamed( _T("Map") ) ) continue;
		
		if ( pMap->GetAttributeValue( _T("ID") ) != xCorpId ) continue;
		if ( ! (pAttri = pMap->GetAttribute( _T("UserId") )) ) continue;
		
		strValue = pAttri->GetValue();
		if ( ! strValue.CompareNoCase( _T("全部") ) ) return pMap;
		
		//
		for ( strValue += '|' ; strValue.GetLength() ; )
		{
			CString str	= strValue.SpanExcluding( _T("|;,") );
			strValue	= strValue.Mid( str.GetLength() + 1 );
			
			INT nCh1 = -1, nCh2 = -1;
			if ( _stscanf( str, "%i-%i", &nCh1, &nCh2 ) == 2 )
			{
				if ( xUserId >= nCh1 && xUserId <= nCh2 ) return pMap;
			}
			else
			{
				if ( xUserId == nCh1 ) return pMap;
			}
		}
		//
	}
	
	return NULL;
}

BOOL CProfiles::IsDenied(CXMLElement* pXML, LPCTSTR xRemoteId) const
{
	CString strValue = pXML->GetAttributeValue( "Limited" );
	if ( ! IsPermitOut( xRemoteId, strValue ) ) return TRUE;
	
	strValue = pXML->GetAttributeValue( "BlackList" );
	if ( IsBlackList( xRemoteId, strValue ) ) return TRUE;
	
	return FALSE;
}

BOOL CProfiles::IsPermitOut(LPCTSTR xRemoted, LPCTSTR xLimited) const
{
	CString strValue = xLimited;
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		CHAR Ch;
		CHAR xField1[21], xField2[21];
		int nField = _stscanf( str, "%[^-]%c%[^^]", xField1, &Ch, xField2 );
		
		if ( nField == 3 )
		{
			if (   _tcsncmp( xRemoted, xField1, _tcslen(xField1) ) >= 0
				&& _tcsncmp( xRemoted, xField2, _tcslen(xField2) ) <= 0 ) return TRUE;
		}
		else if ( nField == 1 )
		{
			if ( _tcsncmp( xRemoted, xField1, _tcslen(xField1) ) == 0 ) return TRUE;
		}
	}
	
	return FALSE;
}

BOOL CProfiles::IsBlackList(LPCTSTR xRemoted, LPCTSTR xBlkFile) const
{
	HANDLE hFile = CreateFile(	xBlkFile, GENERIC_READ, FILE_SHARE_READ, NULL,
								OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	
	if ( hFile == INVALID_HANDLE_VALUE ) return FALSE;
	
	BOOL bResult = IsBlackList( xRemoted, hFile );
	
	CloseHandle( hFile );
	return bResult;
}

BOOL CProfiles::IsBlackList(LPCTSTR xRemoted, HANDLE hFile) const
{
	DWORD nByte = GetFileSize( hFile, NULL );
	if ( nByte > 4096*1024 ) return FALSE;
	
	CString strText;
	ReadFile( hFile, strText.GetBuffer(nByte), nByte, &nByte, NULL );
	strText.ReleaseBuffer( nByte );
	
	if ( strText.Find(xRemoted) != -1 ) return TRUE;
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CProfiles Inter lookup

CXMLElement* CProfiles::FindInter(INT xUserId)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	if ( ! m_pXMLInter ) return NULL;
	
	CString strValue;
	CXMLAttribute* pAttri = NULL;
	
	for ( POSITION pos = m_pXMLInter->GetElementIterator() ; pos ; )
	{
		CXMLElement* pMap = m_pXMLInter->GetNextElement( pos );
		if ( ! pMap->IsNamed( _T("Map") ) ) continue;
		
		if ( ! (pAttri = pMap->GetAttribute( _T("ID") )) ) continue;
		
		strValue = pAttri->GetValue();
		if ( _ttoi(strValue) == xUserId ) return pMap;
	}
	
	return NULL;
}

INT CProfiles::FindInter(LPCTSTR xHost, LPCTSTR xField)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	if ( ! m_pXMLInter ) return NULL;
	
	CXMLAttribute* pAttri = NULL;
	
	for ( POSITION pos = m_pXMLInter->GetElementIterator() ; pos ; )
	{
		CXMLElement* pMap = m_pXMLInter->GetNextElement( pos );
		if ( ! pMap->IsNamed( _T("Map") ) ) continue;
		
		if ( ! (pAttri = pMap->GetAttribute( xField )) ) continue;
		if ( pAttri->GetValue().CompareNoCase( xHost ) ) continue;
		
		if ( ! (pAttri = pMap->GetAttribute( _T("ID") )) ) continue;
		return _ttoi( pAttri->GetValue() );
	}
	
	return -1;
}

//////////////////////////////////////////////////////////////////////
// CProfiles final lookup

CXMLElement* CProfiles::FindFinal(LPCTSTR xFeeCode, CString* xModule)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	if ( ! m_pXMLFinal ) return NULL;
	
	CString strValue;
	CXMLAttribute* pAttri = NULL;
	
	for ( POSITION pos = m_pXMLFinal->GetElementIterator() ; pos ; )
	{
		CXMLElement* pMap = m_pXMLFinal->GetNextElement( pos );
		if ( ! pMap->IsNamed( _T("Map") ) ) continue;
		
		strValue = pMap->GetAttributeValue( _T("Host") );
		for ( strValue += '|' ; strValue.GetLength() ; )
		{
			CString str	= strValue.SpanExcluding( _T("|;,") );
			strValue	= strValue.Mid( str.GetLength() + 1 );
			
			if ( str.CompareNoCase( xFeeCode ) ) continue;
			if ( xModule ) * xModule = pMap->GetAttributeValue( "Module" );
			return pMap;
		}
	}
	
	return NULL;
}

//////////////////////////////////////////////////////////////////////
// CProfiles Analog lookup

CXMLElement* CProfiles::FindAnalog(INT xLinkId, LPCTSTR xLocalId)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	if ( ! m_pXMLAnalog ) return NULL;
	
	CString strValue;
	CXMLAttribute* pAttri = NULL;
	
	for ( POSITION pos = m_pXMLAnalog->GetElementIterator() ; pos ; )
	{
		CXMLElement* pMap = m_pXMLAnalog->GetNextElement( pos );
		
		if ( ! pMap->IsNamed( _T("Map") ) ) continue;
		if ( ! (pAttri = pMap->GetAttribute( _T("LinkId") )) ) continue;
		if ( pMap->GetAttributeValue("LocalId").CompareNoCase(xLocalId) ) continue;
		
		strValue = pAttri->GetValue();
		if ( ! strValue.CompareNoCase( _T("全部") ) ) return pMap;
		
		//
		for ( strValue += '|' ; strValue.GetLength() ; )
		{
			CString str	= strValue.SpanExcluding( _T("|;,") );
			strValue	= strValue.Mid( str.GetLength() + 1 );
			
			INT nCh1 = -1, nCh2 = -1;
			if ( _stscanf( str, "%i-%i", &nCh1, &nCh2 ) == 2 )
			{
				if ( xLinkId >= nCh1 && xLinkId <= nCh2 ) return pMap;
			}
			else
			{
				if ( xLinkId == nCh1 ) return pMap;
			}
		}
		//
	}
	
	return NULL;
}
