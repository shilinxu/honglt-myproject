//
// ChannelMail.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "ChannelMail.h"
#include "XML.h"

#include "Settings.h"
#include "Profiles.h"
#include "Traffic.h"
#include "Network.h"

#include <mapi.h>
#include "WndMain.h"
#include "WndNetwork.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define	FILE_XENO_RCVWORD		"xeno.rcvword.pcm"

//////////////////////////////////////////////////////////////////////
// CMailChannel construction

CMailChannel::CMailChannel(int nChan, LPCTSTR xFeeCode) : CChannel( nChan, xFeeCode )
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
	
	PlyNotify.initiate		= (INITIATE)PlyNotify_init;
	PlyNotify.process		= (PROCESS)PlyNotify_cmplt;
	
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
	
	RcvUsrPass.initiate		= (INITIATE)RcvUsrPass_init;
	RcvUsrPass.process		= (PROCESS)RcvUsrPass_cmplt;
	
	ExitPlay.initiate		= (INITIATE)ExitPlay_init;
	PlyNotOrder.initiate	= (INITIATE)PlyNotOrder_init;
	PlyNotOrder.process		= (PROCESS)PlyNotOrder_cmplt;
	
	ShRcvFnOper		= NULL;
	m_pXML			= NULL;
	CMailChannel::Setup();
}

// 构造goto-> "跳进发传真"
CMailChannel::CMailChannel(CChannel* pParent) : CChannel( pParent->m_nChan, "1000" )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	FeeLogin.initiate		= (INITIATE)FeeRebind_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	rcvMFsig.initiate		= (INITIATE)rcvMFsig_init;
	RcvTalk.initiate		= (INITIATE)RcvTalk_init;
	
	RcvFxByte.initiate		= (INITIATE)RcvFxByte_init;
	RcvFxByte.process		= (PROCESS)RcvFxByte_cmplt;
	
	this->m_hInstance		= pParent->m_hInstance;
	this->FwdPlay			= RcvFxByte;
	
	CMailChannel::Setup();
}

CMailChannel::~CMailChannel()
{
	if ( m_pXML ) delete m_pXML;
	if ( ShRcvFnOper ) delete ShRcvFnOper;
}

//////////////////////////////////////////////////////////////////////
// CMailChannel attributes

void CMailChannel::Setup()
{
	CChannel::Setup();
	fcnp = &FeeLogin;
	
	if ( Profiles["MailBox"] ) m_pXML = Profiles["MailBox"]->Clone();
	
	CMainWnd* pMainWnd	= (CMainWnd*)theApp.m_pMainWnd;
	ASSERT( pMainWnd->GetSafeHwnd() != NULL );
	
	m_pHomeWnd			= (CNetworkWnd*)pMainWnd->GetWindow( RUNTIME_CLASS(CNetworkWnd) );
	ASSERT( m_pHomeWnd->GetSafeHwnd() != NULL );

	SsmSetEvent(E_PROC_PlayFile, m_nChan, FALSE, NULL);
	SsmSetEvent(E_PROC_RecordFile, m_nChan, FALSE, NULL);
	SsmSetEvent(E_SYS_BargeIn, m_nChan, FALSE, NULL);
	SsmSetEvent(E_SYS_NoSound, m_nChan, FALSE, NULL);
	
	SsmSetFlag( m_nChan, F_HighAndLowFreqEnScale, 18 );
	SsmSetFlag( m_nChan, F_DualAndAllFreqEnScale, 16 );
	SsmSetBargeInSens( m_nChan, 6 );
	
SsmSetEvent(E_CHG_ToneAnalyze, m_nChan, FALSE, NULL);
SsmSetEvent(E_CHG_OvrlEnrgLevel, m_nChan, FALSE, NULL);
SsmSetEvent(E_CHG_ToneDetector, m_nChan, FALSE, NULL);
SsmSetEvent(E_CHG_ToneDetectorItem, m_nChan, FALSE, NULL);
}
#if 0
struct state* CMailChannel::FwdCaller(LPCTSTR xLocalId, struct state* lpszDefaultState)
{
	CopyMemory( LocalId, xLocalId, MAX_PHONE );
	CopyMemory( CallType, CA_TALK, MAX_CALLTYPE );
	
