//
// ChannelRadio.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Network.h"
#include "ChannelRadio.h"

#include "Settings.h"
#include "Registry.h"
#include "Profiles.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef NPICKUP
static int ggg=0;
#endif

#define	FILE_NTFY_NAVEMIT		"ntfy.navemit.pcm"
#define	FILE_PAGE_PLYLVWRD		"page.plylvwrd.pcm"

//////////////////////////////////////////////////////////////////////
// CRadioChannel construction

CRadioChannel::CRadioChannel(int nChan, LPCTSTR xFeeCode) : CChannel( nChan, xFeeCode )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	FeeLogin.initiate		= (INITIATE)FeeLogin_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	rcvMFsig.initiate		= (INITIATE)rcvMFsig_init;
	RcvTalk.initiate		= (INITIATE)RcvTalk_init;
	
	FwdPlay.initiate		= (INITIATE)FwdPlay_init;
	FwdPlay.process		= (PROCESS)FwdPlay_cmplt;
	RcvFnOper.initiate		= (INITIATE)RcvFnOper_init;
	RcvFnOper.process		= (PROCESS)RcvFnOper_cmplt;
	
	PlyNotify.initiate		= (INITIATE)PlyNotify_init;
	PlyNotify.process		= (PROCESS)PlyNotify_cmplt;
	
	PlyUlWord.initiate		= (INITIATE)PlyUlWord_init;
	PlyUlWord.process		= (PROCESS)PlyUlWord_cmplt;
	RcvLvWord.initiate		= (INITIATE)RcvLvWord_init;
	RcvLvWord.process		= (PROCESS)RcvLvWord_cmplt;
	
	PlyLvFile.initiate		= (INITIATE)PlyLvFile_init;
	PlyLvFile.process		= (PROCESS)PlyLvFile_cmplt;
	TryLvFile.initiate		= (INITIATE)TryLvFile_init;
	TryLvFile.process		= (PROCESS)TryLvFile_cmplt;
	
	PlayQReal.initiate		= (INITIATE)PlayQReal_init;
	PlayQReal.process		= (PROCESS)PlayQReal_cmplt;
	
	ExitPlay.initiate		= (INITIATE)ExitPlay_init;
	ShRcvFnOper	= NULL;
	m_pXML		= NULL;
	
	CRadioChannel::Setup();
}

CRadioChannel::~CRadioChannel()
{
	if ( ShRcvFnOper ) delete ShRcvFnOper;
	if ( m_pXML ) m_pXML->Delete();
}

//////////////////////////////////////////////////////////////////////
// CRadioChannel attributes

void CRadioChannel::Setup()
{
	CChannel::Setup();
	fcnp = &FeeLogin;
	
#ifdef ZHAOPIN
	if ( Profiles["ZhaoPin"] ) m_pXML = Profiles["ZhaoPin"]->Clone();
#else
	if ( Profiles["Radio"] ) m_pXML = Profiles["Radio"]->Clone();
#endif
	SsmSetFlag( m_nChan, F_HighAndLowFreqEnScale, 18 );
	SsmSetFlag( m_nChan, F_DualAndAllFreqEnScale, 16 );
}

struct state* CRadioChannel::RenewPlay(LPCTSTR xLocalId)
{
	CopyMemory( LocalId, xLocalId, MAX_PHONE );
	CopyMemory( CallType, CA_TALK, MAX_CALLTYPE );
	FeeLogin.initiate	= (INITIATE)CChannel::FeeLogin_init;
	
	Reserved &= 0x0; return fcnp = &FeeLogin;
}

struct state* CRadioChannel::ShRcvFnOper_cmplt(EVTBLK *evtblk, struct state* lpstDefaultState)
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

struct state* CRadioChannel::Next_RcvFnOper(PROCESS cmplt, CHAR nDtmfKey, struct state* lpstDefaultState)
{
	ShRcvFnOper = new keyRcvFnOper( ShRcvFnOper, cmplt );
	ShRcvFnOper->hKey = nDtmfKey;
	
	LocalId[ _tcslen(LocalId) ] = nDtmfKey;
	
	CChannel::OnConnected();
	CChannel::RcvTalk_init( NULL );
	
	Reserved = 0; return lpstDefaultState;
}

struct state* CRadioChannel::Next_RcvFnOper(PROCESS cmplt, CHAR nDtmfKey, struct state* lpstDefaultState, DWORD nReserved)
{
	Next_RcvFnOper( cmplt, nDtmfKey, lpstDefaultState );
	
	ShRcvFnOper->Reserved = nReserved;
	return lpstDefaultState;
}

struct state* CRadioChannel::Prev_RcvFnOper(struct keyRcvFnOper*& ShRcvFnOper, struct state* lpstDefaultState)
{
	if ( ! ShRcvFnOper->previous ) return lpstDefaultState;
	
	keyRcvFnOper* shRcvFnOper = ShRcvFnOper->previous;
	ShRcvFnOper->previous = NULL; delete ShRcvFnOper;
	ShRcvFnOper = shRcvFnOper;
	
	LocalId[ _tcslen(LocalId) - 1 ] = '\0';
	
	Reserved = 0; return lpstDefaultState;
}

