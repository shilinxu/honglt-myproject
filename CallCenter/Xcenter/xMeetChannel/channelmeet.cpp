//
// ChannelMeet.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "ChannelMeet.h"

#include "Network.h"
#include "Settings.h"

#include "Neighbour.h"
#include "Circuitry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CMeetChannel construction

CMeetChannel::CMeetChannel(int nChan, LPCTSTR xFeeCode) : CChannel( nChan, xFeeCode )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	bkOffHk.initiate		= (INITIATE)bkOffHk_init;
	FeeLogin.initiate		= (INITIATE)FeeLogin_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	rcvMFsig.initiate		= (INITIATE)rcvMFsig_init;
	RcvTalk.initiate		= (INITIATE)RcvTalk_init;
	
	FwdPlay.initiate		= (INITIATE)FwdPlay_init;
	FwdPlay.process			= (PROCESS)FwdPlay_cmplt;
	
	RcvMtTele.initiate		= (INITIATE)RcvMtTele_init;
	RcvMtTele.process		= (PROCESS)RcvMtTele_cmplt;
	RegMember.initiate		= (INITIATE)RegMember_init;
	RegMember.process		= (PROCESS)RegMember_cmplt;
	
	JoinRoom.initiate		= (INITIATE)JoinRoom_init;
	JoinRoom.process		= (PROCESS)JoinRoom_cmplt;
	
	RoomNumber		= -1;
	MeetNumber		= NULL;
	CMeetChannel::Setup();
}

// 构造goto-> "跳进会议"
CMeetChannel::CMeetChannel(CChannel* pParent) : CChannel( pParent->m_nChan, "1555" )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	bkOffHk.initiate		= (INITIATE)bkOffHk_init;
	FeeLogin.initiate		= (INITIATE)FeeRebind_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	rcvMFsig.initiate		= (INITIATE)rcvMFsig_init;
	RcvTalk.initiate		= (INITIATE)RcvTalk_init;
	
	RegMember.initiate		= (INITIATE)RegMember_init;
	RegMember.process		= (PROCESS)RegMember_cmplt;
	MsgMember.initiate		= (INITIATE)MsgMember_init;
	MsgMember.process		= (PROCESS)MsgMember_cmplt;
	JoinRoom.initiate		= (INITIATE)JoinRoom_init;
	JoinRoom.process		= (PROCESS)JoinRoom_cmplt;
	
	this->m_hInstance		= pParent->m_hInstance;
	this->FwdPlay			= RegMember;
	
	RoomNumber	= -1;
	CMeetChannel::Setup();
}

CMeetChannel::~CMeetChannel()
{
	if ( MeetNumber ) delete MeetNumber;
}

//////////////////////////////////////////////////////////////////////
// CMeetChannel attributes

void CMeetChannel::Setup()
{
	CChannel::Setup();
	fcnp = &FeeLogin;
}

CChannel* CMeetChannel::ConvertFrom(CChannel* pParent, LPCTSTR xWalkType)
{
	if ( _tcsncmp( xWalkType, "WALK", 4 ) )
	{
		CopyMemory(pParent->LocalId, "96031555", MAX_PHONE);
		CopyMemory(pParent->CallType, CA_TALK, MAX_CALLTYPE );
		
		pParent->OnConnected(); return NULL;
	}
	
	CMeetChannel* pChannel = new CMeetChannel( pParent );
	pChannel->Clone( pParent ); Network.SetChannel( pChannel );
	
	CopyMemory(pChannel->LocalId, "96031555", MAX_PHONE);
	CopyMemory(pChannel->CallType, CA_TALK, MAX_CALLTYPE );
	
	pChannel->FormatObject( xWalkType + 4 );
	(pChannel->*(pChannel->fcnp)->initiate)( NULL );
	
	return pChannel;
}

BOOL CMeetChannel::FormatObject(LPCTSTR xWalkType)
{
	if ( ! xWalkType ) return TRUE;

	m_nTaskId = 0;
	
	for ( LPCTSTR sptr = xWalkType; isdigit(*sptr); sptr++ )
	{
		m_nTaskId = m_nTaskId * 10 + (*sptr) - '0';
	}
	
	if ( ! GetFeeNumber(m_nTaskId, FeeNumber) ) return FALSE;
	RoomNumber = m_nTaskId;
#if 0
	if ( ! _tcsncmp( sptr, "REC", 3 ) )
	{
		IRecdPacket* pPacket = new IRecdPacket(m_nChan);
		OnRecdPacket( pPacket ); pPacket->Release();
	}
#endif
	
	return TRUE;
}

