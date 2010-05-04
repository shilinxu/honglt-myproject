//
// ChannelTalk.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Network.h"
#include "Settings.h"
#include "ChannelTalk.h"

#include "Registry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CTalkChannel construction

CTalkChannel::CTalkChannel(int nChan, LPCTSTR xFeeCode) : CChannel( nChan, xFeeCode )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	FeeLogin.initiate		= (INITIATE)FeeLogin_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	rcvMFsig.initiate		= (INITIATE)rcvMFsig_init;
	RcvTalk.initiate		= (INITIATE)RcvTalk_init;
	RcvTalk.process			= (PROCESS)RcvUlTalk_cmplt;
	
	FwdPlay.initiate		= (INITIATE)FwdPlay_init;
	FwdPlay.process			= (PROCESS)FwdPlay_cmplt;
	RcvLvTel.initiate		= (INITIATE)RcvLvTel_init;
	RcvLvTel.process		= (PROCESS)RcvLvTel_cmplt;
	RcvUlTel.initiate		= (INITIATE)RcvUlTel_init;
	RcvUlTel.process		= (PROCESS)RcvUlTel_cmplt;
	
	PageNumber = UserNumber = NULL;
	CTalkChannel::Setup();
}

CTalkChannel::CTalkChannel(int nChan, LPCTSTR xFeeCode, BYTE nReserved) : CChannel( nChan, xFeeCode )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	RcvTalk.initiate		= (INITIATE)RcvUlWord_init;
	
	PageNumber = UserNumber = NULL;
	CTalkChannel::Setup();
}

CTalkChannel::~CTalkChannel()
{
	if ( PageNumber ) delete [] PageNumber;
	if ( UserNumber ) delete [] UserNumber;
}

//////////////////////////////////////////////////////////////////////
// CTalkChannel attributes

void CTalkChannel::Setup()
{
	CChannel::Setup();
	fcnp = &FeeLogin;
	
	SsmSetEvent(E_PROC_PlayFile, m_nChan, FALSE, NULL);
	SsmSetEvent(E_PROC_RecordFile, m_nChan, FALSE, NULL);
	SsmSetEvent(E_SYS_BargeIn, m_nChan, FALSE, NULL);
	SsmSetEvent(E_SYS_NoSound, m_nChan, FALSE, NULL);
}

void CTalkChannel::KillLinker()
{
	if ( m_nTaskId != -1 )
	SsmPutUserEvent( E_CHG_ChState, m_nTaskId,
		MAKEVENT(m_nChan & 0xFFFF, S_CALL_PENDING) );
	
	CChannel::KillLinker();
}

struct state* CTalkChannel::ShExitCode_cmplt(EVTBLK *evtblk, int nExitCode)
{
	ExitCode = nExitCode;
	Reserved = 0; return &ExitPlay;
}

BOOL CTalkChannel::ReadUserShortMobile(LPCTSTR xRemoted, LPTSTR xPageNumber)
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
	
	return TRUE;
}