int CRadioChannel::ShPlyNavFile(LPCTSTR xNavFile, GKPDAT* kp)
{
	CRegistry pRegistry;
	
	// 股票代码
	
	CString strValue = pRegistry.GetString( "Stock", "Offset_code1" );
	if ( ! strValue.GetLength() ) return SsmPutUserEvent( E_PROC_PlayEnd, m_nChan, 0 );
	
	SsmSetDtmfStopPlay( m_nChan, TRUE );
	SsmClearRxDtmfBuf( m_nChan );
	SsmClearFileList( m_nChan );
	
	DWORD nOffset1 = 0, nOffset2 = 0;
	_stscanf( strValue, "%i-%i", &nOffset1, &nOffset2 );
	
	SsmAddToFileList( m_nChan, (LPTSTR)xNavFile, 6, 0, nOffset1 - 0 );
	
	CompDigital( kp->Code, FALSE );
	
	// 最新价
	
	DWORD nOffset3 = 0, nOffset4 = 0;
	strValue = pRegistry.GetString( "Stock", "Offset_new1" );
	
	_stscanf( strValue, "%i-%i", &nOffset3, &nOffset4 );
	SsmAddToFileList( m_nChan, (LPTSTR)xNavFile, 6, nOffset2, nOffset3 - nOffset2 );
	
	strValue.Format( "%.03f", kp->Close );
	CompDigital( strValue, TRUE );
	
	// 涨跌幅，录音均等分"上涨%" / "下跌%"
	
	DWORD nOffset5 = 0, nOffset6 = 0;
	strValue = pRegistry.GetString( "Stock", "Offset_markup1" );
	
	_stscanf( strValue, "%i-%i", &nOffset5, &nOffset6 );
	float fMarkup = (kp->Close - kp->Yclose) / kp->Yclose * 100;
	
	if ( fMarkup < 0 )	// 下跌，跳至 "上涨/下跌"的1/2
	{
		nOffset4 += ( nOffset5 - nOffset4 ) / 2;
		SsmAddToFileList( m_nChan, (LPTSTR)xNavFile, 6, nOffset4, nOffset5 - nOffset4 );
	}
	else
	{
		SsmAddToFileList( m_nChan, (LPTSTR)xNavFile, 6, nOffset4, (nOffset5 - nOffset4) / 2 );
	}
	
	strValue.Format( "%.02f", fMarkup );
	CompDigital( strValue, TRUE );
	
	SsmAddToFileList( m_nChan, (LPTSTR)xNavFile, 6, nOffset6, 0xFFFFFFFF );
	
	if ( SsmPlayFileList( m_nChan ) ) return SsmPutUserEvent( E_PROC_PlayEnd, m_nChan, 0 );
	return 0;
}

int CRadioChannel::ShPlyNavFile(LPCTSTR xNavFile, GKPDAT* kp, DWORD nReserved)
{
	CRegistry pRegistry;
	CString strValue;
	
	// 大盘指数，录音均等分"上证指数" / "深证成指"
	
	SsmSetDtmfStopPlay( m_nChan, TRUE );
	SsmClearRxDtmfBuf( m_nChan );
	SsmClearFileList( m_nChan );
	
	// 最新价
	
	DWORD nOffset1 = 0, nOffset2 = 0;
	strValue = pRegistry.GetString( "Stock", "Offset_new0" );
	
	_stscanf( strValue, "%i-%i", &nOffset1, &nOffset2 );
	SsmAddToFileList( m_nChan, (LPTSTR)xNavFile, 6, nOffset1 / 2 * nReserved, nOffset1 / 2 );
	
	strValue.Format( "%.02f", kp->Close );
	CompDigital( strValue, FALSE );
	
	// 涨跌，录音均等分"上涨%" / "下跌%"
	
	DWORD nOffset3 = 0, nOffset4 = 0;
	strValue = pRegistry.GetString( "Stock", "Offset_markup0" );
	
	_stscanf( strValue, "%i-%i", &nOffset3, &nOffset4 );
	float fMarkup = kp->Close - kp->Yclose;	// 上涨点数
	
	if ( fMarkup < 0 )	// 下跌，跳至 "上涨/下跌"的1/2
	{
		nOffset2 += ( nOffset3 - nOffset2 ) / 2;
		SsmAddToFileList( m_nChan, (LPTSTR)xNavFile, 6, nOffset2, nOffset3 - nOffset2 );
	}
	else
	{
		SsmAddToFileList( m_nChan, (LPTSTR)xNavFile, 6, nOffset2, (nOffset3 - nOffset2) / 2 );
	}
	
	strValue.Format( "%.02f", fMarkup );
	CompDigital( strValue, TRUE );
	
	SsmAddToFileList( m_nChan, (LPTSTR)xNavFile, 6, nOffset4, 0xFFFFFFFF );
	
	if ( SsmPlayFileList( m_nChan ) ) return SsmPutUserEvent( E_PROC_PlayEnd, m_nChan, 0 );
	return 0;
}

