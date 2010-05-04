//
// ChannelPlay.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Traffic.h"
#include "Network.h"
#include "Settings.h"
#include "ChannelPlay.h"

#include "Registry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern struct sPager Pager;
//////////////////////////////////////////////////////////////////////
// CPlayChannel construction

CPlayChannel::CPlayChannel(int nChan, LPCTSTR xFeeCode) : CChannel( nChan, xFeeCode )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	FeeLogin.initiate		= (INITIATE)FeeLogin_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	rcvMFsig.initiate		= (INITIATE)rcvMFsig_init;
	RcvTalk.initiate		= (INITIATE)RcvTalk_init;
	
	FwdPlay.initiate		= (INITIATE)FwdPlay_init;
	FwdPlay.process			= (PROCESS)FwdPlay_cmplt;
	RcvFnOper.initiate		= (INITIATE)RcvFnOper_init;
	RcvFnOper.process		= (PROCESS)RcvFnOper_cmplt;
	
	RcvLvTele.initiate		= (INITIATE)RcvLvTele_init;
	RcvLvTele.process		= (PROCESS)RcvLvTele_cmplt;
	RcvLvWord.initiate		= (INITIATE)RcvLvWord_init;
	RcvLvWord.process		= (PROCESS)RcvLvWord_cmplt;
	
	PlyLvFile.initiate		= (INITIATE)PlyLvFile_init;
	PlyLvFile.process		= (PROCESS)PlyLvFile_cmplt;
	PlyLvWord.initiate		= (INITIATE)PlyLvWord_init;
	PlyLvWord.process		= (PROCESS)PlyLvWord_cmplt;
	
	PageNumber	= NULL;
	BjPlaying	= FALSE;
	IsBjMusic	= TRUE;
	
	UsrLvTime.SetStatus( COleDateTime::null );
	CPlayChannel::Setup();
}

#if 0
// 构造goto-> "语音信箱"
CPlayChannel::CPlayChannel(CChannel* pParent) : CChannel( pParent->m_nChan, "1222" )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	FeeLogin.initiate		= (INITIATE)FeeLogin_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	
	RcvTalk.initiate		= (INITIATE)RcvTalkLvPlay_init;
	PlyLvFile.initiate		= (INITIATE)PlyLvFile_init;
	PlyLvFile.process		= (PROCESS)PlyLvFile_cmplt;
	PlyLvWord.initiate		= (INITIATE)PlyLvWord_init;
	PlyLvWord.process		= (PROCESS)PlyLvWord_cmplt;
	
	this->m_hInstance		= pParent->m_hInstance;
	
	PageNumber	= NULL;
	BjPlaying	= FALSE;
	IsBjMusic	= TRUE;
	
	UsrLvTime.SetStatus( COleDateTime::null );
	CChannel::Setup();
	fcnp = &RcvTalk;
	
	RcvFnOper = ExitPlay; ExitCode = EXIT_GOODBYE;
}
#else
// 构造goto-> "自听歌曲"
CPlayChannel::CPlayChannel(CChannel* pParent, LPCTSTR xMusic, DWORD nReserved)
	: CChannel( pParent->m_nChan, "1222" )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	FeeLogin.initiate		= (INITIATE)FeeRebind_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	RcvTalk.initiate		= (INITIATE)RcvTalk_init;
	
	PlyMusic.initiate		= (INITIATE)PlyMusic_init;
	PlyMusic.process		= (PROCESS)PlyMusic_cmplt;
	
	this->m_hInstance		= pParent->m_hInstance;
	this->FwdPlay			= PlyMusic;
	PageNumber	= NULL;
	PosMusic	= 0;
	
	CChannel::Setup();
	fcnp = &RcvTalk;
}
#endif

// 构造"回复","代发"
CPlayChannel::CPlayChannel(CChannel* pParent, LPCTSTR xRemoteId)
	: CChannel( pParent->m_nChan, "1222" )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	FeeLogin.initiate		= (INITIATE)FeeRebind_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	RcvTalk.initiate		= (INITIATE)RcvTalk_init;
	
	RcvLvWord.initiate		= (INITIATE)RcvLvWord_init;
	RcvLvWord.process		= (PROCESS)RcvLvWordOk_cmplt;
	
	this->m_hInstance		= pParent->m_hInstance;
	this->FwdPlay			= RcvLvWord;
	
	PageNumber	= _tcsdup( xRemoteId );
	BjPlaying	= FALSE;
	IsBjMusic	= TRUE;
	
	UsrLvTime.SetStatus( COleDateTime::null );
	CChannel::Setup();
	fcnp = &RcvTalk;
}

// 构造"转发"
CPlayChannel::CPlayChannel(CChannel* pParent, LPCTSTR xPCMFile, LPCTSTR xDefaultRemote)
	: CChannel( pParent->m_nChan, "1222" )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	FeeLogin.initiate		= (INITIATE)FeeRebind_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	RcvTalk.initiate		= (INITIATE)RcvTalk_init;
	
	RcvLvTele.initiate		= (INITIATE)RcvLvTele_init;
	RcvLvTele.process		= (PROCESS)RcvLvTeleOk_cmplt;
	
	this->m_hInstance		= pParent->m_hInstance;
	this->FwdPlay			= RcvLvTele;
	
	UsrLvFile	= xPCMFile;
	PageNumber	= NULL;
	
	UsrLvTime.SetStatus( COleDateTime::null );
	CChannel::Setup();
	fcnp = &RcvTalk;
}