BOOL CTalkChannel::JoinUserMobile(LPCTSTR xUserId, LPTSTR xPageNumber)
{
	CString strQuery;
	
	strQuery.Format( "select a.xjsjbh as xjsjbh_a, b.* from XusrCommList b"
					" right join xuser a on a.xjsjbh = b.xjsjbh and b.xfmobile = '%s'"
					" where a.xphone = '%s' ", xPageNumber, xUserId );
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr xRecordset;
	xOpenRecordset(xRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
	
	if ( xRecordset->adoEOF || xRecordset->GetCollect( "xjsjbh" ).vt == VT_NULL )
	{
		_variant_t TheValue = xRecordset->GetCollect( "xjsjbh_a" );
		
		xRecordset->AddNew();
		
		xRecordset->PutCollect( "xjsjbh", TheValue );
		xRecordset->PutCollect( "xfmobile", xPageNumber );
		
		xRecordset->Update();
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CTalkChannel operations

CChannel* CTalkChannel::ConvertFrom(CChannel* pParent, LPCTSTR xString)
{
	CopyMemory(pParent->LocalId, "96031666", MAX_PHONE);
	CopyMemory(pParent->CallType, CA_TALK, MAX_CALLTYPE );
	
	pParent->OnConnected(); return NULL;
}

BOOL CTalkChannel::IsAdministrator(LPCTSTR xRemoteId) const
{
	HANDLE hFile = CreateFile(	"Administrator.txt", GENERIC_READ, FILE_SHARE_READ, NULL,
								OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	
	if ( hFile == INVALID_HANDLE_VALUE ) return FALSE;
	
	CString strText;
	DWORD nByte = 65535;
	
	ReadFile( hFile, strText.GetBuffer(nByte), nByte, &nByte, NULL );
	strText.ReleaseBuffer( nByte );
	
	BOOL bFound = ( strText.Find(xRemoteId) != -1 );
	
	CloseHandle( hFile );
	return bFound;
}

//////////////////////////////////////////////////////////////////////
// CTalkChannel TCP Transfer func

BOOL CTalkChannel::OnPacket(CPacket* pPacket)
{
	if ( pPacket->IsCommandId( ICore_RedLink ) )
	{
		return OnRedPacket( (CRedPacket*)pPacket );
	}
	
	return CChannel::OnPacket( pPacket );
}

BOOL CTalkChannel::OnRedPacket(CRedPacket* pPacket)
{
	switch( pPacket->GetMinutes() )
	{
	case 1:	PlayFile( FILE_TALK_REDPAY1, FT_PROMPT, FALSE );
			break;
	case 2:	PlayFile( FILE_TALK_REDPAY2, FT_PROMPT, FALSE );
			break;
	case 3:	PlayFile( FILE_TALK_REDPAY3, FT_PROMPT, FALSE );
			break;
	}
	
	return TRUE;
}

#if 1
BOOL CTalkChannel::RefuseNotLocalMobileForPager(LPCTSTR xRemoted, LPCTSTR xPageNumber)
{
	if ( _tcslen(xPageNumber) == 11 &&
		!_tcsncmp(xPageNumber, "13", 2) || !_tcsncmp(xPageNumber, "15", 2) ) return FALSE;
	
	HANDLE hFile = CreateFile(	"Not4NotMobile.txt", GENERIC_READ, FILE_SHARE_READ, NULL,
								OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	
	if ( hFile == INVALID_HANDLE_VALUE ) return FALSE;
	
	CString strText;
	DWORD nByte = 65535;
	
	ReadFile( hFile, strText.GetBuffer(nByte), nByte, &nByte, NULL );
	strText.ReleaseBuffer( nByte );
	
	BOOL bFound = ( strText.Find(xRemoted) != -1 );
	
	CloseHandle( hFile );
	return bFound;
}
#endif
#if 1
BOOL CTalkChannel::OnRecordPacket(CString* pxRecFile)
{
	SsmStopRecToFile( m_nChan );
/*	
	CHAR xMsgType[MAX_BUFFER];
	xMsgType[ pPacket->GetMsgType(xMsgType) ] = '\0';
	
	if ( ! _tcscmp(xMsgType, "STOP") ) return TRUE;
*/	
	CSettings::Item* pItem = Settings.GetSetting( _T("Trunk.RecFile") );
	CString xNewFile = pItem ? *pItem->m_pString : "";
	
	if ( xNewFile.Find( "<CHAN>" ) >= 0 )
	{
		CString strValue;
		strValue.Format( "%i", m_nChan );
		Replace( xNewFile, "<CHAN>", strValue );
	}
	
	if ( xNewFile.Find( "<DATE>" ) >= 0 )
	{
		SYSTEMTIME pTime;
		GetLocalTime( &pTime );
		
		CString strValue;
		strValue.Format( "%04i年%02i月%02i日",
			pTime.wYear, pTime.wMonth, pTime.wDay );
		Replace( xNewFile, "<DATE>", strValue );
	}
	
	if ( xNewFile.Find( "<TIME>" ) >= 0 )
	{
		SYSTEMTIME pTime;
		GetLocalTime( &pTime );
		
		CString strValue;
		strValue.Format( "%02i时%02i分%02i秒%03i",
			pTime.wHour, pTime.wMinute, pTime.wSecond, pTime.wMilliseconds );
		Replace( xNewFile, "<TIME>", strValue );
	}
	
	if ( xNewFile.Find( "<REMOTE>" ) >= 0 )
	{
		CString strValue;
		Replace( xNewFile, "<REMOTE>", RemoteId );
	}
	
	pItem = Settings.GetSetting( _T(".PathRec") );
	CString xPathRec = pItem ? *pItem->m_pString : "";
	
	for ( LPCTSTR xPath = xNewFile, xPathExt = NULL;
		  xPathExt = _tcsistr(xPath, "\\"); xPath = xPathExt + 1 )
	{
		CString strValue = xNewFile.Left(xPathExt - (LPCTSTR)xNewFile);
		CreateDirectory( xPathRec + "\\" + strValue, NULL );
	}
	
	SsmSetRecMixer( m_nChan, TRUE, 0 );
	RecordFile( xNewFile, xPathRec );
	
	theApp.Message( MSG_TEMP,"Record[%i] -> %s", m_nChan, xNewFile );
	if ( pxRecFile ) *pxRecFile = xPathRec + "\\" + xNewFile;
	
	return TRUE;
}

BOOL CTalkChannel::PutUsrRecord(LPCTSTR xUsrFile)
{
	try
	{
		return PutUsrRecord( xUsrFile, 0 );
	}
	catch(_com_error e) 
	{ 
		theApp.Message( MSG_ERROR, "error on CTalkChannel[%i]: %s", m_nChan, e.ErrorMessage() );
		return FALSE;
	}
}

BOOL CTalkChannel::PutUsrRecord(LPCTSTR xUsrFile, DWORD nReserved)
{
	CString	strQuery;
	
	strQuery.Format( "insert into xuservoice"
					" (xPCMName,xPhoneNum,xRecDate,xCaller,xTheState)"
					" values('%s','%s',getdate(),'%s','DELIVRD')",
					xUsrFile, RemoteId, LocalId );
	TRACE( strQuery + "\n" );
	xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
	
	return TRUE;
}
#endif
//////////////////////////////////////////////////////////////////////
// CTalkChannel OVERLOAD function

int CTalkChannel::bkIdle_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	SsmSetFlag( nChan, F_ClearInVoiceOnRcvDtmf, 0 );
	SsmSetPlayDest( nChan, 0 );	// 取消上总线
	
	CChannel* pChannel = new CChannel( nChan );
	pChannel->Clone( this ); Network.SetChannel( pChannel );

	return (pChannel->*(pChannel->fcnp)->initiate)(evtblk);
}

struct state* CTalkChannel::FeeLogin_cmplt(EVTBLK *evtblk)
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

int CTalkChannel::rcvMFsig_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( SsmGetChState(nChan) == S_CALL_TALKING ||	// 其他业务转入
		_tcsncmp(CallType, CA_DEBG, 4) == 0 )		// For Debug
	{
		return SsmPutUserEvent( E_CHG_ChState, nChan, S_CALL_TALKING );
	}
	
	return CChannel::rcvMFsig_init(evtblk);
}

int CTalkChannel::RcvTalk_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	CChannel::RcvTalk_init(evtblk);
	
	if ( SsmGetChState(nChan) != S_CALL_TALKING )
		SsmSetPlayDest( nChan, 1 );	// 上总线，用于内线测试
	
	fcnp = &FwdPlay; Reserved = 0;
	return (this->*fcnp->initiate)(evtblk);
}

/////////////////////////////////////////////////////////////////////////////
// CTalkChannel Play function

// 语音接入
int CTalkChannel::FwdPlay_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
#if 1	// 通话录音并写库
	CString xRecFile;
	OnRecordPacket( &xRecFile );
	PutUsrRecord( xRecFile );
#endif
#if 0	// 设置DTMF钳位功能
	if ( ! IsAdministrator(RemoteId) )
	SsmSetFlag( nChan, F_ClearInVoiceOnRcvDtmf, 1);
#endif
	
	if ( PlayFile( FILE_TALK_WELCOME, FT_PROMPT, TRUE ) )
		return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	return 0;
}

struct state* CTalkChannel::FwdPlay_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved = 0; return &RcvLvTel;
	}
	
	return ChkDTSignal( evtblk, &FwdPlay );
}

