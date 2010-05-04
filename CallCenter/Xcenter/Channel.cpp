//
// Channel.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Channel.h"
#include "Network.h"
#include "Settings.h"
#include "Profiles.h"
#include "Traffic.h"

#include "CtrlNetworkChild.h"
#include "XML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CChannel construction

CChannel::CChannel(int nChan, LPCTSTR xFeeCode)	: CTransfer()
{
	bkIdle.initiate			= bkIdle_init;			bkIdle.process			= bkIdle_cmplt;
	bkFree.initiate			= bkFree_init;			bkFree.process			= bkFree_cmplt;
	bkOnHk.initiate			= bkOnHk_init;			bkOnHk.process			= bkOnHk_cmplt;
	bkOffHk.initiate		= bkOffHk_init;			bkOffHk.process			= bkOffHk_cmplt;
	ResetPcm.initiate		= ResetPcm_init;		ResetPcm.process		= ResetPcm_cmplt;
	
	seiConfm.initiate		= seiConfm_init;		seiConfm.process		= seiConfm_cmplt;
	rcvMFsig.initiate		= rcvMFsig_init;		rcvMFsig.process		= rcvMFsig_cmplt;
	RcvTalk.initiate		= RcvTalk_init;			RcvTalk.process			= RcvTalk_cmplt;
	WaitDial.initiate		= WaitDial_init;		WaitDial.process		= WaitDial_cmplt;
	
	FeeLogin.initiate		= FeeLogin_init;		FeeLogin.process		= FeeLogin_cmplt;
	ExitPlay.initiate		= ExitPlay_init;		ExitPlay.process		= ExitPlay_cmplt;
	
	m_nChan		= nChan;
	m_pOwner	= NULL;
	m_nTaskId	= -1;
	m_hInstance	= m_hDllClosing = NULL;
	m_xFeeCode	= _tcsdup( xFeeCode );
	
	CChannel::Setup();
}

CChannel::~CChannel()
{
	CTransfer::Disconnect();
	free( m_xFeeCode ); m_xFeeCode = NULL;
}

HMODULE CChannel::Release()
{
	HMODULE hModule = m_hInstance;
	delete this;
	
	//if ( hModule ) FreeLibrary( hModule );
	return hModule;
}

//////////////////////////////////////////////////////////////////////
// CChannel attributes

void CChannel::SetOwner(CNetworkChild* pOwner)
{
	m_pOwner		= pOwner;
	ASSERT( pOwner->GetSafeHwnd() );
}

void CChannel::PrintChState(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	m_pOwner->SetItem( nChan, evtblk->dwParam & 0xFFFF );
}

void CChannel::Setup()
{
	ZeroMemory( LocalId, MAX_PHONE + 1 );
	ZeroMemory( RemoteId, MAX_PHONE + 1 );
	ZeroMemory( FeeNumber, MAX_PHONE + 1 );
	ZeroMemory( CallType, MAX_CALLTYPE + 1 );
	
	m_hDllClosing= m_hInstance; m_hInstance	= NULL;
	
	KillTimer(); m_nTaskId = -1;
	ExitCode = 0; Reserved = 0;
	
	this->fcnp = &bkIdle;
}

void CChannel::StopChan()
{
	int nChan = m_nChan;
	
	SsmHangup( nChan );
	SsmStopPlay( nChan );
	SsmStopRecToFile( nChan );
	
	KillTimer(); KillLinker();
	SsmClearChBusLink( nChan );
}

void CChannel::SetTimer(DWORD nTimer)
{
	ExpTimer.state = TR_ON;
	ExpTimer.expire= GetTickCount() + nTimer;
}

void CChannel::KillTimer()
{
	ExpTimer.state = TR_OFF;
}

BOOL CChannel::TimerOut()
{
	if ( ExpTimer.state != TR_ON )	return FALSE;
	if ( GetTickCount() < ExpTimer.expire ) return FALSE;

	ExpTimer.state = TR_OFF; return TRUE;
}

void CChannel::SetLinker(int nChan)
{
	KillLinker();
	
	if ( SsmTalkWith( m_nChan, nChan ) )
	{
		CHAR sError[1024];
		SsmGetLastErrMsg( sError );
		theApp.Message( MSG_ERROR, "SsmTalkWith:%i,%i ->%s", m_nChan,nChan,sError );
	}
	
	CChannel* pChannel = Network.FindChannel( nChan );
	if ( ! pChannel ) throw "Invalid channel";
	pChannel->m_nTaskId = m_nChan;
	m_nTaskId = nChan;
	
	theApp.Message(MSG_DEBUG,"SetLinker: %i with %i", m_nChan, nChan);
}