BOOL CMeetChannel::GetFeeNumber(int nRoomId, LPSTR xFeeNumber)
{
	CString	strQuery;
	
	strQuery.Format( "select xmeetingphone"
					" from xmeetingroom"
					" where xmeetingroomcode = '%i'"
					" and xmeetingflag = '1'", nRoomId );
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr xRecordset;
	xRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	if ( xRecordset->adoEOF ) return FALSE;
	
	_variant_t TheValue = xRecordset->GetCollect( "xmeetingphone" );
	if ( VT_NULL == TheValue.vt ) return FALSE;
	
	CString strValue = (LPCTSTR)(_bstr_t)TheValue;
	strValue.TrimLeft(); strValue.TrimRight();
	ZeroMemory( xFeeNumber, MAX_PHONE + 1 );
	CopyMemory( xFeeNumber, (LPCTSTR)strValue, MAX_PHONE );
	
	return TRUE;
}

BOOL CMeetChannel::ReadUserShortMobile(LPCTSTR xRemoted, LPTSTR xPageNumber)
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
// CMeetChannel TCP Transfer func

BOOL CMeetChannel::OnPacket(CPacket* pPacket)
{
	if ( pPacket->IsCommandId( ICore_RedLink ) )
	{
		return OnRedPacket( (CRedPacket*)pPacket );
	}
	
	return CChannel::OnPacket( pPacket );
}

BOOL CMeetChannel::OnRedPacket(CRedPacket* pPacket)
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

BOOL CMeetChannel::OnRecordPacket(CRecordPacket* pPacket)
{
	ASSERT( pPacket->GetChannel() == m_nChan );
	SsmStopRecToFile( m_nChan );
	
	CHAR xMsgType[MAX_BUFFER];
	xMsgType[ pPacket->GetMsgType(xMsgType) ] = '\0';
	
	if ( ! _tcscmp(xMsgType, "STOP") ) return TRUE;
	
	CSettings::Item* pItem = Settings.GetSetting( _T("Meeting.RecFile") );
	CString xNewFile = * pItem->m_pString;
	
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
	
	pItem = Settings.GetSetting( _T("Meeting.RecPath") );
	
	for ( LPCTSTR xPath = xNewFile, xPathExt = NULL;
		  xPathExt = _tcsistr(xPath, "\\"); xPath = xPathExt + 1 )
	{
		CString strValue = xNewFile.Left(xPathExt - (LPCTSTR)xNewFile);
		CreateDirectory( (*pItem->m_pString) + "\\" + strValue, NULL );
	}
	
	SsmSetRecMixer( m_nChan, TRUE, 0 );
	RecordFile( xNewFile, *pItem->m_pString );
	
	PutIntoHistory( RemoteId, (*pItem->m_pString) + "\\" + xNewFile );
	
	theApp.Message( MSG_TEMP,"Record[%i] -> %s", m_nChan, xNewFile );
	return TRUE;
}

BOOL CMeetChannel::OnBindPacket(CBindPacket* pPacket)
{
	if ( FeeLogin.initiate == (INITIATE)FeeRebind_init )
	{
		SsmPutUserEvent( E_FEE_Login, m_nChan, S_FEE_Connected );
		return TRUE;
	}
	
	return CChannel::OnBindPacket( pPacket );
}

//////////////////////////////////////////////////////////////////////
// CMeetChannel operations