// 输入被叫
#if 0
int CTalkChannel::RcvLvTel_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;

	Reserved |= 0x08;
//CHAR pBuffer[255];
//int nLength = SsmGetDtmfStr( nChan, pBuffer );
//if ( nLength >= 0 ) pBuffer[nLength] = '\0';
//theApp.Message(MSG_ERROR, "debug[%i] -> before SsmClearRxDtmfBuf[%i]: {%s}", nChan, nLength, pBuffer);
	SsmClearRxDtmfBuf( nChan );
	
	if ( PageNumber ) delete PageNumber;
	PageNumber = new CHAR [MAX_PHONE + 1];
	ZeroMemory( PageNumber, MAX_PHONE + 1 );
	
	return PlayFile( FILE_TALK_RCVTELE, FT_PROMPT, TRUE );
}

struct state* CTalkChannel::RcvLvTel_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
#if 0
		if ( _tcsncmp(RemoteId,"13867877813",11) == 0 )
			CopyMemory( PageNumber, "13867877813", MAX_PHONE );
#endif
		Reserved |= 0x04;	// 记录播放完毕标志
	}
	
	if ( evtblk->wEventCode == E_CHG_RcvDTMF )
	{
		int nDtmfLen = SsmGetRxDtmfLen( nChan );
		
		if ( '*' == (CHAR)(evtblk->dwParam & 0xFFFF) )
		{
			Reserved = 0; return &RcvLvTel;
		}
		
		if ( '#' == (CHAR)(evtblk->dwParam & 0xFFFF) || nDtmfLen >= MAX_PHONE )
		{
			if ( ! ReadDtmf( PageNumber, '#' ) ) CopyMemory( PageNumber, RemoteId, MAX_PHONE );
		}
	}
	
	if ( (Reserved & 0x04) && _tcslen( PageNumber ) )
	{
#if 1
		if ( _tcslen(PageNumber) <= 6 )
		ReadUserShortMobile( RemoteId, PageNumber );
#endif
#if 1
		Reserved &= 0x00;
		if ( RefuseNotLocalMobileForPager(RemoteId, PageNumber) ) return &RcvLvTel;
#endif
		Reserved &= 0x00; return &RcvUlTel;
	}
	
	return ChkDTSignal( evtblk, &RcvLvTel );
}
#else
int CTalkChannel::RcvLvTel_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;

	Reserved |= 0x08;