CPlayChannel::~CPlayChannel()
{
	if ( PageNumber ) delete PageNumber;
}

//////////////////////////////////////////////////////////////////////
// CPlayChannel attributes

void CPlayChannel::Setup()
{
	CChannel::Setup();
	fcnp = &FeeLogin;
}

struct state* CPlayChannel::FwdNotify(LPCTSTR xLocalId, struct state* lpszDefaultState)
{
	CopyMemory( LocalId, xLocalId, MAX_PHONE );
	CopyMemory( CallType, CA_TALK, MAX_CALLTYPE );
	
	CChannel::OnConnected();
	return lpszDefaultState;
}

void CPlayChannel::FwdNotify(LPCTSTR xLocalId, LPCTSTR xCallType)
{
	CopyMemory( LocalId, xLocalId, MAX_PHONE );
	CopyMemory( CallType, xCallType, MAX_CALLTYPE );
	
	CChannel::OnConnected();
}

int CPlayChannel::ShRcvTalk_init(EVTBLK *evtblk, struct state* lpszDefaultState)
{
	CChannel::RcvTalk_init(evtblk);
	
	fcnp = lpszDefaultState; Reserved = 0;
	return (this->*fcnp->initiate)(evtblk);
}

CChannel* CPlayChannel::ConvertFrom(CChannel* pParent, LPCTSTR xString)
{
theApp.Message(MSG_TEMP,"debug[%i]: %s", pParent->m_nChan, xString);
	
	if ( xString && xString[0] == 'V' )	// 留言
	{
		return CPlayChannel::ConvertPager( pParent, xString + 1 );
	}
	if ( xString && xString[0] == 'T' )	// 点歌带留言
	{
//		return CPlayChannel::ConvertPager( pParent, xString + 1, FALSE );
	}
	if ( xString && xString[0] == 'M' )	// 点歌无留言
	{
		return CPlayChannel::ConvertMusic( pParent, xString + 1 );
		//if ( ((CPlayChannel*)pChannel)->IsOnlyToSelf() ) ;
	}
	
	CopyMemory(pParent->LocalId, "96031222", MAX_PHONE);
	CopyMemory(pParent->CallType, CA_TALK, MAX_CALLTYPE );
	
	pParent->OnConnected(); return NULL;
}

CChannel* CPlayChannel::ConvertPager(CChannel* pParent, LPCTSTR xRemoteId)
{
	CPlayChannel* pChannel = new CPlayChannel( pParent, xRemoteId );
	pChannel->Clone( pParent ); Network.SetChannel( pChannel );
	
	pChannel->FwdNotify( "960312221", CA_TALK );
	EVTBLK *evtblk = new EVTBLK; ZeroMemory( evtblk, sizeof(EVTBLK) );
	
	evtblk->nReference = pChannel->m_nChan;
	(pChannel->*(pChannel->fcnp)->initiate)( evtblk ); delete evtblk;
	
	return pChannel;
}

CChannel* CPlayChannel::ConvertPager(CChannel* pParent, LPCTSTR xRemoteId, BOOL bBjMusic)
{
	CPlayChannel* pChannel = new CPlayChannel( pParent, xRemoteId );
	pChannel->Clone( pParent ); Network.SetChannel( pChannel );
	
	LPCTSTR xBjMusic = _tcsstr( xRemoteId, "#" );
	pChannel->FormatObject( xRemoteId, xBjMusic ? xBjMusic +1 : "" );
	pChannel->IsBjMusic	= bBjMusic;
	
	pChannel->FwdNotify( "960312221", CA_TALK );
	EVTBLK *evtblk = new EVTBLK; ZeroMemory( evtblk, sizeof(EVTBLK) );
	
	evtblk->nReference = pChannel->m_nChan;
	(pChannel->*(pChannel->fcnp)->initiate)( evtblk ); delete evtblk;
	
	return pChannel;
}

CChannel* CPlayChannel::ConvertMusic(CChannel* pParent, LPCTSTR xMusic)
{
	CPlayChannel* pChannel = new CPlayChannel( pParent, xMusic, (DWORD)0 );
	pChannel->Clone( pParent ); Network.SetChannel( pChannel );
	
	LPCTSTR xBjMusic = _tcsstr( xMusic, "#" );
	pChannel->FormatMusic( xBjMusic ? xBjMusic +1 : "" );
	
#if 1
	if (_tcscmp(pChannel->LocalId, "95950") == 0 )
	pChannel->FwdNotify( "95950", "BEST" );
	else
#endif
	pChannel->FwdNotify( "96031222", "BEST" );
	EVTBLK *evtblk = new EVTBLK; ZeroMemory( evtblk, sizeof(EVTBLK) );
	
	evtblk->nReference = pChannel->m_nChan;
	(pChannel->*(pChannel->fcnp)->initiate)( evtblk ); delete evtblk;
	
	return pChannel;
}

