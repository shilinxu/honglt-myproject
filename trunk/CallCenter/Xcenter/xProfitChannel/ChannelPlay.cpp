//
// ChannelPlay.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Network.h"
#include "Settings.h"
#include "ChannelPlay.h"

#include "Registry.h"
#include "Traffic.h"
#include "XML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CPlayChannel construction

CPlayChannel::CPlayChannel(int nChan, LPCTSTR xFeeCode) : CChannel( nChan, xFeeCode )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
//	FeeLogin.initiate		= (INITIATE)FeeLogin_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	rcvMFsig.initiate		= (INITIATE)rcvMFsig_init;
	RcvTalk.initiate		= (INITIATE)RcvTalk_init;
	RcvTalk.process			= (PROCESS)RcvUlTalk_cmplt;
	
	FwdPlay.initiate		= (INITIATE)FwdPlay_init;
	FwdPlay.process			= (PROCESS)FwdPlay_cmplt;
	RcvFnOper.initiate		= (INITIATE)RcvFnOper_init;
	RcvFnOper.process		= (PROCESS)RcvFnOper_cmplt;
	
	PlyUlWord.initiate		= (INITIATE)PlyUlWord_init;
	PlyUlWord.process		= (PROCESS)PlyUlWord_cmplt;
	OkAnswer.initiate		= (INITIATE)OkAnswer_init;
	OkAnswer.process		= (PROCESS)OkAnswer_cmplt;
	
	RcvLvTel.initiate		= (INITIATE)RcvLvTel_init;
	RcvLvTel.process		= (PROCESS)RcvLvTel_cmplt;
	
	ExitPlay.initiate		= (INITIATE)ExitPlay_init;
	BufferPtr = NULL; BufferSize = 0;
	
	CPlayChannel::Setup();
}

CPlayChannel::~CPlayChannel()
{
	FreeBuffer();
}

//////////////////////////////////////////////////////////////////////
// CPlayChannel attributes

void CPlayChannel::Setup()
{
	CChannel::Setup();
	fcnp = &FeeLogin;
/*	
ReadProfit();
	{
		FieldPtr xPlyBlob = m_pRecordset->GetFields()->GetItem("xvoice");
	int aa= PlayBlob( 0, xPlyBlob, FALSE );
	}
	FreeBuffer();
	{
		FieldPtr xPlyBlob = m_pRecordset->GetFields()->GetItem("xvoice");
	int aa= PlayBlob( 0, xPlyBlob, FALSE );
	}
*/	
}

void CPlayChannel::KillLinker()
{
	if ( m_nTaskId != -1 )
	SsmPutUserEvent( E_CHG_ChState, m_nTaskId,
		MAKEVENT(m_nChan & 0xFFFF, S_CALL_PENDING) );
	
	CChannel::KillLinker();
}

LPVOID CPlayChannel::NewBuffer(DWORD nBufSize)
{
	FreeBuffer();
	
	BufferPtr  = new BYTE[nBufSize];
	BufferSize = nBufSize;
	
	return BufferPtr;
}

void CPlayChannel::SetBuffer(LPVOID pBuf, DWORD nBufSize)
{
	NewBuffer( nBufSize );
	
	CopyMemory( BufferPtr, pBuf, nBufSize );
}

void CPlayChannel::FreeBuffer()
{
	if ( BufferPtr ) delete [] BufferPtr;
	BufferPtr = NULL; BufferSize = 0;
}