#if 0
int CMeetChannel::CreateRoom(LPCTSTR xPhone)
{
	CString	strQuery;
	_variant_t vIndex = (long)0;
	
	strQuery.Format( "select max(cast(xmeetingroomcode as int))"
					" from xmeetingroom" );
	
	_RecordsetPtr Record = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	_variant_t TheValue = Record->Fields->GetItem(vIndex)->Value;

	int nRoomId = (VT_NULL == TheValue.vt) ? 0 : TheValue.lVal;
	
	
	PutIntoRoom( ++nRoomId, xPhone, '1', 'A' + 6 );
	
	return nRoomId;
}
#else
int CMeetChannel::CreateRoom(LPCTSTR xPhone)
{
	CString	strQuery;
	
	strQuery.Format( "select top 1 * from xmeetingroomlist"
					" where xmeetingused <> '1'" );
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr pRecordset;
	xOpenRecordset(pRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
	
	if ( pRecordset->adoEOF ) return -1;
	_variant_t TheValue = pRecordset->GetCollect( "xmeetingroomcode" );
	
	pRecordset->PutCollect( "xmeetingroomfee", _variant_t("1") );
	pRecordset->PutCollect( "xmeetingused", _variant_t(xPhone) );
	pRecordset->Update();
	
	PutIntoRoom( TheValue.lVal, xPhone, '1', 'A' + 6 );
	
	return TheValue.lVal;
}
#endif

BOOL CMeetChannel::ClearRoom(int nRoomId)
{
	CString	strQuery;
	
	strQuery.Format( "delete xmeetingroom"
					" where xmeetingroomcode = '%i' and xmeetingflag = '1'", nRoomId );
	TRACE( strQuery + "\n" );
	xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
	
	strQuery.Format( "update xmeetingroom"
					" set xmeetingused = '0'"
					" where xmeetingroomcode = '%i'", nRoomId );
	TRACE( strQuery + "\n" );
	xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
	
	return TRUE;
}

BOOL CMeetChannel::PutIntoRoom(int nRoomId, LPCTSTR xRoomOwner, LPCTSTR xGroup)
{
	LPCTSTR sptr = xGroup, pptr;
	CHAR szFdTel[MAX_PHONE + 1] = {0};
	
	for	( int nLength = GetNextBuffer( sptr, pptr ); *sptr != '\0';
		nLength = GetNextBuffer( sptr = pptr + 1, pptr ) )
	{
		nLength = min( nLength, MAX_PHONE );
		CopyMemory( szFdTel, sptr, nLength ); szFdTel[nLength] = '\0';
#if 1
		if ( _tcslen(szFdTel) <= 6 )
		ReadUserShortMobile( xRoomOwner, szFdTel );
#endif
		
		PutIntoRoom( RoomNumber, szFdTel, '0', '0' );
	}
	return TRUE;
}

BOOL CMeetChannel::PutIntoRoom(int nRoomId, LPCTSTR xPhone, BYTE nFlag, BYTE nState)
{
	CString	strQuery;
	
	if ( nFlag == '1' )
	strQuery.Format( "insert into xmeetingroom"
					" (xmeetingroomcode,xmeetingphone,xmeetingflag,xmeetingphonestate,xmeetingdatetime)"
					" values('%i','%s','%c','%c',getdate())", nRoomId, xPhone, nFlag, nState );
	else
	strQuery.Format( "insert into xmeetingroom"
					" (xmeetingroomcode,xmeetingphone,xmeetingflag,xmeetingphonestate)"
					" values('%i','%s','%c','%c')", nRoomId, xPhone, nFlag, nState );
	TRACE( strQuery + "\n" );
	
	theApp.Message( MSG_DEBUG, strQuery );
	xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);

	return TRUE;
}

BOOL CMeetChannel::PutIntoHistory(LPCTSTR xFeeNumber, LPCTSTR xRecFile)
{
	CString strQuery;
	
	if ( _tcslen(xRecFile) )
	strQuery.Format( "insert into xphonemeethistory(xmeetphone,xmeettime,xrecordfile)"
					" values('%s',getdate(),'%s')", xFeeNumber, xRecFile );
	else
	strQuery.Format( "insert into xphonemeethistory(xmeetphone,xmeettime)"
					" values('%s',getdate())", xFeeNumber );
	TRACE( strQuery + "\n" );
	
	theApp.Message( MSG_DEBUG, strQuery );
	xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
	
	return TRUE;
}