CChannel* CPlayChannel::ConvertForward(CChannel* pParent, LPCTSTR xPCMFile)
{
	CPlayChannel* pChannel = new CPlayChannel( pParent, xPCMFile, "" );
	pChannel->Clone( pParent ); Network.SetChannel( pChannel );
	
	pChannel->FwdNotify( "960312221", CA_TALK );
	EVTBLK *evtblk = new EVTBLK; ZeroMemory( evtblk, sizeof(EVTBLK) );
	
	evtblk->nReference = pChannel->m_nChan;
	(pChannel->*(pChannel->fcnp)->initiate)( evtblk ); delete evtblk;
	
	return pChannel;
}

BOOL CPlayChannel::FormatObject(LPCTSTR xRemoteId, LPCTSTR xBjMusic)
{
	// PageNumber
	
	PageNumber = new CHAR [MAX_BUFFER + 1];
	ZeroMemory( PageNumber, MAX_BUFFER + 1 );
	
	for ( int i = 0; xRemoteId[ i ] != '\0'; i ++ )
	{
		if ( xRemoteId[i] == '#' ) break;
		if ( i >= MAX_BUFFER ) break;

		PageNumber[ i ] = xRemoteId[ i ];
	}
	
	// BjMusic
	
	BjMusic.Empty();
	
	for ( i = 0; xBjMusic[ i ] != '\0'; i ++ )
	{
		if ( xBjMusic[i] == '#' ) break;
		BjMusic += xBjMusic[ i ];
	}
	
	// UsrLvTime
	
	CString strTime;
//	UsrLvTime = COleDateTime::GetCurrentTime();
	
	for ( i ++; xBjMusic[ i ] != '\0'; i ++ )
	{
		if ( xBjMusic[i] == '#' ) break;
		strTime += xBjMusic[ i ];
	}
	
	if ( ! UsrLvTime.ParseDateTime( strTime ) )
	UsrLvTime.SetStatus( COleDateTime::null );
	
	return TRUE;
}