BOOL CPlayChannel::ReadProfit()
{
	if ( CheckBlacklist( RemoteId ) ) return FALSE;
	if ( ! Traffic.CheckOutRule( CHN_ISUP ) ) return FALSE;
	
	CString strQuery;
	
	strQuery.Format( "select * from xadver_marry"
					" where getdate() between xdate and xdateend"
					" order by newid()" );
	TRACE( strQuery + "\n" );
	
//	xOpenRecordset(m_pRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
	m_pRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	if ( m_pRecordset->adoEOF ) return FALSE;
	
	for ( ; ! m_pRecordset->adoEOF; )
	{
		_variant_t TheValue = m_pRecordset->GetCollect( "xrule" );
		if ( TheValue.vt == VT_NULL ) continue;
		
		BYTE nBit = 0xFF;
		CString strValue = CString("A") + (LPCTSTR)(_bstr_t)TheValue;
		for ( strValue += '|' ; strValue.GetLength() ; )
		{
			CString str	= strValue.SpanExcluding( _T("|;,") );
			strValue	= strValue.Mid( str.GetLength() + 1 );
			
			if ( str.IsEmpty() ) continue;
			
			if ( str.GetAt(0) == 'W' || str.GetAt(0) == 'w' )
			{
				if ( (nBit & 0xF0) == 0xF0 ) nBit &= 0x0F;
				nBit |= InWeekProfit( str.Mid(1) ) << 4;
			}
			else 
			if ( str.GetAt(0) >= '0' && str.GetAt(0) <= '9' )
			{
				if ( (nBit & 0x0F) == 0x0F ) nBit &= 0xF0;
				nBit |= InTimeProfit( str );
			}
		}
		
		if ( (nBit & 0x11) == 0x11 ) return TRUE;
		m_pRecordset->MoveNext();
	}
	
	return FALSE;
}

BOOL CPlayChannel::InTimeProfit(LPCTSTR lpszHour) const
{
	int nHour1 = -1, nHour2 = -1;
	COleDateTime tNow = COleDateTime::GetCurrentTime();
	
	if ( _stscanf( lpszHour, "%i-%i", &nHour1, &nHour2 ) == 2 )
	{
		if ( tNow.GetHour() >= nHour1 && tNow.GetHour() <= nHour2 ) return TRUE;
	}
	else
	{
		if ( tNow.GetHour() == nHour1 ) return TRUE;
	}
	
	return FALSE;
}

BOOL CPlayChannel::InWeekProfit(LPCTSTR lpszWeek) const
{
	int nWeek1 = -1, nWeek2 = -1;
	COleDateTime tNow = COleDateTime::GetCurrentTime();
	
	int nWeek = tNow.GetDayOfWeek() - 1;
	if ( ! nWeek ) nWeek = 7;
	
	if ( _stscanf( lpszWeek, "%i-%i", &nWeek1, &nWeek2 ) == 2 )
	{
		if ( nWeek >= nWeek1 && nWeek <= nWeek2 ) return TRUE;
	}
	else
	{
		if ( nWeek == nWeek1 ) return TRUE;
	}
	
	return FALSE;
}

int CPlayChannel::PlayBlob(int nChan, FieldPtr xPlyBlob, BOOL bTermDtmf)
{
	SsmSetDtmfStopPlay( nChan, bTermDtmf );
	SsmClearRxDtmfBuf( nChan );
	
	LONG lDataSize = xPlyBlob->ActualSize;
	if ( lDataSize <= 0 ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
//	_variant_t TheValue = xPlyBlob->GetChunk(lDataSize);
	_variant_t TheValue = xPlyBlob->GetValue();
	LPTSTR pBuf = NULL;
	SafeArrayAccessData(TheValue.parray, (void **)&pBuf);
	this->SetBuffer( pBuf, lDataSize );
	SafeArrayUnaccessData( TheValue.parray );
	
	if ( SsmPlayMem( nChan, 6, (LPBYTE)BufferPtr, BufferSize, 0, BufferSize - 1 ) )
	return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	return 0;
}

struct state* CPlayChannel::ShExitCode_cmplt(EVTBLK *evtblk, int nExitCode)
{
	ExitCode = nExitCode;
	Reserved = 0; return &ExitPlay;
}

//////////////////////////////////////////////////////////////////////
// CPlayChannel OVERLOAD function

int CPlayChannel::bkIdle_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	SsmSetPlayDest( nChan, 0 );	// 取消上总线
#if 1
	if ( ExitCode == EXIT_CALLBACK )
	{
//		Traffic.AddUsrBack( LocalId, "13867877813" );
		Traffic.AddUsrBack( LocalId, RemoteId );
		ExitCode = EXIT_NORM;
	}
#endif
	CChannel* pChannel = new CChannel( nChan );
	pChannel->Clone( this ); Network.SetChannel( pChannel );
	
	return (pChannel->*(pChannel->fcnp)->initiate)(evtblk);
}

