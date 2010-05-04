//
// CtrlUserInfo.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "CtrlUserInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CUserInfoForm, CFormView)
BEGIN_MESSAGE_MAP(CUserInfoForm, CFormView)
	//{{AFX_MSG_MAP(CUserInfoForm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// CUserInfoForm construction

CUserInfoForm::CUserInfoForm() : CFormView(CUserInfoForm::IDD)
{
	//{{AFX_DATA_INIT(CUserInfoForm)
	//}}AFX_DATA_INIT
}

CUserInfoForm::~CUserInfoForm()
{
}

void CUserInfoForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserInfoForm)
	//}}AFX_DATA_MAP
}
