//
// DlgProfilesManager.cpp
//

#include "stdafx.h"
#include "Xgateway.h"
#include "Settings.h"
#include "XML.h"

#include "DlgProfileManager.h"
#include "PageProfileSecure.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CProfileManagerDlg, CSettingsSheet)

BEGIN_MESSAGE_MAP(CProfileManagerDlg, CSettingsSheet)
	//{{AFX_MSG_MAP(CProfileManagerDlg)
	ON_COMMAND(IDRETRY, OnApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProfileManagerDlg construction

CProfileManagerDlg::CProfileManagerDlg() : CSettingsSheet( 0, IDS_PROFILE ), m_pszXML( _T("xProfile.xml") )
{
	m_nTopMargin	= 52;
	m_pXML			= NULL;
}

CProfileManagerDlg::~CProfileManagerDlg()
{
	if ( m_pXML ) delete m_pXML;
}

/////////////////////////////////////////////////////////////////////////////
// CProfileManagerDlg operations

BOOL CProfileManagerDlg::Run(LPCTSTR pszWindow)
{
	CProfileManagerDlg pSheet;
	BOOL bResult = ( pSheet.DoModal( pszWindow ) == IDOK );
	return bResult;
}

int CProfileManagerDlg::DoModal(LPCTSTR pszWindow)
{
	LoadXML();
	CXMLElement* pXML;
	
	pXML = m_pXML->GetElementByName( _T("Secure"), TRUE );
	CSecureProfilePage	pSecure( pXML );

	AddGroup( &pSecure );

	if ( pszWindow ) SetActivePage( GetPage( pszWindow ) );

	int nReturn = CSettingsSheet::DoModal();
	
	return nReturn;
}

void CProfileManagerDlg::AddPage(CSettingsPage* pPage)
{
	CString strCaption = pPage->m_sCaption;
	CSettingsSheet::AddPage( pPage, strCaption );
}

void CProfileManagerDlg::AddGroup(CSettingsPage* pPage)
{
	CString strCaption = pPage->m_sCaption;
	CSettingsSheet::AddGroup( pPage, strCaption.GetLength() ? (LPCTSTR)strCaption : NULL );
}

/////////////////////////////////////////////////////////////////////////////
// CProfileManagerDlg XML

BOOL CProfileManagerDlg::LoadXML()
{
	CString strXML;

	strXML = Settings.General.Path;
	int nLength = strXML.GetLength();

	if ( nLength > 0 && strXML.GetAt(nLength-1) != '\\' ) strXML += '\\';
	strXML += m_pszXML;
	
	m_pXML = CXMLElement::FromFile( strXML, TRUE );
	
	if ( ! m_pXML ) CreateXML();

	if ( ! m_pXML->IsNamed( _T("CallCenter") ) ) return FALSE;
	
	return TRUE;
}

BOOL CProfileManagerDlg::SaveXML()
{
	CString strXML;
	CFile pFile;
	
	strXML = Settings.General.Path;
	int nLength = strXML.GetLength();

	if ( nLength > 0 && strXML.GetAt(nLength-1) != '\\' ) strXML += '\\';
	strXML += m_pszXML;
	
	if ( ! pFile.Open( strXML, CFile::modeWrite|CFile::modeCreate ) ) return FALSE;
	
	if ( m_pXML != NULL )
		strXML = m_pXML->ToString( TRUE, TRUE );
	else
		strXML.Empty();
	
	pFile.Write( (LPCSTR)strXML, strXML.GetLength() );
	
	pFile.Close();
	
	return TRUE;
}

void CProfileManagerDlg::CreateXML()
{
	CString strVersion;

	strVersion.Format( _T("%i.%i.%i.%i"),
		theApp.m_nVersion[0], theApp.m_nVersion[1],
		theApp.m_nVersion[2], theApp.m_nVersion[3] );

	m_pXML = new CXMLElement( NULL, _T("CallCenter") );
	m_pXML->AddAttribute( _T("Author"), _T("Honglt") );
	m_pXML->AddAttribute( _T("Version"), strVersion );
}

/////////////////////////////////////////////////////////////////////////////
// CProfileManagerDlg message handlers

BOOL CProfileManagerDlg::OnInitDialog()
{
	CSettingsSheet::OnInitDialog();

	m_bmHeader.LoadBitmap( IDB_PRODUCTS );

	return TRUE;
}

void CProfileManagerDlg::DoPaint(CDC& dc)
{
	CRect rc;
	GetClientRect( &rc );

	BITMAP pInfo;
	m_bmHeader.GetBitmap( &pInfo );

	CDC mdc;
	mdc.CreateCompatibleDC( &dc );
	CBitmap* pOldBitmap = (CBitmap*)mdc.SelectObject( &m_bmHeader );
	dc.BitBlt( 0, 0, pInfo.bmWidth, pInfo.bmHeight, &mdc, 0, 0, SRCCOPY );
	mdc.SelectObject( pOldBitmap );
	mdc.DeleteDC();

	CSettingsSheet::DoPaint( dc );
}

void CProfileManagerDlg::OnOK()
{
	CSettingsSheet::OnOK();
	SaveXML();
}

void CProfileManagerDlg::OnApply()
{
	CSettingsSheet::OnApply();
	SaveXML();
}
