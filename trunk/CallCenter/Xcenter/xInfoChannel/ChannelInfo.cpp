//
// ChannelInfo.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Network.h"
#include "Traffic.h"
#include "Settings.h"
#include "Profiles.h"
#include "ChannelInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CInfoChannel construction

CInfoChannel::CInfoChannel(int nChan, LPCTSTR xFeeCode) : CChannel( nChan, xFeeCode )
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
	
	PlayXFile.initiate		= (INITIATE)PlayXFile_init;
	PlayXFile.process		= (PROCESS)PlayXFile_cmplt;
	
	PageNumber	= NULL;
	BjPlaying	= FALSE;
	CInfoChannel::Setup();
}

CInfoChannel::~CInfoChannel()
{
	if ( PageNumber ) delete PageNumber;
}

//////////////////////////////////////////////////////////////////////
// CInfoChannel attributes

void CInfoChannel::Setup()
{
	CChannel::Setup();
	
	fcnp = &FeeLogin;
	m_pRecordset = NULL;
}

struct state* CInfoChannel::FwdNotify(LPCTSTR xLocalId)
{
	CopyMemory( LocalId, xLocalId, MAX_PHONE );
	CopyMemory( CallType, CA_TALK, MAX_CALLTYPE );
	FeeLogin.initiate	= (INITIATE)FeeRebind_init;
	
	return fcnp = &FeeLogin;
}

struct state* CInfoChannel::FwdXPlay(BYTE nDtmfKey)
{
#if 1
	// 联通用户另行计费
	if ( nDtmfKey == '6' && IsUnicomUser() )
	{
		CopyMemory( LocalId, "959516", MAX_PHONE );
		CopyMemory( CallType, CA_TALK, MAX_CALLTYPE );
		FeeLogin.initiate	= (INITIATE)FeeLogin_init;
		
		CChannel::OnConnected();
		CChannel::RcvTalk_init( NULL );
	}
#endif
	
	CHAR xIndex[6]; xIndex[0] = nDtmfKey; xIndex[1] = '\0';
	CString xPlay = _T(""); Profiles.FindPlay( xIndex, &xPlay );
	
	if ( ! OpenXRecord( xPlay ) ) return &RcvFnOper;
	return &PlayXFile;
}

BOOL CInfoChannel::OpenXRecord(LPCTSTR xPlay)
{
	CString strQuery;
	
	strQuery.Format( "select * from xvoiceinfo"
					" where xvoicecode = (select xvoicecode from xvoxzb"
						" where xvoicename = '%s') order by newid()", (LPCTSTR)xPlay );
	TRACE( strQuery + "\n" );
	
	m_pRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	if ( m_pRecordset->adoEOF ) return FALSE;
	
	return TRUE;
}

BOOL CInfoChannel::IsOnlyUserId(LPCTSTR xRemoteId)
{
	CSettings::Item* pItem = Settings.GetSetting( _T("Play.OnlyUser") );
	if ( *pItem->m_pDword == FALSE ) return TRUE;
	
	pItem = Settings.GetSetting( _T("Play.OnlyUserId") );
	CString strValue = *pItem->m_pString;
	
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		if ( ! _tcsncmp( str, xRemoteId, str.GetLength() ) ) return TRUE;
	}
	
	return FALSE;
}

BOOL CInfoChannel::IsUnicomUser() const
{
	if (	_tcsncmp( RemoteId, "130", 3 ) >= 0
		&&	_tcsncmp( RemoteId, "133", 3 ) <= 0 ) return TRUE;
	
	return FALSE;
}

LPCTSTR CInfoChannel::GetFileWelcome() const
{
	if ( IsUnicomUser() ) return FILE_INFO_WELCOME_LT;
	
	return FILE_INFO_WELCOME;
}

int CInfoChannel::ShExitCode_init(EVTBLK *evtblk, int nExitCode)
{
	fcnp = &ExitPlay; ExitCode = nExitCode;
	Reserved &= 0; return (this->*fcnp->initiate)(evtblk);
}

//////////////////////////////////////////////////////////////////////
// CInfoChannel operations