BOOL CPlayChannel::FormatMusic(LPCTSTR xString)
{
	CString xBjMusic = xString;
	
	for ( int nPos = xBjMusic.Find('*'); nPos >= 0; )
	{
		LstMusic.Add( xBjMusic.Left( nPos ) );
//	theApp.Message(MSG_TEMP,"add %s",xBjMusic.Left( nPos ));
		xBjMusic = xBjMusic.Mid( nPos + 1 );
		nPos = xBjMusic.Find( '*' );
	}
	
	if ( ! xBjMusic.IsEmpty() ) 
	{
//	theApp.Message(MSG_TEMP,"add %s",xBjMusic);
		LstMusic.Add( xBjMusic );
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CPlayChannel operations

CString CPlayChannel::MakeUsrLvFile(LPTSTR sExtTitle)
{
	CSettings::Item* pItem = Settings.GetSetting( _T(".PathUsr") );
	CString xVbDirect = *pItem->m_pString;
	
	CHAR* pptr;
	CHAR* sptr = PageNumber;
	int nLength = GetNextBuffer( sptr, pptr );
	
	CHAR szFdTel[MAX_PHONE + 1];
	nLength = min( nLength, MAX_PHONE );
	CopyMemory( szFdTel, sptr, nLength ); szFdTel[nLength] = '\0';
	
	if ( (nLength = _tcslen(szFdTel)) > 4 )
	{
		xVbDirect += '\\';
		for ( int i = 0; i < 4; i++ ) xVbDirect += szFdTel[i];
		CreateDirectory( xVbDirect, NULL );
	}
	
	CString xUsrFile;
	SYSTEMTIME pTime;
	GetLocalTime( &pTime );
	
	xUsrFile.Format( "%s\\%04i%02i%02i%02i%02i%02i%03i.%s",
		xVbDirect, pTime.wYear, pTime.wMonth, pTime.wDay,
		pTime.wHour, pTime.wMinute, pTime.wSecond, pTime.wMilliseconds,
		sExtTitle );
	
	MLOG( xUsrFile );
	return xUsrFile;
}

BOOL CPlayChannel::PutUsrRecord(LPCTSTR xUsrFile)
{
	try
	{
		return PutUsrRecord( xUsrFile, 0 );
	}
	catch(_com_error e) 
	{ 
		theApp.Message( MSG_ERROR, "error on CPlayChannel[%i]: %s", m_nChan, e.ErrorMessage() );
		return FALSE;
	}
}

BOOL CPlayChannel::PutUsrRecord(LPCTSTR xUsrFile, DWORD nReserved)
{
	CHAR* sptr;
	CHAR* pptr;
	int nLength;
	CHAR szFdTel[MAX_PHONE + 1];

	for	(
		sptr = PageNumber, nLength = GetNextBuffer( sptr, pptr );
		*sptr != '\0';
		nLength = GetNextBuffer( sptr = pptr + 1, pptr ) )
	{
		nLength = min( nLength, MAX_PHONE );
		CopyMemory( szFdTel, sptr, nLength ); szFdTel[nLength] = '\0';

		if ( TryPagerToGroup(_ttoi(szFdTel), xUsrFile) ) continue;
#if 1
		if ( _tcslen(szFdTel) <= 6 )
		ReadUserShortMobile( RemoteId, szFdTel );
#endif
		
		CString	strQuery;
		
		if ( UsrLvTime.GetStatus() == COleDateTime::valid )
		strQuery.Format( "insert into xuservoice"
						" (xPCMName,xPhoneNum,xRecDate,xCaller)"
						" values('%s','%s','%s','%s')",
						xUsrFile, szFdTel, UsrLvTime.Format("%Y-%m-%d %H:%M"), RemoteId );
		else
		strQuery.Format( "insert into xuservoice"
						" (xPCMName,xPhoneNum,xRecDate,xCaller)"
						" values('%s','%s',getdate(),'%s')",
						xUsrFile, szFdTel, RemoteId );
		TRACE( strQuery + "\n" );
		xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);

		if ( *pptr == '\0' ) break;
	}

	return TRUE;
}

BOOL CPlayChannel::TryPagerToGroup(int xGroupId, LPCTSTR xUsrFile)
{
	CString	strQuery;
	
	strQuery.Format( "select * from xphonemeetinggroupmore"
					" where xgroupnum = '%i'"
					" and xjsjbh = (select xjsjbh from xuser_info"
						" where xphone = '%s')", xGroupId, RemoteId );
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr xRecordset;
	xRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	if ( xRecordset->adoEOF ) return FALSE;
	
	for ( ; ! xRecordset->adoEOF; xRecordset->MoveNext() )
	{
		_variant_t TheValue = xRecordset->GetCollect( "xphone" );
		if ( VT_NULL == TheValue.vt ) continue;
		
		CString xPhone = (LPCTSTR)(_bstr_t)TheValue;
		xPhone.TrimLeft(); xPhone.TrimRight();
		if ( xPhone.IsEmpty() ) continue;
		
		if ( UsrLvTime.GetStatus() == COleDateTime::valid )
		strQuery.Format( "insert into xuservoice"
						" (xPCMName,xPhoneNum,xRecDate,xCaller)"
						" values('%s','%s','%s','%s')",
						xUsrFile, xPhone, UsrLvTime.Format("%Y-%m-%d %H:%M"),RemoteId );
		else
		strQuery.Format( "insert into xuservoice"
						" (xPCMName,xPhoneNum,xRecDate,xCaller)"
						" values('%s','%s',getdate(),'%s')",
						xUsrFile, xPhone, RemoteId );
		TRACE( strQuery + "\n" );
		xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
	}
	
	return TRUE;
}

BOOL CPlayChannel::PutUsrMusic(LPCTSTR xUsrFile, LPCTSTR xMusicFile)
{
	CString xUsrMusic = xMusicFile;
	if ( _tcslen( xUsrFile ) > 0 ) 
	{
		xUsrMusic += ';'; xUsrMusic += xUsrFile;
	}
	
	try
	{
		return PutUsrMusic( xUsrMusic, (DWORD)0 );
	}
	catch(_com_error e) 
	{ 
		theApp.Message( MSG_ERROR, "error on CPlayChannel[%i]: %s", m_nChan, e.ErrorMessage() );
		return FALSE;
	}
}

BOOL CPlayChannel::PutUsrMusic(LPCTSTR xUsrMusic, DWORD nReserved)
{
	CHAR* sptr;
	CHAR* pptr;
	int nLength;
	CHAR szFdTel[MAX_PHONE + 1];

	for	(
		sptr = PageNumber, nLength = GetNextBuffer( sptr, pptr );
		*sptr != '\0';
		nLength = GetNextBuffer( sptr = pptr + 1, pptr ) )
	{
		nLength = min( nLength, MAX_PHONE );
		CopyMemory( szFdTel, sptr, nLength ); szFdTel[nLength] = '\0';

		if ( TryMusicToGroup(_ttoi(szFdTel), xUsrMusic) ) continue;
#if 1
		if ( _tcslen(szFdTel) <= 6 )
		ReadUserShortMobile( RemoteId, szFdTel );
#endif
		
		CString	strQuery;
		
		if ( UsrLvTime.GetStatus() == COleDateTime::valid )
		strQuery.Format( "insert into xopermusichis"
						" (xcallphone,xcalledphone,xmusicname,xopertime,xcode,xindex)"
						" values('%s','%s','%s','%s','9999',0)",
						RemoteId, szFdTel, xUsrMusic, UsrLvTime.Format("%Y-%m-%d %H:%M") );
		else
		strQuery.Format( "insert into xopermusichis"
						" (xcallphone,xcalledphone,xmusicname,xopertime,xcode,xindex)"
						" values('%s','%s','%s',getdate(),'9999',0)",
						RemoteId, szFdTel, xUsrMusic );
		TRACE( strQuery + "\n" );
		xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);

		if ( *pptr == '\0' ) break;
	}

	return TRUE;
}

BOOL CPlayChannel::TryMusicToGroup(int xGroupId, LPCTSTR xUsrMusic)
{
	CString	strQuery;
	
	strQuery.Format( "select * from xphonemeetinggroupmore"
					" where xgroupnum = '%i'"
					" and xjsjbh = (select xjsjbh from xuser_info"
						" where xphone = '%s')", xGroupId, RemoteId );
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr xRecordset;
	xRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	if ( xRecordset->adoEOF ) return FALSE;
	
	for ( ; ! xRecordset->adoEOF; xRecordset->MoveNext() )
	{
		_variant_t TheValue = xRecordset->GetCollect( "xphone" );
		if ( VT_NULL == TheValue.vt ) continue;
		
		CString xPhone = (LPCTSTR)(_bstr_t)TheValue;
		xPhone.TrimLeft(); xPhone.TrimRight();
		if ( xPhone.IsEmpty() ) continue;
		
		strQuery.Format( "insert into xopermusichis"
						" (xcallphone,xcalledphone,xmusicname,xopertime,xcode,xindex)"
						" values('%s','%s','%s',getdate(),'9999',0)",
						RemoteId, xPhone, xUsrMusic );
		TRACE( strQuery + "\n" );
		xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
	}
	
	return TRUE;
}