void CChannel::KillLinker()
{
	if ( m_nTaskId == -1 ) return;
	
	theApp.Message(MSG_DEBUG,"KillLinker: %i with %i", m_nChan, m_nTaskId);
	
	if ( SsmStopTalkWith(m_nChan, m_nTaskId) )
	{
		CHAR sError[1024];
		SsmGetLastErrMsg( sError );
		theApp.Message( MSG_ERROR, "SsmStopTalkWith:%i,%i ->%s", m_nChan,m_nTaskId,sError );
	}
	
	CChannel* pChannel = Network.FindChannel( m_nTaskId );
	if ( ! pChannel ) throw "Invalid channel";
	pChannel->m_nTaskId = -1;
	m_nTaskId = -1;
}

int CChannel::LoadCallerId()
{
	CHAR szBuff[128];
	int nChan = m_nChan;

	int nLength = SsmGetCallerId( nChan, szBuff );
	nLength = max( min(nLength,MAX_PHONE), 0 );
	
	ZeroMemory( RemoteId, MAX_PHONE );
	CopyMemory( RemoteId, szBuff, nLength );

#if 1
	VerifyPhoneNum( RemoteId );
#endif

	return nLength;
}

int CChannel::LoadPhonNumber()
{
	CHAR szBuff[128];
	int nChan = m_nChan;
	
	SsmGetPhoNumStr( nChan, szBuff );
	int nLength = _tcslen( szBuff );
	
	ZeroMemory( LocalId, MAX_PHONE );
	CopyMemory( LocalId, szBuff, min(nLength,MAX_PHONE) );
	
#if 1
	CXMLElement* pXML = Profiles.FindAnalog( nChan, LocalId );
	if ( ! pXML ) return nLength;
	
	CString strHost = pXML->GetAttributeValue( "ID" );
	nLength = min( strHost.GetLength(), MAX_PHONE );
	
	ZeroMemory( LocalId, MAX_PHONE );
	CopyMemory( LocalId, strHost.GetBuffer(0), nLength );
#endif
	
	return nLength;
}

int CChannel::VerifyPhoneNum(CHAR* xPhone)
{
	int nLength = _tcslen( xPhone );

	// for local telephone
	if ( nLength <= 8 )
	{
		MoveMemory( xPhone + 4, xPhone, MAX_PHONE - nLength ); nLength += 4;
		CopyMemory( xPhone, "0574", 4 ); xPhone[nLength] = '\0';
	}
#if 0
	else
	// FOR MOBILE
	if ( nLength > 12 )
	{
		MoveMemory( xPhone, xPhone + nLength - 11, 11 );
		ZeroMemory( xPhone + 11, MAX_PHONE - 11 ); nLength = 11;
	}
#endif

	return _tcslen( xPhone );
}

BOOL CChannel::IsTestNumber(LPCTSTR xPhone, CString* pxCaller)
{
	if ( ! Settings.Test.Enable ) return FALSE;
	
	CString strValue = Settings.Test.Phone;
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		if ( _tcsncmp( str, xPhone, str.GetLength() ) == 0 )
		{
			if ( pxCaller ) *pxCaller = Settings.Test.Caller;
			return TRUE;
		}
	}
	
	return FALSE;
}

BOOL CChannel::IsPoseNumber(LPCTSTR xPhone, LPCTSTR xCaller, CString* pxCaller)
{
	if ( ! Settings.Tricker.Enable ) return FALSE;
	
	CString strValue;
	strValue = Settings.Tricker.ExceptNum;
	
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		if ( ! _tcsncmp( str, xCaller, str.GetLength() ) ) return FALSE;
	}
	
	strValue = Settings.Tricker.PhoneNum;
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		if ( _tcsncmp( str, xPhone, str.GetLength() ) == 0 )
		{
			if ( pxCaller ) *pxCaller = Settings.Tricker.TxCaller;
			return TRUE;
		}
	}
	
	return FALSE;
}

void CChannel::Clone(CChannel* pParent)
{
	CopyMemory( this->LocalId, pParent->LocalId, MAX_PHONE );
	CopyMemory( this->RemoteId, pParent->RemoteId, MAX_PHONE );
	CopyMemory( this->FeeNumber, pParent->FeeNumber, MAX_PHONE );
	
	CopyMemory( this->CallType, pParent->CallType, MAX_CALLTYPE );
	
	this->m_pOwner	= pParent->m_pOwner;
	this->m_nTaskId	= pParent->m_nTaskId;
//theApp.Message(MSG_ERROR,"debug: Clone(1)-> [%i][%s] -->[0x%x][0x%x] <==> [0x%x][0x%x]",
//			   m_nChan, m_xFeeCode, m_hInstance,m_hDllClosing,
//			   pParent->m_hInstance,pParent->m_hDllClosing);
	
	ASSERT( pParent->m_hDllClosing == NULL );
	if ( this->m_hInstance ) this->m_hDllClosing = pParent->m_hInstance;
	else this->m_hInstance	= pParent->m_hInstance;
	pParent->m_hInstance	= NULL;
//theApp.Message(MSG_ERROR,"debug: Clone(2)-> [%i][%s] -->[0x%x][0x%x] <==> [0x%x][0x%x]",
//		m_nChan, m_xFeeCode, m_hInstance,m_hDllClosing,
//		pParent->m_hInstance,pParent->m_hDllClosing);
	
	if ( pParent->m_hSocket != INVALID_SOCKET )
	{
		this->AttachTo( pParent );
	}
}

