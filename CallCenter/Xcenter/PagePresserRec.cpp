//
// PagePresserRec.cpp
//

#include "stdafx.h"
#include "xcenter.h"
#include "Settings.h"
#include "PagePresserRec.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CRecPresserPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CRecPresserPage, CSettingsPage)
	//{{AFX_MSG_MAP(CRecPresserPage)
	ON_BN_CLICKED(IDC_USER_PRESS, OnUserPress)
	ON_BN_CLICKED(IDC_USER_VIEW, OnUserView)
	ON_BN_CLICKED(IDC_REC_COPY_BROWSE, OnRecCopyBrowse)
	ON_BN_CLICKED(IDC_REC_PRESS, OnRecPress)
	ON_BN_CLICKED(IDC_REC_VIEW, OnRecView)
	ON_CBN_CLOSEUP(IDC_REC_NUMBER, OnCloseUpRecNumber)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRecPresserPage property page

CRecPresserPage::CRecPresserPage() : CSettingsPage( CRecPresserPage::IDD )
{
	//{{AFX_DATA_INIT(CRecPresserPage)
	//}}AFX_DATA_INIT
	m_pRecSrcFile = NULL;
}

CRecPresserPage::~CRecPresserPage()
{
	if ( m_pRecSrcFile ) delete m_pRecSrcFile;
}

void CRecPresserPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRecPresserPage)
	DDX_Control(pDX, IDC_REC_COPY, m_wndCopyTo);
	DDX_Control(pDX, IDC_REC_EXPIRE, m_wndLength);
	DDX_Control(pDX, IDC_REC_NUMBER, m_wndPhone);
	DDX_Control(pDX, IDC_USER_NUMBER, m_wndUserId);
	DDX_Control(pDX, IDC_USER_EXPIRE, m_wndExpire);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CRecPresserPage message handlers

BOOL CRecPresserPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();
	
	m_wndUserId.SetCurSel( 0 );
	m_wndExpire.SetCurSel( 2 );	// 3个月以上
	
	m_wndPhone.SetCurSel( 0 );
	m_wndLength.SetCurSel( 2 );	// 3个月以上
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRecPresserPage::OnOK()
{
	CSettingsPage::OnOK();
}

void CRecPresserPage::OnUserPress()
{
	CString xUserId;
	m_wndUserId.GetWindowText( xUserId );
	xUserId.TrimLeft(); xUserId.TrimRight();
	if ( xUserId.IsEmpty() ) return;
	
//	int nUserId =m_wndUserId.FindStringExact( -1, xUserId );
//	if ( nUserId == 1 && ! m_wndUserId.GetItemData(nUserId) ) return;
	
	int nExpire = m_wndExpire.GetCurSel();
	if ( nExpire == 0 ) nExpire = 30;
	else if ( nExpire == 1 ) nExpire = 60;
	else if ( nExpire == 2 ) nExpire = 90;
	else if ( nExpire == 3 ) nExpire = 180;
	else if ( nExpire == 4 ) nExpire = 365;
	else if ( nExpire == 5 ) nExpire = -1;
	else if ( nExpire == -1 ) return;
	
	CString xExpire;
	m_wndExpire.GetWindowText( xExpire ); 
	
	CString str;
	str.Format( "是否清理\"%s\"号码的\"%s\"的所有录音项目?",
				xUserId, xExpire );
	if (AfxMessageBox(str, MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2) == IDCANCEL)
	{
		return;
	}
	
	CWaitCursor pCursor;
	
	if ( xUserId == "全部" )
	{
		PressRecord( Settings.General.PathRec, NULL, nExpire );
	}
	else
	{
		PressRecord( Settings.General.PathRec, (LPCTSTR)xUserId, nExpire );
	}
}