int CPlayChannel::FeeLogin_init(EVTBLK *evtblk)
{
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
#ifdef _DEBUG
		if ( ReadProfit() == TRUE )
		{
		}
#else
		if ( ReadProfit() == TRUE )
		{
			if ( SsmGetChState(nChan) == S_CALL_PICKUPED )	// For CallOut
			{
				KillTimer(); Reserved &= 0; return &WaitDial;
			}
			
			evtblk->dwParam = MAKEVENT(ECore_CallBack & 0xFFFF, S_FEE_Disconnected);
			return CChannel::FeeLogin_cmplt(evtblk);
		}
#endif
		else
		{
			ExitCode = EXIT_BUSY; FwdPlay = ExitPlay;
		}
		
		if ( SsmGetChState(nChan) == S_CALL_RINGING ||	// 确保收到RINGING消息
			_tcsncmp(CallType, CA_DEBG, 4) == 0 )		// For Debug
		{
			evtblk->wEventCode = E_CHG_ChState;
			evtblk->dwParam = S_CALL_RINGING;
			return CChannel::FeeLogin_cmplt(evtblk);
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
	int nChan = evtblk->nReference;
	CChannel::RcvTalk_init(evtblk);
	
	if ( SsmGetChState(nChan) != S_CALL_TALKING )
		SsmSetPlayDest( nChan, 1 );	// 上总线，用于内线测试
	
	fcnp = &FwdPlay; Reserved = 0;
	return (this->*fcnp->initiate)(evtblk);
}

int CPlayChannel::ExitPlay_init(EVTBLK *evtblk)
{
/*	if ( ! Reserved && ExitCode == EXIT_NORECD )	// 不在服务时段
	{
		Reserved |= 0x08;	// 置状态“正在”
		return PlayFile( FILE_PROT_NORECD, FT_PROMPT, FALSE );
	}
	*/
	if ( ! Reserved && ExitCode == EXIT_NOTUSER )	// 回答错误
	{
		Reserved |= 0x08;	// 置状态“正在”
		return PlayFile( FILE_PROT_NOTUSER, FT_PROMPT, FALSE );
	}
	if ( ! Reserved && ExitCode == EXIT_CALLBACK )	// 回拨提醒
	{
#if 1
		if ( _tcsncmp( RemoteId, "1", 1 ) )
		{
			SsmPickup( m_nChan );
			CChannel::RcvTalk_init( NULL );
		}
#endif
		Reserved |= 0x08;	// 置状态“正在”
		return PlayFile( FILE_PROT_CALLBACK, FT_PROMPT, FALSE );
	}
	if ( ! Reserved && ExitCode == EXIT_NOLONG )	// 呼叫限时
	{
		Reserved |= 0x08;	// 置状态“正在”
		return PlayFile( FILE_PROT_NOLONG, FT_PROMPT, FALSE );
	}
	
	return CChannel::ExitPlay_init(evtblk);	
}
/*
struct state* CPlayChannel::ExitPlay_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved &= 0; return &bkOffHk;
	}
	
	return ChkDTSignal( evtblk, &ExitPlay );
}
*/
/////////////////////////////////////////////////////////////////////////////
// CPlayChannel Play function

// 语音接入
int CPlayChannel::FwdPlay_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	if ( PlayFile( FILE_PROT_WELCOME, FT_PROMPT, TRUE ) )
		return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	return 0;
}

struct state* CPlayChannel::FwdPlay_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved = 0; return &PlyUlWord;
	}
	
	return ChkDTSignal( evtblk, &FwdPlay );
}

// 播放节目
int CPlayChannel::PlyUlWord_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	FieldPtr xPlyBlob = m_pRecordset->GetFields()->GetItem("xvoice");
	return PlayBlob( nChan, xPlyBlob, FALSE );
}