void CMeetChannel::SuspendMeet(int nRoomId)
{
	WORD nTotalMember = 0;
	DWORD nData = 0;
	
	SsmGetConfGrpInfo( nRoomId, &nTotalMember, (PWORD)&nData, (PWORD)&nData );
	theApp.Message( MSG_TEMP,"Suspend ROOM[%i], total=%i", nRoomId, nTotalMember );
	
	int* pMemberId = new int [nTotalMember + 1];
	SsmGetConfGrpMmbrId( nRoomId, pMemberId );
	
	for ( int nMember = 0; nMember < nTotalMember; nMember ++ )
	{
		int nChan2 = 0;
		
		SsmGetConfGrpMmbrInfo( nRoomId, pMemberId[nMember], &nChan2,
							(PWORD)&nData, (PWORD)&nData, &nData );
		CChannel* pChannel = Network.FindChannel( nChan2 );
		if ( ! pChannel ) throw "Invalid channel";
		// 设置主持人离线后最大预留时间（包括暂时去人工）
		//pChannel->SetTimer( Settings.Meeting.MaxOutTime * 1000 );
		pChannel->SetTimer( 180 * 1000 );
	}
	
	delete [] pMemberId;
}

void CMeetChannel::ResumeMeet(int nRoomId)
{
	WORD nTotalMember = 0;
	DWORD nData = 0;
	
	SsmGetConfGrpInfo( nRoomId, &nTotalMember, (PWORD)&nData, (PWORD)&nData );
	theApp.Message( MSG_TEMP, "Resume ROOM[%i], total=%i", nRoomId, nTotalMember );
	
	int* pMemberId = new int [nTotalMember + 1];
	SsmGetConfGrpMmbrId( nRoomId, pMemberId );
	
	for ( int nMember = 0; nMember < nTotalMember; nMember ++ )
	{
		int nChan2 = 0;
		
		SsmGetConfGrpMmbrInfo( nRoomId, pMemberId[nMember], &nChan2,
							(PWORD)&nData, (PWORD)&nData, &nData );
		CChannel* pChannel = Network.FindChannel( nChan2 );
		if ( ! pChannel ) throw "Invalid channel";
		pChannel->KillTimer();	// 主持人到会，继续会议
	}
	
	delete [] pMemberId;
}

int CMeetChannel::GetMemberIndex(int nRoomId, int nIndex, int* pnChan)
{
	DWORD nData = 0;
	WORD nTotalMember = 0;
	
	SsmGetConfGrpInfo( nRoomId, &nTotalMember, (PWORD)&nData, (PWORD)&nData );
	if ( nIndex >= nTotalMember ) return -1;
	
	int* pMemberId = new int [nTotalMember + 1];
	SsmGetConfGrpMmbrId( nRoomId, pMemberId );
	
	int nAppChan = -1;
	if ( ! pnChan ) pnChan = &nAppChan;
	
	SsmGetConfGrpMmbrInfo( nRoomId, pMemberId[nIndex], pnChan,
		(PWORD)&nData, (PWORD)&nData, &nData );
	
	int nMmberId = pMemberId[nIndex];
	delete [] pMemberId;
	
	return nMmberId;
}

int CMeetChannel::PlayMemberJoined(CChannel* pChannel, int nMmbrId)
{
	CHAR xDigital[16];
	char szFileName[MAX_PATH];
	
	SsmClearFileList( pChannel->m_nChan );
	
	_itot( nMmbrId, xDigital, 10 );
	pChannel->CompDigital( xDigital, TRUE );
	
	sprintf( szFileName, "%s\\%s", FT_PROMPT, FILE_MEET_MEMBSIT );
	SsmAddToFileList( pChannel->m_nChan, szFileName, 6, 0, 0xFFFFFFFF );
	MLOG( szFileName );
	
	return SsmPlayFileList( pChannel->m_nChan );
}

int CMeetChannel::PlayMemberExited(CChannel* pChannel, int nMmbrId)
{
	CHAR xDigital[16];
	char szFileName[MAX_PATH];
	
	SsmClearFileList( pChannel->m_nChan );
	
	_itot( nMmbrId, xDigital, 10 );
	pChannel->CompDigital( xDigital, TRUE );
	
	sprintf( szFileName, "%s\\%s", FT_PROMPT, FILE_MEET_MEMBEXT );
	SsmAddToFileList( pChannel->m_nChan, szFileName, 6, 0, 0xFFFFFFFF );
	MLOG( szFileName );
	
	return SsmPlayFileList( pChannel->m_nChan );
}