CXMLElement* CRadioChannel::FindPlay(struct keyRcvFnOper* ShRcvFnOper, CHAR nDtmfKey)
{
	BYTE szKey[16] = {0};
	if ( ShRcvFnOper ) ShRcvFnOper->RenderKey( szKey );
	
	CString strKey;
	strKey.Format( "%s.%s%c", m_xFeeCode, szKey, nDtmfKey );
	
	CXMLElement* pParent = (CXMLElement*)ShRcvFnOper->Reserved;

	if ( CXMLElement* pXML = Profiles.LookupKey( pParent, strKey ) )
	{
		CString strOwner = 	pXML->GetAttributeValue( "Admin" );
		
		strOwner += '|' ;
		strOwner = strOwner.SpanExcluding( _T("|;,") );
		
		CString strValue = pXML->GetAttributeValue( "Group" );
		int nGroup = _ttoi(strValue);
		
		if ( ! nGroup || VerifyRoomGroup(strOwner, nGroup, RemoteId) ) return pXML;
	}
	
	return NULL;
}

BOOL CRadioChannel::VerifyRoomGroup(LPCTSTR xOwnerId, int nGroupId, LPCTSTR xRemoteId)
{
	CString	strQuery;
	
	strQuery.Format( "select xphone from xphonemeetinggroupmore"
					" where left('%s',len(xphone)) = xphone and xgroupnum = %i"
					" and xjsjbh in (select xjsjbh from xuser_info"
									" where xphone = '%s')", xRemoteId, nGroupId, xOwnerId );
	TRACE( strQuery + "\n" );
theApp.Message(MSG_TEMP, strQuery);
	
	_RecordsetPtr xRecordset;
	xRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	
	if ( xRecordset->adoEOF ) return FALSE;
	return TRUE;
}

BOOL CRadioChannel::IsPlayManager(CXMLElement* pXML, LPCTSTR xRemoteId) const
{
	CString strValue = pXML->GetAttributeValue( "Admin" );
	
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		str.TrimLeft(); str.TrimRight();
		if ( str.IsEmpty() ) continue;
		
		if ( ! _tcsncmp( str, xRemoteId, str.GetLength() ) ) return TRUE;
	}
	
	if ( ! pXML->GetParent() ) return IsAdministrator( xRemoteId );
	else return IsPlayManager( pXML->GetParent(), xRemoteId );
}

BOOL CRadioChannel::IsAdministrator(LPCTSTR xRemoteId) const
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
// CRadioChannel TCP Transfer func

BOOL CRadioChannel::OnBindPacket(CBindPacket* pPacket)
{
	if ( FeeLogin.initiate == (INITIATE)CChannel::FeeLogin_init ) return TRUE;
	return CChannel::OnBindPacket( pPacket );
}

BOOL CRadioChannel::OnErrPacket(CErrPacket* pPacket)
{
	return TRUE;
}

BOOL CRadioChannel::OnRecordFile(BOOL bRecMixer, int nVolMixer, CString* pstrFile)
{
	CSettings::Item* pItem = Settings.GetSetting( _T("Trunk.RecFile") );
	CString xNewFile = ( pItem ) ? *pItem->m_pString : "";
	
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
		strValue.Format( "%.4i%.2i%.2i",
			pTime.wYear, pTime.wMonth, pTime.wDay );
		Replace( xNewFile, "<DATE>", strValue );
	}
	
	if ( xNewFile.Find( "<TIME>" ) >= 0 )
	{
		SYSTEMTIME pTime;
		GetLocalTime( &pTime );
		
		CString strValue;
		strValue.Format( "%.2i%.2i%.2i%.3i",
			pTime.wHour, pTime.wMinute, pTime.wSecond, pTime.wMilliseconds );
		Replace( xNewFile, "<TIME>", strValue );
	}
	
	if ( xNewFile.Find( "<REMOTE>" ) >= 0 )
	{
		CString strValue;
		Replace( xNewFile, "<REMOTE>", RemoteId );
	}
	
	for ( LPCTSTR xPath = xNewFile, xPathExt = NULL;
		  xPathExt = _tcsistr(xPath, "\\"); xPath = xPathExt + 1 )
	{
		CString strValue = xNewFile.Left(xPathExt - (LPCTSTR)xNewFile);
		CreateDirectory( Settings.General.PathRec + "\\" + strValue, NULL );
	}
	
	SsmSetRecMixer( m_nChan, bRecMixer, nVolMixer );
	RecordFile( xNewFile, Settings.General.PathRec );
	
	theApp.Message( MSG_TEMP,"Record[%i] -> %s", m_nChan, xNewFile );
	if ( pstrFile ) *pstrFile = Settings.General.PathRec + "\\" + xNewFile;
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CRadioChannel OVERLOAD function

int CRadioChannel::bkIdle_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	(this->*bkOffHk.initiate)(evtblk);
	SsmSetPlayDest( nChan, 0 );	// 取消上总线
	SsmSetFlag( nChan, F_ClearInVoiceOnRcvDtmf, 0 );
	
	CChannel* pChannel = new CChannel( nChan );
	pChannel->Clone( this ); Network.SetChannel( pChannel );
	
	return (pChannel->*(pChannel->fcnp)->initiate)(evtblk);
}

int CRadioChannel::FeeLogin_init(EVTBLK *evtblk)
{
	return 0;
}

struct state* CRadioChannel::FeeLogin_cmplt(EVTBLK *evtblk)
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
			 SsmGetChState(nChan) == S_CALL_RINGING ||	// 确保收到RINGING消息
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

