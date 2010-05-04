//
// PagePresserFax.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Settings.h"
#include "PagePresserFax.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CFaxPresserPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CFaxPresserPage, CSettingsPage)
	//{{AFX_MSG_MAP(CFaxPresserPage)
	ON_BN_CLICKED(IDC_USER_PRESS, OnUserPress)
	ON_BN_CLICKED(IDC_USER_VIEW, OnUserView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFaxPresserPage property page

CFaxPresserPage::CFaxPresserPage() : CSettingsPage( CFaxPresserPage::IDD )
{
	//{{AFX_DATA_INIT(CFaxPresserPage)
	//}}AFX_DATA_INIT
}

CFaxPresserPage::~CFaxPresserPage()
{
}

void CFaxPresserPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFaxPresserPage)
	DDX_Control(pDX, IDC_USER_NUMBER, m_wndUserId);
	DDX_Control(pDX, IDC_USER_EXPIRE, m_wndExpire);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CFaxPresserPage message handlers

BOOL CFaxPresserPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();
	
	m_wndUserId.SetCurSel( 0 );
	m_wndExpire.SetCurSel( 2 );	// 3个月以上
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFaxPresserPage::OnOK()
{
	CSettingsPage::OnOK();
}

void CFaxPresserPage::OnUserPress()
{
	CString xUserId;
	m_wndUserId.GetWindowText( xUserId );
	xUserId.TrimLeft(); xUserId.TrimRight();
	if ( xUserId.IsEmpty() ) return;
	
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
	str.Format( "是否清理\"%s\"号码的\"%s\"的所有传真项目?",
				xUserId, xExpire );
	if (AfxMessageBox(str, MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2) == IDCANCEL)
	{
		return;
	}
	
	CWaitCursor pCursor;
	
	if ( xUserId == "全部" ) PressRecord(NULL, nExpire);
	else PressRecord( (LPCTSTR)xUserId, nExpire );
}

BOOL CFaxPresserPage::PressRecord(LPCTSTR xUserId, int nExpire)
{
	CString strPath = Settings.General.PathFax + _T("\\Backup");
#ifdef _DEBUG
	strPath.MakeLower();
	theApp.Message( MSG_TEMP, "debug: Replace [PathFax] with [Net Path]" );
	Replace( strPath, "d:\\", "\\\\192.168.0.2\\\\d$\\" );
#endif
	CreateDirectory( (LPCTSTR)strPath, NULL );
	
	CString strQuery;
	
	if ( xUserId == NULL )
	strQuery.Format( "select * from xfax"
					" where datediff(day,xRecDate,getdate()) > %i", nExpire );
	else
	strQuery.Format( "select * from xfax"
					" where xPhoneNum = '%s'"
					" and datediff(day,xRecDate,getdate()) > %i", xUserId, nExpire );
	
	try
	{
		_RecordsetPtr Recordvox = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
		TRACE( strQuery + "\n" );
		
		for ( ; ! Recordvox->adoEOF; Recordvox->MoveNext() )
		{
			_variant_t xPhoneNum = Recordvox->GetCollect( "xPhoneNum" );
			_variant_t xCallerId = Recordvox->GetCollect( "xCaller" );
			_variant_t xPCMName  = Recordvox->GetCollect( "xfaxpath" );
			_variant_t xjsjbh    = Recordvox->GetCollect( "xbh" );
			
			strQuery.Format( "delete xfax"
							" where xbh = %i", xjsjbh.lVal );
			TRACE( strQuery + "\n" );
			
			xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
			
			CString xUsrFile = (LPCTSTR)(_bstr_t)xPCMName;
			xUsrFile.TrimLeft(); xUsrFile.TrimRight();
			if ( IsSysFile(xUsrFile) ) continue;
			
#ifdef _DEBUG
	xUsrFile.MakeLower();
	theApp.Message( MSG_TEMP, "debug: Replace [xUsrFile] with [Net Path]" );
	Replace( xUsrFile, "d:\\", "\\\\192.168.0.2\\\\d$\\" );
#endif
			
			CString xNewFile = "";
			
			if ( xPhoneNum.vt != VT_NULL ) xNewFile += (LPCTSTR)(_bstr_t)xPhoneNum;
			xNewFile.TrimLeft(); xNewFile.TrimRight();
			xNewFile += '_';
			if ( xCallerId.vt != VT_NULL ) xNewFile += (LPCTSTR)(_bstr_t)xCallerId;
			xNewFile.TrimLeft(); xNewFile.TrimRight();
			xNewFile += '_';
			
			xNewFile += xUsrFile.Mid( xUsrFile.ReverseFind( '\\' ) + 1 );
			xNewFile  =	strPath + '\\' + xNewFile;
			
			MoveFile( xUsrFile, xNewFile );
			
			CString xUsrPath;
			xUsrPath = xUsrFile.Left( xUsrFile.ReverseFind( '\\' ) );
			RemoveDirectory( xUsrPath );
		}
	}
	catch(_com_error e) 
	{
		CString str = "数据库操作失败: ";
		str += e.ErrorMessage();
		AfxMessageBox( str );
		return FALSE;
	}
	
	return TRUE;
}

void CFaxPresserPage::OnUserView()
{
	CString strPath = Settings.General.PathFax + _T("\\Backup");
	
	ShellExecute( GetSafeHwnd(), _T("open"),
			(LPCTSTR)strPath, NULL, NULL, SW_SHOWNORMAL );
}

BOOL CFaxPresserPage::IsSysFile(LPCTSTR xPath) const
{
#if 0
	CString xSysPath = FT_FAX;
	xSysPath += '\\';
	
	return ( _tcsnistr(xPath, xSysPath, xSysPath.GetLength()) != NULL );
#else
	return FALSE;
#endif
}
