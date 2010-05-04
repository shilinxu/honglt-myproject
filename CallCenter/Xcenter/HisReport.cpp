//
// HisReport.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "HisReport.h"
#include "XML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CHisReport construction

CHisReport::CHisReport()
{
	m_strPath	= _T("");
	m_strFile	= _T("");
	m_bEnabled	= FALSE;
	m_pXML		= NULL;
}

CHisReport::~CHisReport()
{
	ExitReport();
}

//////////////////////////////////////////////////////////////////////
// CHisReport attributes

BOOL CHisReport::IsEnabled() const
{
	return m_bEnabled;
}

//////////////////////////////////////////////////////////////////////
// CHisReport operations

BOOL CHisReport::InitReport()
{
	ExitReport();
	
	CString strXML;
	CTime tt = CTime::GetCurrentTime();
	strXML.Format( _T("%04d年%02d月.xml"), tt.GetYear(), tt.GetMonth() );
	
	m_strFile = m_strPath; if ( ! m_strPath.IsEmpty() ) m_strFile += '\\';
	
	m_strFile += strXML; m_bEnabled = LoadReport();
	
	return m_bEnabled;
}

void CHisReport::ExitReport()
{
	if ( IsEnabled() ) SaveReport();
	
	if ( m_pXML ) delete m_pXML;
	m_pXML = NULL;

	m_bEnabled = FALSE;
}

BOOL CHisReport::LoadReport()
{
	CString strXML;
	strXML = m_strFile;
	
	m_pXML = CXMLElement::FromFile( strXML, TRUE );
	
	if ( ! m_pXML ) Create();

	return TRUE;
}

BOOL CHisReport::SaveReport()
{
	CFile pFile;
	CString strXML;
	
	strXML = m_strFile;
	
	if ( ! pFile.Open( strXML, CFile::modeWrite|CFile::modeCreate ) ) return FALSE;
	
	if ( m_pXML != NULL )
		strXML = m_pXML->ToString( TRUE, TRUE );
	else
		strXML.Empty();
	
	pFile.Write( (LPCSTR)strXML, strXML.GetLength() );
	
	pFile.Close();
	
	return TRUE;
}

void CHisReport::Create()
{
	if ( ! m_pXML ) delete m_pXML;
	m_pXML = new CXMLElement( NULL, _T("History") );
}

BOOL CHisReport::CheckOut()
{
	CTime tt = CTime::GetCurrentTime();
	DWORD tCurrChk = 0;
	
	tCurrChk  = tt.GetYear() - 1;
	tCurrChk *= 12;
	tCurrChk += tt.GetMonth() - 1;
	tCurrChk *= 31;
	tCurrChk += tt.GetDay() - 1;
	tCurrChk *= 24;
	tCurrChk += tt.GetHour();
//	tCurrChk *= 4;
//	tCurrChk += tt.GetMinute() / 15;
	
	if ( tCurrChk == m_tLastChk ) return FALSE;
	
	m_tLastChk = tCurrChk; return TRUE;
}

CXMLElement* CHisReport::FindToday() const
{
	CString strValue;
	
	WORD nYear, nMonth, nDay;
	CTime tt = CTime::GetCurrentTime();
	nYear = tt.GetYear(); nMonth = tt.GetMonth(); nDay = tt.GetDay();
	
	for ( POSITION pos = m_pXML->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = m_pXML->GetNextElement( pos );
		if ( ! pXML->IsNamed( _T("History") ) ) continue;
		
		WORD nYear2, nMonth2, nDay2;
		nYear2 = nMonth2 = nDay2 = 0;
		
		strValue = pXML->GetAttributeValue( _T("Date") );
		_stscanf( strValue, _T("%d年%d月%d日"), &nYear2, &nMonth2, &nDay2 );
		
		if ( nYear != nYear2 || nMonth != nMonth2 || nDay != nDay2 ) continue;
		
		return pXML;
	}
	
	return NULL;
}

CXMLElement* CHisReport::FindToday(BOOL bCreate)
{
	CXMLElement* pXML = FindToday();
	if ( pXML || ! bCreate ) return pXML;
	
	WORD nYear, nMonth, nDay;
	CTime tt = CTime::GetCurrentTime();
	nYear = tt.GetYear(); nMonth = tt.GetMonth(); nDay = tt.GetDay();
	
	CString strValue;
	
	strValue.Format( _T("%d年%d月%d日"), nYear, nMonth, nDay );
	pXML = m_pXML->AddElement( _T("History") );
	pXML->AddAttribute( _T("Date"), (LPCTSTR)strValue );
	
	return pXML;
}

DWORD CHisReport::GetTotal() const
{
	if ( ! IsEnabled() ) return 0;
	
	CString strValue = m_pXML->GetAttributeValue( "Total" );
	
	return _ttol( strValue );
}

DWORD CHisReport::GetToday() const
{
	if ( ! IsEnabled() ) return 0;
	
	CXMLElement* pXML = FindToday();
	
	CString strValue = pXML->GetAttributeValue( "Total" );
	
	return _ttol( strValue );
}

void CHisReport::Register(int nInk)
{
	DWORD nValue;
	CString strValue;
	CXMLAttribute* pAttri;
	
	CXMLElement* pXML = FindToday( TRUE );
	
	pAttri = pXML->GetAttribute( "Total" );
	if ( ! pAttri ) pAttri = pXML->AddAttribute( "Total" );

	strValue = pAttri->GetValue(); nValue = _ttol( strValue );
	strValue.Format( _T("%lu"), nValue + nInk ); pAttri->SetValue( strValue );
	
	pAttri = m_pXML->GetAttribute( "Total" );
	if ( ! pAttri ) pAttri = m_pXML->AddAttribute( "Total" );

	strValue = pAttri->GetValue(); nValue = _ttol( strValue );
	strValue.Format( _T("%lu"), nValue + nInk ); pAttri->SetValue( strValue );
}