BOOL CRecPresserPage::PressRecord(LPCTSTR xPath, LPCTSTR xUserId, int nExpire)
{
	WIN32_FIND_DATA pFind;
	CString strPath;
	HANDLE hSearch;
	
	strPath.Format( _T("%s\\*.*"), xPath, xUserId );
	theApp.Message( MSG_TEMP, "PressRecord: %s", strPath );
	
	hSearch = FindFirstFile( strPath, &pFind );
	
	if ( hSearch != INVALID_HANDLE_VALUE )
	{
		do
		{
			MSG msg;
			if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
			{
				TranslateMessage( &msg ); DispatchMessage( &msg );
			}
			
			if ( pFind.cFileName[0] == '.' ) continue;

			if ( pFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				strPath.Format( _T("%s\\%s"),
					xPath ? xPath : _T(""), pFind.cFileName );

				PressRecord( strPath, xUserId, nExpire );
			}
			else if (	_tcsistr( pFind.cFileName, _T(".pcm") ) != NULL )
			{
				if ( xUserId && _tcsistr( pFind.cFileName, _T("-") ) + 1 !=
					_tcsistr( pFind.cFileName, xUserId ) ) continue;
				
				COleDateTime xFileDate = pFind.ftCreationTime;
				COleDateTimeSpan xSpan = COleDateTime::GetCurrentTime() - xFileDate;
				
				if ( xSpan.GetTotalDays() < nExpire ) continue;
				
				strPath.Format( _T("%s\\%s"),
					xPath ? xPath : _T(""), pFind.cFileName );
	
				CString xNewFile = Settings.General.PathRec + _T("\\Backup");
				CreateDirectory( (LPCTSTR)xNewFile, NULL );
				xNewFile += '\\'; xNewFile += pFind.cFileName;
				
				MoveFileEx( strPath, xNewFile, MOVEFILE_REPLACE_EXISTING );
			}
		}
		while ( FindNextFile( hSearch, &pFind ) );

		FindClose( hSearch );
	}
	
	RemoveDirectory( xPath );
	return TRUE;
}

void CRecPresserPage::OnUserView()
{
	CString strPath = Settings.General.PathRec + _T("\\Backup");
	
	ShellExecute( GetSafeHwnd(), _T("open"),
			(LPCTSTR)strPath, NULL, NULL, SW_SHOWNORMAL );
}

void CRecPresserPage::OnRecCopyBrowse() 
{
	TCHAR szPath[MAX_PATH];
	LPITEMIDLIST pPath;
	LPMALLOC pMalloc;
	BROWSEINFO pBI;
		
	ZeroMemory( &pBI, sizeof(pBI) );
	pBI.hwndOwner		= AfxGetMainWnd()->GetSafeHwnd();
	pBI.pszDisplayName	= szPath;
	pBI.lpszTitle		= _T("选择录音存放的路径:");
	pBI.ulFlags			= BIF_RETURNONLYFSDIRS;
	
	pPath = SHBrowseForFolder( &pBI );

	if ( pPath == NULL ) return;

	SHGetPathFromIDList( pPath, szPath );
	SHGetMalloc( &pMalloc );
	pMalloc->Free( pPath );
	pMalloc->Release();
	
	m_wndCopyTo.SetWindowText( szPath );
}