struct state* CChannel::ChkDTSignal(EVTBLK *evtblk,struct state* lpstDefaultState)
{
	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		switch (evtblk->dwParam & 0xFFFF)
		{
		case	S_CALL_UNAVAILABLE:
			return &bkFree;
		case	S_CALL_SS1WaitIdleCAS:
		case	S_TUP_WaitPcmReset:
		case	S_ISUP_WaitReset:
			return &ResetPcm;
		case	S_CALL_STANDBY:
			return &bkIdle;
		case	S_CALL_PENDING:
			return &bkOffHk;
		}
	}
	
	if ( evtblk->wEventCode == E_FEE_Login )
	{
		if ( (evtblk->dwParam & 0xFFFF) == S_FEE_Disconnected )
			return &bkOffHk;
	}
	
	return lpstDefaultState;
}

//////////////////////////////////////////////////////////////////////
// CChannel operations

int CChannel::PlayFile(LPCTSTR xFileName, LPCTSTR xFilePath, BOOL bTermDtmf)
{
	int nChan = m_nChan;

	SsmSetDtmfStopPlay( nChan, bTermDtmf );
	SsmClearRxDtmfBuf( nChan );

	CString strFile;
	
	if ( xFilePath == NULL )
		strFile.Format( _T("%s"), xFileName );
	else
	if ( _tcsistr( xFilePath, FT_PROMPT ) )
		strFile.Format( _T("%s\\%s\\%s"), Settings.General.Path, xFilePath, xFileName );
	else
		strFile.Format( _T("%s\\%s"), xFilePath, xFileName );

	theApp.Message( MSG_DEBUG, "Play[%d]: %s", nChan, (LPCTSTR)strFile );
	return SsmPlayFile( nChan, strFile.GetBuffer(0), 6, 0, 0xFFFFFFFF );
}

int CChannel::RecordFile(LPCTSTR xFileName, LPCTSTR xFilePath, LPCTSTR xTermDtmf)
{
	int nChan = m_nChan;

	SsmSetDTMFStopRecCharSet( nChan, (LPSTR)xTermDtmf );
	SsmClearRxDtmfBuf( nChan );

	CString strFile;
	
	if ( xFilePath == NULL )
		strFile.Format( _T("%s"), xFileName );
	else
		strFile.Format( _T("%s\\%s"), xFilePath, xFileName );

	theApp.Message( MSG_DEBUG, "Record[%d]: %s", nChan, (LPCTSTR)strFile );
	return SsmRecToFile( nChan, strFile.GetBuffer(0), -1, 0L, -1, 10, 0 );
}

int CChannel::RecordFax(LPCTSTR xFileName, LPCTSTR xFilePath)
{
	int nChan = m_nChan;

	CString strFile;
	
	if ( xFilePath == NULL )
		strFile.Format( _T("%s"), xFileName );
	else
		strFile.Format( _T("%s\\%s"), xFilePath, xFileName );

	theApp.Message( MSG_DEBUG, "RecordFax[%d]: %s", nChan, (LPCTSTR)strFile );
	return SsmFaxStartReceive( nChan, strFile.GetBuffer(0) );
}

int CChannel::SendFax(LPCTSTR xFileName, LPCTSTR xFilePath, BOOL bAppend)
{
	int nChan = m_nChan;

	CString strFile;
	
	if ( xFilePath == NULL )
		strFile.Format( _T("%s"), xFileName );
	else
		strFile.Format( _T("%s\\%s"), xFilePath, xFileName );

	theApp.Message( MSG_DEBUG, "SendFax[%d]: %s", nChan, (LPCTSTR)strFile );
	if ( bAppend ) return SsmFaxAppendSend( nChan, strFile.GetBuffer(0) );

	return SsmFaxStartSend( nChan, strFile.GetBuffer(0) );
}

