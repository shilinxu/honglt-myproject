//
// ChannelPager1.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Network.h"
#include "Traffic.h"
#include "Settings.h"
#include "ChannelPager1.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BOOL APIENTRY DllMain (HANDLE, DWORD, LPVOID)					 
{
    return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CPagerChannel1 construction

CPagerChannel1::CPagerChannel1(int nChan, LPCTSTR xFeeCode) : CChannel( nChan, xFeeCode )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	FeeLogin.initiate		= (INITIATE)FeeLogin_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	RcvTalk.initiate		= (INITIATE)RcvTalk_init;
	
	FwdPager.initiate		= (INITIATE)FwdPager_init;
	FwdPager.process		= (PROCESS)FwdPager_cmplt;
	RcvLvTele.initiate		= (INITIATE)RcvLvTele_init;
	RcvLvTele.process		= (PROCESS)RcvLvTele_cmplt;
	RcvLvWord.initiate		= (INITIATE)RcvLvWord_init;
	RcvLvWord.process		= (PROCESS)RcvLvWord_cmplt;
	
	PageNumber	= new CHAR [MAX_BUFFER + 1];
	UsrLvFile	= _T("");
	CPagerChannel1::Setup();
}

CPagerChannel1::~CPagerChannel1()
{
	delete [] PageNumber;
}

CChannel* CPagerChannel1::Create(int nChan, LPCTSTR xFeeCode)
{
	return new CPagerChannel1( nChan, xFeeCode );
}

//////////////////////////////////////////////////////////////////////
// CPagerChannel1 attributes

void CPagerChannel1::Setup()
{
	CChannel::Setup();
	fcnp = &FeeLogin;
	
	ZeroMemory( PageNumber, MAX_BUFFER + 1 );
	SsmSetHangupStopRecFlag( m_nChan, TRUE );
}

CString CPagerChannel1::MakeLvWordFile(LPCTSTR sExtTitle)
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

BOOL CPagerChannel1::PutLvRecord(LPCTSTR xUsrFile)
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

BOOL CPagerChannel1::TryPagerToGroup(int xGroupId, LPCTSTR xUsrFile)
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

void CPagerChannel1::Clone(CChannel* pParent)
{
	CChannel::Clone( pParent );
	if ( ! pParent->m_xFeeCode || _tcscmp(pParent->m_xFeeCode, "1021") ) return;

	CPagerChannel1* pChannel = (CPagerChannel1*)pParent;
	
	CopyMemory( PageNumber, pChannel->PageNumber, MAX_BUFFER );
	UsrLvFile	= pChannel->UsrLvFile;
	BjMusic		= pChannel->BjMusic;
}

//////////////////////////////////////////////////////////////////////
// CPagerChannel1 operations

CPagerChannel1* CPagerChannel1::ConvertFrom(CChannel* pParent, LPCTSTR xString)
{
	CString xString2 = xString;
	CPagerChannel1* pChannel = new CPagerChannel1( pParent->m_nChan );
	pChannel->CChannel::Clone( pParent ); Network.SetChannel( pChannel );

	pChannel->FormatObject( xString2 );
	
	return pChannel;
}

BOOL CPagerChannel1::FormatObject(LPCTSTR xString)
{
	if ( ! xString ) return TRUE;

	for ( int i = 0; xString[ i ] != '\0'; i ++ )
	{
		if ( xString[i] == '#' ) break;
		if ( i >= MAX_BUFFER ) break;

		PageNumber[ i ] = xString[ i ];
	}

	if ( xString[i] == '#' )
	{
		BjMusic.Empty(); i += 1;
		for ( ; xString[ i ] != '\0'; i ++ )
		{
			if ( xString[i] == '#' ) break;
			BjMusic += xString[ i ];
		}
	}

	if ( xString[i] == '#' )
	{
		UsrLvFile.Empty(); i += 1;
		for ( ; xString[ i ] != '\0'; i ++ )
		{
			UsrLvFile += xString[ i ];
		}
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CPagerChannel1 OVERLOAD function

int CPagerChannel1::bkIdle_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	CChannel* pChannel = new CChannel( nChan );
	pChannel->Clone( this ); Network.SetChannel( pChannel );
	
	return (pChannel->*(pChannel->fcnp)->initiate)(evtblk);
}

int CPagerChannel1::FeeLogin_init(EVTBLK *evtblk)
{
	return 0;
}

struct state* CPagerChannel1::FeeLogin_cmplt(EVTBLK *evtblk)
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
	}
	
	return CChannel::FeeLogin_cmplt(evtblk);
}