BOOL CPlayChannel::OpenUsrRecord(LPCTSTR xPhone)
{
	CString strQuery;
	
	strQuery.Format( "select * from xuservoice"
					" where xPhoneNum like '%%%s'"
					" order by xjsjbh desc", xPhone );
	TRACE( strQuery + "\n" );
	
	xOpenRecordset(m_pRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
	if ( m_pRecordset->adoEOF ) return FALSE;
	
	return TRUE;
}

int CPlayChannel::GetUsrLvCount()
{
	return m_pRecordset->RecordCount;
}

BOOL CPlayChannel::OpenFavRecord(LPCTSTR xPhone)
{
	CString strQuery;

	strQuery.Format( "select a.*, c.xphone"
					" from Xvoiceinfo a inner join"
					" xvoicesub b on a.xvoicecode = b.xvoicecode inner join"
					" Xuser c on b.xjsjbh = c.xjsjbh"
					" where c.xphone = '%s'"
					" order by xbh desc", xPhone );
	TRACE( strQuery + "\n" );
	
	xOpenRecordset(m_pRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
	if ( m_pRecordset->adoEOF ) return FALSE;
	
	return TRUE;
}

int CPlayChannel::ShPlyNavFile(int nChan, LPCTSTR xNavFile)
{
	CRegistry pRegistry;
	if ( ! pRegistry.GetInt( "Pager", "NavPlay" ) ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	CString strValue = pRegistry.GetString( "Pager", "NavOffset" );
	if ( ! strValue.GetLength() ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	SsmSetDtmfStopPlay( nChan, FALSE );
	SsmClearRxDtmfBuf( nChan );
	SsmClearFileList( nChan );
	
	DWORD nOffset1 = 0, nOffset2 = 0;
	_stscanf( strValue, "%i-%i", &nOffset1, &nOffset2 );
	
	SsmAddToFileList( nChan, (LPTSTR)xNavFile, 6, 0, nOffset1 );
	
	strValue.Format( "%i", GetUsrLvCount() );
	CompDigital( (LPCTSTR)strValue, TRUE );
	
	SsmAddToFileList( nChan, (LPTSTR)xNavFile, 6, nOffset2, 0xFFFFFFFF );
	
	if ( SsmPlayFileList( nChan ) ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	return 0;
}

BOOL CPlayChannel::IsOnlyToSelf() const
{
	if ( ! PageNumber ) return FALSE;
	
	return TRUE;
}

BOOL CPlayChannel::ReadUserShortMobile(LPCTSTR xRemoted, LPTSTR xPageNumber)
{
	CString strQuery;
	
	strQuery.Format( "select * from XusrCommList"
					" where xjsjbh in (select xjsjbh from xuser where xphone = '%s')"
					" and right(xfmobile, %i) = '%s'"
					" order by jsjbh desc", xRemoted, _tcslen(xPageNumber), xPageNumber );
	TRACE( strQuery + "\n" );
theApp.Message(MSG_TEMP, strQuery );
	
	_RecordsetPtr xRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	if ( xRecordset->adoEOF ) return FALSE;
#if 0	
	strQuery.Format( "right(xfmobile, %i) = '%s'", _tcslen(xPageNumber), xPageNumber );
	theApp.Message(MSG_TEMP, strQuery );
#ifdef _DEBUG
	afxDump << (LPCTSTR)strQuery << "\n";
#endif
	
	xRecordset->Find( (_bstr_t)strQuery, 0, adSearchForward, "" );
	if ( xRecordset->adoEOF ) return FALSE;
	
	theApp.Message(MSG_TEMP, "GetCollect" );
#endif
	_variant_t TheValue = xRecordset->GetCollect( "xfmobile" );
	int nLength = _tcslen( (LPCTSTR)(_bstr_t)TheValue );
	
	CopyMemory( xPageNumber, (LPCTSTR)(_bstr_t)TheValue, min(MAX_PHONE, nLength) );
	xPageNumber[ min(MAX_PHONE, nLength) ] = '\0';
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CPlayChannel TCP Transfer func

BOOL CPlayChannel::OnBindPacket(CBindPacket* pPacket)
{
	if ( FeeLogin.initiate == (INITIATE)FeeRebind_init )
	{
		SsmPutUserEvent( E_FEE_Login, m_nChan, S_FEE_Connected );
		CChannel::RcvTalk_init( NULL ); return TRUE;
	}
	
	return CChannel::OnBindPacket( pPacket );
}

//////////////////////////////////////////////////////////////////////
// CPlayChannel OVERLOAD function

int CPlayChannel::bkIdle_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	CChannel* pChannel = new CChannel( nChan );
	pChannel->Clone( this ); Network.SetChannel( pChannel );
	
	return (pChannel->*(pChannel->fcnp)->initiate)(evtblk);
}

int CPlayChannel::FeeLogin_init(EVTBLK *evtblk)
{
	return 0;
}

int CPlayChannel::FeeRebind_init(EVTBLK *evtblk)
{
	if ( Reserved & 0x08 ) return 0;
	Reserved |= 0x08;	// 置状态“正在”
	
	CChannel::OnConnected();
	SetTimer( 5* 1000 );
	
	return 0;
}

struct state* CPlayChannel::FeeLogin_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_RINGING )
		{
			KillTimer(); Reserved &= 0; return &rcvMFsig;
		}
	}
	
	if ( evtblk->wEventCode == E_FEE_Login &&
		(evtblk->dwParam & 0xFFFF) == S_FEE_Connected )
	{
		if ( SsmGetChState(nChan) == S_CALL_TALKING ||	// 其他业务转入
			_tcsncmp(CallType, CA_DEBG, 4) == 0 )		// For Debug
		{
			SsmPutUserEvent( E_CHG_ChState, nChan, S_CALL_RINGING );
		}
		else
		if ( SsmGetChState(nChan) == S_CALL_PICKUPED )	// For CallOut
		{
			KillTimer(); Reserved &= 0; return &WaitDial;
		}
	}
	
	return CChannel::FeeLogin_cmplt(evtblk);
}

int CPlayChannel::rcvMFsig_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( SsmGetChState(nChan) == S_CALL_TALKING ||	// 其他业务转入
		_tcsncmp(CallType, CA_DEBG, 4) == 0 )		// For Debug
	{
		return SsmPutUserEvent( E_CHG_ChState, nChan, S_CALL_TALKING );
	}
	
	return CChannel::rcvMFsig_init(evtblk);
}

int CPlayChannel::RcvTalk_init(EVTBLK *evtblk)
{
	return ShRcvTalk_init( evtblk, &FwdPlay );
}

/////////////////////////////////////////////////////////////////////////////
// CPlayChannel Pager function

int CPlayChannel::FwdPlay_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	if ( PlayFile( FILE_PLAY_WELCOME, FT_PROMPT, TRUE ) )
		return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	return 0;
}