	FeeLogin.initiate	= (INITIATE)CChannel::FeeLogin_init;
	
	Reserved &= 0x0; return fcnp = &FeeLogin;
	CChannel::OnConnected();
	return lpszDefaultState;
}
#endif
struct state* CMailChannel::ShRcvFnOper_cmplt(EVTBLK *evtblk, struct state* lpstDefaultState)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x04;	// 记录播放完毕标志
		if ( (Reserved & 0x03) <= 3 ) SetTimer( 5000 );
	}
	
	if ( evtblk->wEventCode == E_SYS_TIMEOUT )
	{
		Reserved &= 0x03;	// 清除播放完毕标志
		return lpstDefaultState;
	}
	
	if ( (Reserved & 0x04) && SsmGetRxDtmfLen( nChan ) )
	{
		CHAR nDtmfKey = '\0';
		SsmGetLastDtmf( nChan, &nDtmfKey );
		
		KillTimer(); Reserved &= 0x3;
		
		if ( nDtmfKey == '*' && ShRcvFnOper->previous )	// 返回上层
		{
			return Prev_RcvFnOper( ShRcvFnOper, &RcvFnOper );
		}
	}
	
	return ChkDTSignal( evtblk, lpstDefaultState );
}

struct state* CMailChannel::Next_RcvFnOper(PROCESS cmplt, CHAR nDtmfKey, struct state* lpstDefaultState)
{
	ShRcvFnOper = new keyRcvFnOper( ShRcvFnOper, cmplt );
	ShRcvFnOper->hKey = nDtmfKey;
	
	LocalId[ _tcslen(LocalId) ] = nDtmfKey;
	
	CChannel::OnConnected();
	CChannel::RcvTalk_init( NULL );
	
	Reserved = 0; return lpstDefaultState;
}

struct state* CMailChannel::Next_RcvFnOper(PROCESS cmplt, CHAR nDtmfKey, struct state* lpstDefaultState, DWORD nReserved)
{
	Next_RcvFnOper( cmplt, nDtmfKey, lpstDefaultState );
	
	ShRcvFnOper->Reserved = nReserved;
	return lpstDefaultState;
}

struct state* CMailChannel::Prev_RcvFnOper(struct keyRcvFnOper*& ShRcvFnOper, struct state* lpstDefaultState)
{
	if ( ! ShRcvFnOper->previous ) return lpstDefaultState;
	
	keyRcvFnOper* shRcvFnOper = ShRcvFnOper->previous;
	ShRcvFnOper->previous = NULL; delete ShRcvFnOper;
	ShRcvFnOper = shRcvFnOper;
	
	LocalId[ _tcslen(LocalId) - 1 ] = '\0';
	
	Reserved = 0; return lpstDefaultState;
}

CXMLElement* CMailChannel::FindPlay(struct keyRcvFnOper* ShRcvFnOper, CHAR nDtmfKey)
{
	BYTE szKey[16] = {0};
	if ( ShRcvFnOper ) ShRcvFnOper->RenderKey( szKey );
	
	CString strKey;
	strKey.Format( "%s.%s%c", m_xFeeCode, szKey, nDtmfKey );
	
	CXMLElement* pParent = (CXMLElement*)ShRcvFnOper->Reserved;

	return Profiles.LookupKey( pParent, strKey );
}

BOOL CMailChannel::ChkUserState(LPCTSTR xUsrNumber, LPCTSTR xUsrWord)
{
	CString	strQuery;
	
	strQuery.Format( "select * from xuser_info"
					" where xjsjbh ="
						" (select top 1 xjsjbh from xuser"
						" where xphone = '%s')", xUsrNumber );
	TRACE( strQuery + "\n" );

	xOpenRecordset(m_pRecordusr, theApp.m_pConnection, (_bstr_t)strQuery);
//	m_pRecordusr = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	if ( m_pRecordusr->adoEOF ) return FALSE;
	
	if ( xUsrWord && ! ChkUserState(xUsrWord, (DWORD)0) ) return FALSE;
	
	_variant_t xUserId = m_pRecordusr->GetCollect( "xjsjbh" );
	
	strQuery.Format( "select * from xusefee"
					" where xjsjbh = '%s' and xfeecode = '%s'",
					(LPCTSTR)(_bstr_t)xUserId, "8802" );
	TRACE( strQuery + "\n" );

	_RecordsetPtr Recordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	return ( Recordset->adoEOF == FALSE );
}