int CMeetChannel::PlayMemberSpeaker(int nRoomId, int nChan)
{
	int nMmbrId = -1; DWORD nData = 0;
	SsmGetConfChInfo( nChan, (PINT)&nData, &nMmbrId, (PWORD)&nData, (PWORD)&nData, &nData );
	
	if ( CMeetChannel::GetMemberIndex( nRoomId, 0, (PINT)&nData ) != -1 )
	SsmPutUserEvent( E_TAK_Connect, nData, MAKEVENT(nMmbrId & 0xFFFF, S_TAK_Speaker) );
	
	return 0;
}

int CMeetChannel::PrefixIPSpeaker(LPCTSTR lpszHost, int nChan)
{
	CSingleLock pLock( &Neighbour.m_pSection, TRUE );
	
	CCircuitry* pChannel = Neighbour.FindChannel( lpszHost );
	if ( ! pChannel ) return -1;
	
	pChannel->m_nTaskId = -1 ; pChannel->Reset();
	if ( ! pChannel->Start( nChan, TRUE ) ) return -1;
	
	SsmSetDtmfStopPlay( nChan, FALSE );
	return 0;
}

//////////////////////////////////////////////////////////////////////
// CMeetChannel OVERLOAD function

int CMeetChannel::bkIdle_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	(this->*bkOffHk.initiate)(evtblk);
	SsmSetPlayDest( nChan, 0 );	// 取消上总线
	
	CChannel* pChannel = new CChannel( nChan );
	pChannel->Clone( this ); Network.SetChannel( pChannel );

	return (pChannel->*(pChannel->fcnp)->initiate)(evtblk);
}

int CMeetChannel::bkOffHk_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	SsmExitConfGroup( nChan, TRUE );
	WORD nTotalMember = 0;
	DWORD nData = 0;
	
	SsmGetConfGrpInfo( RoomNumber, &nTotalMember, (PWORD)&nData, (PWORD)&nData );
	
	int* pMemberId = new int [nTotalMember + 1];
	SsmGetConfGrpMmbrId( RoomNumber, pMemberId );
	
	for ( int nMember = 0; nMember < nTotalMember; nMember ++ )
	{
		int nChan2 = 0;
		
		SsmGetConfGrpMmbrInfo( RoomNumber, pMemberId[nMember], &nChan2,
							(PWORD)&nData, (PWORD)&nData, &nData );
		
		SsmPutUserEvent( E_CHG_ChState, nChan2,
						MAKEVENT(nChan & 0xFFFF, S_CALL_PENDING) );
	}
	
	delete [] pMemberId;
	ClearRoom( RoomNumber );
	
	return CChannel::bkOffHk_init(evtblk);
}

int CMeetChannel::FeeLogin_init(EVTBLK *evtblk)
{
	return 0;
}

int CMeetChannel::FeeRebind_init(EVTBLK *evtblk)
{
	if ( Reserved & 0x08 ) return 0;
	Reserved |= 0x08;	// 置状态“正在”
	
	CChannel::OnConnected();
	SetTimer( 5* 1000 );
	
	return 0;
}

struct state* CMeetChannel::FeeLogin_cmplt(EVTBLK *evtblk)
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

int CMeetChannel::rcvMFsig_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( SsmGetChState(nChan) == S_CALL_TALKING ||	// 其他业务转入
		_tcsncmp(CallType, CA_DEBG, 4) == 0 )		// For Debug
	{
		return SsmPutUserEvent( E_CHG_ChState, nChan, S_CALL_TALKING );
	}
	
	return CChannel::rcvMFsig_init( evtblk );
}

int CMeetChannel::RcvTalk_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	CChannel::RcvTalk_init(evtblk);
	
	if ( SsmGetChState(nChan) != S_CALL_TALKING )
	SsmSetPlayDest( nChan, 1 );	// 上总线，用于内线测试
	
	CRecordPacket* pPacket = new CRecordPacket(nChan);
	OnRecordPacket( pPacket ); pPacket->Release();
	
	fcnp = &FwdPlay; Reserved = 0;
	return (this->*fcnp->initiate)(evtblk);
}

/////////////////////////////////////////////////////////////////////////////
// CMeetChannel Play function

int CMeetChannel::FwdPlay_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	if ( PlayFile( FILE_MEET_WELCOME, FT_PROMPT, TRUE ) )
		return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	return 0;
}

struct state* CMeetChannel::FwdPlay_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved = 0; return &RcvMtTele;
	}
	
	return ChkDTSignal( evtblk, &FwdPlay );
}