int CChannel::CompDigital(LPCTSTR xDigital, BOOL bUnit)
{
	int nCount = 0;
	int nChan = m_nChan;
	
	CString strFile;
//	CString strPath = Settings.General.Path + _T("Media");

	LPCTSTR T_UNIT1[] = {"", "digital.wan.pcm", "digital.yi.pcm"};
	LPCTSTR T_UNIT2[] = {"", "digital.shi.pcm", "digital.bai.pcm", "digital.qian.pcm" };

	LPCTSTR ptr = xDigital;
	bool say = true;
	bool say2 = false;

	for ( LPCTSTR sptr = xDigital; (*sptr) != '.' && (*sptr) != '\0'; sptr ++ )
	{
		if ( (int)(sptr - xDigital) >= 12 ) break;
	}
	for ( int nLength = min(12, (int)(sptr - xDigital)); nLength > 0; nLength -- )
	{
		bool say3 = false;
		char ch = *(sptr - nLength);
		if ( ch < '0' || ch > '9' ) ch = '0';

		if ( bUnit == TRUE )
		{
			if ( ch == '0' )	// 默认'0'不发音
			{
				say = false; say2 = false;
			}
			else
			{
				if ( say2 == false && nCount ) // 判断前1位是否被忽略，则补'0'发音
				{
					strFile.Format( "%s\\digital.%c.pcm", FT_PROMPT, '0' );
					SsmAddToFileList( nChan, strFile.GetBuffer(0), 6, 0, 0xFFFFFFFF );
					MLOG( (LPCTSTR)strFile );
					nCount ++;
				}
				
				say = true;		// 恢复默认发音状态
				say2 = true;	// 要求发音“单位”

				if ( ch == '1' && nLength % 4 == 2 )// 省去"一十"里面的'一'发音
				{
					say = false;
				}
				if ( nLength % 4 == 1 )
				{
					say2 = false;	// 个位不发音
					if ( nLength > 4 )	say3 = true;	// 发音“千”或“万”
				}
			}
		}
		
		if ( say == true )
		{
			strFile.Format( "%s\\digital.%c.pcm", FT_PROMPT, ch );
			SsmAddToFileList( nChan, strFile.GetBuffer(0), 6, 0, 0xFFFFFFFF );
			MLOG( (LPCTSTR)strFile );
			nCount ++;
		}
		if ( say2 == true )
		{
			strFile.Format( "%s\\%s", FT_PROMPT, T_UNIT2[ (nLength - 1) % 4 ] );
			SsmAddToFileList( nChan, strFile.GetBuffer(0), 6, 0, 0xFFFFFFFF );
			MLOG( (LPCTSTR)strFile );
			nCount ++;
		}
		if ( say3 == true )
		{
			strFile.Format( "%s\\%s", FT_PROMPT, T_UNIT1[ (nLength - 1) / 4 ] );
			SsmAddToFileList( nChan, strFile.GetBuffer(0), 6, 0, 0xFFFFFFFF );
			MLOG( (LPCTSTR)strFile );
			nCount ++;
			say2 = true;	// 置单位发音为真
		}
	}
	if ( bUnit == TRUE && nCount == 0 )//整数位长度等于0，补发'0'
	{
		strFile.Format( "%s\\digital.%c.pcm", FT_PROMPT, '0' );
		SsmAddToFileList( nChan, strFile.GetBuffer(0), 6, 0, 0xFFFFFFFF );
		MLOG( (LPCTSTR)strFile );
		nCount ++;
	}
	for ( ; (*sptr) != '\0'; sptr ++ )
	{
		char ch = *sptr;
		
		if ( ch == '.' )
			strFile.Format( "%s\\digital.dot.pcm", FT_PROMPT );
		else
		{
			if ( ch < '0' || ch > '9' ) ch = '0';
			strFile.Format( "%s\\digital.%c.pcm", FT_PROMPT, ch );
		}

		SsmAddToFileList( nChan, strFile.GetBuffer(0), 6, 0, 0xFFFFFFFF );
		MLOG( (LPCTSTR)strFile );
		nCount ++;
	}

	return nCount;
}

int CChannel::ReadDtmf(LPTSTR pBuffer, CHAR cp)
{
	int nLength = SsmGetDtmfStr( m_nChan, pBuffer );
	if ( nLength >= 0 ) pBuffer[nLength] = '\0';

	for ( int i = 0; i < nLength; i++ )
	{
		if ( pBuffer[i] == cp )
		{
			pBuffer[i] = '\0'; return i;
		}
	}
	
	return nLength;
}

int CChannel::GetNextBuffer(LPCTSTR xSrc, LPCTSTR& xDst, CHAR cp)
{
	if ( (*xSrc) == '\0' || (*xSrc) == cp ) return ( (xDst = xSrc) - xSrc );
	
	return 1 + GetNextBuffer(xSrc + 1, (xDst = xSrc), cp);
}

//////////////////////////////////////////////////////////////////////
// CChannel packet unserialize

