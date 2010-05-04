//
// XsalesAgency.cpp
//

#include "stdafx.h"
#include "XsalesAgency.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CXsalesAgencyApp, CWinApp)
	//{{AFX_MSG_MAP(CXsalesAgencyApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CXsalesAgencyApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CXsalesAgencyApp construction

CXsalesAgencyApp::CXsalesAgencyApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// Export functions

#include "WndAgency.h"
#include "WindowManager.h"

CChildWnd* Create(CXMLElement* pXML, CWindowManager* pWindows)
{
	if ( CChildWnd* pChildWnd = pWindows->Find( RUNTIME_CLASS(CAgencyWnd) ) )
	{
		FreeLibrary( AfxGetResourceHandle() );
		
		pChildWnd->BringWindowToTop(); return pChildWnd;
	}
	
	CChildWnd* pChildWnd = pWindows->Open( RUNTIME_CLASS(CAgencyWnd) );
	((CAgencyWnd *)pChildWnd)->LayoutJobs( pXML );
	
	return pChildWnd;
}