int CInfoChannel::ShPlyNavFile(int nChan, LPCTSTR xNavFile)
{
	CSettings::Item* pItem = Settings.GetSetting( _T("Pager.NavPlay") );
	if ( ! pItem || ! pItem->m_pDword )	return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	pItem = Settings.GetSetting( _T("Pager.NavOffset") );
	if ( ! pItem || ! pItem->m_pString ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	SsmSetDtmfStopPlay( nChan, FALSE );
	SsmClearRxDtmfBuf( nChan );
	SsmClearFileList( nChan );
	
	DWORD nOffset1 = 0, nOffset2 = 0;
	_stscanf( *pItem->m_pString, "%i-%i", &nOffset1, &nOffset2 );
	
	SsmAddToFileList( nChan, (LPTSTR)xNavFile, 6, 0, nOffset1 );
	
	CString strValue;
	strValue.Format( "%i", GetUsrLvCount() );
	CompDigital( (LPCTSTR)strValue, TRUE );
	
	SsmAddToFileList( nChan, (LPTSTR)xNavFile, 6, nOffset2, 0xFFFFFFFF );
	
	if ( SsmPlayFileList( nChan ) ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	return 0;
}

CString CInfoChannel::MakeUsrLvFile(LPTSTR sExtTitle)
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

BOOL CInfoChannel::PutUsrRecord(LPCTSTR xUsrFile)
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
		
		CString	strQuery;
		
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

BOOL CInfoChannel::TryPagerToGroup(int xGroupId, LPCTSTR xUsrFile)
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
		
		strQuery.Format( "insert into xuservoice"
						" (xPCMName,xPhoneNum,xRecDate,xCaller)"
						" values('%s','%s',getdate(),'%s')",
						xUsrFile, xPhone, RemoteId );
		TRACE( strQuery + "\n" );
		xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
	}
	
	return TRUE;
}

BOOL CInfoChannel::OpenUsrRecord(LPCTSTR xPhone)
{
	CString strQuery;
	
	strQuery.Format( "select * from xuservoice"
					" where xPhoneNum like '%%%s'"
					" order by xTheIndex asc", xPhone );
	TRACE( strQuery + "\n" );
	
	xOpenRecordset(m_pRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
	if ( m_pRecordset->adoEOF ) return FALSE;
	
	return TRUE;
}

int CInfoChannel::GetUsrLvCount()
{
	return m_pRecordset->RecordCount;
}

//////////////////////////////////////////////////////////////////////
// CInfoChannel TCP Transfer func

BOOL CInfoChannel::OnBindPacket(CBindPacket* pPacket)
{
	if ( FeeLogin.initiate == (INITIATE)FeeLogin_init ) return TRUE;
	return CChannel::OnBindPacket( pPacket );
}

//////////////////////////////////////////////////////////////////////
// CInfoChannel OVERLOAD function

int CInfoChannel::bkIdle_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	CChannel* pChannel = new CChannel( nChan );
	pChannel->Clone( this ); Network.SetChannel( pChannel );

	return (pChannel->*(pChannel->fcnp)->initiate)(evtblk);
}

int CInfoChannel::FeeLogin_init(EVTBLK *evtblk)
{
	return 0;
}

int CInfoChannel::FeeRebind_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	CChannel::OnConnected();
	SetTimer( 5* 1000 );
	
	return 0;
}

struct state* CInfoChannel::FeeLogin_cmplt(EVTBLK *evtblk)
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

int CInfoChannel::rcvMFsig_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( ! IsOnlyUserId(RemoteId) ) return ShExitCode_init(evtblk, EXIT_NOTUSER);
	
	if ( SsmGetChState(nChan) == S_CALL_TALKING ||	// 其他业务转入
		_tcsncmp(CallType, CA_DEBG, 4) == 0 )		// For Debug
	{
		return SsmPutUserEvent( E_CHG_ChState, nChan, S_CALL_TALKING );
	}
	
	return CChannel::rcvMFsig_init(evtblk);
}

int CInfoChannel::RcvTalk_init(EVTBLK *evtblk)
{
	CChannel::RcvTalk_init(evtblk);
	
	fcnp = &FwdPlay; Reserved = 0;
	return (this->*fcnp->initiate)(evtblk);
}

/////////////////////////////////////////////////////////////////////////////
// CInfoChannel Play function

int CInfoChannel::FwdPlay_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	if ( PlayFile( GetFileWelcome(), FT_PROMPT, TRUE ) )
		return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	return 0;
}

struct state* CInfoChannel::FwdPlay_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved = 0; return &RcvFnOper;
	}
	
	return ChkDTSignal( evtblk, &FwdPlay );
}

int CInfoChannel::RcvFnOper_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	if ( (Reserved & 0x03) < 3 )
	{
		SsmClearRxDtmfBuf(nChan);
		Reserved++; Reserved |= 0x08;
		return PlayFile( FILE_INFO_RCVOPER, FT_PROMPT, TRUE );
	}
	
	ExitCode = EXIT_GOODBYE; fcnp = &ExitPlay;
	return (this->*fcnp->initiate)(evtblk);
}