BOOL CMailChannel::ChkUserState(LPCTSTR xUsrWord, DWORD)
{
	ASSERT ( ! m_pRecordusr->adoEOF );
	
	_variant_t TheValue = m_pRecordusr->GetCollect( "xuserpwd" );
	if ( TheValue.vt == VT_NULL ) return TRUE;
	
	return _tcscmp(xUsrWord, (LPCTSTR)(_bstr_t)TheValue) == 0;
}

int CMailChannel::ShRcvTalk_init(EVTBLK *evtblk, struct state* lpszDefaultState)
{
	CChannel::RcvTalk_init(evtblk);
	
	fcnp = lpszDefaultState; Reserved = 0;
	return (this->*fcnp->initiate)(evtblk);
}

int CMailChannel::ShExitCode_init(EVTBLK *evtblk, int nExitCode)
{
	ExitCode = nExitCode; fcnp = &ExitPlay;
	Reserved = 0; return (this->*fcnp->initiate)(evtblk);
}

CString CMailChannel::MakeUsrFaxFile(LPCTSTR sBuffer, LPTSTR sExtTitle)
{
	CString strValue = sBuffer;
	CString str	= (strValue + '|').SpanExcluding( _T("*|,") );
	
	CString xVbDirect = Settings.General.PathFax;
	xVbDirect += '\\'; xVbDirect += str.Left( 4 );
	CreateDirectory( xVbDirect, NULL );
	
	SYSTEMTIME pTime;
	GetLocalTime( &pTime );
	
	CString xUsrFile;
	xUsrFile.Format( "%s\\%04i%02i%02i%02i%02i%02i%03i.%s",
		xVbDirect, pTime.wYear, pTime.wMonth, pTime.wDay,
		pTime.wHour, pTime.wMinute, pTime.wSecond, pTime.wMilliseconds,
		sExtTitle );
	
	MLOG( xUsrFile );
	return xUsrFile;
}

BOOL CMailChannel::RepackFile(LPCTSTR xFaxFile, LPCTSTR xFaxNumber)
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
	fBmp_AddTxtToTif( xNewFile.GetBuffer(0), RemoteId, (LPSTR)xFaxNumber,
		"", xDateTime, (LPSTR)xFaxFile, 0 );
	if ( nResult == -1 ) return FALSE;
	
	DeleteFile( xNewFile );
	
	return TRUE;
}

BOOL CMailChannel::OpenUsrRecord(LPCTSTR xPhone, int nTopic)
{
	CString strQuery;
	
	strQuery.Format( "select top %i * from xfax"
					//" where xstate <> '1'"
					" where xphonenum = '%s'", nTopic, xPhone );
	TRACE( strQuery + "\n" );
	
	xOpenRecordset(m_pRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
	if ( m_pRecordset->adoEOF ) return FALSE;

	return TRUE;
}

BOOL CMailChannel::PutUsrRecord(LPCTSTR xFaxFile, LPCTSTR xFaxNumber)
{
	CString strValue = xFaxNumber;
	BOOL bMuti = strValue.FindOneOf( _T("|*") );
	
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|*") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		if ( TryPagerToGroup(_ttoi(str), xFaxFile) ) continue;
		
		if ( bMuti == TRUE )
			PutUsrRecord( xFaxFile, str, "xfeenumber", RemoteId );
		else
			PutUsrRecord( xFaxFile, str, "xfeetableId", m_xFeeCode );
	}
	
	return TRUE;
}

