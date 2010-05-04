//
// ChannelMail.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Network.h"
#include "Settings.h"
#include "Traffic.h"
#include "ChannelMail.h"

#include <mapi.h>
#include "WndMain.h"
#include "WndNetwork.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CMailChannel construction

CMailChannel::CMailChannel(int nChan, LPCTSTR xFeeCode) : CChannel( nChan, xFeeCode )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	FeeLogin.initiate		= (INITIATE)FeeLogin_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	RcvTalk.initiate		= (INITIATE)RcvTalk_init;
	
	FwdPlay.initiate		= (INITIATE)FwdPlay_init;
	FwdPlay.process			= (PROCESS)FwdPlay_cmplt;
	RcvFnOper.initiate		= (INITIATE)RcvFnOper_init;
	RcvFnOper.process		= (PROCESS)RcvFnOper_cmplt;
	
	RcvFxTele.initiate		= (INITIATE)RcvFxTele_init;
	RcvFxTele.process		= (PROCESS)RcvFxTele_cmplt;
	RcvFxByte.initiate		= (INITIATE)RcvFxByte_init;
	RcvFxByte.process		= (PROCESS)RcvFxByte_cmplt;
	
	GetFxTele.initiate		= (INITIATE)GetFxTele_init;
	GetFxTele.process		= (PROCESS)GetFxTele_cmplt;
	GetFxPass.initiate		= (INITIATE)GetFxPass_init;
	GetFxPass.process		= (PROCESS)GetFxPass_cmplt;
	GetFxByte.initiate		= (INITIATE)GetFxByte_init;
	GetFxByte.process		= (PROCESS)GetFxByte_cmplt;
	
	UsrLvFax.Empty();
	FaxNumber = FaxWord = NULL;
	CMailChannel::Setup();
}

CMailChannel::~CMailChannel()
{
	if ( FaxNumber ) delete [] FaxNumber;
	if ( FaxWord ) delete [] FaxWord;
}

//////////////////////////////////////////////////////////////////////
// CMailChannel attributes

void CMailChannel::Setup()
{
	CChannel::Setup();
	fcnp = &FeeLogin;
	
	CMainWnd* pMainWnd	= (CMainWnd*)theApp.m_pMainWnd;
	ASSERT( pMainWnd->GetSafeHwnd() != NULL );
	
	m_pHomeWnd			= (CNetworkWnd*)pMainWnd->GetWindow( RUNTIME_CLASS(CNetworkWnd) );
	ASSERT( m_pHomeWnd->GetSafeHwnd() != NULL );
}

struct state* CMailChannel::FwdCaller(LPCTSTR xLocalId)
{
	CopyMemory( LocalId, xLocalId, MAX_PHONE );
	CopyMemory( CallType, CA_TALK, MAX_CALLTYPE );
	FeeLogin.initiate	= (INITIATE)FeeRebind_init;
	
	return fcnp = &FeeLogin;
}

BOOL CMailChannel::ChkUserState(LPCTSTR xUsrNumber, LPCTSTR xUsrWord)
{
	CString	strQuery;
	
	strQuery.Format( "select * from xuser_info"
					" where xjsjbh ="
						" (select top 1 xjsjbh from xuser"
						" where xphone = '%s')", xUsrNumber );
	TRACE( strQuery + "\n" );

	_RecordsetPtr Record = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	if ( Record->adoEOF ) return FALSE;
	
	if ( xUsrWord != NULL )
	{
		_variant_t TheValue = Record->GetCollect( "xuserpwd" );
		if ( TheValue.vt == VT_NULL ) return FALSE;
		
		if ( _tcscmp(xUsrWord, (LPCTSTR)(_bstr_t)TheValue) ) return FALSE;
	}
	
	_variant_t xUserId = Record->GetCollect( "xjsjbh" );
	
	strQuery.Format( "select * from xusefee"
					" where xjsjbh = '%s' and xfeecode = '%s'",
					(LPCTSTR)(_bstr_t)xUserId, m_xFeeCode );
	TRACE( strQuery + "\n" );

	Record = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	return ( Record->adoEOF == FALSE );
}

int CMailChannel::ShExitCode_init(EVTBLK *evtblk, int nExitCode)
{
	ExitCode = nExitCode; fcnp = &ExitPlay;
	Reserved = 0; return (this->*fcnp->initiate)(evtblk);
}

int CMailChannel::GetFxTeleCount() 
{
	CHAR* sptr;
	CHAR* pptr;
	int nCount = 0;
	
	for	(
	sptr = FaxNumber, GetNextBuffer( sptr, pptr ); *sptr != '\0';
	GetNextBuffer( sptr = pptr + 1, pptr ) )
	{
		nCount += 1;
	}
	
	return nCount;
}

