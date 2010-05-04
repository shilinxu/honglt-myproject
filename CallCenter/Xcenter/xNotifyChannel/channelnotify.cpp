//
// ChannelNotify.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "ChannelNotify.h"
#include "Network.h"

#include "Settings.h"
#include "Registry.h"
#include "XML.h"

#include "Neighbour.h"
#include "Circuitry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CNotifyChannel construction

CNotifyChannel::CNotifyChannel(int nChan, LPCTSTR xFeeCode) : CChannel( nChan, xFeeCode )
{
	bkIdle.initiate			= (INITIATE)bkIdle_init;
	FeeLogin.initiate		= (INITIATE)FeeLogin_init;
	FeeLogin.process		= (PROCESS)FeeLogin_cmplt;
	rcvMFsig.initiate		= (INITIATE)rcvMFsig_init;
	RcvTalk.initiate		= (INITIATE)RcvTalk_init;
	
	FwdNotify.initiate		= (INITIATE)FwdNotify_init;
	FwdNotify.process		= (PROCESS)FwdNotify_cmplt;
	
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
	ExitPlay.initiate		= (INITIATE)ExitPlay_init;
	
	RcvLvChat.initiate		= (INITIATE)RcvLvChat_init;
	RcvLvChat.process		= (PROCESS)RcvLvChat_cmplt;
	
	RcvLvMedia.initiate		= (INITIATE)RcvLvMedia_init;
	RcvLvMedia.process		= (PROCESS)RcvLvMedia_cmplt;
	
	ShRcvFnOper		= NULL;
	m_pXML			= NULL;
	
	CNotifyChannel::Setup();
}

CNotifyChannel::~CNotifyChannel()
{
	if ( m_pXML ) delete m_pXML;
	if ( ShRcvFnOper ) delete ShRcvFnOper;
}

//////////////////////////////////////////////////////////////////////
// CNotifyChannel attributes

void CNotifyChannel::Setup()
{
	CChannel::Setup();
	fcnp = &FeeLogin;

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
	LoadXML( Settings.General.Path );
}

BOOL CNotifyChannel::LoadXML(LPCTSTR pszPath)
{
	ASSERT( m_pXML == NULL );
	
	CString strXML = ( pszPath ) ? pszPath : ".";
	strXML += '\\'; strXML += _T("xProfile.xml");
	
	if ( CXMLElement* pXML = CXMLElement::FromFile( strXML, TRUE ) )
	{
		CXMLElement* pXMLNotify = pXML->GetElementByName( _T("Notify"), TRUE );
		m_pXML = pXMLNotify->Clone();
		
		delete pXML; return TRUE;
	}
	
	return FALSE;
}

struct state* CNotifyChannel::RenewPlay(LPCTSTR xLocalId)
{
	CopyMemory( LocalId, xLocalId, MAX_PHONE );
	CopyMemory( CallType, CA_TALK, MAX_CALLTYPE );
	FeeLogin.initiate	= (INITIATE)FeeRebind_init;
	
	Reserved &= 0x0; return fcnp = &FeeLogin;
}

struct state* CNotifyChannel::ShRcvFnOper_cmplt(EVTBLK *evtblk, struct state* lpstDefaultState)
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

struct state* CNotifyChannel::Next_RcvFnOper(PROCESS cmplt, CHAR nDtmfKey, struct state* lpstDefaultState)
{
	ShRcvFnOper = new keyRcvFnOper( ShRcvFnOper, cmplt );
	ShRcvFnOper->hKey = nDtmfKey;
	
	LocalId[ _tcslen(LocalId) ] = nDtmfKey;
	
	CChannel::OnConnected();
	CChannel::RcvTalk_init( NULL );
	
	Reserved = 0; return lpstDefaultState;
}

struct state* CNotifyChannel::Next_RcvFnOper(PROCESS cmplt, CHAR nDtmfKey, struct state* lpstDefaultState, DWORD nReserved)
{
	Next_RcvFnOper( cmplt, nDtmfKey, lpstDefaultState );
	
	ShRcvFnOper->Reserved = nReserved;
	return lpstDefaultState;
}

struct state* CNotifyChannel::Prev_RcvFnOper(struct keyRcvFnOper*& ShRcvFnOper, struct state* lpstDefaultState)
{
	if ( ! ShRcvFnOper->previous ) return lpstDefaultState;
	
	keyRcvFnOper* shRcvFnOper = ShRcvFnOper->previous;
	ShRcvFnOper->previous = NULL; delete ShRcvFnOper;
	ShRcvFnOper = shRcvFnOper;
	
	LocalId[ _tcslen(LocalId) - 1 ] = '\0';
	
	Reserved = 0; return lpstDefaultState;
}

//////////////////////////////////////////////////////////////////////
// CNotifyChannel TCP Transfer func

BOOL CNotifyChannel::OnBindPacket(CBindPacket* pPacket)
{
	if ( FeeLogin.initiate == (INITIATE)FeeLogin_init ) return TRUE;
	return CChannel::OnBindPacket( pPacket );
}

BOOL CNotifyChannel::OnErrPacket(CErrPacket* pPacket)
{
	return TRUE;
}

