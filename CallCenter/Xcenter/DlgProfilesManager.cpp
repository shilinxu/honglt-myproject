//
// DlgProfilesManager.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Settings.h"
#include "Profiles.h"
#include "XML.h"

#include "DlgProfilesManager.h"
#include "PageProfileFinal.h"
#include "PageProfileUser.h"
#include "PageProfileCorp.h"
#include "PageProfileInter.h"
#include "PageProfileAnalog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CProfilesManagerDlg, CSettingsSheet)

BEGIN_MESSAGE_MAP(CProfilesManagerDlg, CSettingsSheet)
	//{{AFX_MSG_MAP(CProfilesManagerDlg)
	ON_COMMAND(IDRETRY, OnApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProfilesManagerDlg construction

CProfilesManagerDlg::CProfilesManagerDlg() : CSettingsSheet( 0, IDS_PROFILE ), m_pszXML( _T("xProfile.xml") )
{
	m_nTopMargin	= 52;
	m_pXML			= NULL;
}

CProfilesManagerDlg::~CProfilesManagerDlg()
{
	if ( m_pXML ) delete m_pXML;
}

/////////////////////////////////////////////////////////////////////////////
// CProfilesManagerDlg operations

BOOL CProfilesManagerDlg::Run(LPCTSTR pszWindow)
{
	CProfilesManagerDlg pSheet;
	BOOL bResult = ( pSheet.DoModal( pszWindow ) == IDOK );
	return bResult;
}

int CProfilesManagerDlg::DoModal(LPCTSTR pszWindow)
{
	LoadXML();
	CXMLElement* pXML;
	
	pXML = m_pXML->GetElementByName( _T("Final"), TRUE );
	CFinalProfilePage	pFinal( pXML );
	AddGroup( &pFinal );
	
	pXML = m_pXML->GetElementByName( _T("User"), TRUE );
	CUserProfilePage	pUser( pXML );
	AddGroup( &pUser );
	
	pXML = m_pXML->GetElementByName( _T("Corp"), TRUE );
	CCorpProfilePage	pCorp( pXML );
	AddGroup( &pCorp );
	
	pXML = m_pXML->GetElementByName( _T("Inter"), TRUE );
	CInterProfilePage	pInter( pXML );
	AddGroup( &pInter );
	
	pXML = m_pXML->GetElementByName( _T("Analog"), TRUE );
	CAnalogProfilePage	pAnalog( pXML );
	AddGroup( &pAnalog );
	
	CMap<CSettingsPage*, CSettingsPage*, HINSTANCE, HINSTANCE> pList;
	AddRecursive( pList, m_pXML );

	if ( pszWindow ) SetActivePage( GetPage( pszWindow ) );

	int nReturn = CSettingsSheet::DoModal();
	
	pXML = m_pXML->Clone();
	delete m_pXML; m_pXML = pXML;
	
	for ( POSITION posNext = pList.GetStartPosition(); posNext; )
	{
		CSettingsPage* pPage;
		HINSTANCE hInstance;
		
		pList.GetNextAssoc( posNext, pPage, hInstance );
		
		pPage->DestroyWindow(); delete pPage;
		FreeLibrary( hInstance );
	}
	
	pList.RemoveAll();
	return nReturn;
}

void CProfilesManagerDlg::AddRecursive(CMap<CSettingsPage*, CSettingsPage*, HINSTANCE, HINSTANCE>& pList, CXMLElement* pXML, LPCTSTR pszPath)
{
	WIN32_FIND_DATA pFind;
	CString strPath;
	HANDLE hSearch;
	
	strPath.Format( _T("%s\\%s*.dll"), (LPCTSTR)Settings.General.Path,
		pszPath ? pszPath : _T("") );
	
	hSearch = FindFirstFile( strPath, &pFind );
	
	if ( hSearch != INVALID_HANDLE_VALUE )
	{
		do
		{
			if ( pFind.cFileName[0] == '.' ) continue;
			if ( pFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) continue;
			
			strPath.Format( _T("%s%s"), pszPath ? pszPath : _T(""), pFind.cFileName );
			
			HINSTANCE hPrevInst = AfxGetResourceHandle();
			if ( HINSTANCE hInstance = LoadLibrary( Settings.General.Path + _T("\\") + strPath ) )
			{
				typedef CSettingsPage* (* PCTOR) (CXMLElement*);
				AfxSetResourceHandle(hInstance);
				
				if ( PCTOR pCtor = (PCTOR) GetProcAddress( hInstance, _T("XPageProfiles") ) )
				{
					CSettingsPage* pPage = pCtor( pXML );
					pPage->m_hInstance = hInstance;
					
					pList.SetAt( pPage, hInstance );
					AddGroup( pPage );
				}
				else
				{
					FreeLibrary( hInstance );
				}
			}
			AfxSetResourceHandle( hPrevInst );
		}
		while ( FindNextFile( hSearch, &pFind ) );
		
		FindClose( hSearch );
	}
}

void CProfilesManagerDlg::AddPage(CSettingsPage* pPage)
{
	CString strCaption = pPage->m_sCaption;
	CSettingsSheet::AddPage( pPage, strCaption );
}

void CProfilesManagerDlg::AddGroup(CSettingsPage* pPage)
{
#if 0
	if ( pPage->IsKindOf( RUNTIME_CLASS(CRichSettingsPage) ) )
	{
		CString strCaption = ((CRichSettingsPage*)pPage)->m_sCaption;
		CSettingsSheet::AddGroup( pPage, strCaption );
	}
	else
#endif
	{
		CString strCaption = pPage->m_sCaption;
		CSettingsSheet::AddGroup( pPage, strCaption.GetLength() ? (LPCTSTR)strCaption : NULL );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CProfilesManagerDlg XML

BOOL CProfilesManagerDlg::LoadXML()
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

BOOL CProfilesManagerDlg::SaveXML()
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

	Profiles.Setup();
	
	return TRUE;
}

void CProfilesManagerDlg::CreateXML()
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
// CProfilesManagerDlg message handlers

BOOL CProfilesManagerDlg::OnInitDialog()
{
	CSettingsSheet::OnInitDialog();

	m_bmHeader.LoadBitmap( IDB_PRODUCTS );

	return TRUE;
}

void CProfilesManagerDlg::DoPaint(CDC& dc)
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

void CProfilesManagerDlg::OnOK()
{
	CSettingsSheet::OnOK();
#if 0
	if ( m_pXML && m_pXML->GetElementCount() )
#endif
	SaveXML();
}

void CProfilesManagerDlg::OnApply()
{
	CSettingsSheet::OnApply();
#if 0
	if ( m_pXML && m_pXML->GetElementCount() )
#endif
	SaveXML();
}