CString CMailChannel::MakeUsrFaxFile(LPTSTR sExtTitle)
{
	CSettings::Item* pItem = Settings.GetSetting( _T(".PathFax") );
	CString xVbDirect = *pItem->m_pString;
	
	CHAR* pptr;
	CHAR* sptr = FaxNumber;
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

BOOL CMailChannel::RepackFile(LPCTSTR xFaxFile)
{
	CString xNewFile;
	SYSTEMTIME pTime;
	GetLocalTime( &pTime );
	
	xNewFile.Format( "%04i%02i%02i%02i%02i%02i%03i.tif",
		pTime.wYear, pTime.wMonth, pTime.wDay,
		pTime.wHour, pTime.wMinute, pTime.wSecond, pTime.wMilliseconds );

	int nResult =
	fBmp_CutTifHeader( (LPSTR)xFaxFile, xNewFile.GetBuffer(0), 16, -1,  0 );
	if ( nResult == -1 ) return FALSE;
	
	CTime pNow = CTime::GetCurrentTime();
	
	CHAR xDateTime[32];
	sprintf( xDateTime, "%.4i-%.2i-%.2i %.2i:%.2i:%.2i",
		pNow.GetYear(), pNow.GetMonth(), pNow.GetDay(),	
		pNow.GetHour(), pNow.GetMinute(), pNow.GetSecond() );
	
	nResult =
	fBmp_AddTxtToTif( xNewFile.GetBuffer(0), RemoteId, FaxNumber, "",
		xDateTime, (LPSTR)xFaxFile, 0 );
	if ( nResult == -1 ) return FALSE;
	
	DeleteFile( xNewFile );
	
	return TRUE;
}

BOOL CMailChannel::OpenUsrRecord(LPCTSTR xPhone)
{
	CString strQuery;
	
	strQuery.Format( "select * from xfax"
					" where xstate <> '1'"
					" and xphonenum = '%s'", xPhone );
	TRACE( strQuery + "\n" );
	
	xOpenRecordset(m_pRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
	if ( m_pRecordset->adoEOF ) return FALSE;

	return TRUE;
}

BOOL CMailChannel::PutUsrRecord(LPCTSTR xFaxFile)
{
	CHAR* sptr;
	CHAR* pptr;
	int nLength;
	BOOL bMutiFax = FALSE;
	CHAR szFdTel[MAX_PHONE + 1];

	for	(
		sptr = FaxNumber, nLength = GetNextBuffer( sptr, pptr );
		*sptr != '\0';
		nLength = GetNextBuffer( sptr = pptr + 1, pptr ) )
	{
		if ( ! bMutiFax && *pptr != '\0' ) bMutiFax = TRUE;
		
		nLength = min( nLength, MAX_PHONE );
		CopyMemory( szFdTel, sptr, nLength ); szFdTel[nLength] = '\0';

		CString	strQuery;
		
		if ( bMutiFax == FALSE )
		strQuery.Format( "insert into xfax"
						" (xfaxpath,xPhoneNum,xRecDate,xCaller,xfeetableId)"
						" values('%s','%s',getdate(),'%s','%s')",
						xFaxFile, szFdTel, RemoteId, m_xFeeCode );
		else
		strQuery.Format( "insert into xfax"
						" (xfaxpath,xPhoneNum,xRecDate,xCaller,xfeenumber)"
						" values('%s','%s',getdate(),'%s','%s')",
						xFaxFile, szFdTel, RemoteId, RemoteId );
		TRACE( strQuery + "\n" );
		
		xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
		
		CHAR xPath[MAX_PATH + 1];
		xPath[ GetCurrentDirectory(MAX_PATH,xPath) ] = '\0';
		
#ifdef NPICKUP
		PutUsrEmail( szFdTel, RemoteId, Settings.General.PathFax+"\\1386787\\20060227212822906.tif" );
#else
		PutUsrEmail( szFdTel, RemoteId, xFaxFile );
#endif
		
		SetCurrentDirectory( xPath );
	}
	
	return TRUE;
}

BOOL CMailChannel::PutUsrEmail(LPCTSTR xUsrNumber, LPCTSTR xCallerId, LPCTSTR xFaxFile)
{
	CString	strQuery;
	
	strQuery.Format( "select * from xuser"
					" where xphone = '%s'", xUsrNumber );
	TRACE( strQuery + "\n" );

	_RecordsetPtr Recordusr = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	if ( Recordusr->adoEOF ) return FALSE;
	
	_variant_t TheValue = Recordusr->GetCollect( "xemail" );
	if ( TheValue.vt == VT_NULL ) return FALSE;
	CString xAddress = (LPCTSTR)(_bstr_t)TheValue;

    if ( ! GetProfileInt("Mail", "MAPI", 0) ) return FALSE;
	
    HINSTANCE hMAPIInst = LoadLibrary( "MAPI32.DLL" );
	if ( hMAPIInst == NULL ) return FALSE;
	
    LPMAPILOGON pMAPILogon		= (LPMAPILOGON)
							GetProcAddress(hMAPIInst, "MAPILogon");
    LPMAPILOGOFF pMAPILogoff	= (LPMAPILOGOFF)
							GetProcAddress(hMAPIInst, "MAPILogoff");
    LPMAPISENDMAIL pMAPISendMail= (LPMAPISENDMAIL)
							GetProcAddress(hMAPIInst, "MAPISendMail");
	
	
    LHANDLE lhSession;
    if ( pMAPILogon(0, NULL, NULL, MAPI_LOGON_UI, 0, &lhSession) ) return FALSE;
    
	MapiRecipDesc pRecipDesc[1];
	ZeroMemory( &pRecipDesc, sizeof(MapiRecipDesc) * 1 );
	pRecipDesc[0].ulRecipClass = MAPI_TO;

	pRecipDesc[0].lpszName = xAddress.GetBuffer(0);
//	pRecipDesc[0].lpszName = (LPTSTR)(_bstr_t)TheValue;
//	pRecipDesc[0].lpszAddress = (LPTSTR)(_bstr_t)TheValue;
	pRecipDesc[0].lpszAddress = xAddress.GetBuffer(0);
	//pRecipDesc[0].lpszAddress = _tcsdup((LPCTSTR)(_bstr_t)TheValue);
	
	MapiFileDesc pFileDesc[1];
	ZeroMemory( &pFileDesc, sizeof(MapiFileDesc) * 1 );
	pFileDesc[0].flFlags = MAPI_OLE;
	pFileDesc[0].nPosition  = -1;
	pFileDesc[0].lpszPathName = (CHAR*)xFaxFile;
	
	MapiMessage pMessage;
	ZeroMemory( &pMessage, sizeof(pMessage) );
	
	CString xSubject;
	xSubject.Format( "来自%s给您的一封传真", xCallerId );
	
	pMessage.lpszSubject = xSubject.GetBuffer(0);
	pMessage.lpszNoteText= "需要帮助请咨询96031，谢谢！";
	pMessage.nRecipCount = 1;
	pMessage.lpRecips    = pRecipDesc;
	pMessage.nFileCount  = 1;
	pMessage.lpFiles     = pFileDesc;
	
	int nResult = pMAPISendMail(lhSession, 0, &pMessage, 0, 0);
	pMAPILogoff( lhSession, 0, 0, 0 );
	
	return nResult == 0;
}

//////////////////////////////////////////////////////////////////////
// CMailChannel OVERLOAD function

int CMailChannel::bkIdle_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	CChannel* pChannel = new CChannel( nChan );
	pChannel->Clone( this ); Network.SetChannel( pChannel );

	return (pChannel->*(pChannel->fcnp)->initiate)(evtblk);
}

int CMailChannel::FeeLogin_init(EVTBLK *evtblk)
{
	return 0;
}

int CMailChannel::FeeRebind_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	CChannel::OnConnected();
	SetTimer( 5* 1000 );
	
	return 0;
}