struct state* CInfoChannel::RcvFnOper_cmplt(EVTBLK *evtblk)
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
			Reserved &= 0x00; return &RcvLvTele;
			
		case	'2':		// 听语音
			Reserved &= 0x00; return &PlyLvFile;
			
		case	'3':		// 娱乐新天地
			Reserved &= 0x00; return FwdNotify( "959513" );
			
		case	'0':		// 转人工
			Reserved &= 0x00; return FwdNotify( "95950" );
			
		default:
			Reserved &= 0x00; return FwdXPlay( nDtmfKey );
		}
	}
	
	return ChkDTSignal( evtblk, &RcvFnOper );
}

/////////////////////////////////////////////////////////////////////////////
// CInfoChannel send voice info

// 短信号码
int CInfoChannel::RcvLvTele_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;
	SsmClearRxDtmfBuf( nChan );
	PageNumber = new CHAR [MAX_BUFFER + 1];
	ZeroMemory( PageNumber, MAX_BUFFER + 1 );
	
	return PlayFile( FILE_PAGE_WTLVTELE, FT_PROMPT, TRUE );
}

struct state* CInfoChannel::RcvLvTele_cmplt(EVTBLK *evtblk)
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
int CInfoChannel::RcvLvWord_init(EVTBLK *evtblk)
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
	return PlayFile( FILE_PAGE_BJMUSIC, FT_PROMPT, FALSE );
}

struct state* CInfoChannel::RcvLvWord_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;

	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		if ( (Reserved &= 0x07) == 0x01 )	// 提示语播放完毕
		{
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
	
	//if ( (Reserved & 0x04) == 0x04 )	// 短信录制完毕
	if ( (Reserved & 0x04) == 0x04 && ! BjPlaying )	// 短信录制完毕
	{
		PutUsrRecord( UsrLvFile ); SetEvent( Traffic.m_pWakeup );
		Reserved &= 0x00; return &RcvFnOper;
	}
	
	if ( (Reserved & 0x07) == 0x02 && evtblk->wEventCode == E_CHG_ChState &&
		((evtblk->dwParam & 0xFFFF) == S_CALL_STANDBY || (evtblk->dwParam & 0xFFFF) == S_CALL_PENDING) )
	{
		PutUsrRecord( UsrLvFile ); SetEvent( Traffic.m_pWakeup );
	}
	
	return ChkDTSignal( evtblk, &RcvLvWord );
}

/////////////////////////////////////////////////////////////////////////////
// CInfoChannel read voice info

// 导播提示
int CInfoChannel::PlyLvFile_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	if ( ! OpenUsrRecord( RemoteId ) )
		return PlayFile( FILE_PROMPT_NORECD, FT_PROMPT, FALSE );
	
	CString strFile;
	strFile.Format( _T("%s\\%s\\%s"), Settings.General.Path, FT_PROMPT, FILE_PAGE_PLYLVWRD );
	
	return ShPlyNavFile( nChan, (LPCTSTR)strFile );
}

struct state* CInfoChannel::PlyLvFile_cmplt(EVTBLK *evtblk)
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
int CInfoChannel::PlyLvWord_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
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

struct state* CInfoChannel::PlyLvWord_cmplt(EVTBLK *evtblk)
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
// CInfoChannel play xfile function

int CInfoChannel::PlayXFile_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	SsmClearRxDtmfBuf(nChan);
	
	if ( m_pRecordset->adoEOF ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 1 );
	
	_variant_t TheValue = m_pRecordset->GetCollect( "xvoicepath" );
	CString xUsrFile = (LPCTSTR)(_bstr_t)TheValue;
	xUsrFile.TrimLeft(); xUsrFile.TrimRight();
	
	Reserved |= 0x08;	// 置状态“正在”
	if ( PlayFile( (LPCTSTR)xUsrFile, NULL, TRUE ) )
	{
		CHAR sError[1024];
		SsmGetLastErrMsg( sError );
		theApp.Message( MSG_ERROR, sError );
		
		return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	}
	
	return 0;
}

struct state* CInfoChannel::PlayXFile_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		CHAR nDtmfKey = '\0';
		Reserved = 0; KillTimer();
		
		SsmGet1stDtmf( nChan, &nDtmfKey );
		if ( nDtmfKey == '*' ) return &RcvFnOper;
		
		m_pRecordset->MoveNext(); if ( m_pRecordset->adoEOF )
		return &RcvFnOper; else return &PlayXFile;
	}

	return ChkDTSignal( evtblk, &PlayXFile );
}