struct state* CPlayChannel::FwdPlay_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved = 0; return &RcvFnOper;
	}
	
	return ChkDTSignal( evtblk, &FwdPlay );
}

int CPlayChannel::RcvFnOper_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	SsmClearRxDtmfBuf(nChan);
	
	if ( Reserved++ < 3 )
	{
		Reserved |= 0x08;	// 置状态“正在”
		return PlayFile( FILE_PLAY_RCVOPER, FT_PROMPT, TRUE );
	}
	
	ExitCode = EXIT_GOODBYE; fcnp = &ExitPlay;
	return (this->*fcnp->initiate)(evtblk);
}

struct state* CPlayChannel::RcvFnOper_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x04;	// 记录播放完毕标志
		SetTimer( 5000 ); //return &RcvFnOper;
	}
	
	if ( evtblk->wEventCode == E_SYS_TIMEOUT )
	{
		Reserved &= 0x03;	// 清除播放完毕标志
		return &RcvFnOper;
	}
	
	if ( (Reserved & 0x04) && SsmGetRxDtmfLen( nChan ) )
	{
		CHAR nDtmfKey = '\0';
		KillTimer();
		
		SsmGet1stDtmf( nChan, &nDtmfKey );
		
		switch ( nDtmfKey )
		{
		case	'1':		// 发语音
#if 0
			Reserved &= 0x00; return &RcvLvTele;
#else
			FeeLogin.initiate	= (INITIATE)FeeRebind_init;
			Reserved &= 0x00; return FwdNotify( "960312221", &RcvLvTele );
#endif
			
		case	'2':		// 听语音
			FeeLogin.initiate	= (INITIATE)FeeRebind_init;
			PlyLvFile.initiate	= (INITIATE)PlyLvFile_init;
			PlyLvWord.initiate	= (INITIATE)PlyLvWord_init;
			Reserved &= 0x00; return FwdNotify( "960312222", &PlyLvFile );
			
		case	'3':		// 中意的栏目
			FeeLogin.initiate	= (INITIATE)FeeRebind_init;
			PlyLvFile.initiate	= (INITIATE)PlyFavFile_init;
			PlyLvWord.initiate	= (INITIATE)PlyFavWord_init;
			Reserved &= 0x00; return FwdNotify( "960312223", &PlyLvFile );
			
		case	'0':		// 转人工
			FeeLogin.initiate	= (INITIATE)FeeLogin_init;
			//Reserved &= 0x00;
			return FwdNotify( "96031", &RcvFnOper );
			
		default:
			Reserved &= 0x00; return &RcvFnOper;
		}
	}
	
	return ChkDTSignal( evtblk, &RcvFnOper );
}

/////////////////////////////////////////////////////////////////////////////
// CPlayChannel send voice info

// 短信号码
int CPlayChannel::RcvLvTele_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;
	SsmClearRxDtmfBuf( nChan );
	PageNumber = new CHAR [MAX_BUFFER + 1];
	ZeroMemory( PageNumber, MAX_BUFFER + 1 );
	
	return PlayFile( FILE_PAGE_WTLVTELE, FT_PROMPT, TRUE );
}