struct state* CMailChannel::FeeLogin_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_TALKING )
		{
			KillTimer(); Reserved &= 0; return &RcvTalk;
		}
	}
	if ( evtblk->wEventCode == E_FEE_Login &&
		(evtblk->dwParam & 0xFFFF) == S_FEE_Connected )
	{
		if ( SsmGetChState(nChan) == S_CALL_TALKING ||	// 其他业务转入
			_tcsncmp(CallType, CA_DEBG, 4) == 0 )		// For Debug
		{
			SsmPutUserEvent( E_CHG_ChState, nChan, S_CALL_TALKING );
		}
		else
		if ( SsmGetChState(nChan) == S_CALL_PICKUPED )	// For CallOut
		{
			KillTimer(); Reserved &= 0; return &WaitDial;
		}
	}
	
	return CChannel::FeeLogin_cmplt(evtblk);
}

int CMailChannel::RcvTalk_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	CChannel::RcvTalk_init(evtblk);
	
	if ( SsmGetChState(nChan) != S_CALL_TALKING )
	SsmSetPlayDest( nChan, 1 );	// 上总线，用于内线测试

	fcnp = &FwdPlay; Reserved = 0;
	return (this->*fcnp->initiate)(evtblk);
}

/////////////////////////////////////////////////////////////////////////////
// CMailChannel Play function

