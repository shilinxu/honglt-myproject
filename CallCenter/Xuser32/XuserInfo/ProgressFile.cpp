//
// ProgressFile.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "ProgressFile.h"
#include <afxinet.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CProgressFile, CDialog)
	//{{AFX_MSG_MAP(CProgressFile)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define TEMP_BUFFER		1024

//////////////////////////////////////////////////////////////////////
// CProgressFile construction

CProgressFile::CProgressFile(CFile* pSource, CFile* pTarget, FileType nType)
: CDialog(CProgressFile::IDD, NULL), m_nType(nType), m_pSrcFile(pSource), m_pTagFile(pTarget)
{
	//{{AFX_DATA_INIT(CProgressFile)
	//}}AFX_DATA_INIT
	
	m_hThread		= NULL;
	m_bThread		= FALSE;
}

void CProgressFile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgressFile)
	DDX_Control(pDX, IDC_ANIMATE, m_wndAnimate);
	DDX_Control(pDX, IDC_DESC, m_wndDesc);
	DDX_Control(pDX, IDC_PROGRESS, m_wndProgress);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CProgressFile message handlers

BOOL CProgressFile::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CString strDesc;
	CString strSrcFile = m_pSrcFile->GetFilePath();
	CString strTagFile = m_pTagFile->GetFilePath();
	
	switch( m_nType )
	{
	case	FileType::FileCopy:
		SetWindowText( _T("复制...") );
		m_wndAnimate.Open( IDR_AVI_FILECOPY );
		
		strDesc.Format( _T("复制 %s 到 %s"),
			strSrcFile.GetLength() > 32 ? strSrcFile.Left(14) + _T("...") + strSrcFile.Right(13) : strSrcFile,
			strTagFile.GetLength() > 32 ? strTagFile.Left(14) + _T("...") + strTagFile.Right(13) : strTagFile );
		m_wndDesc.SetWindowText( strDesc );
		break;
	
	case	FileType::FileDownload:
		SetWindowText( _T("下载...") );
		m_wndAnimate.Open( IDR_AVI_DOWNLOAD );
		
		strDesc.Format( _T("下载 %s 到 %s"),
			strSrcFile.GetLength() > 32 ? strSrcFile.Left(14) + _T("...") + strSrcFile.Right(13) : strSrcFile,
			strTagFile.GetLength() > 32 ? strTagFile.Left(14) + _T("...") + strTagFile.Right(13) : strTagFile );
		m_wndDesc.SetWindowText( strDesc );
		break;
	
	case	FileType::FileUpload:
		SetWindowText( _T("上传文件") );
		m_wndAnimate.Open( IDR_AVI_UPLOAD );
		
		strDesc.Format( _T("上传 %s 到 %s"),
			strSrcFile.GetLength() > 32 ? strSrcFile.Left(14) + _T("...") + strSrcFile.Right(13) : strSrcFile,
			strTagFile.GetLength() > 32 ? strTagFile.Left(14) + _T("...") + strTagFile.Right(13) : strTagFile );
		m_wndDesc.SetWindowText( strDesc );
		break;
	}
	
	StartThread();
	return TRUE;
}

void CProgressFile::OnDestroy() 
{
	CDialog::OnDestroy();
	
	StopThread();
}

//////////////////////////////////////////////////////////////////////
// CProgressFile thread run

void CProgressFile::StartThread()
{
	if ( m_hThread != NULL && m_bThread ) return;
	
	m_bThread = TRUE;
	CWinThread* pThread = AfxBeginThread( ThreadStart, this, THREAD_PRIORITY_NORMAL );
	m_hThread = pThread->m_hThread;
}

void CProgressFile::StopThread()
{
	if ( m_hThread == NULL ) return;
	m_bThread = FALSE;
	
	for ( int nAttempt = 50 ; nAttempt > 0 ; nAttempt-- )
	{
		DWORD nCode;
		if ( ! GetExitCodeThread( m_hThread, &nCode ) ) break;
		if ( nCode != STILL_ACTIVE ) break;
		Sleep( 100 );
	}
	
	if ( nAttempt == 0 )
	{
		TerminateThread( m_hThread, 0 );
		_RPTF0(_CRT_WARN, _T("WARNING: Terminating CProgressFile thread.\n"));
		Sleep( 100 );
	}
	
	m_hThread = NULL;
}

UINT CProgressFile::ThreadStart(LPVOID pParam)
{
	CProgressFile* pThread = (CProgressFile*)pParam;
	
	try
	{
		pThread->OnRun();
	}
	catch (CException* pException)
	{
		pException->ReportError();
		pException->Delete();
		
		pThread->PostMessage( WM_COMMAND, IDCANCEL );
		return 0;
	}
	
	return 0;
}

BOOL CProgressFile::OnRun()
{
	DWORD nOffset = 0;
	BYTE* pBuffer = new BYTE[ TEMP_BUFFER ];
	
	m_wndProgress.SetRange32( 0, m_pSrcFile->GetLength() );
	
	while ( m_bThread )
	{
		DWORD nBuffer = m_pSrcFile->Read( pBuffer, TEMP_BUFFER );
		
		if ( nBuffer == 0 ) {PostMessage( WM_COMMAND, IDOK ); break;}
		
		m_pTagFile->Write( pBuffer, nBuffer );
		
		m_wndProgress.SetPos( nOffset += nBuffer );
	}
	
	delete [] pBuffer;
	return TRUE;
}
