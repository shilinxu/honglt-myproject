//
// CtrlHomeUser.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "CtrlHomeUser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CHomeUser, CFormView)
BEGIN_MESSAGE_MAP(CHomeUser, CFormView)
	//{{AFX_MSG_MAP(CHomeUser)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// CHomeUser construction

CHomeUser::CHomeUser() : CFormView(CHomeUser::IDD)
{
	//{{AFX_DATA_INIT(CHomeUser)
	//}}AFX_DATA_INIT
}

CHomeUser::~CHomeUser()
{
}

void CHomeUser::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHomeUser)
	//}}AFX_DATA_MAP
}