int CMailChannel::FwdPlay_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	if ( PlayFile( FILE_MAIL_WELCOME, FT_PROMPT, TRUE ) )
		return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	return 0;
}

struct state* CMailChannel::FwdPlay_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved = 0; return &RcvFnOper;
	}
	
	return ChkDTSignal( evtblk, &FwdPlay );
}

int CMailChannel::RcvFnOper_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	SsmClearRxDtmfBuf(nChan);
	
	if ( Reserved++ < 3 )
	{
		Reserved |= 0x08;	// 置状态“正在”
		return PlayFile( FILE_MAIL_RCVOPER, FT_PROMPT, TRUE );
	}
	
	ExitCode = EXIT_GOODBYE; fcnp = &ExitPlay;
	return (this->*fcnp->initiate)(evtblk);
}

struct state* CMailChannel::RcvFnOper_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x04;	// 记录播放完毕标志
		SetTimer( 5000 ); return &RcvFnOper;
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
		case	'1':	// 发传真
			Reserved &= 0x00; return &RcvFxTele;
		
		case	'2':	// 收传真
			Reserved &= 0x00; return &GetFxTele;
		
		case	'0':	// 人工
			Reserved &= 0x00; return FwdCaller( "96031" );
		
		default:
			Reserved &= 0x07; return &RcvFnOper;
		}
	}
	
	return ChkDTSignal( evtblk, &RcvFnOper );
}

/////////////////////////////////////////////////////////////////////////////
// CMailChannel send fax

// 传真号码
int CMailChannel::RcvFxTele_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;

	Reserved |= 0x08;
	SsmClearRxDtmfBuf( nChan );
	FaxNumber = new CHAR [MAX_PHONE + 1];
	ZeroMemory( FaxNumber, MAX_PHONE + 1 );
	
	return PlayFile( FILE_MAIL_WTFXTEL, FT_PROMPT, TRUE );
}

struct state* CMailChannel::RcvFxTele_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x04;	// 记录播放完毕标志
	}
	
	if ( evtblk->wEventCode == E_CHG_RcvDTMF )
	{
		int nDtmfLen = SsmGetRxDtmfLen( nChan );
		
		if ( '#' == (CHAR)(evtblk->dwParam & 0xFFFF) || nDtmfLen >= MAX_PHONE )
		{
			if ( ! ReadDtmf( FaxNumber, '#' ) ) CopyMemory( FaxNumber, RemoteId, MAX_PHONE );
		}
	}
	
	if ( (Reserved & 0x04) && _tcslen( FaxNumber ) )
	{
		Reserved &= 0x00; return &RcvFxByte;
	}
	
	return ChkDTSignal( evtblk, &RcvFxTele );
}

// 发送传真
int CMailChannel::RcvFxByte_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	CChannel* pChannel = m_pHomeWnd->FindChannel( CHN_SOFTFAX, S_CALL_STANDBY );
	if ( pChannel == NULL ) return ShExitCode_init( evtblk, EXIT_BUSY );
	
	Reserved |= 0x08; SetLinker( pChannel->m_nChan );
	return PlayFile( FILE_MAIL_RCVFAX, FT_PROMPT, FALSE );
}

struct state* CMailChannel::RcvFxByte_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		UsrLvFax   = MakeUsrFaxFile( "tif" );
		
		CChannel* pChannel = Network.FindChannel( m_nTaskId );
		if ( ! pChannel ) throw "Invalid channel";
		
		if ( pChannel->RecordFax( UsrLvFax, NULL ) )
		{
			CHAR sError[1024];
			SsmGetLastErrMsg( sError );
			theApp.Message( MSG_ERROR, sError );
		}
	}
///////////////////////////	
	if ( evtblk->wEventCode == E_PROC_FaxEnd )//&& m_bPending == TRUE )
	{
		if ( ! RepackFile( (LPCTSTR)UsrLvFax ) )
		{
			CHAR sError[1024];
			fBmp_GetErrMsg( sError );
			theApp.Message( MSG_ERROR, sError );
		}
		
		PutUsrRecord( UsrLvFax ); SetEvent( Traffic.m_pWakeup );
	}
	
	return ChkDTSignal( evtblk, &RcvFxByte );
}

/////////////////////////////////////////////////////////////////////////////
// CMailChannel get unread fax