int CRadioChannel::rcvMFsig_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( SsmGetChState(nChan) == S_CALL_TALKING ||	// 其他业务转入
		_tcsncmp(CallType, CA_DEBG, 4) == 0 )		// For Debug
	{
		return SsmPutUserEvent( E_CHG_ChState, nChan, S_CALL_TALKING );
	}
#if 1
	CHAR szBuff[128];
	int nLength = SsmGet1stPhoNumStr( nChan, szBuff );
	
	if ( nLength = max( min(nLength,MAX_PHONE), 0 ) )
	{
		CHAR xRemoteId[MAX_PHONE + 1] = {0};
		CopyMemory( xRemoteId, szBuff, nLength );
		//VerifyPhoneNum( RemoteId );
theApp.Message( MSG_TEMP, "debug[%i]: OriginalCallerID:=[%s]", nChan, xRemoteId );
	}
#endif
	
	return CChannel::rcvMFsig_init(evtblk);
}

int CRadioChannel::RcvTalk_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	CChannel::RcvTalk_init(evtblk);
	
	if ( SsmGetChState(nChan) != S_CALL_TALKING )
	SsmSetPlayDest( nChan, 1 );	// 上总线，用于内线测试
	
	fcnp = &FwdPlay; Reserved = 0;
	return (this->*fcnp->initiate)(evtblk);
}

int CRadioChannel::ExitPlay_init(EVTBLK *evtblk)
{
	if ( ! Reserved && ExitCode == EXIT_GOODBYE )
	{
		CString strFile;
		strFile.Format( FILE_RADO_GOODBYE, m_xFeeCode );
		
		Reserved |= 0x08;	// 置状态“正在”
		if ( ! PlayFile( strFile, FT_PROMPT, FALSE ) ) return 0;
		return SsmPutUserEvent( E_PROC_PlayEnd, m_nChan, 0 );
	}
	
	return CChannel::ExitPlay_init(evtblk);	
}

/////////////////////////////////////////////////////////////////////////////
// CRadioChannel Play function

int CRadioChannel::FwdPlay_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	SsmSetFlag( nChan, F_ClearInVoiceOnRcvDtmf, 1);
	
	CString strFile;
	strFile.Format( FILE_RADO_WELCOME, m_xFeeCode );
	
	if ( PlayFile( strFile, FT_PROMPT, FALSE ) )
		return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	return 0;
}

struct state* CRadioChannel::FwdPlay_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		ShRcvFnOper = new keyRcvFnOper( ShRcvFnOper, (PROCESS)RcvFnOper_cmplt );
		ShRcvFnOper->Reserved = (DWORD)m_pXML;
		
		Reserved = 0; return &RcvFnOper;
	}
	
	return ChkDTSignal( evtblk, &FwdPlay );
}

int CRadioChannel::RcvFnOper_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	SsmClearRxDtmfBuf(nChan);
	RcvFnOper.process = ShRcvFnOper ? ShRcvFnOper->process : (PROCESS)RcvFnOper_cmplt;
	
	CString strFile;
	strFile.Format( FILE_RADO_RCVOPER, m_xFeeCode );
	
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
	
	ExitCode = EXIT_GOODBYE; fcnp = &ExitPlay;
	Reserved = 0; return (this->*fcnp->initiate)(evtblk);
}
#if 0
struct state* CRadioChannel::RcvFnOper_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
#ifdef NPICKUP
	if ( evtblk->wEventCode == E_SYS_TIMEOUT )
	{
		theApp.Message(MSG_ERROR,"debug[%i]: RcvFnOper_cmplt-> {E_SYS_TIMEOUT}", nChan);
		SsmStopPlayFile( nChan);
		return &RcvFnOper;
	}
#endif
	
	struct state* state = ShRcvFnOper_cmplt( evtblk, &RcvFnOper );
	if ( state != &RcvFnOper ) return state;
#ifdef NPICKUP
	if ( (Reserved & 0x04) )
	{
		static int ii = '1';
		CHAR nDtmfKey = ii;//++;
		ii='1';
#else
	
	if ( (Reserved & 0x04) && SsmGetRxDtmfLen( nChan ) )
	{
		CHAR nDtmfKey = '\0';
		SsmGet1stDtmf( nChan, &nDtmfKey );
#endif
		
		switch ( nDtmfKey )
		{
		case	'1':	// 实时行情
			{
				GdatOpen();
				m_pPosNext = NULL;
				return Next_RcvFnOper( (PROCESS)RcvHqOper_cmplt, nDtmfKey, &PlyNotify );
			}
			
//		default:
//			return KeyNotify( nDtmfKey, (PROCESS)RcvFnOper2_cmplt );
		}
	}
	
	return &RcvFnOper;
}
#else

struct state* CRadioChannel::RcvFnOper_cmplt(EVTBLK *evtblk)
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

struct state* CRadioChannel::RcvFnOper_cmplt(CXMLElement* pXML, CHAR nDtmfKey)
{
	CRegistry pRegistry;
	int nExpired = pRegistry.GetInt( "Radio", "PageExpire", 7*24 * 3600 );
	
	CString strValue = pXML->GetAttributeValue( "Class" );
	