CChannel* CNotifyChannel::ConvertFrom(CChannel* pParent, LPCTSTR xString)
{
	CopyMemory(pParent->LocalId, "96031111", MAX_PHONE);
	CopyMemory(pParent->CallType, CA_TALK, MAX_CALLTYPE );
	
	pParent->OnConnected(); return NULL;
}

BOOL CNotifyChannel::OnRecordFile(BOOL bRecMixer, int nVolMixer, CString* pstrFile)
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
// CNotifyChannel OVERLOAD function

int CNotifyChannel::bkIdle_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	(this->*bkOffHk.initiate)(evtblk);
	SsmSetPlayDest( nChan, 0 );	// 取消上总线
	SsmSetFlag( nChan, F_ClearInVoiceOnRcvDtmf, 0 );
	
	CChannel* pChannel = new CChannel( nChan );
	pChannel->Clone( this ); Network.SetChannel( pChannel );
	
	return (pChannel->*(pChannel->fcnp)->initiate)(evtblk);
}

int CNotifyChannel::FeeLogin_init(EVTBLK *evtblk)
{
	return 0;
}

int CNotifyChannel::FeeRebind_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	CChannel::OnConnected();
	SetTimer( 45* 1000 );
	
	return 0;
}

struct state* CNotifyChannel::FeeLogin_cmplt(EVTBLK *evtblk)
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
#if 1
			SsmSetIsupFlag( nChan, ISUP_CallerParam, 0x1001, NULL );
#endif
			KillTimer(); Reserved &= 0; return &WaitDial;
		}
	}
	
	return CChannel::FeeLogin_cmplt(evtblk);
}

int CNotifyChannel::rcvMFsig_init(EVTBLK *evtblk)
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

int CNotifyChannel::RcvTalk_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	CChannel::RcvTalk_init(evtblk);
	
	if ( SsmGetChState(nChan) != S_CALL_TALKING )
	SsmSetPlayDest( nChan, 1 );	// 上总线，用于内线测试
	
	fcnp = &FwdNotify; Reserved = 0;
	return (this->*fcnp->initiate)(evtblk);
}

int CNotifyChannel::ExitPlay_init(EVTBLK *evtblk)
{
	if ( ! Reserved && ExitCode == EXIT_GOODBYE )
	{
		Reserved |= 0x08;	// 置状态“正在”
		return PlayFile( FILE_NTFY_GOODBYE, FT_PROMPT, FALSE );
	}
	
	return CChannel::ExitPlay_init(evtblk);	
}

/////////////////////////////////////////////////////////////////////////////
// CNotifyChannel Play function

int CNotifyChannel::FwdNotify_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	SsmSetFlag( nChan, F_ClearInVoiceOnRcvDtmf, 1);
	
	CString strFile = FILE_NTFY_WELCOME;
	strFile.Insert( strFile.GetLength() - 4, '.' );
	strFile.Insert( strFile.GetLength() - 4, m_xFeeCode );
	
	if ( PlayFile( strFile, FT_PROMPT, FALSE ) )
		return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	return 0;
}

struct state* CNotifyChannel::FwdNotify_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		ShRcvFnOper = new keyRcvFnOper( ShRcvFnOper, (PROCESS)RcvFnOper_cmplt );
		ShRcvFnOper->Reserved = (DWORD)m_pXML;
		
		Reserved = 0; return &RcvFnOper;
	}
	
	return ChkDTSignal( evtblk, &FwdNotify );
}

int CNotifyChannel::RcvFnOper_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	SsmClearRxDtmfBuf(nChan);
	RcvFnOper.process = ShRcvFnOper ? ShRcvFnOper->process : (PROCESS)RcvFnOper_cmplt;
	
	CString strFile = FILE_NTFY_RCVOPER;

	strFile.Insert( strFile.GetLength() - 4, '.' );
	strFile.Insert( strFile.GetLength() - 4, m_xFeeCode );
	
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

struct state* CNotifyChannel::RcvFnOper_cmplt(EVTBLK *evtblk)
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

int CNotifyChannel::PlyNotify_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	CString strFile = FILE_NTFY_WIZARD;
	strFile.Insert( strFile.GetLength() - 4, '.' );
	strFile.Insert( strFile.GetLength() - 4, m_xFeeCode );
	
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

struct state* CNotifyChannel::PlyNotify_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved &= 0x0; return &RcvFnOper;
	}
	
	return ChkDTSignal( evtblk, &PlyNotify );
}

struct state* CNotifyChannel::RcvFnOper_cmplt(CXMLElement* pXML, CHAR nDtmfKey)
{
	CString strValue = pXML->GetAttributeValue( "Class" );
	