CPacket* CChannel::ReadBuffer(CBuffer* pBuffer)
{
	CString strLine;
	ASSERT( pBuffer != NULL );
	
	if ( ! pBuffer->ReadLine( strLine, TRUE ) ) return NULL;
	
	CPacket* pPacket = CPacketCore::ReadBuffer( pBuffer->m_pBuffer, pBuffer->m_nLength );
	int nLength = strLine.GetLength();
	
	if ( pPacket == NULL )
	{
		pBuffer->Remove( nLength + 1 + (pBuffer->m_pBuffer[nLength] == '\r') ); // + [\r] \n"
		SendPacket( new CErrPacket(ECore_Unknown) ); return NULL;
	}
	else
	if ( pPacket->CommandId != NULL )
	{
		nLength = pPacket->GetBufferSize();
		pBuffer->Remove( nLength + 1 + (pBuffer->m_pBuffer[nLength] == '\r') ); // + [\r] \n"
		return pPacket;
	}
	
	return pPacket;
}

//////////////////////////////////////////////////////////////////////
// CChannel run function

BOOL CChannel::OnRun()
{
	if ( TimerOut() )
	{
		SsmPutUserEvent( E_SYS_TIMEOUT, m_nChan, 0 );
	}
	
	if ( m_hDllClosing )
	{
//theApp.Message(MSG_TEMP,"debug: FreeLibrary-> [%i][%s] -->0x%x", m_nChan, m_xFeeCode, m_hDllClosing);
		FreeLibrary( m_hDllClosing ); m_hDllClosing = NULL;
	}
	
	return CTransfer::OnRun();
}

//////////////////////////////////////////////////////////////////////
// CChannel socket event handlers

BOOL CChannel::OnConnected()
{
	CBindPacket* pPacket = new CBindPacket;
	
	pPacket->SetLocalId( LocalId );
	pPacket->SetRemoteId( RemoteId );
	pPacket->SetFeeNumber( FeeNumber );
	
	pPacket->SetCallType( CallType );
	
	SendPacket( pPacket );
	
	return TRUE;
}

BOOL CChannel::OnPacket(CPacket* pPacket)
{
	if ( pPacket->IsCommandId( ICore_BindLink ) )
	{
		return OnBindPacket( (CBindPacket*)pPacket );
	}
	if ( pPacket->IsCommandId( ICore_DialRemote ) )
	{
		return OnDialPacket( (CDialPacket*)pPacket );
	}
	if ( pPacket->IsCommandId( ICore_RecordFile ) )
	{
		return OnRecordPacket( (CRecordPacket*)pPacket );
	}
	if ( pPacket->IsCommandId( ICore_Listen ) )
	{
		return OnListenPacket( (CListenPacket*)pPacket );
	}
	if ( pPacket->IsCommandId( ICore_HaltLink ) )
	{
		return OnHaltPacket( (CHaltPacket*)pPacket );
	}
	
	return CTransfer::OnPacket( pPacket );
}

BOOL CChannel::OnBindPacket(CBindPacket* pPacket)
{
	CTransfer::OnConnected();
	CString xFeeCode;
	
	int nLength = pPacket->GetFeeCode( xFeeCode.GetBuffer(4) );
	xFeeCode.ReleaseBuffer( nLength );
	
	if ( CChannel* pChannel = Network.MakeChannel( m_nChan, xFeeCode ) )
	{
		pChannel->Clone( this ); Network.SetChannel( pChannel );
		
		if ( _tcslen(pChannel->FeeNumber) )
		theApp.Message( MSG_DEFAULT, "Rcv[%i][%s]: Local=[%s], Remote=[%s], Fee=[%s]",
			pChannel->m_nChan, pChannel->m_xFeeCode, pChannel->LocalId, pChannel->RemoteId, pChannel->FeeNumber );
		else
		theApp.Message( MSG_DEFAULT, "Rcv[%i][%s]: Local=[%s], Remote=[%s]",
			pChannel->m_nChan, pChannel->m_xFeeCode, pChannel->LocalId, pChannel->RemoteId );
		
		SsmPutUserEvent( E_FEE_Login, pChannel->m_nChan, S_FEE_Connected );
	}
	else
	{
		theApp.Message( MSG_ERROR, "N/A[%i][%s]: Local=[%s], Remote=[%s], Fee=[%s]",
			m_nChan, m_xFeeCode, LocalId, RemoteId, FeeNumber );
		
		SsmPutUserEvent( E_FEE_Login, m_nChan,
			MAKEVENT(ECore_Unknown & 0xFFFF, S_FEE_Disconnected) );
	}
	
	return TRUE;
}