struct state* CPlayChannel::PlyUlWord_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved = 0; return &RcvFnOper;
	}
	
	return ChkDTSignal( evtblk, &PlyUlWord );
}

// 选择操作
int CPlayChannel::RcvFnOper_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	SsmClearRxDtmfBuf(nChan);
	
	Reserved |= 0x08;	// 置状态“正在”
	FieldPtr xPlyBlob = m_pRecordset->GetFields()->GetItem("xanswer");
	
	if ( xPlyBlob->ActualSize <= 0 )
	{
		fcnp = &RcvLvTel; Reserved = 0;
		return (this->*fcnp->initiate)(evtblk);
	}
	
	return PlayBlob( nChan, xPlyBlob, TRUE );
}

struct state* CPlayChannel::RcvFnOper_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x04;		// 记录播放完毕标志
		SetTimer( 5000 );
	}
	
	if ( evtblk->wEventCode == E_SYS_TIMEOUT )
	{
		Reserved &= 0x03;	// 清除播放完毕标志
		return ShExitCode_cmplt( evtblk, EXIT_NOTUSER );
	}
	
	if ( (Reserved & 0x04) && SsmGetRxDtmfLen( nChan ) )
	{
		CHAR nDtmfKey = '\0';
		KillTimer(); Reserved &= 0x00;
		
		SsmGet1stDtmf( nChan, &nDtmfKey );
		
		_variant_t TheValue = m_pRecordset->GetCollect( "xkey" );
		if ( TheValue.vt == VT_NULL ) return ShExitCode_cmplt( evtblk, EXIT_NOTUSER );
		
		if ( nDtmfKey == *(LPCTSTR)(_bstr_t)TheValue ) return &OkAnswer;
		return ShExitCode_cmplt( evtblk, EXIT_NOTUSER );
	}
	
	return ChkDTSignal( evtblk, &RcvFnOper );
}

// 回答正确
int CPlayChannel::OkAnswer_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	if ( PlayFile( FILE_PROT_OKANSWER, FT_PROMPT, FALSE ) )
		return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	return 0;
}

struct state* CPlayChannel::OkAnswer_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved = 0; return &RcvLvTel;
	}
	
	return ChkDTSignal( evtblk, &OkAnswer );
}

// 输入被叫
int CPlayChannel::RcvLvTel_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;

	Reserved |= 0x08;
	SsmClearRxDtmfBuf( nChan );
	
	NewBuffer( MAX_PHONE + 1 );
	ZeroMemory( BufferPtr, MAX_PHONE + 1 );
	
	return PlayFile( FILE_TALK_RCVTELE, FT_PROMPT, TRUE );
}

struct state* CPlayChannel::RcvLvTel_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x04;	// 记录播放完毕标志
		SetTimer( 5000 );
	}
	
	if ( evtblk->wEventCode == E_SYS_TIMEOUT )
	{
		SsmPutUserEvent( E_CHG_RcvDTMF, nChan, '#' );
	}
	
	if ( evtblk->wEventCode == E_CHG_RcvDTMF )
	{
		SetTimer( 5000 );
		int nDtmfLen = SsmGetRxDtmfLen( nChan );
		
		if ( '*' == (CHAR)(evtblk->dwParam & 0xFFFF) )
		{
			Reserved = 0; return &RcvLvTel;
		}
		
		if ( '#' == (CHAR)(evtblk->dwParam & 0xFFFF) || nDtmfLen >= MAX_PHONE )
		{
			if ( ! ReadDtmf( (LPTSTR)BufferPtr, '#' ) ) CopyMemory( (LPTSTR)BufferPtr, RemoteId, MAX_PHONE );
			//if ( ! ReadDtmf( (LPTSTR)BufferPtr, '#' ) ) SsmPutUserEvent( E_CHG_RcvDTMF, nChan, '*' );
		}
	}
	
	if ( (Reserved & 0x04) && _tcslen( (LPTSTR)BufferPtr ) )
	{
		KillTimer(); Reserved &= 0x00;
		return RcvUlWord_cmplt( evtblk );
	}
	
	return ChkDTSignal( evtblk, &RcvLvTel );
}