	if ( ! strValue.CompareNoCase( _T("语音类") ) )
	{
		OpenUsrRecord( pXML->GetAttributeValue( _T("Host") ) );
		PlyUlWord.process	= (PROCESS)PlyUlWord_cmplt;
		
		return Next_RcvFnOper( NULL, nDtmfKey, &PlyUlWord, (DWORD)pXML );
	}
	else if ( ! strValue.CompareNoCase( _T("收藏夹") ) )
	{
		if ( ! IsPlayManager(pXML, RemoteId) ) return &RcvFnOper;
		
		OpenUsrRecord( pXML->GetAttributeValue( _T("Host") ) );
		PlyUlWord.process	= (PROCESS)PlyLvWord_cmplt;
		
		return Next_RcvFnOper( NULL, nDtmfKey, &PlyLvFile, (DWORD)pXML );
	}
	else if ( ! strValue.CompareNoCase( _T("听留言") ) )
	{
		OpenUsrRecord( pXML->GetAttributeValue( _T("Host") ) );
		PlyUlWord.process	= (PROCESS)PlyLvWord_cmplt;
		
		return Next_RcvFnOper( NULL, nDtmfKey, &PlyLvFile, (DWORD)pXML );
	}
	else if ( ! strValue.CompareNoCase( _T("写留言") ) )
	{
		return Next_RcvFnOper( (PROCESS)RcvTryOper_cmplt, nDtmfKey, &RcvLvWord, (DWORD)pXML );
	}
	else if ( ! strValue.CompareNoCase( _T("网络类") ) )
	{
		CString strValue = pXML->GetAttributeValue( _T("Host") );
		
		CSingleLock pLock( &Neighbour.m_pSection, TRUE );
		
		CCircuitry* pChannel = Neighbour.FindChannel( strValue );
		if ( ! pChannel || ! pChannel->Start( m_nChan, TRUE ) ) return &RcvFnOper;
		
		SsmSetDtmfStopPlay( m_nChan, FALSE );
		return Next_RcvFnOper( NULL, nDtmfKey, &RcvLvMedia, (DWORD)pChannel );
	}
	else if ( ! strValue.CompareNoCase( _T("会议类") ) )
	{
		CString strValue = pXML->GetAttributeValue( _T("Host") );
		
		int nRoomId = _ttoi( strValue );
		int nJoinMode = VerifyRoomId( nRoomId );
		
		if ( nJoinMode == -1 ) return &RcvFnOper;
		if ( nJoinMode == 1 && CheckBlacklist( RemoteId ) ) nJoinMode = 2;	// 听众模式
		
		return Next_RcvFnOper( (PROCESS)RcvLvChat_cmplt, nDtmfKey, &RcvLvChat, MAKEWORD(nRoomId, nJoinMode) );
	}
	else if ( ! strValue.CompareNoCase( _T("子类") ) )
	{
		CSettings::Item* pItem = Settings.GetSetting( _T(".Path") );

		CString xNavFile;
		xNavFile.Format( _T("%s\\%s\\%s"), *pItem->m_pString, FT_PROMPT, FILE_NTFY_NAVEMIT );
		
		int nClick = WriteUserClick( CString(LocalId) + nDtmfKey );
		ShPlyLvWord( m_nChan, xNavFile, nClick );
				
		return Next_RcvFnOper( (PROCESS)RcvFnOper_cmplt, nDtmfKey, &TryLvFile, (DWORD)pXML );
//		return Next_RcvFnOper( (PROCESS)RcvFnOper_cmplt, nDtmfKey, &PlyNotify, (DWORD)pXML );
	}
	else
	{
		return RenewPlay( strValue );
	}
}

CXMLElement* CNotifyChannel::FindPlay(struct keyRcvFnOper* ShRcvFnOper, CHAR nDtmfKey)
{
	BYTE szKey[16] = {0};
	if ( ShRcvFnOper ) ShRcvFnOper->RenderKey( szKey );
	
	CString strKey;
	strKey.Format( "%s.%s%c", m_xFeeCode, szKey, nDtmfKey );
	
	CXMLElement* pParent = (CXMLElement*)ShRcvFnOper->Reserved;

	for ( POSITION posNext = pParent->GetElementIterator() ; posNext ; )
	{
		CXMLElement* pXML = pParent->GetNextElement( posNext );
	//	if ( ! pXML->IsNamed( _T("Play") ) ) continue;
		
		if ( pXML->GetAttributeValue("ID").CompareNoCase(strKey) ) continue;
		
		CString strOwner = 	pXML->GetAttributeValue( "Admin" );
		
		strOwner += '|' ;
		strOwner = strOwner.SpanExcluding( _T("|;,") );
		
		CString strValue = pXML->GetAttributeValue( "Group" );
		int nGroup = _ttoi(strValue);
		
		if ( ! nGroup || VerifyRoomGroup(strOwner, nGroup, RemoteId) ) return pXML;
		else return NULL;
	}
	
	return NULL;
}

BOOL CNotifyChannel::IsPlayManager(CXMLElement* pXML, LPCTSTR xRemoteId) const
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

BOOL CNotifyChannel::IsAdministrator(LPCTSTR xRemoteId) const
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

/////////////////////////////////////////////////////////////////////////////
// CNotifyChannel play ulword function

int CNotifyChannel::PlyUlWord_init(EVTBLK *evtblk)
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

struct state* CNotifyChannel::PlyUlWord_cmplt(EVTBLK *evtblk)
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
// CNotifyChannel rec lvword function

int CNotifyChannel::RcvLvWord_init(EVTBLK *evtblk)
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
	
	if ( PlayFile( FILE_NTFY_RCVLVWRD, FT_PROMPT, FALSE ) )
	{
		return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	}
	
	return 0;
}

