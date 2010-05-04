//
// Profiles.cpp
//

#include "stdafx.h"
#include "Xmonitor.h"
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
		
		if ( pXMLSub = pXML->GetElementByName( _T("Monitor") ) )
		{
			m_pXMLMonitor = pXMLSub->Clone();
		}

		delete pXML; return TRUE;
	}

	return FALSE;
}

void CProfiles::Release()
{
	CSingleLock pLock( &m_pSection, TRUE );

	if ( m_pXMLMonitor != NULL )
	{
		delete m_pXMLMonitor; m_pXMLMonitor = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
// CProfiles construction

CProfiles::CProfiles()
{
	m_pXMLMonitor	= NULL;
}

CProfiles::~CProfiles()
{
	Release();
}

//////////////////////////////////////////////////////////////////////
// CProfiles Monitor lookup

CXMLElement* CProfiles::FindProcess(LPCTSTR xJobId, POSITION& posNext, CXMLElement** ppActionXML)
{
	CSingleLock pLock( &m_pSection, TRUE );
	if ( ! m_pXMLMonitor ) return NULL;
	
	CXMLElement* pAction = NULL;
	if ( ! ppActionXML ) ppActionXML = &pAction;
	
	*ppActionXML = m_pXMLMonitor->GetElementByName( _T("ShortMessage") );
	
	if ( CXMLElement* pXML = m_pXMLMonitor->GetElementByName( _T("Process") ) )
	{
		if ( ! posNext ) posNext = pXML->GetElementIterator();
		if ( ! posNext ) return NULL;
		
		CXMLElement* pMap = pXML->GetNextElement( posNext );
		if ( ! pMap->IsNamed( _T("Map") ) ) return posNext ? FindProcess( xJobId, posNext ) : NULL;
		
		if ( ! xJobId || ! _tcscmp(xJobId, _T("ÈÎÒâ")) ) return pMap;
		
		CXMLAttribute* pAttri = pMap->GetAttribute( "Host" );
		if ( pAttri && pAttri->GetValue() == xJobId ) return pMap;
		
		return posNext ? FindProcess( xJobId, posNext, ppActionXML ) : NULL;
	}
	
	return NULL;
}
