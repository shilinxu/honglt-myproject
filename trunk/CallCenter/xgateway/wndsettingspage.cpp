//
// WndSettingsPage.cpp
//

#include "stdafx.h"
#include "Xgateway.h"
#include "CoolInterface.h"
#include <afxpriv.h>
#include <afximpl.h>
#include "Skin.h"

#include "WndSettingsSheet.h"
#include "WndSettingsPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CSettingsPage, CDialog)

BEGIN_MESSAGE_MAP(CSettingsPage, CDialog)
	//{{AFX_MSG_MAP(CSettingsPage)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSettingsPage construction

CSettingsPage::CSettingsPage(UINT nIDTemplate, LPCTSTR pszCaption) : CDialog( nIDTemplate )
{
	//{{AFX_DATA_INIT(CSettingsPage)
	//}}AFX_DATA_INIT
	
	if ( pszCaption != NULL ) m_sCaption = pszCaption;
	else if ( m_lpszTemplateName != NULL ) LoadDefaultCaption();
}

CSettingsPage::~CSettingsPage()
{
}

void CSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingsPage)
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CSettingsPage operations

BOOL CSettingsPage::LoadDefaultCaption()
{
	CDialogTemplate pTemplate;
	DLGTEMPLATE* pData;
	LPWORD pWord;

	if ( ! pTemplate.Load( m_lpszTemplateName ) ) return FALSE;

	pData = (DLGTEMPLATE*)GlobalLock( pTemplate.m_hTemplate );

	if ( ((DLGTEMPLATEEX*)pData)->signature == 0xFFFF )
	{
		pWord = (WORD*)( (DLGTEMPLATEEX*)pData + 1 );
	}
	else
	{
		pWord = (WORD*)( pData + 1 );
	}
	if ( *pWord == 0xFFFF )
	{
		pWord += 2;
	}
	else
	{
		while ( *pWord++ );
	}

	if ( *pWord == 0xFFFF )
	{
		pWord += 2;
	}
	else
	{
		while ( *pWord++ );
	}

	m_sCaption = (wchar_t*)pWord;

	GlobalUnlock( pTemplate.m_hTemplate );

	return m_sCaption.GetLength() > 0;
}

BOOL CSettingsPage::Create(CRect& rcPage, CWnd* pSheetWnd)
{
	ASSERT_VALID(this);
	ASSERT( m_lpszTemplateName != NULL );

	CDialogTemplate pTemplate;
	LPDLGTEMPLATE pData;

	if ( ! pTemplate.Load( m_lpszTemplateName ) ) return FALSE;
	pData = (LPDLGTEMPLATE)GlobalLock( pTemplate.m_hTemplate );

	DWORD dwExStyle = WS_EX_LTRREADING|WS_EX_LEFT|WS_EX_RIGHTSCROLLBAR;

	if ( ((DLGTEMPLATEEX*)pData)->signature == 0xFFFF )
	{
		DLGTEMPLATEEX* pEx = (DLGTEMPLATEEX*)pData;
		pEx->style		= WS_CHILDWINDOW|WS_OVERLAPPED|DS_3DLOOK|DS_SETFONT|DS_CONTROL;
		pEx->exStyle	= dwExStyle|WS_EX_WINDOWEDGE|WS_EX_CONTROLPARENT;
	}
	else
	{
		pData->style			= WS_CHILDWINDOW|WS_OVERLAPPED|DS_3DLOOK|DS_SETFONT|DS_CONTROL;
		pData->dwExtendedStyle	= dwExStyle|WS_EX_WINDOWEDGE|WS_EX_CONTROLPARENT;
	}

	GlobalUnlock( pTemplate.m_hTemplate );
	CreateIndirect( pTemplate.m_hTemplate, pSheetWnd );
	SetFont( &CoolInterface.m_fntNormal );

	MoveWindow( rcPage );

	return ( m_hWnd != NULL );
}

CSettingsPage* CSettingsPage::GetPage(CRuntimeClass* pClass) const
{
	return GetSheet()->GetPage( pClass );
}

/////////////////////////////////////////////////////////////////////////////
// CSettingsPage message handlers

void CSettingsPage::SetModified(BOOL bChanged)
{
	ASSERT_VALID(this);
	GetSheet()->SetModified( bChanged );
}

BOOL CSettingsPage::OnApply()
{
	ASSERT_VALID(this);
	OnOK();
	return TRUE;
}

void CSettingsPage::OnReset()
{
	ASSERT_VALID(this);
	OnCancel();
}

void CSettingsPage::OnOK()
{
	ASSERT_VALID(this);
}

void CSettingsPage::OnCancel()
{
	ASSERT_VALID(this);
}

BOOL CSettingsPage::OnSetActive()
{
	ASSERT_VALID(this);
	return TRUE;
}

BOOL CSettingsPage::OnKillActive()
{
	ASSERT_VALID(this);
	if ( ! UpdateData() ) return FALSE;
	return TRUE;
}

BOOL CSettingsPage::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect( &rc );
	pDC->FillSolidRect( &rc, Skin.m_crDialog );
	return TRUE;
}

HBRUSH CSettingsPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor( pDC, pWnd, nCtlColor );

	if ( nCtlColor == CTLCOLOR_DLG || nCtlColor == CTLCOLOR_STATIC )
	{
		pDC->SetBkColor( Skin.m_crDialog );
		hbr = Skin.m_brDialog;
	}

	return hbr;
}