struct state* CNotifyChannel::RcvLvWord_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		if ( (Reserved & 0x1) != 0x1 )
		{
			OnRecordFile( FALSE, 0, &UsrLvFile );
			SsmSetDTMFStopRecCharSet( nChan, "#" );
			
			Reserved &= ~0x4;	// 清除录音标记以开始采录
			Reserved |= 0x01;	// 记录放音完毕标志
		}
	}
	if ( evtblk->wEventCode == E_PROC_RecordEnd && (Reserved & 0x1) )
	{
		Reserved |= 0x04;	// 记录录音完毕标志
//	theApp.Message(MSG_ERROR, "debug[%i]: E_PROC_RecordEnd -> 0x%x", nChan, Reserved);
		SsmStopPlayFile( nChan ); //return &RcvLvWord;
	}
	
	if ( (Reserved & 0x07) == (0x01 | 0x04) )	// 短信录制完毕
	{
#if 0
		PutUsrRecord( UsrLvFile, LocalId );
#endif
		Reserved &= 0x00; return &RcvFnOper;
	}
	
	return ChkDTSignal( evtblk, &RcvLvWord );
}

BOOL CNotifyChannel::PutUsrRecord(LPCTSTR xUsrFile, LPCTSTR xPhone, int nKey)
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
		theApp.Message( MSG_ERROR, "error on CNotifyChannel[%i]: %s", m_nChan, e.ErrorMessage() );
		return FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CNotifyChannel read voice info

// 导播提示
int CNotifyChannel::PlyLvFile_init(EVTBLK *evtblk)
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

struct state* CNotifyChannel::PlyLvFile_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved = 0; return &PlyUlWord;
	}
	
	return ChkDTSignal( evtblk, &PlyLvFile );
}

struct state* CNotifyChannel::PlyLvWord_cmplt(EVTBLK *evtblk)
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

BOOL CNotifyChannel::OpenUsrRecord(LPCTSTR hKey, int nIndex)
{
	CString strQuery;
	
#ifdef _DEBUG
	strQuery.Format( "select top 3 * from xuservoice"
#else
	strQuery.Format( "select * from xuservoice"
#endif
					" where xPhoneNum = '%s'"
					" order by xRecDate desc, xjsjbh desc", hKey );
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

int CNotifyChannel::GetUsrLvCount(BOOL bForce)
{
	if ( m_pRecordset == NULL || ! m_pRecordset->State ) return 0;
	
	if ( bForce ) m_pRecordset->Requery( adCmdUnspecified );
	return m_pRecordset->RecordCount;
}

int CNotifyChannel::WriteUserClick(LPCTSTR pszCatalog)
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

int CNotifyChannel::ShPlyNavFile(int nChan, LPCTSTR xNavFile)
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

/////////////////////////////////////////////////////////////////////////////
// CNotifyChannel try voice info

struct state* CNotifyChannel::RcvTryOper_cmplt(EVTBLK *evtblk)
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

int CNotifyChannel::TryLvFile_init(EVTBLK *evtblk)
{
	return 0;
}

struct state* CNotifyChannel::TryLvFile_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved = 0; return &RcvFnOper;
	}
	
	return ChkDTSignal( evtblk, &TryLvFile );
}

int	CNotifyChannel::ShPlyLvWord(int nChan, LPCTSTR xLvFile)
{
	SsmClearRxDtmfBuf( nChan );
	
	if ( PlayFile( xLvFile, NULL, TRUE ) )
		return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	
	return 0;
}
#if 0
int	CNotifyChannel::ShPlyLvWord(int nChan, LPCTSTR xNavFile, int nCount)
{
	TCHAR szT[80];
	CSettings::Item* pItem = Settings.GetSetting( _T(".Path") );
	
	::GetPrivateProfileString( "Notify", "PutLvWord",
		"0-0", szT, 80, *pItem->m_pString + "\\MediaCfg.ini" );
	
	SsmSetDtmfStopPlay( nChan, TRUE );
	SsmClearRxDtmfBuf( nChan );
	SsmClearFileList( nChan );
	
	DWORD nOffset1 = 0, nOffset2 = 0;
	_stscanf( szT, "%i-%i", &nOffset1, &nOffset2 );
//theApp.Message(MSG_ERROR, "debug[%i]-> ShPlyLvWord: %i, %i", nChan, nOffset1, nOffset2);
	
	SsmAddToFileList( nChan, (LPTSTR)xNavFile, 6, 0, nOffset1 );
	
	_itot( nCount, szT, 10 );
	CompDigital( szT, TRUE );
	
	SsmAddToFileList( nChan, (LPTSTR)xNavFile, 6, nOffset2, 0xFFFFFFFF );
	
	if ( SsmPlayFileList( nChan ) ) return SsmPutUserEvent( E_PROC_PlayEnd, nChan, 0 );
	return 0;
}
#else
int	CNotifyChannel::ShPlyLvWord(int nChan, LPCTSTR xNavFile, int nCount)
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
#endif
/////////////////////////////////////////////////////////////////////////////
// CNotifyChannel Chat function

int CNotifyChannel::RcvLvChat_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	CString strFile = FILE_NTFY_WIZARD;
	strFile.Insert( strFile.GetLength() - 4, '.' );
	strFile.Insert( strFile.GetLength() - 4, m_xFeeCode );
	
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

