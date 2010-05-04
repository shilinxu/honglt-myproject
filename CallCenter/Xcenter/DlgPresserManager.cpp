//
// DlgPresserManager.cpp
//

#include "stdafx.h"
#include "Xcenter.h"

#include "DlgPresserManager.h"
#include "PagePresserUser.h"
#include "PagePresserFax.h"
#include "PagePresserRec.h"
//#include "PagePresserInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CPresserManagerDlg, CSettingsSheet)

BEGIN_MESSAGE_MAP(CPresserManagerDlg, CSettingsSheet)
	//{{AFX_MSG_MAP(CPresserManagerDlg)
	ON_COMMAND(IDRETRY, OnApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPresserManagerDlg construction

CPresserManagerDlg::CPresserManagerDlg() : CSettingsSheet( 0, IDS_PRESSER )
{
	m_nTopMargin	= 52;
}

CPresserManagerDlg::~CPresserManagerDlg()
{
}

/////////////////////////////////////////////////////////////////////////////
// CPresserManagerDlg operations

BOOL CPresserManagerDlg::Run(LPCTSTR pszWindow)
{
	CPresserManagerDlg pSheet;
	BOOL bResult = ( pSheet.DoModal( pszWindow ) == IDOK );
	return bResult;
}

int CPresserManagerDlg::DoModal(LPCTSTR pszWindow)
{
	CUserPresserPage	pUser;
	CFaxPresserPage		pFax;
	CRecPresserPage		pRec;
//	CInfoPresserPage	pInfo;

	AddGroup( &pUser );
//	AddPage( &pInfo );
	AddGroup( &pFax );
	AddGroup( &pRec );

	if ( pszWindow ) SetActivePage( GetPage( pszWindow ) );

	int nReturn = CSettingsSheet::DoModal();

	return nReturn;
}

void CPresserManagerDlg::AddPage(CSettingsPage* pPage)
{
	CString strCaption = pPage->m_sCaption;
	CSettingsSheet::AddPage( pPage, strCaption );
}

void CPresserManagerDlg::AddGroup(CSettingsPage* pPage)
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
// CPresserManagerDlg message handlers

BOOL CPresserManagerDlg::OnInitDialog()
{
	CSettingsSheet::OnInitDialog();

	m_bmHeader.LoadBitmap( IDB_PRODUCTS );
	m_wndApply.EnableWindow( FALSE ); m_wndApply.ShowWindow( SW_HIDE );

	return TRUE;
}

void CPresserManagerDlg::DoPaint(CDC& dc)
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

void CPresserManagerDlg::OnOK()
{
	CSettingsSheet::OnOK();
}

void CPresserManagerDlg::OnApply()
{
	CSettingsSheet::OnApply();
}