void CRecPresserPage::OnRecPress() 
{
	CString xUserId;
	m_wndPhone.GetWindowText( xUserId );
	xUserId.TrimLeft(); xUserId.TrimRight();
	if ( xUserId.IsEmpty() ) return;
	
	int nUserId =m_wndPhone.FindStringExact( -1, xUserId );
	if ( nUserId == 1 && ! m_wndPhone.GetItemData(nUserId) ) return;
	
	int nExpire = m_wndLength.GetCurSel();
	if ( nExpire == 0 ) nExpire = 30;
	else if ( nExpire == 1 ) nExpire = 60;
	else if ( nExpire == 2 ) nExpire = 90;
	else if ( nExpire == 3 ) nExpire = 180;
	else if ( nExpire == 4 ) nExpire = 365;
	else if ( nExpire == 5 ) nExpire = -1;
	else if ( nExpire == -1 ) return;
	
	CString xExpire;
	m_wndLength.GetWindowText( xExpire ); 
	
	CString xTarget;
	m_wndCopyTo.GetWindowText( xTarget ); 
	
	CString str;
	str.Format( "是否提取\"%s\"号码的\"%s\"的所有录音项目?",
				xUserId, xExpire );
	if (AfxMessageBox(str, MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2) == IDCANCEL)
	{
		return;
	}
	
	CWaitCursor pCursor;
	int nSuccNum = 0;
	
	if ( nUserId == 0 )
	{
		nSuccNum = PressRecord( Settings.General.PathRec, xTarget, NULL, nExpire );
	}
	else if ( nUserId == 1 && m_wndPhone.GetItemData(nUserId) )
	{
		CString* xFile = (CString*)m_wndPhone.GetItemData(nUserId);
		CHAR szBuff[128];
		
		FILE* fp = fopen( (LPCTSTR)*xFile, "r" );
		while ((fp) && fgets(szBuff,80,fp) && (!feof(fp)))
		{
			szBuff[21] = '\0';
			char* ptr = strstr(szBuff, "\n");
			if ( ptr != NULL ) *ptr = '\0';
			if ( '\0' == szBuff[0] ) continue;
			
			nSuccNum += PressRecord( Settings.General.PathRec, xTarget, szBuff, nExpire );
		}
		
		if ( fp ) fclose( fp );
	}
	else
	{
		PressRecord( Settings.General.PathRec, xTarget, (LPCTSTR)xUserId, nExpire );
	}
	
	CString strText;
	strText.Format("提取号码完毕，累计成功:= %i。", nSuccNum );
	AfxMessageBox( strText );
}

int CRecPresserPage::PressRecord(LPCTSTR xPath, LPCTSTR xTarget, LPCTSTR xUserId, int nExpire)
{
	WIN32_FIND_DATA pFind;
	CString strPath;
	HANDLE hSearch;
	int nCount = 0;
	
	strPath.Format( _T("%s\\*.*"), xPath, xUserId );
	
	hSearch = FindFirstFile( strPath, &pFind );
	
	if ( hSearch != INVALID_HANDLE_VALUE )
	{
		do
		{
			if ( pFind.cFileName[0] == '.' ) continue;

			if ( pFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				strPath.Format( _T("%s\\%s"),
					xPath ? xPath : _T(""), pFind.cFileName );

				nCount += PressRecord( strPath, xTarget, xUserId, nExpire );
			}
			else if (	_tcsistr( pFind.cFileName, _T(".pcm") ) != NULL )
			{
				if ( xUserId && _tcsistr( pFind.cFileName, _T("-") ) + 1 !=
					_tcsistr( pFind.cFileName, xUserId ) ) continue;
				
				COleDateTime xFileDate = pFind.ftCreationTime;
				COleDateTimeSpan xSpan = COleDateTime::GetCurrentTime() - xFileDate;
				
				if ( xSpan.GetTotalDays() < nExpire ) continue;
				
				strPath.Format( _T("%s\\%s"),
					xPath ? xPath : _T(""), pFind.cFileName );
	
				CString xNewFile = xTarget;
				CreateDirectory( (LPCTSTR)xNewFile, NULL );
				xNewFile += '\\'; xNewFile += pFind.cFileName;
				
				if ( CopyFile( strPath, xNewFile, FALSE ) ) nCount++;
			}
		}
		while ( FindNextFile( hSearch, &pFind ) );

		FindClose( hSearch );
	}
	
	return nCount;
}

void CRecPresserPage::OnRecView() 
{
	CString xTarget;
	m_wndCopyTo.GetWindowText( xTarget ); 
	
	ShellExecute( GetSafeHwnd(), _T("open"),
			(LPCTSTR)xTarget, NULL, NULL, SW_SHOWNORMAL );
}

void CRecPresserPage::OnCloseUpRecNumber() 
{
	if ( m_wndPhone.GetCurSel() != 1 ) return;

	CFileDialog dlg( TRUE, NULL, NULL,
		OFN_HIDEREADONLY|OFN_ENABLESIZING,
		_T("Text Files|*.txt|All Files|*.*||"), this );
	
	if ( dlg.DoModal() != IDOK ) return;
	if ( ! m_pRecSrcFile ) m_pRecSrcFile = new CString;
	* m_pRecSrcFile = dlg.GetPathName();
	m_wndPhone.SetItemData( 1, (DWORD)m_pRecSrcFile );
}