// 愉快交流
int CPlayChannel::RcvUlWord_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	CChannel::RcvTalk_init(evtblk);
	Reserved |= 0x08; return 0;
}

struct state* CPlayChannel::RcvUlWord_cmplt(EVTBLK *evtblk)
{
	LPTSTR PageNumber = (LPTSTR)BufferPtr;
	
#if 1
	if ( _tcslen(PageNumber) < 8 || ! _tcsncmp(PageNumber, "9", 1)
		|| ! _tcsncmp(PageNumber, "0574", 4) ) return ShExitCode_cmplt( evtblk, EXIT_BUSY );
#endif
#if 0
	CSettings::Item* pItem = Settings.GetSetting( _T("Trunk.TrunkFind") );
	CChannel* pChannel = Network.LookOutPcm( *pItem->m_pString );
#else
	CChannel* pChannel = Network.LookOutPhs( PageNumber, "057496031999" );
#endif
	if ( pChannel == NULL ) return ShExitCode_cmplt( evtblk, EXIT_BUSY );
	
	CDialPacket* pPacket = new CDialPacket; pPacket->SetLocalId( "96031666" );
	pPacket->SetRemoteId( PageNumber ); pPacket->SetCallerId( RemoteId );
	
	CHAR xFeeNumber[MAX_PHONE + 1];
	ZeroMemory( xFeeNumber, MAX_PHONE + 1 );
	
	CopyMemory( xFeeNumber, LocalId, MAX_PHONE ); VerifyPhoneNum( xFeeNumber );
	pPacket->SetFeeNumber( xFeeNumber );
	
#if 0
	//	if ( PageNumber[0] !='1' && _tcsncmp(PageNumber,"013",3) && _tcsncmp(PageNumber,"015",3) )
	if ( ( _tcsncmp(PageNumber,"130",3) >= 0 && _tcsncmp(PageNumber,"133",3) <= 0 ) ||
		( _tcsncmp(PageNumber,"153",3) == 0 || _tcsncmp(PageNumber,"156",3) == 0 ) ||
		( _tcsncmp(PageNumber,"0130",4) >= 0 && _tcsncmp(PageNumber,"0133",4) <= 0 ) ||
		( _tcsncmp(PageNumber,"0153",4) == 0 || _tcsncmp(PageNumber,"0156",4) == 0 ) )
	{
	}
	else
	if ( ( _tcsncmp(PageNumber,"134",3) >= 0 && _tcsncmp(PageNumber,"139",3) <= 0 ) ||
		( _tcsncmp(PageNumber,"157",3) >= 0 && _tcsncmp(PageNumber,"159",3) <= 0 ) ||
		( _tcsncmp(PageNumber,"150",3) == 0 ) ||
		( _tcsncmp(PageNumber,"0134",4) >= 0 && _tcsncmp(PageNumber,"0139",4) <= 0 ) ||
		( _tcsncmp(PageNumber,"0157",4) >= 0 && _tcsncmp(PageNumber,"0159",4) <= 0 ) ||
		( _tcsncmp(PageNumber,"0150",4) == 0 ) )
	{
#if 0
		if ( ( _tcsncmp(UserNumber,"134",3) >= 0 && _tcsncmp(UserNumber,"139",3) <= 0 ) ||
			( _tcsncmp(UserNumber,"157",3) >= 0 && _tcsncmp(UserNumber,"159",3) <= 0 ) ||
			( _tcsncmp(UserNumber,"150",3) == 0 ) )
		{
			pPacket->SetCallerId( "057496031666" );
		}
#endif
	}
	else
#else
	if ( _tcsncmp(PageNumber,"1",1) == 0 ||
		(_tcsncmp(PageNumber,"01",2) == 0 && _tcsncmp(PageNumber,"010",3)) )
	{
		pPacket->SetCallerId( LocalId );
	}
	else
	if ( _tcsncmp(PageNumber,"5",1) == 0 || _tcsncmp(PageNumber,"05745",5) == 0 ||
		 _tcsncmp(PageNumber,"96031",5) == 0 )
	{
		pPacket->SetCallerId( LocalId );
	}
	else
#endif
	{
		pPacket->SetCallerId( LocalId );
	}
	