BOOL CMailChannel::PutUsrRecord(LPCTSTR xFaxFile, LPCTSTR xFaxNumber, LPCTSTR xFeeType, LPCTSTR xFeeNumber)
{
	CString	strQuery;
	
	strQuery.Format( "insert into xfax"
		" (xfaxpath,xPhoneNum,xRecDate,xCaller,%s)"
		" values('%s','%s',getdate(),'%s','%s')",
		xFeeType, xFaxFile, xFaxNumber, RemoteId, xFeeNumber );
	TRACE( strQuery + "\n" );
	
	xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
	
	CHAR xPath[MAX_PATH + 1];
	xPath[ GetCurrentDirectory(MAX_PATH,xPath) ] = '\0';
	
	PutUsrEmail( xFaxNumber, RemoteId, xFaxFile );
	SetCurrentDirectory( xPath );
	
	return TRUE;
}

BOOL CMailChannel::TryPagerToGroup(int xGroupId, LPCTSTR xUsrFile)
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
		
		PutUsrRecord( xUsrFile, xPhone, "xfeenumber", RemoteId );
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
	pMessage.lpszNoteText= "更多精彩请咨询96031，谢谢！";
	pMessage.nRecipCount = 1;
	pMessage.lpRecips    = pRecipDesc;
	pMessage.nFileCount  = 1;
	pMessage.lpFiles     = pFileDesc;
	
	int nResult = pMAPISendMail(lhSession, 0, &pMessage, 0, 0);
	pMAPILogoff( lhSession, 0, 0, 0 );
	
	return nResult == 0;
}

//////////////////////////////////////////////////////////////////////
// CMailChannel operations

CChannel* CMailChannel::ConvertFrom(CChannel* pParent, LPCTSTR xString)
{
	if ( ! xString || ! _tcslen(xString) )
	{
		CopyMemory(pParent->LocalId, "96031000", MAX_PHONE);
		CopyMemory(pParent->CallType, CA_TALK, MAX_CALLTYPE );
		
		pParent->OnConnected(); return NULL;
	}
	
	CMailChannel* pChannel = new CMailChannel( pParent );
	pChannel->Clone( pParent ); Network.SetChannel( pChannel );
	
	CopyMemory(pChannel->LocalId, "96031000", MAX_PHONE);
	CopyMemory(pChannel->CallType, CA_TALK, MAX_CALLTYPE );
	
	pChannel->FormatObject( xString );
	(pChannel->*(pChannel->fcnp)->initiate)( NULL );
	
	return pChannel;
}

BOOL CMailChannel::FormatObject(LPCTSTR xString)
{
#if 0	
	if ( ! xString ) return TRUE;
	
	FaxNumber = new CHAR [4096 + 1];
	ZeroMemory( FaxNumber, 4096 + 1 );

	for ( int i = 0; xString[ i ] != '\0'; i ++ )
	{
		if ( xString[i] == '#' ) break;
		if ( i >= 4096 ) break;

		FaxNumber[ i ] = xString[ i ];
	}
#endif

	return TRUE;
}

struct state* CMailChannel::ShRcvFxDtmf_cmplt(EVTBLK *evtblk, struct state* lpstDefaultState, int nMax)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd ) Reserved |= 0x01;
	
	if ( evtblk->wEventCode == E_CHG_RcvDTMF )
	{
		CHAR cDtmfKey= (CHAR)( evtblk->dwParam & 0xFFFF );
		int nDtmfLen = min( MAX_BUFFER, evtblk->dwParam >> 16 );
		
		if ( '#' != cDtmfKey ) *(m_pBuffer + nDtmfLen - 1) = cDtmfKey;
		if ( '#' == cDtmfKey || nDtmfLen >= MAX_BUFFER ) Reserved |= 0x02;
	}
	
	if ( (Reserved & 0x03) == 0x03 )
	{
		Reserved &= 0x00; return lpstDefaultState;
	}
	
	return ChkDTSignal( evtblk, this->fcnp );
}

//////////////////////////////////////////////////////////////////////
// CMailChannel TCP Transfer func

BOOL CMailChannel::OnBindPacket(CBindPacket* pPacket)
{
	if ( FeeLogin.initiate == (INITIATE)FeeLogin_init ) return TRUE;
	return CChannel::OnBindPacket( pPacket );
}