/////////////////////////////////////////////////////////////////////////////
// CMeetChannel meeting group's

// 会议成员
int CMeetChannel::RcvMtTele_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;
	SsmClearRxDtmfBuf( nChan );
	MeetNumber = new CHAR [MAX_BUFFER + 1];
	ZeroMemory( MeetNumber, MAX_BUFFER + 1 );
	
	return PlayFile( FILE_MEET_WTMTTELE, FT_PROMPT, TRUE );
}

struct state* CMeetChannel::RcvMtTele_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x01;	// 记录播放完毕标志
	}
	
	if ( evtblk->wEventCode == E_CHG_RcvDTMF )
	{
		CHAR cDtmfKey= (CHAR)( evtblk->dwParam & 0xFFFF );
		int nDtmfLen = min( MAX_BUFFER, evtblk->dwParam >> 16 );
		
		if ( '#' != cDtmfKey ) *(MeetNumber + nDtmfLen - 1) = cDtmfKey;
		
		if ( '#' == cDtmfKey || nDtmfLen >= MAX_BUFFER )
		{
			Reserved |= 0x02;	// 标记DTMF#接收成功
		}
	}
	
	if ( (Reserved & 0x03) == 0x03 )
	{
		Reserved &= 0x00; return &RegMember;
	}
	
	return ChkDTSignal( evtblk, &RcvMtTele );
}

int CMeetChannel::RegMember_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	if ( RoomNumber != -1 )
	{
		ResumeMeet( RoomNumber );
		return PlayFile( FILE_MEET_PLYCALL, FT_PROMPT, FALSE );
	}
	
	RoomNumber = CreateRoom( RemoteId );
	PutIntoRoom( RoomNumber, RemoteId, MeetNumber );
	
	return PlayFile( FILE_MEET_PLYCALL, FT_PROMPT, TRUE );
}

struct state* CMeetChannel::RegMember_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		SsmJoinConfGroup( RoomNumber, nChan, 0,	1, TRUE, TRUE );
		
		Reserved = 0; return &JoinRoom;
	}
	
	return ChkDTSignal( evtblk, &RegMember );
}

int CMeetChannel::JoinRoom_init(EVTBLK *evtblk)
{
	return 0;
}

struct state* CMeetChannel::JoinRoom_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_TAK_Connect )
	{
		if ( (evtblk->dwParam & 0xFFFF) == S_TAK_Connected )
		{
			CMeetChannel::PlayMemberJoined( this, evtblk->dwParam >> 16 );
			CMeetChannel::PlayMemberSpeaker( RoomNumber, nChan );
		}
		if ( (evtblk->dwParam & 0xFFFF) == S_TAK_Disconnected )
		{
			CMeetChannel::PlayMemberExited( this, evtblk->dwParam >> 16 );
			CMeetChannel::PlayMemberSpeaker( RoomNumber, nChan );
		}
		
		if ( (evtblk->dwParam & 0xFFFF) == S_TAK_Speaker )
		{
			CMeetChannel::PrefixIPSpeaker( FeeNumber, nChan );
		}
	}
	
	return ChkDTSignal( evtblk, &JoinRoom );
}

/////////////////////////////////////////////////////////////////////////////
// CMeetChannel Pager function

int CMeetChannel::MsgMember_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;

	ASSERT( RoomNumber != -1 );
	CChannel::RcvTalk_init(evtblk);
	
	Reserved |= 0x08;	// 置状态“正在”
	return PlayFile( FILE_MEET_PLYUTAK, FT_PROMPT, FALSE );
}

struct state* CMeetChannel::MsgMember_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		int nMmbrId = -1;
		DWORD nData = 0;
		
		SsmJoinConfGroup( RoomNumber, nChan, 1,	-1, TRUE, TRUE );
		SsmGetConfChInfo( nChan, (PINT)&nData, &nMmbrId, (PWORD)&nData, (PWORD)&nData, &nData );
		
		SsmPutUserEvent( E_TAK_Connect, nChan, MAKEVENT(nMmbrId & 0xFFFF, S_TAK_Connected) );
	
		Reserved = 0; return &JoinRoom;
	}
	
	return ChkDTSignal( evtblk, &MsgMember );
}