struct state* CNotifyChannel::RcvLvChat_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
		Reserved |= 0x04;	// 记录播放完毕标志
		
		if ( (Reserved & 0x2) != 0x2 )
		{
			PlayMemberJoined( nChan, LOBYTE(ShRcvFnOper->Reserved), HIBYTE(ShRcvFnOper->Reserved) );
			SetTimer( 15* 1000 );	// 规定n秒内不可离开会议
			Reserved |= 0x3;	// 写进会议标志和n秒等待标志
		}
		return &RcvLvChat;
	}
	
	if ( evtblk->wEventCode == E_SYS_TIMEOUT )
	{
// 非主持人利用监听法找人，关闭此段
#if 0
		if ( ! HIBYTE(ShRcvFnOper->Reserved) )
		{
			int nRoomId = (int)LOBYTE(ShRcvFnOper->Reserved);
			int nMmbrId = (short)HIWORD(ShRcvFnOper->Reserved);
			
			ExcludeMember( nRoomId, nMmbrId, FALSE );
		}
#endif
		Reserved &= ~0x1;	// 清写会议标志
	}
	
	if ( evtblk->wEventCode == E_CHG_RcvDTMF &&
		'*' == (CHAR)(evtblk->dwParam & 0xFFFF) && (Reserved & 0x7) == 0x6 )
	{
		// (Reserved & 0x7) == 0x6: 表示找人和等待不在进行中
		PlayMemberExited( nChan );
		
		return Prev_RcvFnOper( ShRcvFnOper, &RcvFnOper );
//////////////////////////////////////////////////////////////////////////
#ifdef NPICKUP
	ggg=999;
#endif
	}
	
#if 0	// 主持人利用排除法找人
	if ( evtblk->wEventCode == E_CHG_RcvDTMF && //! (Reserved & 0x1) &&
		'#' == (CHAR)(evtblk->dwParam & 0xFFFF) && ! HIBYTE(ShRcvFnOper->Reserved) )
	{
		int nRoomId = (int)LOBYTE(ShRcvFnOper->Reserved);
		int nMmbrId = (short)HIWORD(ShRcvFnOper->Reserved);
		
		ExcludeMember( nRoomId, nMmbrId, FALSE );
		
		if ( FindConfSpeaker( nRoomId, ++nMmbrId, &nMmbrId ) != -1 )
		{
			ExcludeMember( nRoomId, nMmbrId, TRUE );
			ShRcvFnOper->Reserved = MAKELONG( LOWORD(ShRcvFnOper->Reserved), nMmbrId );
			
			SetTimer( 15* 1000 );	// 设置查找的最长时限
			Reserved |= 0x1;	// 写进会议标志和n秒等待标志
		}
		else
		{
			ShRcvFnOper->Reserved = MAKELONG( LOWORD(ShRcvFnOper->Reserved), -1 );
		}
		return &RcvLvChat;
	}
#else	// 主持人利用监听法找人
	if ( evtblk->wEventCode == E_CHG_RcvDTMF && (Reserved & 0x2) &&
		'#' == (CHAR)(evtblk->dwParam & 0xFFFF) && ! HIBYTE(ShRcvFnOper->Reserved) )
	{
		int nRoomId = (int)LOBYTE(ShRcvFnOper->Reserved);
		int nMmbrId = (short)HIWORD(ShRcvFnOper->Reserved);
		
		ExcludeMember( nRoomId, nMmbrId, FALSE );
		
		if ( Reserved & 0x1 )
		{
			CHAR szDtmfKey[MAX_BUFFER] = {0};
			ReadDtmf( szDtmfKey, '#' );
			
			ParseOrderChat( szDtmfKey, nRoomId, nMmbrId );
			Reserved &= ~0x1;	// 清写会议标志
		}
		
		if ( FindConfSpeaker( nRoomId, ++nMmbrId, &nMmbrId ) != -1 )
		{
			SsmClearRxDtmfBuf( nChan );
			PlayMemberExited( nChan );
			
			ExcludeMember( nRoomId, nMmbrId, TRUE );
			
			Reserved |= 0x1;	// 写监听找人和等待标志
			ShRcvFnOper->Reserved = MAKELONG( LOWORD(ShRcvFnOper->Reserved), nMmbrId );
		}
		else
		{
			//if ( Reserved & 0x1 )
			PlayMemberJoined( nChan, LOBYTE(ShRcvFnOper->Reserved), HIBYTE(ShRcvFnOper->Reserved) );
			
			Reserved &= ~0x1;	// 清写会议标志
			ShRcvFnOper->Reserved = MAKELONG( LOWORD(ShRcvFnOper->Reserved), -1 );
		}
		return &RcvLvChat;
	}
#endif
#if 0	// 主持人踢人
	if ( evtblk->wEventCode == E_CHG_RcvDTMF && (Reserved & 0x1) &&
		'0' == (CHAR)(evtblk->dwParam & 0xFFFF) && ! HIBYTE(ShRcvFnOper->Reserved) )
	{
		int nRoomId = (int)LOBYTE(ShRcvFnOper->Reserved);
		int nMmbrId = (short)HIWORD(ShRcvFnOper->Reserved);
		
		KickOutMember( nRoomId, nMmbrId );
		Reserved &= ~0x1;	// 清写会议标志
		return &RcvLvChat;
	}