	if ( ! strValue.CompareNoCase( _T("语音类") ) )
	{
		OpenUsrRecord( pXML->GetAttributeValue( _T("Host") ), nExpired );
		PlyUlWord.process	= (PROCESS)PlyUlWord_cmplt;
		
		return Next_RcvFnOper( NULL, nDtmfKey, &PlyUlWord, (DWORD)pXML );
	}
	else if ( ! strValue.CompareNoCase( _T("听留言") ) )
	{
		OpenUsrRecord( pXML->GetAttributeValue( _T("Host") ), nExpired );
		PlyUlWord.process	= (PROCESS)PlyLvWord_cmplt;
		
		return Next_RcvFnOper( NULL, nDtmfKey, &PlyLvFile, (DWORD)pXML );
	}
	else if ( ! strValue.CompareNoCase( _T("写留言") ) )
	{
		return Next_RcvFnOper( (PROCESS)RcvTryOper_cmplt, nDtmfKey, &RcvLvWord, (DWORD)pXML );
	}
	else if ( ! strValue.CompareNoCase( _T("子类") ) )
	{
		CSettings::Item* pItem = Settings.GetSetting( _T(".Path") );

		CString xNavFile;
		xNavFile.Format( _T("%s\\%s\\%s"), *pItem->m_pString, FT_PROMPT, FILE_NTFY_NAVEMIT );
		
		int nClick = WriteUserClick( CString(LocalId) + nDtmfKey );
		ShPlyLvWord( m_nChan, xNavFile, nClick );
				
		return Next_RcvFnOper( (PROCESS)RcvFnOper_cmplt, nDtmfKey, &TryLvFile, (DWORD)pXML );
		//return Next_RcvFnOper( (PROCESS)RcvFnOper_cmplt, nDtmfKey, &PlyNotify, (DWORD)pXML );
	}
	
	return RenewPlay( strValue );
}
#endif
int CRadioChannel::PlyNotify_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	CString strFile = FILE_RADO_WIZARD;
	strFile.Format( FILE_RADO_WIZARD, m_xFeeCode );
	
	BYTE szKey[16] = {0};
	if ( ShRcvFnOper ) ShRcvFnOper->RenderKey( szKey );
	
	if ( _tcslen( (LPTSTR)szKey ) )
	{
		strFile.Insert( strFile.GetLength() - 4, '.' );
		strFile.Insert( strFile.GetLength() - 4, (LPTSTR)szKey );
	}
	
	if ( PlayFile( strFile, FT_PROMPT, TRUE ) )
	{
		Reserved &= 0x0; fcnp = &RcvFnOper;
		return (this->*fcnp->initiate)(evtblk); 
	}
	
	return 0;
}

struct state* CRadioChannel::PlyNotify_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved &= 0x0; return &RcvFnOper;
	}
	
	return ChkDTSignal( evtblk, &PlyNotify );
}

int CRadioChannel::WriteUserClick(LPCTSTR pszCatalog)
{
	CString strQuery;
	
	strQuery.Format( "select * from xuserclick"
					" where xphone = '%s'", pszCatalog );
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr xRecordset;
	xOpenRecordset(xRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
	
	int nCount = 1;
	if ( xRecordset->adoEOF )
	{
		xRecordset->AddNew();
		xRecordset->PutCollect( "xphone", _variant_t(pszCatalog) );
	}
	else
	{
		nCount += xRecordset->GetCollect( "xcount" ).lVal;
	}
	
	_variant_t TheValue = xRecordset->GetCollect( "xcount" );
	xRecordset->PutCollect( "xcount", _variant_t((long)nCount) );
	
	xRecordset->Update();
	return nCount;
}

/////////////////////////////////////////////////////////////////////////////
// CRadioChannel rec gpcode function

struct state* CRadioChannel::RcvHqOper_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
#ifdef NPICKUP
	if ( evtblk->wEventCode == E_SYS_TIMEOUT )
	{
		theApp.Message(MSG_ERROR,"debug[%i]: RcvHqOper_cmplt-> {E_SYS_TIMEOUT}", nChan);
		SsmStopPlayFile( nChan);
		return &RcvFnOper;
	}
#endif
#ifdef NPICKUP
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x04;	// 记录播放完毕标志
	}
	if ( (Reserved & 0x04) )
	{
		CHAR szDtmfKey[MAX_BUFFER] = {0};
		KillTimer(); Reserved &= 0x00;
		CopyMemory( szDtmfKey, "600177*0001", 11 );
#else
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x04;	// 记录播放完毕标志
	}
	if ( (Reserved & 0x04) && SsmGetRxDtmfLen( nChan ) )
	{
		CHAR nDtmfKey = '\0';
		SsmGetLastDtmf( nChan, &nDtmfKey );
		
		if ( nDtmfKey != '#' )
		{
			return ChkDTSignal( evtblk, &RcvFnOper );
		}
		
		KillTimer(); Reserved &= 0x3;
		
		CHAR szDtmfKey[MAX_BUFFER] = {0};
		ReadDtmf( szDtmfKey, '#' );
#endif
		FormatIntoList( szDtmfKey );
		Reserved = 0x0; return &PlayQReal;
	}
	
	return ShRcvFnOper_cmplt( evtblk, &RcvFnOper );
}

int CRadioChannel::PlayQReal_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	if ( ! m_pPosNext ) m_pPosNext = m_pList.GetHeadPosition();
	if ( ! m_pPosNext )
	{
		fcnp = Prev_RcvFnOper( ShRcvFnOper, &RcvFnOper );
		return (this->*fcnp->initiate)(evtblk); 
	}
	
	Reserved = 0x08;	// 置状态“正在”
	SsmClearRxDtmfBuf(nChan);
	
	GKPDAT kpdat[100];
	CHAR szCode[1][100];
	