//	SsmClearRxDtmfBuf( nChan );
	
	if ( PageNumber ) delete PageNumber;
	PageNumber = new CHAR [MAX_PHONE + 1];
	ZeroMemory( PageNumber, MAX_PHONE + 1 );
	
	return PlayFile( FILE_TALK_RCVTELE, FT_PROMPT, TRUE );
}

struct state* CTalkChannel::RcvLvTel_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x01;	// 记录播放完毕标志
	}
	
	if ( evtblk->wEventCode == E_CHG_RcvDTMF )
	{
	//	int nDtmfLen = min( evtblk->dwParam >> 16, MAX_PHONE );
	//	PageNumber[ max(nDtmfLen - 1, 0) ] = (CHAR)(evtblk->dwParam & 0xFFFF);
		PageNumber[ _tcslen(PageNumber) ] = (CHAR)(evtblk->dwParam & 0xFFFF);
		
		if ( '#' == (CHAR)(evtblk->dwParam & 0xFFFF) || _tcslen(PageNumber) >= MAX_PHONE )
		{
			if ( '#' == (CHAR)(evtblk->dwParam & 0xFFFF) ) PageNumber[ _tcslen(PageNumber) - 1 ] = '\0';
			Reserved |= 0x02;	// 记录按键完毕标志
		}
		
		if ( '*' == (CHAR)(evtblk->dwParam & 0xFFFF) || ! _tcslen(PageNumber) )
		{
			Reserved = 0; return &RcvLvTel;
		}
	}
	
	if ( (Reserved & 0x03) == 0x03 )
	{
#if 1
		if ( _tcslen(PageNumber) <= 6 )
		ReadUserShortMobile( RemoteId, PageNumber );
#endif
		Reserved &= 0x00; return &RcvUlTel;
	}
	
	return ChkDTSignal( evtblk, &RcvLvTel );
}
#endif
// 输入主叫
int CTalkChannel::RcvUlTel_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	SsmClearRxDtmfBuf( nChan );
	UserNumber = new CHAR [MAX_PHONE + 1];
	ZeroMemory( UserNumber, MAX_PHONE + 1 );
	
	CRegistry pRegistry;
	if ( pRegistry.GetInt( "Talker", "EnableTrick" ) || IsTestNumber( RemoteId ) )
	{
		return PlayFile( FILE_TALK_RCVUSER, FT_PROMPT, TRUE );
	}
	else
	{
		Reserved |= 0x04;	// 记录播放完毕标志
		return SsmPutUserEvent( E_CHG_RcvDTMF, nChan, '#' );
	}
}