#endif
	
	return ChkDTSignal( evtblk, &RcvLvChat );
}

int CNotifyChannel::bkOffHk_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	PlayMemberExited( nChan );
	
	return CChannel::bkOffHk_init(evtblk);
}

int CNotifyChannel::PlayMemberJoined(int nChan, int nRoomId, int nJoinMode)
{
	int nMmbrId = -1;
	DWORD nData = 0;
	
	SsmJoinConfGroup( nRoomId, nChan, nJoinMode, nJoinMode == 0 ? 1 : 0, TRUE, TRUE );
	SsmGetConfChInfo( nChan, (int*)&nData, &nMmbrId, (PWORD)&nData, (PWORD)&nData, &nData );
	
	if ( ! nJoinMode ) SsmSetBargeInSens( m_nChan, 10 );
	bkOffHk.initiate	= (INITIATE)bkOffHk_init;
	
	int nChan2 = FindConfSpeaker( nRoomId, 0 );
	if ( nChan2 == -1 ) nChan2 = nChan;
	
if ( ! nJoinMode )
theApp.Message(MSG_ERROR, "debug[%i][%s]: SsmJoinConfGroup(%i, %i, %i), PlayMemberJoined(%i, %i)",
			   nChan, RemoteId, nRoomId, nChan, nJoinMode, nChan2, nMmbrId );
	return PlayMemberJoined( nChan2, nMmbrId );
}

int CNotifyChannel::PlayMemberJoined(int nChan, int nMmbrId)
{
	CHAR xDigital[16];
	char szFileName[MAX_PATH];
	
	SsmSetDtmfStopPlay( nChan, FALSE );
	SsmClearRxDtmfBuf( nChan );
	SsmClearFileList( nChan );
	
	SsmSetPlayDest( nChan, 1 );	// 上总线，用于广播
	
	CChannel* pChannel = Network.FindChannel( nChan );
	if ( ! pChannel ) throw "Invalid channel";
	
	_itot( nMmbrId, xDigital, 10 );
	pChannel->CompDigital( xDigital, TRUE );
	
	sprintf( szFileName, "%s\\%s", FT_PROMPT, FILE_MEET_MEMBSIT );
	SsmAddToFileList( nChan, szFileName, 6, 0, 0xFFFFFFFF );
	MLOG( szFileName );
	
	return SsmPlayFileList( nChan );
}

int CNotifyChannel::PlayMemberExited(int nChan)
{
	int nRoomId = -1;
	int nMmbrId = -1;
	DWORD nData = 0;

	if ( SsmGetConfChInfo( nChan, &nRoomId, &nMmbrId,
			(PWORD)&nData, (PWORD)&nData, &nData ) ) return -1;
	
	//if ( m_nTaskId != -1 ) ExcludeMember( nRoomId, m_nTaskId );
	SsmExitConfGroup( nChan, TRUE );
	bkOffHk.initiate	= CChannel::bkOffHk_init;

	int nChan2 = FindConfSpeaker( nRoomId, 0 );
	if ( nChan2 == -1 ) return 0;
	
//theApp.Message(MSG_ERROR, "debug[%i][%s]: SsmExitConfGroup(%i, %i), PlayMemberExited(%i, %i)",
//			   nChan, RemoteId, nRoomId, nChan, nChan2, nMmbrId );
	return PlayMemberExited( nChan2, nMmbrId );
}

int CNotifyChannel::PlayMemberExited(int nChan, int nMmbrId)
{
	CHAR xDigital[16];
	char szFileName[MAX_PATH];
	
	SsmClearFileList( nChan );
	
	CChannel* pChannel = Network.FindChannel( nChan );
	if ( ! pChannel ) throw "Invalid channel";
	
	_itot( nMmbrId, xDigital, 10 );
	pChannel->CompDigital( xDigital, TRUE );
	
	sprintf( szFileName, "%s\\%s", FT_PROMPT, FILE_MEET_MEMBEXT );
	SsmAddToFileList( nChan, szFileName, 6, 0, 0xFFFFFFFF );
	MLOG( szFileName );
	
	return SsmPlayFileList( nChan );
}

int CNotifyChannel::ParseOrderChat(LPCTSTR lpszOrder, int nRoomId, int nMmbrId)
{
	if ( _tcscmp(lpszOrder, "99") == 0 )	// 主持人踢人
	{
		return KickOutMember( nRoomId, nMmbrId );
	}
	
	return 0;
}