int CPagerChannel1::RcvTalk_init(EVTBLK *evtblk)
{
	CChannel::RcvTalk_init(evtblk);
	
	fcnp = &FwdPager; Reserved = 0;
	return (this->*fcnp->initiate)(evtblk);
}

/////////////////////////////////////////////////////////////////////////////
// CPagerChannel1 Pager function

int CPagerChannel1::FwdPager_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	if ( PlayFile( FILE_PAGE_WELCOME, FT_PROMPT, TRUE ) )
		return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	return 0;
}

struct state* CPagerChannel1::FwdPager_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved = 0; return &RcvLvTele;
	}
	
	return ChkDTSignal( evtblk, &FwdPager );
}

int CPagerChannel1::RcvLvTele_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	SsmClearRxDtmfBuf( nChan );
	
	Reserved |= 0x08;	// 置状态“正在”
	return PlayFile( FILE_PAGE_WTLVTEL, FT_PROMPT, TRUE );
}

struct state* CPagerChannel1::RcvLvTele_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x04;	// 记录播放完毕标志
#ifdef NPICKUP
theApp.Message( MSG_TEMP, "debug[%i]:: PageNumber:=13867877813", nChan );
if (_tcslen(PageNumber)==0)
CopyMemory(PageNumber,"13867877813",11);
Reserved |= 0x01;
#endif
	}
	
	if ( evtblk->wEventCode == E_CHG_RcvDTMF )
	{
		int nDtmfLen = _tcslen(PageNumber);
		CHAR cDtmfKey= (CHAR)(evtblk->dwParam & 0xFFFF);
		if ( '#' != cDtmfKey ) *(PageNumber+nDtmfLen++) = cDtmfKey;
		
		if ( '#' == cDtmfKey || nDtmfLen >= MAX_BUFFER )
		{
			Reserved |= 0x01;	// 标记DTMF#接收成功
			if ( ! nDtmfLen ) CopyMemory( PageNumber, RemoteId, MAX_PHONE );
		}
	}
	
	if ( (Reserved & 0x04) && (Reserved & 0x01) )
	{
		Reserved &= 0x00; return &RcvLvWord;
	}
	
	return ChkDTSignal( evtblk, &RcvLvTele );
}

int CPagerChannel1::RcvLvWord_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	if ( Reserved++ == 0 )
	{
		Reserved |= 0x08;	// 置状态“正在”
		return PlayFile( FILE_PAGE_RCVLVWRD, FT_PROMPT, FALSE );
	}
	
	Reserved |= 0x08;	// 置状态“正在”
	if ( BjMusic.IsEmpty() )
	return PlayFile( FILE_PAGE_BJMUSIC, FT_PROMPT, FALSE );
	else
	return PlayFile( (LPCTSTR)BjMusic, NULL, FALSE );
}

struct state* CPagerChannel1::RcvLvWord_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;

	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		if ( (Reserved &= 0x07) == 0x01 )	// 提示语播放完毕
		{
			UsrLvFile = MakeLvWordFile( _T("pcm") );
			RecordFile( UsrLvFile, NULL, "#" );
		}
		else
		if ( (Reserved & 0x04) == 0x04 )	// 短信录制完毕
		{
			Reserved &= 0x00; SsmStopPlayFile( nChan );
			ExitCode = EXIT_GOODBYE; return &ExitPlay;
		}
		
		Reserved = 0x01; // 循环播放背景音乐
	}
	if ( evtblk->wEventCode == E_PROC_RecordEnd )
	{
		Reserved |= 0x04;	// 记录播放完毕标志
		SsmStopPlayFile( nChan );
		PutLvRecord( UsrLvFile ); SetEvent( Traffic.m_pWakeup );
	}
	
	if ( ! UsrLvFile.IsEmpty() && evtblk->wEventCode == E_CHG_ChState &&
		((evtblk->dwParam & 0xFFFF) == S_CALL_STANDBY || (evtblk->dwParam & 0xFFFF) == S_CALL_PENDING) )
	{
		PutLvRecord( UsrLvFile ); SetEvent( Traffic.m_pWakeup );
	}
	
	return ChkDTSignal( evtblk, &RcvLvWord );
}