BOOL CChannel::OnDialPacket(CDialPacket* pPacket)
{
	LocalId[ pPacket->GetLocalId( LocalId ) ] = '\0';
	RemoteId[ pPacket->GetRemoteId( RemoteId ) ] = '\0';
	FeeNumber[ pPacket->GetFeeNumber( FeeNumber ) ] = '\0';
	
	CHAR xCallerId[MAX_PHONE + 1];
	xCallerId[ pPacket->GetCallerId( xCallerId ) ] = '\0';
	
	CString xNewCaller;
	if ( xCallerId[0] != '0' && IsPoseNumber( RemoteId, xCallerId, &xNewCaller ) )
	{
		int nLength = xNewCaller.GetLength();
		nLength = min( nLength, MAX_PHONE );
		MoveMemory( xCallerId + nLength, xCallerId, MAX_PHONE - nLength );
		CopyMemory( xCallerId, xNewCaller.GetBuffer(0), nLength );
		theApp.Message(MSG_TEMP, "Pose[%i]: set callerid to [%s]", m_nChan, xCallerId);
	}
	
	if ( IsTestNumber( RemoteId, &xNewCaller ) )
	{
		ZeroMemory( xCallerId, MAX_PHONE + 1 );
		int nLength = xNewCaller.GetLength();
		nLength = min( nLength, MAX_PHONE );
		CopyMemory( xCallerId, xNewCaller.GetBuffer(0), nLength );
		theApp.Message(MSG_TEMP, "Test[%i]: set callerid to [%s]", m_nChan, xCallerId);
	}
	
	SsmSetTxCallerId( m_nChan, xCallerId );
	CopyMemory( CallType, CA_HOOK, MAX_CALLTYPE );
	return TRUE;
}

BOOL CChannel::OnRecordPacket(CRecordPacket* pPacket)
{
	ASSERT( pPacket->GetChannel() == m_nChan );
	SsmStopRecToFile( m_nChan );
	
	CHAR xMsgType[MAX_BUFFER];
	xMsgType[ pPacket->GetMsgType(xMsgType) ] = '\0';
	
	if ( ! _tcscmp(xMsgType, "STOP") ) return TRUE;
	
	CString xNewFile = Settings.Trunk.RecFile;
	
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
	
	for ( LPCTSTR xPath = xNewFile, xPathExt = NULL;
		  xPathExt = _tcsistr(xPath, "\\"); xPath = xPathExt + 1 )
	{
		CString strValue = xNewFile.Left(xPathExt - (LPCTSTR)xNewFile);
		CreateDirectory( Settings.General.PathRec + "\\" + strValue, NULL );
	}
	
	SsmSetRecMixer( m_nChan, TRUE, 0 );
	RecordFile( xNewFile, Settings.General.PathRec );
	theApp.Message( MSG_TEMP,"Record[%i] -> %s", m_nChan, xNewFile );
	return TRUE;
}

BOOL CChannel::OnListenPacket(CListenPacket* pPacket)
{
	SsmStopLinkFrom( m_nTaskId, m_nChan );
	SsmSetFlag( m_nTaskId, 10, FALSE);
	
	CHAR xMsgType[MAX_BUFFER];
	xMsgType[ pPacket->GetMsgType(xMsgType) ] = '\0';
	
	if ( ! _tcscmp(xMsgType, "STOP") ) return TRUE;
	SsmStopSendTone( m_nChan );
	
	m_nTaskId = pPacket->GetChannel();
	
	SsmLinkFrom( m_nTaskId, m_nChan );
	SsmSetFlag( m_nTaskId, 10, TRUE);
	
	theApp.Message(MSG_DEBUG,"SsmLinkFrom: %i with %i", m_nTaskId, m_nChan);
	return TRUE;
}

BOOL CChannel::OnErrPacket(CErrPacket* pPacket)
{
	theApp.Message( MSG_ERROR, "Err[%i][%i]: Local=[%s], Remote=[%s], Fee=[%s]",
		m_nChan, pPacket->m_nErrCode, LocalId, RemoteId, FeeNumber );
	
#ifdef _DEBUG
	SsmPutUserEvent( E_FEE_Login, m_nChan, S_FEE_Connected );
#else
	SsmPutUserEvent( E_FEE_Login, m_nChan,
		MAKEVENT(pPacket->m_nErrCode & 0xFFFF, S_FEE_Disconnected) );
#endif
	
	CTransfer::Disconnect();
	return TRUE;
}

BOOL CChannel::OnHaltPacket(CHaltPacket* pPacket)
{
	ASSERT( pPacket->LinkId == m_nChan );
	
	Disconnect(); StopChan();
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CChannel disconnect

void CChannel::Disconnect()
{
	if ( m_hSocket != INVALID_SOCKET )
	SsmPutUserEvent( E_FEE_Login, m_nChan, S_FEE_Disconnected );
	
	CTransfer::Disconnect();
}

//////////////////////////////////////////////////////////////////////
// CChannel process func

int CChannel::bkIdle_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	CChannel* pChannel = new CChannel( nChan );
	pChannel->Clone( this ); Network.SetChannel( pChannel );
	
	pChannel->Setup(); return 0;
}

