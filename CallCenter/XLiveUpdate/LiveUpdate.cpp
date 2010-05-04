//
// LiveUpdate.cpp
//

#include "stdafx.h"
#include "LiveUpdate.h"
#include "WizardSheet.h"
#include "VersionChecker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CLiveUpdateApp, CWinApp)
	//{{AFX_MSG_MAP(CLiveUpdateApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CLiveUpdateApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CLiveUpdateApp construction

CLiveUpdateApp::CLiveUpdateApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// CLiveUpdateApp initialization

BOOL CLiveUpdateApp::InitInstance()
{
	Enable3dControls();
	AfxEnableControlContainer();
	
	CString strValue = m_lpCmdLine;
	strValue		+= '|';
	
	CString str		= strValue.SpanExcluding( _T(" |;,") );
	strValue		= strValue.Mid( str.GetLength() + 1 );
	m_nProcessId	= _ttoi( str );
	
	WSADATA wsaData;
	if ( WSAStartup( 0x0101, &wsaData ) ) return FALSE;
	
	CreateFonts();
	VersionChecker.Setup();
	
	CWizardSheet::RunWizard();
	
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CLiveUpdateApp termination

int CLiveUpdateApp::ExitInstance() 
{
	CWaitCursor pCursor;
	
	VersionChecker.Release();
	WSACleanup();

	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// CLiveUpdateApp help (suppress F1)

void CLiveUpdateApp::WinHelp(DWORD dwData, UINT nCmd) 
{
}

//////////////////////////////////////////////////////////////////////
// CLiveUpdateApp fonts

void CLiveUpdateApp::CreateFonts(LPCTSTR pszFace, int nSize)
{
	if ( ! pszFace ) pszFace = _T("Tahoma");
	if ( ! nSize ) nSize = 11;
	
	if ( m_fntNormal.m_hObject ) m_fntNormal.DeleteObject();
	if ( m_fntBold.m_hObject ) m_fntBold.DeleteObject();
	if ( m_fntUnder.m_hObject ) m_fntUnder.DeleteObject();
	if ( m_fntCaption.m_hObject ) m_fntCaption.DeleteObject();
	if ( m_fntItalic.m_hObject ) m_fntItalic.DeleteObject();
	
	m_fntNormal.CreateFont( -nSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_DONTCARE, pszFace );
	
	m_fntBold.CreateFont( -nSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_DONTCARE, pszFace );
	
	m_fntUnder.CreateFont( -nSize, 0, 0, 0, FW_NORMAL, FALSE, TRUE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_DONTCARE, pszFace );
	
	m_fntCaption.CreateFont( -nSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_DONTCARE, pszFace );

	m_fntItalic.CreateFont( -nSize, 0, 0, 0, FW_NORMAL, TRUE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_DONTCARE, pszFace );
}

/////////////////////////////////////////////////////////////////////////////
// CLiveUpdateApp get error string

CString CLiveUpdateApp::GetErrorString()
{
	LPTSTR pszMessage = NULL;
	CString strMessage;
	
	FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&pszMessage, 0, NULL );
	
	if ( pszMessage != NULL )
	{
		strMessage = pszMessage;
		LocalFree( pszMessage );
	}
	
	return strMessage;
}
