//
// PageTesterGeneral.cpp
//

#include "stdafx.h"
#include "XfaceTest.h"

#include "PageTesterGeneral.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CGeneralTesterPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CGeneralTesterPage, CSettingsPage)
	//{{AFX_MSG_MAP(CGeneralTesterPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGeneralTesterPage property page

CGeneralTesterPage::CGeneralTesterPage() : CSettingsPage( CGeneralTesterPage::IDD )
{
	//{{AFX_DATA_INIT(CGeneralTesterPage)
	//}}AFX_DATA_INIT
}

CGeneralTesterPage::~CGeneralTesterPage()
{
}

void CGeneralTesterPage::DoDataExchange(CDataExchange* pDX)
{
	CSettingsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGeneralTesterPage)
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CGeneralTesterPage message handlers

BOOL CGeneralTesterPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGeneralTesterPage::OnOK()
{
	CSettingsPage::OnOK();
}