struct state* CChannel::bkIdle_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		switch (evtblk->dwParam & 0xFFFF)
		{
		case	S_CALL_PENDING:
			return &seiConfm;
		case	S_CALL_PICKUPED:
		case	S_CALL_LOCKED:
			return &bkOnHk;
		}
	}
	if ( evtblk->wEventCode == E_CHG_CIDExBuf ||
		 evtblk->wEventCode == E_CHG_RxPhoNumBuf )
	{
		if ( ! Network.VerifyIncoming( nChan ) ) return &bkOffHk;
		return &seiConfm;
	}
	
	if ( evtblk->wEventCode == E_FEE_Login )
	{
		if ( (evtblk->dwParam & 0xFFFF) == S_FEE_Disconnected )
			return &bkIdle;
	}
	
	return ChkDTSignal( evtblk, &bkIdle );
}

int CChannel::bkFree_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	Disconnect(); StopChan();
	
	return 0;
}

struct state* CChannel::bkFree_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		switch (evtblk->dwParam & 0xFFFF)
		{
		case	S_CALL_SS1WaitIdleCAS:
		case	S_TUP_WaitPcmReset:
		case	S_ISUP_WaitReset:
			return &ResetPcm;
		}
	}
	
	return &ResetPcm;
}

int CChannel::bkOnHk_init(EVTBLK *evtblk)
{
	fcnp = &FeeLogin; Reserved = 0;
	
	return (this->*fcnp->initiate)(evtblk);
}

struct state* CChannel::bkOnHk_cmplt(EVTBLK *evtblk)
{
	return &WaitDial;
}

int CChannel::bkOffHk_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	Disconnect(); StopChan();
	
	return 0;
}

struct state* CChannel::bkOffHk_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_STANDBY )
		{
			if ( ExitCode == EXIT_CALLBACK ) Traffic.AddUsrBack( LocalId, RemoteId );
			SsmStopSendTone( nChan ); return &bkIdle;
		}
	}
	
	return ChkDTSignal( evtblk, &bkOffHk );
}

int CChannel::ResetPcm_init(EVTBLK *evtblk)
{
	return (this->*bkOffHk.initiate)(evtblk);
}

struct state* CChannel::ResetPcm_cmplt(EVTBLK *evtblk)
{
	return ChkDTSignal( evtblk, &ResetPcm );
}

int CChannel::seiConfm_init(EVTBLK *evtblk)
{
	LoadPhonNumber(); LoadCallerId();
	
	SetTimer( 1500 ); return 0;
}

struct state* CChannel::seiConfm_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_SYS_TIMEOUT )
	{
		CopyMemory( CallType, CA_TALK, MAX_CALLTYPE );
		Reserved = 0; return &FeeLogin;
	}
	if ( evtblk->wEventCode == E_CHG_ChState &&
		(evtblk->dwParam & 0xFFFF) == S_CALL_PENDING )
	{
		Reserved = 0; return &seiConfm;
	}
#if 0
	if ( evtblk->wEventCode == E_CHG_ChState &&
		(evtblk->dwParam & 0xFFFF) == S_TUP_WaitCLF )
	{
theApp.Message(MSG_TEMP,"debug[%i]: CallBack[%s] -> [%s]", nChan, LocalId, RemoteId);
		Traffic.AddUsrBack(LocalId, RemoteId);
	}
#endif
	
	return ChkDTSignal( evtblk, &seiConfm );
}

int CChannel::FeeLogin_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	SetTimer( Settings.Master.TimeOut * 1000 );
	ConnectTo( Settings.Master.Host, Settings.Master.Port );
	
	return 0;
}

struct state* CChannel::FeeLogin_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_RINGING )
		{
			KillTimer(); Reserved &= 0; return &rcvMFsig;
		}
	}
	
	if ( evtblk->wEventCode == E_FEE_Login )
	{
		if ( (evtblk->dwParam & 0xFFFF) == S_FEE_Connected )
		{
#ifdef NPICKUP
	SsmPutUserEvent( E_CHG_ChState, nChan, S_CALL_RINGING );
#endif
			SsmSetKB( nChan, IsTestNumber(RemoteId) ? 7 : 1 ); 
			KillTimer(); return &FeeLogin;
		}
		
		if ( (evtblk->dwParam & 0xFFFF) == S_FEE_Disconnected )
		{
			KillTimer(); Reserved &= 0;
			
			if ( (evtblk->dwParam >> 16) == ECore_NoPay || (evtblk->dwParam >> 16) == ECore_NoUser )
			{
				SsmSetKB( nChan, 1 );	// 计费
				ExitCode = EXIT_NOPAY; return &ExitPlay;
			}
			else
			if ( (evtblk->dwParam >> 16) == ECore_CallBack )
			{
				SsmSetKB( nChan, 1 );	// 计费
				ExitCode = EXIT_CALLBACK; return &ExitPlay;
			}
			else
			{
				SsmSetKB( nChan, 4 ); return &bkOffHk;
			}
		}
	}
	
	if ( evtblk->wEventCode == E_SYS_TIMEOUT )
	{
		SsmSetKB( nChan, 4 ); Reserved &= 0;
		CTransfer::Disconnect(); return &bkOffHk;
	}
	
	return ChkDTSignal( evtblk, &FeeLogin );
}