struct state* CPlayChannel::RcvLvTele_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x04;	// 记录播放完毕标志
	}
	
	if ( evtblk->wEventCode == E_CHG_RcvDTMF )
	{
		int nDtmfLen = SsmGetRxDtmfLen( nChan );
		
		if ( '#' == (CHAR)(evtblk->dwParam & 0xFFFF) || nDtmfLen >= MAX_BUFFER )
		{
			if ( ! ReadDtmf( PageNumber, '#' ) ) CopyMemory( PageNumber, RemoteId, MAX_PHONE );
		}
	}
	
	if ( (Reserved & 0x04) && _tcslen( PageNumber ) )
	{
		Reserved &= 0x00; return &RcvLvWord;
	}
	
	return ChkDTSignal( evtblk, &RcvLvTele );
}

// 接收留言
int CPlayChannel::RcvLvWord_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	if ( Reserved == 0 )
	{
		Reserved |= (0x08 | 0x01);
		return PlayFile( FILE_PAGE_RCVLVWRD, FT_PROMPT, FALSE );
	}
	
	BjPlaying	= TRUE;
	Reserved |= (0x08 | 0x02);
	if ( BjMusic.IsEmpty() )
	return PlayFile( FILE_PAGE_BJMUSIC, FT_PROMPT, FALSE );
	
	return PlayFile( (LPCTSTR)BjMusic, NULL, FALSE );
}

struct state* CPlayChannel::RcvLvWord_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
#if 0
	SYSTEMTIME pTime;
	GetLocalTime( &pTime );

	theApp.Message( MSG_ERROR, "[%i]: %02i:%02i:%02i.%03i Chan: %i Event: %04x Param: %08x(%i, %i)",
		Reserved,
		pTime.wHour, pTime.wMinute, pTime.wSecond, pTime.wMilliseconds,
		evtblk->nReference, evtblk->wEventCode, evtblk->dwParam,
		evtblk->dwParam >> 16, evtblk->dwParam & 0xFFFF );
#endif

	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		if ( (Reserved &= 0x07) == 0x01 )	// 提示语播放完毕
		{
#if 1
			SetTimer( 52 * 1000 );
#endif
			UsrLvFile = MakeUsrLvFile( "pcm" );
			RecordFile( UsrLvFile, NULL, _T("#") );
		}
		if ( (Reserved & 0x04) == 0x04 )	// 短信录制完毕
		{
			BjPlaying = FALSE;
		}
		else
		return &RcvLvWord;	// 循环播放背景音乐
	}
	if ( evtblk->wEventCode == E_PROC_RecordEnd )
	{
		Reserved |= 0x04;	// 记录播放完毕标志
		if ( SsmStopPlayFile( nChan ) ) BjPlaying = FALSE;
	}
	if ( evtblk->wEventCode == E_SYS_TIMEOUT )
	{
		SsmStopRecToFile( nChan );
	}
	
	//if ( (Reserved & 0x04) == 0x04 )	// 短信录制完毕
	if ( (Reserved & 0x04) == 0x04 && ! BjPlaying )	// 短信录制完毕
	{
		if ( IsBjMusic ) PutUsrRecord( UsrLvFile );
		else PutUsrMusic( UsrLvFile, BjMusic );
		
		SetEvent( Traffic.m_pWakeup ); Reserved &= 0x00; return &RcvFnOper;
	}
	
	if ( (Reserved & 0x02) == 0x02 && evtblk->wEventCode == E_CHG_ChState &&
		((evtblk->dwParam & 0xFFFF) == S_CALL_STANDBY || (evtblk->dwParam & 0xFFFF) == S_CALL_PENDING) )
	{
//theApp.Message(MSG_ERROR, "IsBjMusic := %i", IsBjMusic);
		if ( IsBjMusic ) PutUsrRecord( UsrLvFile );
		else PutUsrMusic( UsrLvFile, BjMusic );
		
		SetEvent( Traffic.m_pWakeup );
	}
	
	return ChkDTSignal( evtblk, &RcvLvWord );
}

/////////////////////////////////////////////////////////////////////////////
// CPlayChannel read voice info

// 导播提示
int CPlayChannel::PlyLvFile_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	if ( ! OpenUsrRecord( RemoteId ) )
		return PlayFile( FILE_PROMPT_NORECD, FT_PROMPT, FALSE );
	
	CString strFile;
	CSettings::Item* pItem = Settings.GetSetting( _T(".Path") );
	strFile.Format( _T("%s\\%s\\%s"), *pItem->m_pString, FT_PROMPT, FILE_PAGE_PLYLVWRD );
	
	return ShPlyNavFile( nChan, (LPCTSTR)strFile );
}

struct state* CPlayChannel::PlyLvFile_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved = 0;
		return ( m_pRecordset->adoEOF ) ? &RcvFnOper : &PlyLvWord;
	}
	
	return ChkDTSignal( evtblk, &PlyLvFile );
}

