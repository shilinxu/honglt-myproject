//
// DlgSettingsManager.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Network.h"
#include "Settings.h"

#include "DlgSettingsManager.h"
#include "PageSettingsGeneral.h"
#include "PageSettingsMaster.h"
#include "PageSettingsMonitor.h"
#include "PageSettingsUser.h"
//#include "PageSettingsMeeting.h"
#include "PageSettingsTrunk.h"
#include "PageSettingsOptimize.h"
#include "PageSettingsPager.h"
#include "PageSettingsCentric.h"

#include "WndMain.h"
#include "WndPager.h"
#include "WndMail.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CSettingsManagerDlg, CSettingsSheet)

BEGIN_MESSAGE_MAP(CSettingsManagerDlg, CSettingsSheet)
	//{{AFX_MSG_MAP(CSettingsManagerDlg)
	ON_COMMAND(IDRETRY, OnApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettingsManagerDlg construction

CSettingsManagerDlg::CSettingsManagerDlg() : CSettingsSheet( 0, IDS_SETTINGS )
{
	m_nTopMargin	= 52;
}

CSettingsManagerDlg::~CSettingsManagerDlg()
{
}

/////////////////////////////////////////////////////////////////////////////
// CSettingsManagerDlg operations

BOOL CSettingsManagerDlg::Run(LPCTSTR pszWindow)
{
	CSettingsManagerDlg pSheet;
	BOOL bResult = ( pSheet.DoModal( pszWindow ) == IDOK );
	return bResult;
}

int CSettingsManagerDlg::DoModal(LPCTSTR pszWindow)
{
	CGeneralSettingsPage	pGeneral;
	AddGroup( &pGeneral );
	
	CMasterSettingsPage		pMaster;
	AddGroup( &pMaster );
	
	CMonitorSettingsPage	pMonitor;
	AddGroup( &pMonitor );
	
	CUserSettingsPage		pUser;
	AddGroup( &pUser );
	
	CTrunkSettingsPage		pTrunk;
	AddGroup( &pTrunk );
	
//	CMeetingSettingsPage	pMeeting;
//	AddGroup( &pMeeting );
	
	CCentricSettingsPage	pCentric;
	AddGroup( &pCentric );
	
	COptimizeSettingsPage	pOptimize;
	AddGroup( &pOptimize );
	
	CMap<CSettingsPage*, CSettingsPage*, HINSTANCE, HINSTANCE> pList;
	AddRecursive( pList, NULL );
	
#if 0
	CPlaySettingsPage		pPlay;
	AddGroup( &pPlay );
	
	CPagerSettingsPage		pPager;
	AddGroup( &pPager );
#endif
	
	if ( pszWindow ) SetActivePage( GetPage( pszWindow ) );
	
	int nReturn = CSettingsSheet::DoModal();
	
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

void CSettingsManagerDlg::AddRecursive(CMap<CSettingsPage*, CSettingsPage*, HINSTANCE, HINSTANCE>& pList, LPCTSTR pszPath)
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
				typedef CSettingsPage* (* PCTOR) ();
				AfxSetResourceHandle(hInstance);
				
				if ( PCTOR pCtor = (PCTOR) GetProcAddress( hInstance, _T("XPageSettings") ) )
				{
					CSettingsPage* pPage = pCtor();
					pPage->m_hInstance = hInstance;
					
					pList.SetAt( pPage, hInstance );
					AddPage( pPage );
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

void CSettingsManagerDlg::AddPage(CSettingsPage* pPage)
{
	CString strCaption = pPage->m_sCaption;
	CSettingsSheet::AddPage( pPage, strCaption );
}

void CSettingsManagerDlg::AddGroup(CSettingsPage* pPage)
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
// CSettingsManagerDlg message handlers

BOOL CSettingsManagerDlg::OnInitDialog()
{
	CSettingsSheet::OnInitDialog();

	m_bmHeader.LoadBitmap( IDB_PRODUCTS );

	return TRUE;
}

void CSettingsManagerDlg::DoPaint(CDC& dc)
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

void CSettingsManagerDlg::OnOK()
{
	CSettingsSheet::OnOK();
	
	if ( Network.m_pSection.Lock( 500 ) )
	{
		CMainWnd* pMainWnd = (CMainWnd*)AfxGetMainWnd();
		CChildWnd* pChildWnd = pMainWnd->m_pWindows.Find( RUNTIME_CLASS(CPagerWnd) );
		if ( pChildWnd ) ((CPagerWnd*)pChildWnd)->m_bIsOpen = FALSE;
		
		pChildWnd = pMainWnd->m_pWindows.Find( RUNTIME_CLASS(CMailWnd) );
		if ( pChildWnd ) ((CMailWnd*)pChildWnd)->m_bIsOpen = FALSE;
		
		Network.m_pSection.Unlock();
	}
}

void CSettingsManagerDlg::OnApply()
{
	CSettingsSheet::OnApply();
}