int CChannel::rcvMFsig_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	
	return SsmPickup( nChan );
}

struct state* CChannel::rcvMFsig_cmplt(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;

	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_TALKING )
		{
			SsmStopSendTone( nChan );
			Reserved &= 0; return &RcvTalk;
		}
	}
	
	return ChkDTSignal( evtblk, &rcvMFsig );
}

int CChannel::RcvTalk_init(EVTBLK *evtblk)
{
	SendPacket( new CTalkPacket );
	
	CTransfer::OnConnected();
	return 0;
}

struct state* CChannel::RcvTalk_cmplt(EVTBLK *evtblk)
{
	return ChkDTSignal( evtblk, &RcvTalk );
}

int CChannel::WaitDial_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	Reserved |= 0x08;	// 置状态“正在”

#if 1
	// 移除局方特殊符号
	for ( WORD i = 0; i < _tcslen(LocalId); i++ )
	{
		if ( ! isdigit(LocalId[i]) ) LocalId[i] = '\0';
	}
#endif
	
#if 1	
	// 移除被叫方0574
	if ( ! _tcsncmp( RemoteId, "0574", 4 ) )
	{
		CHAR xRemoteId[MAX_PHONE + 1]; ZeroMemory(xRemoteId, MAX_PHONE + 1);
		CopyMemory( xRemoteId, RemoteId + 4, MAX_PHONE - 4 );
		return SsmAutoDial( nChan, xRemoteId );
	}
#endif
		
	SetTimer( Settings.Trunk.TimeOut * 1000 );
	return SsmAutoDial( nChan, RemoteId );
}

struct state* CChannel::WaitDial_cmplt(EVTBLK *evtblk)
{
/*
	if ( evtblk->wEventCode == E_PROC_AutoDial )
	{
		if ( (evtblk->dwParam & 0xFFFF) == DIAL_ECHOTONE )
		{
			SetTimer( 30 * 1000 );
		}
	}
*/
	if ( evtblk->wEventCode == E_SYS_TIMEOUT )
	{
		Reserved &= 0; return &bkOffHk;
	}
	
	if ( evtblk->wEventCode == E_CHG_ChState )
	{
		if ( (evtblk->dwParam & 0xFFFF) == S_CALL_TALKING )
		{
			KillTimer(); Reserved &= 0; return &RcvTalk;
		}
	}
	
	return ChkDTSignal( evtblk, &WaitDial );
}

int CChannel::ExitPlay_init(EVTBLK *evtblk)
{
	int nChan = evtblk->nReference;
	if ( Reserved & 0x08 ) return 0;
	
	Reserved |= 0x08;	// 置状态“正在”
	
	switch ( ExitCode )
	{
	case EXIT_GOODBYE:	// 谢谢使用
		return PlayFile( FILE_PROMPT_GOODBYE, FT_PROMPT, FALSE );
		
	case EXIT_NOTUSER:	// 无此号码
		return PlayFile( FILE_PROMPT_NOTUSER, FT_PROMPT, FALSE );
		
	case EXIT_NOPAY:	// 无权呼叫
		return PlayFile( FILE_PROMPT_NOPAY, FT_PROMPT, FALSE );
		
	case EXIT_BUSY:		// 系统繁忙
		return PlayFile( FILE_PROMPT_BUSY, FT_PROMPT, FALSE );
		
	case EXIT_NORECD:	// 无此记录
		return PlayFile( FILE_PROMPT_NORECD, FT_PROMPT, FALSE );
		
	case EXIT_CALLBACK:	// 回拨提醒
		return PlayFile( FILE_PROMPT_CALLBACK, FT_PROMPT, FALSE );
		
	default:
		return (this->*bkOffHk.initiate)(evtblk);
	}
	
	return 0;
}

struct state* CChannel::ExitPlay_cmplt(EVTBLK *evtblk)
{
	if ( evtblk->wEventCode == E_PROC_PlayEnd )
	{
#if 0
		if ( ExitCode == EXIT_CALLBACK )
		{
			SsmPickup( m_nChan ); Sleep( 250 );
		}
#endif
		
		Reserved &= 0; return &bkOffHk;
	}
	
	return ChkDTSignal( evtblk, &ExitPlay );
}