// 播放语音
int CPlayChannel::PlyLvWord_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	SsmClearRxDtmfBuf(nChan);
	
	_variant_t TheValue = m_pRecordset->GetCollect( "xPCMName" );
	if ( VT_NULL == TheValue.vt ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	CString xUsrFile = (LPCTSTR)(_bstr_t)TheValue;
	xUsrFile.TrimLeft(); xUsrFile.TrimRight();
	
#ifdef _DEBUG
	theApp.Message( MSG_TEMP, "debug[%i]: Replace [xUsrFile] with [Net Path]", nChan );
	Replace( xUsrFile, "d:\\", "\\\\192.168.0.2\\\\d$\\" );
#endif
	
	int nPos = xUsrFile.Find( ';' );
	if ( nPos > 0 )
	{
		SsmSetDtmfStopPlay( nChan, TRUE );
		SsmClearFileList( nChan );
		
		// 设置5秒断点以播放插曲
		SsmAddToFileList( nChan, xUsrFile.Left(nPos).GetBuffer(0), 6, 0, 5 * 8000 );
		// 播放插曲
		SsmAddToFileList( nChan, xUsrFile.Right(xUsrFile.GetLength() - nPos).GetBuffer(0), 6, 0, 0xFFFFFFFF );
		// 恢复断点以播放主题曲
		SsmAddToFileList( nChan, xUsrFile.Left(nPos).GetBuffer(0), 6, 5 * 8000, 0xFFFFFFFF );
		
		if ( SsmPlayFileList( nChan ) ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
		return 0;
	}
	
	if ( PlayFile( (LPCTSTR)xUsrFile, NULL ) ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	return 0;
}

struct state* CPlayChannel::PlyLvWord_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		CHAR nDtmfKey = '\0';
		Reserved = 0; KillTimer();
		
		SsmGet1stDtmf( nChan, &nDtmfKey );
		if ( nDtmfKey == '*' ) return &RcvFnOper;
		
		m_pRecordset->MoveNext(); if ( m_pRecordset->adoEOF )
		return &RcvFnOper; else return &PlyLvWord;
	}
	
	return ChkDTSignal( evtblk, &PlyLvWord );
}

/////////////////////////////////////////////////////////////////////////////
// CPlayChannel read favo voice info

// 导播提示
int CPlayChannel::PlyFavFile_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	if ( ! OpenFavRecord( RemoteId ) )
		return PlayFile( FILE_PROMPT_NORECD, FT_PROMPT, FALSE );
	
	CString strFile;
	CSettings::Item* pItem = Settings.GetSetting( _T(".PathUsr") );
	strFile.Format( _T("%s\\%s\\%s"), pItem->m_pString, FT_PROMPT, FILE_PAGE_PLYLVWRD );
	
	return ShPlyNavFile( nChan, (LPCTSTR)strFile );
}

// 播放语音
int CPlayChannel::PlyFavWord_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	_variant_t TheValue = m_pRecordset->GetCollect( "xvoicepath" );
	if ( VT_NULL == TheValue.vt ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	CString xFavFile = (LPCTSTR)(_bstr_t)TheValue;
	xFavFile.TrimLeft(); xFavFile.TrimRight();
	
	if ( PlayFile( (LPCTSTR)xFavFile, NULL ) ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CPlayChannel pager interface function
/*
int CPlayChannel::RcvTalkLvWord_init(EVTBLK *evtblk)
{
	CChannel::OnConnected();
	
	return ShRcvTalk_init( evtblk, &RcvLvTele );
}

int CPlayChannel::RcvTalkLvPlay_init(EVTBLK *evtblk)
{
	CChannel::OnConnected();
	
	return ShRcvTalk_init( evtblk, &PlyLvFile );
}

int CPlayChannel::RcvTalkMusic_init(EVTBLK *evtblk)
{
	CChannel::OnConnected();
	
	return ShRcvTalk_init( evtblk, &PlyMusic );
}

int CPlayChannel::RcvTalkLvResp_init(EVTBLK *evtblk)
{
	RcvLvWord.initiate		= (INITIATE)RcvLvWord_init;
	RcvLvWord.process		= (PROCESS)RcvLvWordOk_cmplt;
	
	return ShRcvTalk_init( evtblk, &RcvLvWord );
}

int CPlayChannel::RcvTalkLvBack_init(EVTBLK *evtblk)
{
	RcvLvTele.initiate		= (INITIATE)RcvLvTele_init;
	RcvLvTele.process		= (PROCESS)RcvLvTeleOk_cmplt;
	
	return ShRcvTalk_init( evtblk, &RcvLvTele );
}
*/
struct state* CPlayChannel::RcvLvWordOk_cmplt(EVTBLK *evtblk)
{
	struct state* stNext = RcvLvWord_cmplt( evtblk );
	if ( stNext != &RcvFnOper ) return stNext;
	
	ExitCode = EXIT_GOODBYE;
	Reserved &= 0x00; return &ExitPlay;
}

struct state* CPlayChannel::RcvLvTeleOk_cmplt(EVTBLK *evtblk)
{
	struct state* stNext = RcvLvTele_cmplt( evtblk );
	if ( stNext != &RcvLvWord ) return stNext;
	
	PutUsrRecord( UsrLvFile ); SetEvent( Traffic.m_pWakeup );
	
	ExitCode = EXIT_GOODBYE;
	Reserved &= 0x00; return &ExitPlay;
}

/////////////////////////////////////////////////////////////////////////////
// CPlayChannel caller function

int CPlayChannel::PlyMusic_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	PosMusic %= LstMusic.GetSize();
	CString BjMusic = LstMusic.GetAt( PosMusic ++ );
//theApp.Message(MSG_TEMP,"play %s",BjMusic);
	
	return PlayFile( (LPCTSTR)BjMusic, NULL, FALSE );
}

struct state* CPlayChannel::PlyMusic_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;

	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved &= 0x00; return &PlyMusic;	// 循环播放
	}
	
	return ChkDTSignal( evtblk, &PlyMusic );
}