//////////////////////////////////////////////////////////////////////
// CMailChannel OVERLOAD function

int CMailChannel::bkIdle_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	(this->*bkOffHk.initiate)(evtblk);
	SsmSetPlayDest( nChan, 0 );	// 取消上总线
	SsmSetFlag( nChan, F_ClearInVoiceOnRcvDtmf, 0 );
	
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

int CMailChannel::rcvMFsig_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( SsmGetChState(nChan) == S_CALL_TALKING ||	// 其他业务转入
		_tcsncmp(CallType, CA_DEBG, 4) == 0 )		// For Debug
	{
		return SsmPutUserEvent( E_CHG_ChState, nChan, S_CALL_TALKING );
	}
	
	return CChannel::rcvMFsig_init( evtblk );
}

int CMailChannel::RcvTalk_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	CChannel::RcvTalk_init(evtblk);
	
	if ( SsmGetChState(nChan) != S_CALL_TALKING )
	SsmSetPlayDest( nChan, 1 );	// 上总线，用于内线测试

	return ShRcvTalk_init( evtblk, &FwdPlay );
}

int CMailChannel::ExitPlay_init(EVTBLK *evtblk)
{
	if ( ! Reserved && ExitCode == MAIL_REPLYOK )
	{
		Reserved |= 0x08;	// 置状态“正在”
		return PlayFile( FILE_MAIL_REPLYOK, FT_PROMPT, FALSE );
	}
	if ( ! Reserved && ExitCode == MAIL_CHANGEPASSOK )
	{
		Reserved |= 0x08;	// 置状态“正在”
		return PlayFile( FILE_MAIL_CHGPASOK, FT_PROMPT, FALSE );
	}
	if ( ! Reserved && ExitCode == MAIL_ORDEROK )
	{
		Reserved |= 0x08;	// 置状态“正在”
		return PlayFile( FILE_MAIL_ORDEROK, FT_PROMPT, FALSE );
	}
	if ( ! Reserved && ExitCode == MAIL_UNORDEROK )
	{
		Reserved |= 0x08;	// 置状态“正在”
		return PlayFile( FILE_MAIL_UNORDEROK, FT_PROMPT, FALSE );
	}
	
	return CChannel::ExitPlay_init(evtblk);	
}

/////////////////////////////////////////////////////////////////////////////
// CMailChannel Play function

int CMailChannel::FwdPlay_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	SsmSetFlag( nChan, F_ClearInVoiceOnRcvDtmf, 1);
	
	if ( PlayFile( FILE_MAIL_WELCOME, FT_PROMPT, FALSE ) )
		return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	return 0;
}

struct state* CMailChannel::FwdPlay_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		ShRcvFnOper = new keyRcvFnOper( ShRcvFnOper, (PROCESS)RcvFnOper_cmplt );
		ShRcvFnOper->Reserved = (DWORD)m_pXML;
		
		Reserved = 0; return &RcvFnOper;
	}
	
	return ChkDTSignal( evtblk, &FwdPlay );
}