TRACE("1)-> %i-> 0x%x\n", GetTickCount(), m_pPosNext);
	CString strCode = m_pList.GetNext(m_pPosNext);
TRACE("2)-> %i-> 0x%x\n", GetTickCount(), m_pPosNext);
	LPCTSTR pszCode = (LPCTSTR)strCode;
	
	if ( strCode.Left(2) == "ZS" ) pszCode += 3;	// "ZSA","ZSB"
	_tcscpy( szCode[0], pszCode );
	
	if ( GdatMultiKp( kpdat, szCode, 1 ) < 0 )
	{
theApp.Message(MSG_ERROR, "SsmPutUserEvent-> E_PROC_PlayEnd");
		return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	}
//return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	CString strFile;
	CSettings::Item* pItem = Settings.GetSetting( _T(".Path") );
	
	strFile.Format( _T("%s\\%s\\%s"), *pItem->m_pString, FT_PROMPT,
		strCode.Left(2) == "ZS" ? FILE_STOK_READDAP : FILE_STOK_READGEG );
	
	if ( strCode.Left(2) == "ZS" )
		return ShPlyNavFile( (LPCTSTR)strFile, kpdat, strCode.GetAt(2) - 'A' );
	else
		return ShPlyNavFile( (LPCTSTR)strFile, kpdat );
}

struct state* CRadioChannel::PlayQReal_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	//////////////////////////////////////////////////////////////////////////
#ifdef NPICKUP
	if ( evtblk->wEventCode == E_SYS_TIMEOUT )
	{
		SsmStopPlayFile( nChan);
		return &PlayQReal;
	}
#endif
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x04;	// 记录播放完毕标志
		Reserved &= 0x7;
	}
	
	if ( (Reserved & 0x04) && SsmGetRxDtmfLen( nChan ) )
	{
		CHAR nDtmfKey = '\0';
		SsmGetLastDtmf( nChan, &nDtmfKey );
		
		if ( nDtmfKey == '*' )
		{
			GdatClose();
			return ShRcvFnOper_cmplt( evtblk, &RcvFnOper );
		}
	}
	
	return ChkDTSignal( evtblk, &PlayQReal );
}

void CRadioChannel::FormatIntoList(LPCTSTR lpzText)
{
	m_pList.RemoveAll();
	m_pList.AddTail( "ZSA999999" );
	m_pList.AddTail( "ZSB399001" );
	
	CString strValue = lpzText;
	
	for ( strValue += '*' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("*") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		str.TrimLeft(); str.TrimRight();
		if ( ! str.IsEmpty() ) m_pList.AddTail( str );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CRadioChannel play ulword function

int CRadioChannel::PlyUlWord_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	if ( m_pRecordset == NULL || m_pRecordset->adoEOF || m_pRecordset->BOF )
	{
		fcnp = Prev_RcvFnOper( ShRcvFnOper, &RcvFnOper );
		return (this->*fcnp->initiate)(evtblk); 
	}
	
	Reserved |= 0x08;	// 置状态“正在”
	SsmClearRxDtmfBuf(nChan);
	
	_variant_t TheValue = m_pRecordset->GetCollect( "xPCMName" );
	if ( VT_NULL == TheValue.vt ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	CString xUsrFile = (VT_NULL != TheValue.vt) ? (LPCTSTR)(_bstr_t)TheValue : "";
	xUsrFile.TrimLeft(); xUsrFile.TrimRight();
	
	if ( PlayFile( (LPCTSTR)xUsrFile, NULL, TRUE ) )
	{
		CHAR sError[1024];
		SsmGetLastErrMsg( sError );
		theApp.Message( MSG_ERROR, sError );
		
		if ( ! SsmStopPlayFile( nChan ) ) return 0;
		return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	}
	
//	TheValue = m_pRecordset->GetCollect( "xTheIndex" );
//	ShRcvFnOper->Reserved = MAKELONG( (long)TheValue, HIWORD(ShRcvFnOper->Reserved) );
	return 0;
}

struct state* CRadioChannel::PlyUlWord_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x04;	// 记录播放完毕标志
	}
	
	if ( (Reserved & 0x04) == 0x4 )
	{
		CHAR nDtmfKey = '\0';
		SsmGetLastDtmf( nChan, &nDtmfKey );
		
		Reserved = 0; KillTimer();
		
		if ( nDtmfKey == '#' || ! nDtmfKey )
		{
			m_pRecordset->MoveNext();
		}
		else if ( nDtmfKey == '*' )	// 返回上层
		{
			xCloseRecordset(m_pRecordset);
		}
		else
		{
			m_pRecordset->MovePrevious();
		}
		
		return &PlyUlWord;
	}
	
	return ChkDTSignal( evtblk, &PlyUlWord );
}

/////////////////////////////////////////////////////////////////////////////
// CRadioChannel rec lvword function