	pChannel->OnDialPacket( pPacket ); pPacket->Release();
	SetLinker( pChannel->m_nChan );
	
	SsmSetPlayDest( this->m_nChan, 1 );	// 上总线，允许播放广告
	SsmSetPlayDest( pChannel->m_nChan, 1 );	// 上总线，允许运行于欠费提示
	
	CRegistry pRegistry;
	DWORD nRetry = pRegistry.GetInt( "Profit", "PlayRetry", 180 );
	SetTimer( nRetry * 1000 ); // 设置n'min后播放原广告
	
	RcvTalk.initiate = (INITIATE)RcvUlWord_init;
	Reserved &= 0x00; return &RcvTalk;
}

struct state* CPlayChannel::RcvUlTalk_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_SYS_TIMEOUT )
	{
#if 0
		FieldPtr xPlyBlob = m_pRecordset->GetFields()->GetItem("xvoice");
		PlayBlob( nChan, xPlyBlob, FALSE );
		
		CRegistry pRegistry;
		DWORD nRetry = pRegistry.GetInt( "Profit", "PlayRetry", 180 );
		SetTimer( nRetry * 1000 ); // 设置n'min后播放原广告
#else
		ExitCode = EXIT_NOLONG;
		Reserved = 0; return &ExitPlay;
#endif
	}
	
	return CChannel::RcvTalk_cmplt(evtblk);
}

/////////////////////////////////////////////////////////////////////////////
// CPlayChannel Blacklist function

BOOL CPlayChannel::CheckBlacklist(LPCTSTR xRemoteId)
{
	CString strXML;
	
	strXML = Settings.General.Path;
	int nLength = strXML.GetLength();

	if ( nLength > 0 && strXML.GetAt(nLength-1) != '\\' ) strXML += '\\';
	strXML += _T("xBlacklst.xml");
	
	BOOL bResult = FALSE;
	if ( CXMLElement* pXML = CXMLElement::FromFile( strXML, TRUE ) )
	{
		if ( CXMLElement* pXMLBlk = CheckBlacklist( pXML, xRemoteId ) )
		{
			COleDateTime tDate;
			tDate.ParseDateTime( pXMLBlk->GetAttributeValue( _T("Date") ) );
			
			if ( tDate.GetStatus() == COleDateTime::valid )
			{
				COleDateTimeSpan tSpan = COleDateTime::GetCurrentTime() - tDate;
				if ( tSpan.GetTotalHours() >= 3 )
				{
					pXMLBlk->Delete();
					SaveXML( strXML, pXML );
				}
				else	
				{
					bResult = TRUE;
				}
			}
		}
		
		delete pXML;
	}
	
	return bResult;
}

CXMLElement* CPlayChannel::CheckBlacklist(CXMLElement* pXML, LPCTSTR xRemoteId)
{
	for ( POSITION pos = pXML->GetElementIterator() ; pos ; )
	{
		CXMLElement* pMap = pXML->GetNextElement( pos );
		if ( ! pMap->IsNamed( _T("Map") ) ) continue;
		
		CString strValue = pMap->GetAttributeValue( _T("UserId") );
		if ( ! _tcsncmp(strValue, xRemoteId, strValue.GetLength()) ) return pMap;
	}
	
	return NULL;
}

BOOL CPlayChannel::SaveXML(LPCTSTR lpszFile, CXMLElement* pXML)
{
	CFile pFile;
	if ( ! pFile.Open( lpszFile, CFile::modeWrite|CFile::modeCreate ) ) return FALSE;
	
	CString strXML = pXML->ToString( TRUE, TRUE );
	pFile.Write( (LPCSTR)strXML, strXML.GetLength() );
	
	pFile.Close();
	return TRUE;
}