int CMailChannel::RcvFnOper_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	SsmClearRxDtmfBuf(nChan);
	RcvFnOper.process = ShRcvFnOper ? ShRcvFnOper->process : (PROCESS)RcvFnOper_cmplt;
	
	CString strFile = FILE_MAIL_RCVOPER;
	
	BYTE szKey[16] = {0};
	if ( ShRcvFnOper ) ShRcvFnOper->RenderKey( szKey );
	
	if ( _tcslen( (LPTSTR)szKey ) )
	{
		strFile.Insert( strFile.GetLength() - 4, '.' );
		strFile.Insert( strFile.GetLength() - 4, (LPTSTR)szKey );
	}
	
	if ( (Reserved++ < 3) && (Reserved |= 0x08) )
	{
		if ( ! PlayFile( strFile, FT_PROMPT, TRUE ) ) return 0;
		return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	}
	
	return ShExitCode_init(evtblk, EXIT_GOODBYE);
}
#if 0
struct state* CMailChannel::RcvFnOper_cmplt(EVTBLK *evtblk)
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
		case	'1':	// 发传真
			Reserved &= 0x00; return &RcvFxTele;
		
		case	'2':	// 收传真
			Reserved &= 0x00; return &GetFxTele;
		
		case	'0':	// 人工
			//Reserved &= 0x00; 
			return FwdCaller( "96031", &RcvFnOper );
		
		default:
			Reserved &= 0x07; return &RcvFnOper;
		}
	}
	
	return ChkDTSignal( evtblk, &RcvFnOper );
}
#else
struct state* CMailChannel::RcvFnOper_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x04;	// 记录播放完毕标志
	}
	if ( (Reserved & 0x04) && SsmGetRxDtmfLen( nChan ) )
	{
		CHAR nDtmfKey = '\0';
		SsmGetLastDtmf( nChan, &nDtmfKey );
		
		if ( nDtmfKey == '*' )
		{
			return ShRcvFnOper_cmplt( evtblk, &RcvFnOper );
		}
		
		CXMLElement* pXML = (CXMLElement*)ShRcvFnOper->Reserved;
		CString strValue = pXML->GetAttributeValue( "Host" );
		
		if ( ! strValue.IsEmpty() && strValue.Find(nDtmfKey) == -1 )
		{
			return ChkDTSignal( evtblk, &RcvFnOper );
		}
		
		KillTimer(); Reserved &= 0x3;
		
		CHAR szDtmfKey[MAX_BUFFER] = {0};
		ReadDtmf( szDtmfKey, strValue.GetLength() ? strValue.GetAt(0) : '#' );
		
		nDtmfKey = min( (BYTE)_ttoi(szDtmfKey), 35 );
		nDtmfKey +=	( nDtmfKey < 10 ) ? '0' : 'A' - 10;
		
		if ( CXMLElement* pXML = FindPlay(ShRcvFnOper, nDtmfKey) )
		{
			return RcvFnOper_cmplt( pXML, nDtmfKey );
		}
	}
	
	return ShRcvFnOper_cmplt( evtblk, &RcvFnOper );
}

struct state* CMailChannel::RcvFnOper_cmplt(CXMLElement* pXML, CHAR nDtmfKey)
{
	CString strValue = pXML->GetAttributeValue( "Class" );
	
	if ( ! strValue.CompareNoCase( _T("发送") ) )
	{
		return Next_RcvFnOper( (PROCESS)RcvFxTele_cmplt, nDtmfKey, &PlyNotify );
	}
	else if ( ! strValue.CompareNoCase( _T("接收") ) )
	{
		return Next_RcvFnOper( (PROCESS)GetFxTele_cmplt, nDtmfKey, &PlyNotify );
	}
	else if ( ! strValue.CompareNoCase( _T("转发") ) )
	{
		RcvFxTele.process		= (PROCESS)ReplyTele_cmplt;
		return Next_RcvFnOper( (PROCESS)RcvFxTele_cmplt, nDtmfKey, &PlyNotify );
	}
	else if ( ! strValue.CompareNoCase( _T("退订服务") ) )
	{
		return Next_RcvFnOper( (PROCESS)RcvUnorder_cmplt, nDtmfKey, &PlyNotify );
	}
	else if ( ! strValue.CompareNoCase( _T("订阅服务") ) )
	{
		return Next_RcvFnOper( (PROCESS)RcvOrder_cmplt, nDtmfKey, &PlyNotify );
	}
	else if ( ! strValue.CompareNoCase( _T("修改密码") ) )
	{
		return Next_RcvFnOper( (PROCESS)RcvUsrPass_cmplt, nDtmfKey, &PlyNotify );
	}
	else if ( ! strValue.CompareNoCase( _T("子类") ) )
	{
		return Next_RcvFnOper( (PROCESS)RcvFnOper_cmplt, nDtmfKey, &PlyNotify, (DWORD)pXML );
	}
	