int CRadioChannel::RcvLvWord_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	if ( CheckBlacklist( RemoteId ) )
	{
		ExitCode = EXIT_GOODBYE; fcnp = &ExitPlay;
		Reserved = 0; return (this->*fcnp->initiate)(evtblk);
	}
	
	Reserved |= 0x08;	// 置状态“正在”
	SsmStopRecToFile( nChan );
	
	CString strFile;
	strFile.Format( FILE_RADO_RCVLVWRD, m_xFeeCode );
	
	if ( ! PlayFile( strFile, FT_PROMPT, FALSE ) ) return 0;
	return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
}

struct state* CRadioChannel::RcvLvWord_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		if ( (Reserved & 0x1) != 0x1 )
		{
			OnRecordFile( FALSE, 0, &UsrLvFile );
			SsmSetDTMFStopRecCharSet( nChan, "#" );
			
			CRegistry pRegistry;
			SetTimer( pRegistry.GetInt( "Radio", "RecLength", 3* 60 * 1000 ) );
			
			Reserved &= ~0x4;	// 清除录音标记以开始采录
			Reserved |= 0x01;	// 记录放音完毕标志
		}
	}
	if ( evtblk->wEventCode == E_PROC_RecordEnd && (Reserved & 0x1) )
	{
		Reserved |= 0x04;	// 记录录音完毕标志
//	theApp.Message(MSG_ERROR, "debug[%i]: E_PROC_RecordEnd -> 0x%x", nChan, Reserved);
		SsmStopPlayFile( nChan ); //return &RcvLvWord;
		KillTimer();
	}
	
	if ( (Reserved & 0x07) == (0x01 | 0x04) )	// 短信录制完毕
	{
#if 0
		PutUsrRecord( UsrLvFile, LocalId );
#endif
		Reserved &= 0x00; return &RcvFnOper;
	}
	if ( evtblk->wEventCode == E_SYS_TIMEOUT )
	{
		SsmStopPlayFile( nChan );
	}
	
	return ChkDTSignal( evtblk, &RcvLvWord );
}

BOOL CRadioChannel::PutUsrRecord(LPCTSTR xUsrFile, LPCTSTR xPhone, int nKey)
{
	try
	{
		CString	strQuery;
		
		strQuery.Format( "insert into xuservoice"
					" (xPCMName,xPhoneNum,xRecDate,xCaller,xTheIndex,xTheState)"
					" values('%s','%s',getdate(),'%s',%i,'DELIVRD')",
					xUsrFile, xPhone, RemoteId, nKey );
		TRACE( strQuery + "\n" );
		
		xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
		return TRUE;
	}
	catch(_com_error e) 
	{ 
		theApp.Message( MSG_ERROR, "error on CRadioChannel[%i]: %s", m_nChan, e.ErrorMessage() );
		return FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CRadioChannel try voice info

struct state* CRadioChannel::RcvTryOper_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x04;	// 记录播放完毕标志
	}
	
	if ( (Reserved & 0x04) && SsmGetRxDtmfLen( nChan ) )
	{
		CHAR nDtmfKey = '\0';
		KillTimer(); Reserved &= 0x0;
		
		SsmGet1stDtmf( nChan, &nDtmfKey );
		
		switch ( nDtmfKey )
		{
		case	'1':
			ShPlyLvWord( nChan, UsrLvFile );
			return &TryLvFile;
		
		case	'2':
			{
				CString strValue = LocalId;
				
				if ( ShRcvFnOper->Reserved )
				{
					CXMLElement* pXML = (CXMLElement*)ShRcvFnOper->Reserved;
					if ( pXML->GetAttribute("Host") ) strValue = pXML->GetAttributeValue( "Host" );
				}
				
				PutUsrRecord( UsrLvFile, strValue );
				CSettings::Item* pItem = Settings.GetSetting( _T(".Path") );
				
				CString xNavFile;
				xNavFile.Format( _T("%s\\%s\\%s"), *pItem->m_pString, FT_PROMPT, FILE_NTFY_NAVEMIT );
				
				OpenUsrRecord( strValue );
				ShPlyLvWord( nChan, xNavFile, GetUsrLvCount() );
				
				return Prev_RcvFnOper( ShRcvFnOper, &TryLvFile );
			}

		default:
			DeleteFile( (LPCTSTR)UsrLvFile );
			return Prev_RcvFnOper( ShRcvFnOper, &RcvFnOper );
		}
	}
	
	return ShRcvFnOper_cmplt( evtblk, &RcvFnOper );
}

int CRadioChannel::TryLvFile_init(EVTBLK *evtblk)
{
	return 0;
}

struct state* CRadioChannel::TryLvFile_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved = 0; return &RcvFnOper;
	}
	
	return ChkDTSignal( evtblk, &TryLvFile );
}

int	CRadioChannel::ShPlyLvWord(int nChan, LPCTSTR xLvFile)
{
	SsmClearRxDtmfBuf( nChan );
	
	if ( PlayFile( xLvFile, NULL, TRUE ) )
		return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	return 0;
}