// 传真号码
int CMailChannel::GetFxTele_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;

	Reserved |= 0x08;
	SsmClearRxDtmfBuf( nChan );
	FaxNumber = new CHAR [MAX_PHONE + 1];
	ZeroMemory( FaxNumber, MAX_PHONE + 1 );
	
	return PlayFile( FILE_MAIL_WTUSRTEL, FT_PROMPT, TRUE );
}

struct state* CMailChannel::GetFxTele_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x04;	// 记录播放完毕标志
	}
	
	if ( evtblk->wEventCode == E_CHG_RcvDTMF )
	{
		int nDtmfLen = SsmGetRxDtmfLen( nChan );
		
		if ( '#' == (CHAR)(evtblk->dwParam & 0xFFFF) || nDtmfLen >= MAX_PHONE )
		{
			if ( ! ReadDtmf( FaxNumber, '#' ) ) CopyMemory( FaxNumber, RemoteId, MAX_PHONE );
		}
	}

	if ( (Reserved & 0x04) && _tcslen( FaxNumber ) )
	{
		Reserved &= 0x00; return &GetFxPass;
	}
	
	return ChkDTSignal( evtblk, &GetFxTele );
}

int CMailChannel::GetFxPass_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;

	Reserved |= 0x08;
	SsmClearRxDtmfBuf( nChan );
	FaxWord = new CHAR [MAX_PASSWORD + 1];
	ZeroMemory( FaxWord, MAX_PASSWORD + 1 );
	
	return PlayFile( FILE_MAIL_WTUSRPAS, FT_PROMPT, TRUE );
}

struct state* CMailChannel::GetFxPass_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x04;	// 记录播放完毕标志
	}
	
	if ( evtblk->wEventCode == E_CHG_RcvDTMF )
	{
		int nDtmfLen = SsmGetRxDtmfLen( nChan );
		
		if ( '#' == (CHAR)(evtblk->dwParam & 0xFFFF) || nDtmfLen >= MAX_PASSWORD )
		{
			if ( ! ReadDtmf( FaxWord, '#' ) ) CopyMemory( FaxWord, "666666", MAX_PHONE );
		}
	}

	if ( (Reserved & 0x04) && _tcslen( FaxWord ) )
	{
		Reserved &= 0x0; return &GetFxByte;
	}
	
	return ChkDTSignal( evtblk, &GetFxPass );
}

int CMailChannel::GetFxByte_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;

	if ( ! ChkUserState(FaxNumber, FaxWord) ) return ShExitCode_init(evtblk, EXIT_NOPAY);
	if ( ! OpenUsrRecord(FaxNumber) ) return ShExitCode_init(evtblk, EXIT_GOODBYE);
	
	CChannel* pChannel = m_pHomeWnd->FindChannel( CHN_SOFTFAX, S_CALL_STANDBY );
	if ( pChannel == NULL ) return ShExitCode_init( evtblk, EXIT_BUSY );
	
	Reserved |= 0x08; SetLinker( pChannel->m_nChan );
	return PlayFile( FILE_MAIL_GETFAX, FT_PROMPT, FALSE );
}

struct state* CMailChannel::GetFxByte_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		CChannel* pChannel = Network.FindChannel( m_nTaskId );
		if ( ! pChannel ) throw "Invalid channel";
		
		_variant_t TheValue = m_pRecordset->GetCollect( "xfaxpath" );
		pChannel->SendFax( (LPCTSTR)(_bstr_t)TheValue, NULL );
	}
	
	if ( evtblk->wEventCode == E_CHG_FaxPages && m_pRecordset != NULL )
	{
		m_pRecordset->PutCollect( "xstate", _variant_t("1") );
		
		COleDateTime tt = COleDateTime::GetCurrentTime();
		m_pRecordset->PutCollect( "xsenddate", _variant_t(tt) );
		
		m_pRecordset->Update();
		
		for ( m_pRecordset->MoveNext(); !m_pRecordset->adoEOF; m_pRecordset->MoveNext() )
		{
			CChannel* pChannel = Network.FindChannel( m_nTaskId );
			if ( ! pChannel ) throw "Invalid channel";
			
			_variant_t TheValue = m_pRecordset->GetCollect( "xfaxpath" );
			pChannel->SendFax( (LPCTSTR)(_bstr_t)TheValue, NULL, TRUE );
			
			m_pRecordset->PutCollect( "xstate", _variant_t("1") );
			
			COleDateTime tt = COleDateTime::GetCurrentTime();
			m_pRecordset->PutCollect( "xsenddate", _variant_t(tt) );
			
			m_pRecordset->Update();
		}
	}
	
	return ChkDTSignal( evtblk, &GetFxByte );
}