int CNotifyChannel::FindConfSpeaker(int nRoomId, int nFirstMmbr, int* pnNextMmbr)
{
	int nChan2 = -1;
	WORD nJoinMode, nSpeaking;
	DWORD nData = 0;
	
	int nNextMmbr = nFirstMmbr;
	if ( ! pnNextMmbr ) pnNextMmbr = &nNextMmbr;
	
	if ( SsmGetConfGrpMmbrInfo( nRoomId, nFirstMmbr, &nChan2,
				(PWORD)&nJoinMode, (PWORD)&nSpeaking, &nData ) )
	{
		WORD nTotalMember = 0;
		SsmGetConfGrpInfo( nRoomId, &nTotalMember, (PWORD)&nData, (PWORD)&nData );
	
		int* pMemberId = new int [nTotalMember + 1];
		SsmGetConfGrpMmbrId( nRoomId, pMemberId );
		
		for ( int nMmber = 0; nMmber < nTotalMember; nMmber++ )
		{
			if ( nFirstMmbr >= pMemberId[ nMmber ] ) continue;
			
			*pnNextMmbr = pMemberId[ nMmber ]; break;
		}
		
		delete [] pMemberId;
		if ( nMmber >= nTotalMember ) return -1;
		
		return FindConfSpeaker( nRoomId, *pnNextMmbr, pnNextMmbr );
	}
	
	if ( nSpeaking || nJoinMode < 2 ) return nChan2;
	return FindConfSpeaker( nRoomId, ++(*pnNextMmbr), pnNextMmbr  );
}
/*
int CNotifyChannel::ExcludeMember(int nRoomId, int nChan2)
{
//theApp.Message(MSG_ERROR, "debug[%i]: ExcludeMember(%i, %i, %i), set to %s",
//			   m_nChan, nRoomId, m_nChan, nChan2, "FALSE" );
	SsmSetContactInConf( nRoomId, nChan2, m_nChan, FALSE );
	SsmSetContactInConf( nRoomId, m_nChan, nChan2, FALSE );
	
	KillLinker(); 
	return 0;
}
*/
#if 0	// 主持人利用排除法找人
int CNotifyChannel::ExcludeMember(int nRoomId, int nMmbrId, BOOL bExclude)
{
	int nChan2 = -1;
	DWORD nData = 0;
	
	if ( SsmGetConfGrpMmbrInfo( nRoomId, nMmbrId, &nChan2,
		(PWORD)&nData, (PWORD)&nData, &nData ) ) return -1;
#if 1	
	if ( nChan2 == m_nChan ) return 1;
#endif
	
	bExclude ? SetLinker( nChan2 ) : KillLinker();
	SsmSetContactInConf( nRoomId, nChan2, m_nChan, bExclude );
	
if ( bExclude == TRUE )
{
	if ( CChannel* pChannel = Network.FindChannel( nChan2 ) )
	{
		theApp.Message(MSG_ERROR, "debug[%i][%s]: ExcludeMember(%i, %i), set [%i][%s] to %s",
			   m_nChan, RemoteId, nRoomId, nMmbrId, nChan2, pChannel->RemoteId, bExclude ? "TRUE" : "FALSE" );
	}
}
	return 0;
}
#else	// 主持人利用监听法找人
int CNotifyChannel::ExcludeMember(int nRoomId, int nMmbrId, BOOL bExclude)
{
	int nChan2 = -1;
	DWORD nData = 0;
	
	if ( SsmGetConfGrpMmbrInfo( nRoomId, nMmbrId, &nChan2,
		(PWORD)&nData, (PWORD)&nData, &nData ) ) return -1;
	
	if ( bExclude ) SsmLinkFrom( nChan2, m_nChan );
	//else SsmStopLinkFrom( nChan2, m_nChan );
	else SsmClearChBusLink( m_nChan );

	
if ( bExclude == TRUE )
{
	if ( CChannel* pChannel = Network.FindChannel( nChan2 ) )
	{
		theApp.Message(MSG_ERROR, "debug[%i][%s]: ExcludeMember(%i, %i), set [%i][%s] to %s",
			   m_nChan, RemoteId, nRoomId, nMmbrId, nChan2, pChannel->RemoteId, bExclude ? "TRUE" : "FALSE" );
	}
}
	return 0;
}
#endif
int CNotifyChannel::KickOutMember(int nRoomId, int nMmbrId)
{
	int nChan2 = -1;
	DWORD nData = 0;
	
	if ( SsmGetConfGrpMmbrInfo( nRoomId, nMmbrId, &nChan2,
		(PWORD)&nData, (PWORD)&nData, &nData ) ) return -1;
	
	if ( nChan2 == m_nChan ) return 0;
	SsmSetContactInConf( nRoomId, nChan2, m_nChan, FALSE );
	
	if ( CChannel* pChannel = Network.FindChannel( nChan2 ) )
	{
		WriteBlacklist( pChannel->RemoteId, "96031111", 3 );
		SsmHangup( nChan2 );
		
theApp.Message(MSG_TEMP, "debug[%i][%s]: KickOutMember(%i, %i) kill [%i][%s]",
			   m_nChan, RemoteId, nRoomId, nMmbrId, nChan2, pChannel->RemoteId );
	}
	return 0;
}

int CNotifyChannel::VerifyRoomId(int nRoomId)
{
	CString strXML;
	
	strXML = Settings.General.Path;
	int nLength = strXML.GetLength();

	if ( nLength > 0 && strXML.GetAt(nLength-1) != '\\' ) strXML += '\\';
	strXML += _T("xProfile.xml");
	
	if ( CXMLElement* pXML = CXMLElement::FromFile( strXML, TRUE ) )
	{
		int bResult = -1;
		
		if ( CXMLElement* pXMLSub = pXML->GetElementByName( _T("ChatRoom") ) )
		{
			bResult = VerifyRoomId( nRoomId, pXMLSub );
		}
		
		delete pXML;
		return bResult;
	}
	
	return -1;
}

