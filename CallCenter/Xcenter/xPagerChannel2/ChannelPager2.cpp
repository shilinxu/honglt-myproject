//
// ChannelPager1.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Network.h"
#include "Settings.h"
#include "ChannelPager2.h"

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
// CPagerChannel2 construction

CPagerChannel2::CPagerChannel2(int nChan, LPCTSTR xFeeCode) : CChannel( nChan, xFeeCode )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	FeeLogin.initiate		= (INITIATE)FeeLogin_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	RcvTalk.initiate		= (INITIATE)RcvTalk_init;
	
	FwdPager.initiate		= (INITIATE)FwdPager_init;
	FwdPager.process		= (PROCESS)FwdPager_cmplt;
	PlyLvWord.initiate		= (INITIATE)PlyLvWord_init;
	PlyLvWord.process		= (PROCESS)PlyLvWord_cmplt;
	
	CPagerChannel2::Setup();
}

CPagerChannel2::~CPagerChannel2()
{
}

CChannel* CPagerChannel2::Create(int nChan, LPCTSTR xFeeCode)
{
	return new CPagerChannel2( nChan, xFeeCode );
}

//////////////////////////////////////////////////////////////////////
// CPagerChannel2 attributes

void CPagerChannel2::Setup()
{
	CChannel::Setup();
	fcnp = &FeeLogin;
}

int CPagerChannel2::PlayNavFile(int nChan, LPCTSTR xNavFile, BOOL bTermDtmf)
{
	CString strQuery;
	
	strQuery.Format( "select * from xuservoice"
					" where xPhoneNum = '%s'"
					// add 5/24, for local tele or remote mobile
					" or '0574'+xPhoneNum = '%s' or xPhoneNum = '0%s'"
					" order by xTheIndex asc", RemoteId, RemoteId, RemoteId );
	TRACE( strQuery + "\n" );
	
	xOpenRecordset(m_pRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
	if ( m_pRecordset->adoEOF ) return PlayFile( FILE_PROMPT_NORECD, FT_PROMPT, FALSE );
	
	CSettings::Item* pItem = Settings.GetSetting( _T("Pager.NavPlay") );
	if ( ! pItem || ! pItem->m_pDword )	return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	pItem = Settings.GetSetting( _T("Pager.NavOffset") );
	if ( ! pItem || ! pItem->m_pString ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	SsmSetDtmfStopPlay( nChan, bTermDtmf );
	SsmClearRxDtmfBuf( nChan );
	SsmClearFileList( nChan );
	
	DWORD nOffset1 = 0, nOffset2 = 0;
	_stscanf( *pItem->m_pString, "%i-%i", &nOffset1, &nOffset2 );
	
	CString strFile;
	strFile.Format( _T("%s\\%s\\%s"), Settings.General.Path, FT_PROMPT, xNavFile );
	
	strQuery.Format( "%i", m_pRecordset->RecordCount );
	SsmAddToFileList( nChan, strFile.GetBuffer(0), 6, 0, nOffset1 );
	CompDigital( (LPCTSTR)strQuery, TRUE );
	SsmAddToFileList( nChan, strFile.GetBuffer(0), 6, nOffset2, 0xFFFFFFFF );
	
	return SsmPlayFileList( nChan );
}

//////////////////////////////////////////////////////////////////////
// CPagerChannel2 OVERLOAD function

int CPagerChannel2::bkIdle_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	CChannel* pChannel = new CChannel( nChan );
	pChannel->Clone( this ); Network.SetChannel( pChannel );
	
	return (pChannel->*(pChannel->fcnp)->initiate)(evtblk);
}

int CPagerChannel2::FeeLogin_init(EVTBLK *evtblk)
{
	return 0;
}

struct state* CPagerChannel2::FeeLogin_cmplt(EVTBLK *evtblk)
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

int CPagerChannel2::RcvTalk_init(EVTBLK *evtblk)
{
	CChannel::RcvTalk_init(evtblk);
	
	fcnp = &FwdPager; Reserved = 0;
	return (this->*fcnp->initiate)(evtblk);
}

/////////////////////////////////////////////////////////////////////////////
// CPagerChannel2 Pager function

int CPagerChannel2::FwdPager_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	if ( PlayFile( FILE_PROMPT_QUERY, FT_PROMPT, FALSE ) )
		return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	return 0;
}

struct state* CPagerChannel2::FwdPager_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		if ( PlayNavFile(nChan, FILE_PAGE_PLYLVWRD, FALSE) ) Reserved = 0;
		
		return &PlyLvWord;
	}
	
	return ChkDTSignal( evtblk, &FwdPager );
}

int CPagerChannel2::PlyLvWord_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	_variant_t TheValue = m_pRecordset->GetCollect( "xPCMName" );
	CString xUsrFile = (LPCTSTR)(_bstr_t)TheValue;
#ifdef _DEBUG
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

struct state* CPagerChannel2::PlyLvWord_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved = 0;
		if ( ! m_pRecordset->adoEOF ) m_pRecordset->MoveNext();
		
		if ( m_pRecordset->adoEOF )
		{
			ExitCode = EXIT_GOODBYE;
			Reserved = 0; return &ExitPlay;
		}
	}
	
	return ChkDTSignal( evtblk, &PlyLvWord );
}
