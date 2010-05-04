//
// HisReport.cpp
//

#include "stdafx.h"
#include "xmaster.h"
#include "HisReport.h"
#include "Settings.h"
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

	m_strFile = Settings.General.Path;
	int nLength = m_strFile.GetLength();

	if ( nLength > 0 && m_strFile.GetAt(nLength-1) != '\\' ) m_strFile += '\\';
	//m_strFile += _T("Data\\") + strXML;
	m_strFile += strXML;
	
	m_tLastChk	= tt.GetYear() - 1;
	m_tLastChk *= 12;
	m_tLastChk += tt.GetMonth() - 1;
	m_tLastChk *= 31;
	m_tLastChk += tt.GetDay() - 1;
	m_tLastChk *= 24;
	m_tLastChk += tt.GetHour();
//	m_tLastChk *= 4;
//	m_tLastChk += tt.GetMinute() / 15;

	m_bEnabled = LoadReport();

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
	
	return ( tCurrChk != m_tLastChk );
}

CXMLElement* CHisReport::FindToday() const
{
	CTime tt = CTime::GetCurrentTime();
	WORD nYear = tt.GetYear();
	WORD nMonth = tt.GetMonth();
	WORD nDay = tt.GetDay();

	for ( POSITION pos = m_pXML->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = m_pXML->GetNextElement( pos );
		if ( ! pXML->IsNamed( _T("History") ) ) continue;
		
		WORD nYear2, nMonth2, nDay2;
		nYear2 = nMonth2 = nDay2 = 0;

		CString strValue = pXML->GetAttributeValue( _T("Date") );
		_stscanf( strValue, _T("%d年%d月%d日"), &nYear2, &nMonth2, &nDay2 );

		if ( nYear != nYear2 || nMonth != nMonth2 || nDay != nDay2 ) continue;
		
		return pXML;
	}

	return NULL;
}

DWORD CHisReport::GetTotal() const
{
	if ( ! IsEnabled() ) return 0;
	
	DWORD nValue;
	CString strValue;
	CXMLAttribute* pAttri;
	
	pAttri = m_pXML->GetAttribute( "Total" );
	if ( ! pAttri ) return 0;
	
	strValue = pAttri->GetValue(); nValue = _ttol( strValue );
	return nValue;
}

DWORD CHisReport::GetToday() const
{
	if ( ! IsEnabled() ) return 0;
	
	CXMLElement* pXML = FindToday();
	if ( ! pXML ) return 0;
	
	DWORD nValue;
	CString strValue;
	CXMLAttribute* pAttri;
	
	pAttri = pXML->GetAttribute( "Normal" );
	if ( ! pAttri ) return 0;
	
	strValue = pAttri->GetValue(); nValue = _ttol( strValue );
	return nValue;
}

void CHisReport::Register(int nInk)
{
	DWORD nValue;
	CString strValue;
	CXMLAttribute* pAttri;
	
	CXMLElement* pXML = FindToday();
	
	if ( pXML == NULL )
	{
		CTime tt = CTime::GetCurrentTime();
		WORD nYear = tt.GetYear();
		WORD nMonth = tt.GetMonth();
		WORD nDay = tt.GetDay();
		
		strValue.Format( _T("%04d年%02d月%02d日"), nYear, nMonth, nDay );
		pXML = m_pXML->AddElement( _T("History") );
		pXML->AddAttribute( _T("Date"), strValue );
	}
	
	pAttri = pXML->GetAttribute( "Normal" );
	if ( ! pAttri ) pAttri = pXML->AddAttribute( "Normal" );

	strValue = pAttri->GetValue(); nValue = _ttol( strValue );
	strValue.Format( _T("%lu"), nValue + nInk ); pAttri->SetValue( strValue );
	
	pAttri = m_pXML->GetAttribute( "Total" );
	if ( ! pAttri ) pAttri = m_pXML->AddAttribute( "Total" );

	strValue = pAttri->GetValue(); nValue = _ttol( strValue );
	strValue.Format( _T("%lu"), nValue + nInk ); pAttri->SetValue( strValue );
}
