//
// Profiles.cpp
//

#include "stdafx.h"
#include "Xmaster.h"
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

	CString strXML;
	
	strXML = Settings.General.Path;
	int nLength = strXML.GetLength();

	if ( nLength > 0 && strXML.GetAt(nLength-1) != '\\' ) strXML += '\\';
	strXML += _T("xProfile.xml");
	
	if ( CXMLElement* pXML = CXMLElement::FromFile( strXML, TRUE ) )
	{
		CXMLElement* pXMLSub;
		
		if ( pXMLSub = pXML->GetElementByName( _T("Secure") ) )
		{
			m_pXMLSecure = pXMLSub->Clone();
		}
		
		if ( pXMLSub = pXML->GetElementByName( _T("Ruler") ) )
		{
			m_pXMLRuler = pXMLSub->Clone();
		}

		delete pXML; return TRUE;
	}

	return FALSE;
}

void CProfiles::Release()
{
	CSingleLock pLock( &m_pSection, TRUE );

	if ( m_pXMLSecure != NULL )
	{
		delete m_pXMLSecure; m_pXMLSecure = NULL;
	}
	
	if ( m_pXMLRuler != NULL )
	{
		delete m_pXMLRuler; m_pXMLRuler = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
// CProfiles construction

CProfiles::CProfiles()
{
	m_pXMLSecure	= NULL;
	m_pXMLRuler		= NULL;
}

CProfiles::~CProfiles()
{
	Release();
}

//////////////////////////////////////////////////////////////////////
// CProfiles Host lookup

BOOL CProfiles::IsDenied(IN_ADDR* pAddress)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	if ( ! m_pXMLSecure ) return FALSE;
	
	CString strValue;
	BOOL bFound = FALSE;
	CXMLAttribute* pAttri = NULL;
	
	for ( POSITION pos = m_pXMLSecure->GetElementIterator() ; pos ; )
	{
		CXMLElement* pMap = m_pXMLSecure->GetNextElement( pos );
		if ( ! pMap->IsNamed( _T("Map") ) ) continue;
		
		if ( pAttri = pMap->GetAttribute( _T("Host") ) )
		{
			strValue = pAttri->GetValue();
			if ( strValue.CompareNoCase( _T("任意") ) == 0
			  || inet_addr((LPCTSTR)strValue) == *(DWORD*)pAddress )
			{
				bFound = TRUE; break;
			}
		}
	}
	
	// 拒绝条件: 拒绝 && bFound ==> TRUE
	//         : !拒绝 && !Found ==> TRUE
	// 受理方法: 异或 = 0 ==> TRUE, 否则 FALSE
	CString xAction = m_pXMLSecure->GetAttributeValue( _T("Action") );
	if ( xAction.CompareNoCase( _T("拒绝") ) ^ bFound  ) return FALSE;
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CProfiles BlackList lookup

BOOL CProfiles::IsDenied(LPCTSTR xPhone)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	if ( ! m_pXMLRuler ) return FALSE;

	CXMLElement* pXML = m_pXMLRuler->GetElementByName( _T("BlackList") );
	if ( ! pXML ) return NULL;
	
	CXMLAttribute* pAttri = NULL;

	for ( POSITION pos = pXML->GetElementIterator() ; pos ; )
	{
		CXMLElement* pMap = pXML->GetNextElement( pos );
		if ( ! pMap->IsNamed( _T("Map") ) ) continue;
		
		if ( pAttri = pMap->GetAttribute( _T("ID") ) )
		{
			if ( _tcsistr(xPhone, pAttri->GetValue()) == xPhone )
			{
				return TRUE;
			}
		}
	}
	
	return FALSE;
}