struct state* CTalkChannel::RcvUlTel_cmplt(EVTBLK *evtblk)
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
#if 0
			if ( ! ReadDtmf( UserNumber, '#' ) ) CopyMemory( UserNumber, RemoteId, MAX_PHONE );
#else
			if ( ! ReadDtmf( UserNumber, '#' ) ) CopyMemory( UserNumber, LocalId, MAX_PHONE );
#endif
		}
	}
	
	if ( (Reserved & 0x04) && _tcslen( UserNumber ) )
	{
		Reserved &= 0x00; return RcvUlWord_cmplt( evtblk );
	}
	
	return ChkDTSignal( evtblk, &RcvUlTel );
}

// 愉快交流
int CTalkChannel::RcvUlWord_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	CChannel::RcvTalk_init(evtblk);
	Reserved |= 0x08; return 0;
}

struct state* CTalkChannel::RcvUlWord_cmplt(EVTBLK *evtblk)
{
	CChannel* pChannel = Network.LookOutPhs( PageNumber, LocalId );
	if ( pChannel == NULL ) return ShExitCode_cmplt( evtblk, EXIT_BUSY );
	
	CDialPacket* pPacket = new CDialPacket;
	pPacket->SetLocalId( LocalId ); pPacket->SetRemoteId( PageNumber );
	pPacket->SetFeeNumber( RemoteId ); pPacket->SetCallerId( UserNumber );
	
	pChannel->OnDialPacket( pPacket ); pPacket->Release();
	SetLinker( pChannel->m_nChan );
	SsmSetPlayDest( pChannel->m_nChan, 1 );	// 上总线，允许运行于欠费提示
	
	SetTimer( 60 * 1000 ); // 设置1min后写通信录
	
	RcvTalk.initiate = (INITIATE)RcvUlWord_init;
	Reserved &= 0x00; return &RcvTalk;
}

struct state* CTalkChannel::RcvUlTalk_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_PENDING && m_nTaskId == -1 )
		{
			SsmPutUserEvent( E_CHG_ChState, nChan, S_CALL_TALKING );
			Reserved = 0; return &RcvLvTel;
		}
	}
	
	if ( evtblk->wEventCode == E_CHG_RcvDTMF &&
		'*' == (CHAR)(evtblk->dwParam & 0xFFFF) || m_nTaskId == -1 )
	{
		KillLinker(); Reserved = 0; return &RcvLvTel;
	}
	
	if ( evtblk->wEventCode == E_SYS_TIMEOUT )
	{
		JoinUserMobile( RemoteId, PageNumber );
	}
	
	return CChannel::RcvTalk_cmplt(evtblk);
}