	return &RcvFnOper;
}
#endif
int CMailChannel::PlyNotify_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	CString strFile = FILE_MAIL_WIZARD;
	
	BYTE szKey[16] = {0};
	if ( ShRcvFnOper ) ShRcvFnOper->RenderKey( szKey );
	
	if ( _tcslen( (LPTSTR)szKey ) )
	{
		strFile.Insert( strFile.GetLength() - 4, '.' );
		strFile.Insert( strFile.GetLength() - 4, (LPTSTR)szKey );
	}
	
	if ( PlayFile( strFile, FT_PROMPT, TRUE ) )
	{
		Reserved &= 0x0; fcnp = (this->*ShRcvFnOper->process)(evtblk);
		return (this->*fcnp->initiate)(evtblk); 
	}
	
	return 0;
}

struct state* CMailChannel::PlyNotify_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved &= 0x0; return (this->*ShRcvFnOper->process)(evtblk);
	}
	
	return ChkDTSignal( evtblk, &PlyNotify );
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
	ZeroMemory( m_pBuffer, MAX_BUFFER + 1 );
	
	return PlayFile( FILE_MAIL_WTFXTEL, FT_PROMPT, TRUE );
}

struct state* CMailChannel::RcvFxTele_cmplt(EVTBLK *evtblk)
{
	this->fcnp = &RcvFxTele;
	return ShRcvFxDtmf_cmplt(evtblk, &RcvFxByte);
}

// 发送传真
int CMailChannel::RcvFxByte_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	CChannel* pChannel = m_pHomeWnd->FindChannel( CHN_SOFTFAX, S_CALL_STANDBY );
#ifdef NPICKUP
	if ( ! pChannel ) throw "Not found fax channel";
#endif
	if ( pChannel == NULL ) return ShExitCode_init( evtblk, EXIT_BUSY );
	
	Reserved |= 0x08; SetLinker( pChannel->m_nChan );
	return PlayFile( FILE_MAIL_RCVFAX, FT_PROMPT, FALSE );
}

struct state* CMailChannel::RcvFxByte_cmplt(EVTBLK *evtblk)
{
	static CString strFile;
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		strFile = MakeUsrFaxFile( m_pBuffer, "tif" );
		
		CChannel* pChannel = Network.FindChannel( m_nTaskId );
		if ( ! pChannel ) throw "Invalid channel";
		
		if ( pChannel->RecordFax( strFile, NULL ) )
		{
			CHAR sError[1024];
			SsmGetLastErrMsg( sError );
			theApp.Message( MSG_ERROR, sError );
		}
	}
	if ( evtblk->wEventCode == E_PROC_FaxEnd )//&& m_bPending == TRUE )
	{
		if ( ! RepackFile( strFile, m_pBuffer ) )
		{
			CHAR sError[1024];
			fBmp_GetErrMsg( sError );
			theApp.Message( MSG_ERROR, sError );
		}
		
		PutUsrRecord( strFile, m_pBuffer ); SetEvent( Traffic.m_pWakeup );
	}
	
	return ChkDTSignal( evtblk, &RcvFxByte );
}

/////////////////////////////////////////////////////////////////////////////
// CMailChannel get read/unread fax

// 传真号码
int CMailChannel::GetFxTele_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;

	Reserved |= 0x08;
	SsmClearRxDtmfBuf( nChan );
	ZeroMemory( m_pBuffer, MAX_BUFFER + 1 );
	
	return PlayFile( FILE_MAIL_WTUSRTEL, FT_PROMPT, TRUE );
}

struct state* CMailChannel::GetFxTele_cmplt(EVTBLK *evtblk)
{
	this->fcnp = &GetFxTele;
	
	struct state* st = ShRcvFxDtmf_cmplt(evtblk, &GetFxPass);
	if ( st != &GetFxPass ) return st;
	
	if ( ! ChkUserState(m_pBuffer, (LPCTSTR)0) )
	{
#if 0
		ShExitCode_init(evtblk, EXIT_NOPAY); return &ExitPlay;
#else
		Reserved &= 0x0; return &PlyNotOrder;
#endif
	}
	
	return &GetFxPass;
}

int CMailChannel::GetFxPass_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;

	Reserved |= 0x08;
	SsmClearRxDtmfBuf( nChan );
	ZeroMemory( m_pBuffer, MAX_BUFFER + 1 );
	
	return PlayFile( FILE_MAIL_WTUSRPAS, FT_PROMPT, TRUE );
}