int	CRadioChannel::ShPlyLvWord(int nChan, LPCTSTR xNavFile, int nCount)
{
	CRegistry pRegistry;
	CString strValue = pRegistry.GetString( "Notify", "NavOffset" );
	if ( ! strValue.GetLength() ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	SsmSetDtmfStopPlay( nChan, FALSE );
	SsmClearRxDtmfBuf( nChan );
	SsmClearFileList( nChan );
	
	DWORD nOffset1 = 0, nOffset2 = 0;
	_stscanf( strValue, "%i-%i", &nOffset1, &nOffset2 );
	
	SsmAddToFileList( nChan, (LPTSTR)xNavFile, 6, 0, nOffset1 );
	
	strValue.Format( "%i", nCount );
	CompDigital( (LPCTSTR)strValue, TRUE );
	
	SsmAddToFileList( nChan, (LPTSTR)xNavFile, 6, nOffset2, 0xFFFFFFFF );
	
	if ( SsmPlayFileList( nChan ) ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CRadioChannel read voice info
// 导播提示
int CRadioChannel::PlyLvFile_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	if ( ! GetUsrLvCount() )
		return PlayFile( FILE_PROMPT_NORECD, FT_PROMPT, FALSE );
	
	CString strFile;
	CSettings::Item* pItem = Settings.GetSetting( _T(".Path") );
	strFile.Format( _T("%s\\%s\\%s"), *pItem->m_pString, FT_PROMPT, FILE_PAGE_PLYLVWRD );
	
	return ShPlyNavFile( nChan, (LPCTSTR)strFile );
}

struct state* CRadioChannel::PlyLvFile_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved = 0; return &PlyUlWord;
	}
	
	return ChkDTSignal( evtblk, &PlyLvFile );
}

struct state* CRadioChannel::PlyLvWord_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		CHAR nDtmfKey = '\0';
		//Reserved = 0; KillTimer();
		
		SsmGet1stDtmf( nChan, &nDtmfKey );
		CXMLElement* pXML = (CXMLElement*)ShRcvFnOper->Reserved;
		
		if ( nDtmfKey == '0' && IsPlayManager(pXML, RemoteId) )
		{
			_variant_t TheValue = m_pRecordset->GetCollect( "xPCMName" );
			m_pRecordset->Delete( adAffectCurrent );
			
			if ( VT_NULL != TheValue.vt )
			{
				CString xUsrPath = (LPCTSTR)(_bstr_t)TheValue;
				DeleteFile( (LPCTSTR)xUsrPath );
					
				xUsrPath = xUsrPath.Left( xUsrPath.ReverseFind( '\\' ) );
				RemoveDirectory( xUsrPath );
			}
		}
		if ( nDtmfKey == '1' && IsPlayManager(pXML, RemoteId) )
		{
			CHAR nLastDtmf = ShRcvFnOper->hKey - '0' + 'A' - 1;
			if ( CXMLElement* pXML = FindPlay(ShRcvFnOper->previous, nLastDtmf) )
			{
				Prev_RcvFnOper( ShRcvFnOper, &RcvFnOper );
				
				return RcvFnOper_cmplt( pXML, nLastDtmf );
			}
		}
	}
	
	return PlyUlWord_cmplt( evtblk );
}

int CRadioChannel::ShPlyNavFile(int nChan, LPCTSTR xNavFile)
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

BOOL CRadioChannel::OpenUsrRecord(LPCTSTR hKey, int nExpired, int nIndex)
{
	CString strQuery;
	
#ifdef _DEBUG
	strQuery.Format( "select top 3 * from xuservoice"
#else
	strQuery.Format( "select * from xuservoice"
#endif
					" where xPhoneNum = '%s'"
					" and (datediff(Minute, xRecDate, getdate()) & 0x7fffffff)<=%i"
					" order by xRecDate desc, xjsjbh desc", hKey, nExpired / 60 );
	TRACE( strQuery + "\n" );
	
	xOpenRecordset(m_pRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
	if ( m_pRecordset->adoEOF ) return FALSE;
	
	if ( nIndex != -1 )
	{
		strQuery.Format( "xTheIndex = %i", nIndex );
		m_pRecordset->Filter = (_bstr_t)strQuery;
		
		if ( m_pRecordset->adoEOF ) return FALSE;
	}
	
	return TRUE;
}

int CRadioChannel::GetUsrLvCount(BOOL bForce)
{
	if ( m_pRecordset == NULL || ! m_pRecordset->State ) return 0;
	
	if ( bForce ) m_pRecordset->Requery( adCmdUnspecified );
	return m_pRecordset->RecordCount;
}

/////////////////////////////////////////////////////////////////////////////
// CRadioChannel Blacklist function

BOOL CRadioChannel::CheckBlacklist(LPCTSTR xRemoteId, LPCTSTR xLocalId)
{
	CString	strQuery;
	
	strQuery.Format( "select xphone from xenterblacklst"
					" where left('%s',len(xphone)) = xphone and getdate() <= xlocked"
					" and xjsjbh in (select xjsjbh from xuser_info"
									" where xphone = '%s')", xRemoteId, xLocalId );
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr xRecordset;
	xRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	
	if ( xRecordset->adoEOF ) return FALSE;
	return TRUE;
}

BOOL CRadioChannel::WriteBlacklist(LPCTSTR xRemoteId, LPCTSTR xLocalId, int nHour)
{
	CString	strQuery;
	
	strQuery.Format( "insert into xenterblacklst(xjsjbh,xphone,xlocked)"
					" select xjsjbh, '%s', dateadd(hour, %i, getdate()) from xuser_info"
					" where xphone = '%s'", xRemoteId, nHour, xLocalId );
	TRACE( strQuery + "\n" );
	
	xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
	return TRUE;
}