int CNotifyChannel::VerifyRoomId(int nRoomId, CXMLElement* pXML)
{
	for ( POSITION pos = pXML->GetElementIterator() ; pos ; )
	{
		CXMLElement* pMap = pXML->GetNextElement( pos );
		if ( ! pMap->IsNamed( _T("Chat") ) ) continue;
		
		CString strValue = pMap->GetAttributeValue( "ID" );
		int nRoom1 = -1, nRoom2 = -1;
		
		if ( _stscanf( strValue, "%i-%i", &nRoom1, &nRoom2 ) == 2 )
		{
			if ( nRoomId < nRoom1 || nRoomId > nRoom2 ) continue;
		}
		else
		{
			if ( nRoomId != nRoom1 ) continue;
		}
		
		return VerifyRoomUser(pMap, RemoteId, nRoomId);
	}
	
	return -1;
}

int CNotifyChannel::VerifyRoomUser(CXMLElement* pXML, LPCTSTR xRemoteId, int nRoomId)
{
	if ( IsAdministrator( xRemoteId ) ) return 0;	// 主持人模式
	
	CString strValue = pXML->GetAttributeValue( "RoomOwner" );
	if ( VerifyRoomOwner( strValue, xRemoteId ) ) return 0;	// 主持人模式
#if 1
	strValue = pXML->GetAttributeValue( "MeetOnly" );
	if ( _ttoi(strValue) && ! SsmValidateGrpId(nRoomId) ) return -1;
	
	BOOL bInGroup = VerifyRoomGroup( pXML, xRemoteId );
#endif
	WORD nTotalMember = 0;
	WORD nTotalSpeaker = 0, nTotalSpeaking = 0;
	SsmGetConfGrpInfo( nRoomId, &nTotalMember, &nTotalSpeaker, &nTotalSpeaking );
	
	strValue = pXML->GetAttributeValue( "MmbrNum" );
	if ( bInGroup && _ttoi(strValue) > nTotalSpeaker ) return 1;	// 动态发言模式
	
	strValue = pXML->GetAttributeValue( "AuditNum" );
	if ( _ttoi( strValue ) > nTotalMember - nTotalSpeaker ) return 2;	// 听众模式
	
	return -1;
}

BOOL CNotifyChannel::VerifyRoomOwner(LPCTSTR lpszText, LPCTSTR xRemoteId)
{
	CString strValue = lpszText;
	
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		str.TrimLeft(); str.TrimRight();
		if ( str.IsEmpty() ) continue;
		
		if ( ! _tcsncmp( str, xRemoteId, str.GetLength() ) ) return TRUE;
	}
	
	return FALSE;
}

BOOL CNotifyChannel::VerifyRoomGroup(CXMLElement* pXML, LPCTSTR xRemoteId)
{
	CString strOwner = 	pXML->GetAttributeValue( "RoomOwner" );
	strOwner.TrimLeft(); strOwner.TrimRight();
	
	if ( strOwner.IsEmpty() ) return TRUE;
	
	strOwner += '|' ;
	strOwner = strOwner.SpanExcluding( _T("|;,") );
	
	CString strValue = pXML->GetAttributeValue( "MeetGroup" );
	int nGroupId = _ttoi( strValue );
	
	if ( nGroupId ) return VerifyRoomGroup(strOwner, nGroupId, xRemoteId);
	return TRUE;
}

BOOL CNotifyChannel::VerifyRoomGroup(LPCTSTR xOwnerId, int nGroupId, LPCTSTR xRemoteId)
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

/////////////////////////////////////////////////////////////////////////////
// CNotifyChannel Network function

int CNotifyChannel::RcvLvMedia_init(EVTBLK *evtblk)
{
	return 0;
}

struct state* CNotifyChannel::RcvLvMedia_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_CHG_RcvDTMF )
	{
		if ( '*' == (CHAR)(evtblk->dwParam & 0xFFFF) )
		{
			CCircuitry* pChannel = (CCircuitry *)ShRcvFnOper->Reserved;
			pChannel->m_nTaskId = -1; pChannel->Reset();
			
			return Prev_RcvFnOper( ShRcvFnOper, &RcvFnOper );
		}
	}
	
	return ChkDTSignal( evtblk, &RcvLvMedia );
}

/////////////////////////////////////////////////////////////////////////////
// CNotifyChannel Blacklist function

BOOL CNotifyChannel::CheckBlacklist(LPCTSTR xRemoteId, LPCTSTR xLocalId)
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

BOOL CNotifyChannel::WriteBlacklist(LPCTSTR xRemoteId, LPCTSTR xLocalId, int nHour)
{
	CString	strQuery;
	
	strQuery.Format( "insert into xenterblacklst(xjsjbh,xphone,xlocked)"
					" select xjsjbh, '%s', dateadd(hour, %i, getdate()) from xuser_info"
					" where xphone = '%s'", xRemoteId, nHour, xLocalId );
	TRACE( strQuery + "\n" );
	
	xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
	return TRUE;
}