struct state* CMailChannel::GetFxPass_cmplt(EVTBLK *evtblk)
{
	struct state* st = ShRcvFxDtmf_cmplt(evtblk, &GetFxByte, MAX_PASSWORD);
	if ( st != &GetFxByte ) return st;
	
	if ( ! ChkUserState(m_pBuffer, (DWORD)0) )
	{
#if 0
		ShExitCode_init(evtblk, EXIT_NOPAY); return &ExitPlay;
#else
		Reserved &= 0x0; return &PlyNotOrder;
#endif
	}
	
	//xCloseRecordset(m_pRecordusr);
	return &GetFxByte;
}

int CMailChannel::GetFxByte_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;

	_variant_t TheValue = m_pRecordusr->GetCollect( "xphone" );
	if ( ! OpenUsrRecord((LPCTSTR)(_bstr_t)TheValue) ) return ShExitCode_init(evtblk, EXIT_GOODBYE);
	xCloseRecordset(m_pRecordusr);
	
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

int CMailChannel::PlyNotOrder_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	if ( PlayFile( FILE_MAIL_NOTORDER, FT_PROMPT, FALSE ) )
		return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	return 0;
}

struct state* CMailChannel::PlyNotOrder_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		return Prev_RcvFnOper( ShRcvFnOper, &RcvFnOper );
	}
	
	return ChkDTSignal( evtblk, &RcvFnOper );
}

/////////////////////////////////////////////////////////////////////////////
// CMailChannel reply fax

struct state* CMailChannel::ReplyTele_cmplt(EVTBLK *evtblk)
{
	this->fcnp = &RcvFxTele;
	
	struct state* st = ShRcvFxDtmf_cmplt(evtblk, &RcvFxByte);
	if ( st != &RcvFxByte ) return st;
	
	if ( OpenUsrRecord(RemoteId, 1) )
	{
		_variant_t TheValue = m_pRecordset->GetCollect( "xfaxpath" );
		PutUsrRecord( (LPCTSTR)(_bstr_t)TheValue, m_pBuffer );
	}
	
	ShExitCode_init(evtblk, MAIL_REPLYOK); return &ExitPlay;
}

/////////////////////////////////////////////////////////////////////////////
// CMailChannel order/unorder relation

struct state* CMailChannel::RcvOrder_cmplt(EVTBLK *evtblk)
{
	CString	strQuery;
	
	strQuery.Format( "exec xsyncorder '%s', '8802'", this->RemoteId );
	TRACE( strQuery + "\n" );
	
	xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
	
	ShExitCode_init(evtblk, MAIL_ORDEROK); return &ExitPlay;
}

struct state* CMailChannel::RcvUnorder_cmplt(EVTBLK *evtblk)
{
	CString	strQuery;
	
	strQuery.Format( "exec xunsyncorder '%s', '8802'", this->RemoteId );
	TRACE( strQuery + "\n" );
	
	xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
	
	ShExitCode_init(evtblk, MAIL_UNORDEROK); return &ExitPlay;
}

/////////////////////////////////////////////////////////////////////////////
// CMailChannel change password

int CMailChannel::RcvUsrPass_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;

	Reserved |= 0x08;
	SsmClearRxDtmfBuf( nChan );
	ZeroMemory( m_pBuffer, MAX_BUFFER + 1 );
	
	return PlayFile( FILE_XENO_RCVWORD, FT_PROMPT, TRUE );
}

struct state* CMailChannel::RcvUsrPass_cmplt(EVTBLK *evtblk)
{
	this->fcnp = &RcvUsrPass;
	
	struct state* st = ShRcvFxDtmf_cmplt(evtblk, NULL, MAX_PASSWORD);
	if ( st != NULL ) return st;
	
	ChkUserState( this->RemoteId, (LPCTSTR)NULL );
	
	m_pRecordusr->PutCollect( "xuserpwd", m_pBuffer );
	m_pRecordusr->Update();

	xCloseRecordset(m_pRecordusr);
	ShExitCode_init(evtblk, MAIL_CHANGEPASSOK); return &ExitPlay;
}
