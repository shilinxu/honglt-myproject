/*
 * dllmain.cxx
 *
 * DLL main entry point for Xuserface.dll
 *
 */

#include "stdafx.h"

#include "Network.h"
#include "Datagrams.h"
#include "RenderGraph.h"
#include "XML.h"

BOOL APIENTRY DllMain (HANDLE, DWORD, LPVOID)
{
    return TRUE;
}

int ParseMatch(CXMLElement* pXML, LPCTSTR pszMethod);

CCriticalSection m_pSection;

//////////////////////////////////////////////////////////////////////////
// SOCKET connection

int WINAPI XConnect(LPCTSTR pAddress, int nPort)
{
	XDisconnect();
	Network.ConnectTo(pAddress, nPort);
	
	DWORD nResult = Network.CallWorker( NULL );
	if ( nResult ) return nResult;
	
	// Datagrams
	
	DWORD nAddress = htonl( INADDR_ANY );
	CDatagrams* pDatagrams = &Datagrams;
	
	SOCKADDR_IN	pHost;
	CopyMemory( &pHost, &Network.m_pHost, sizeof(pHost) );
	
	pDatagrams->m_nType	= Payload_Alaw;
	pDatagrams->Connect( (IN_ADDR*)&nAddress, nPort );
	
	pDatagrams = pDatagrams->Attach( new CDatagrams(Payload_H263) );
	pDatagrams->Connect( (IN_ADDR*)&nAddress, nPort + 1 );
	
	return 0;
}

void WINAPI XDisconnect()
{
	XResetRender();
	Network.Disconnect( 0 );
	Datagrams.Release();
}

//////////////////////////////////////////////////////////////////////////
// User's operations

int WINAPI XLoginHost(LPCTSTR xUserId, LPCTSTR xPassword, BOOL bTestor)
{
	CSingleLock pLock( &Network.m_pSection, TRUE );
	
	CXMLElement* pXML = new CXMLElement( NULL, ISyncOrderRelation );
	pXML->AddAttribute( "method", "login" );
	
	if ( bTestor ) pXML->AddAttribute( "operator" );
	
	CXMLElement* pSync = pXML->AddElement( "UserId" );
	pSync->SetValue( xUserId );
	
	CString strValue;
	int nLength	= CalcHashBytes( strValue.GetBuffer(32), xPassword );
	strValue.ReleaseBuffer( nLength );
	
	pSync = pXML->AddElement( "Hashing" );
	pSync->SetValue( strValue );
	
	Network.SendPacket( new CSyncOrderRelation(pXML) );
	delete pXML; pXML = NULL;
	
	Network.SetUserId( xUserId );
	pLock.Unlock();
	
	int nResult = Network.CallWorker( &pXML );
	if ( ! nResult ) nResult = ParseMatch( pXML, "Welcome" );
	
	pXML->Delete();
	if ( nResult ) {XDisconnect(); return nResult;}
	
	// Datagrams
	
	SOCKADDR_IN	pHost;
	CopyMemory( &pHost, &Network.m_pHost, sizeof(pHost) );
	
	CDatagrams* pDatagram = &Datagrams;
	pHost.sin_port = pDatagram->m_pHost.sin_port;
	pDatagram->JoinMirror( &pHost, Network.m_xUserId );
	
	pDatagram = pDatagram->m_pNext;
	pHost.sin_port = pDatagram->m_pHost.sin_port;
	pDatagram->JoinMirror( &pHost, Network.m_xUserId );
	
	return nResult;
}

int ParseMatch(CXMLElement* pXML, LPCTSTR pszMethod)
{
	CString strValue = pXML->GetAttributeValue("method");
	
	if ( ! strValue.CompareNoCase(pszMethod) ) return 0;
	
#if 1
	if ( ! pXML->IsNamed("FaultResp") )
	{
		strValue = pXML->GetAttributeValue("faultcode");
		return _ttoi( strValue );
	}
#endif
	
	return EBase_Unknown;
}

LONG WINAPI XCallWorker(LPTSTR string, DWORD nTimeOut)
{
	CXMLElement* pXML;
	LONG nResult = Network.CallWorker(&pXML, nTimeOut);
	
	if ( nResult || ! string ) return nResult;
	
	CString strXML = pXML->ToString( FALSE, TRUE );
	_tcscpy( string, (LPCTSTR)strXML );
	
	pXML->Delete();
	return nResult;
}

//////////////////////////////////////////////////////////////////////////
// Directshow operations

CGraphRender* m_pCapture = NULL;
CGraphRender* m_pRenderer = NULL;

HRESULT WINAPI XStartPreview(const RECT& rect, HWND hWnd, LPCTSTR lpszName)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	CGraphRender* pRenderer = m_pRenderer;
	if ( ! lpszName ) pRenderer = m_pCapture;
	
	if ( ! pRenderer ) return E_FAIL;
	return pRenderer->StartPreview(rect, hWnd);
}

HRESULT WINAPI XRenderPreview(WORD nPayloadMask, LPCTSTR lpszName)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	CGraphRender*& pRenderer = m_pRenderer;
	if ( ! pRenderer ) pRenderer = new CGraphRender( lpszName );
	
	CDatagrams* pDatagram = Datagrams.SeekNext(Payload_Alaw);
	if ( RenderPayload(nPayloadMask) & PayloadMask_Video ) pDatagram = Datagrams.SeekNext(Payload_H263);
	
	if ( PreviewPayload(0) & nPayloadMask ) pRenderer->RenderPreview( nPayloadMask, NULL, (DWORD)pDatagram );
	else ASSERT( FALSE );
	
	return S_OK;
}

HRESULT WINAPI XRenderStream(WORD nPayloadMask, LPCTSTR lpszName)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	CGraphRender*& pRenderer = m_pCapture;
	if ( ! pRenderer ) pRenderer = new CGraphRender( lpszName ); 
	
	CDatagrams* pDatagram = Datagrams.SeekNext(Payload_Alaw);;
	if ( RenderPayload(nPayloadMask) & PayloadMask_Video ) pDatagram = Datagrams.SeekNext(Payload_H263);
	
	if ( RenderPayload(0) & nPayloadMask ) pRenderer->RenderStream( nPayloadMask, NULL, (DWORD)pDatagram );
	if ( PreviewPayload(0) & nPayloadMask ) pRenderer->RenderPreview( nPayloadMask );
	
	return S_OK;
}

void WINAPI XResetRender()
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	if ( m_pRenderer ) delete m_pRenderer;
	m_pRenderer	= NULL;
	
	if ( m_pCapture ) delete m_pCapture;
	m_pCapture	= NULL;
}

//////////////////////////////////////////////////////////////////////////
// Chat operations

HRESULT WINAPI XDialupWith(LPCTSTR xRemoteId, LPCTSTR xCallerId)
{
	CXMLElement* pXML = new CXMLElement( NULL, ISyncHostFile );
	pXML->AddAttribute( "method", "dialup" );
	
	CXMLElement* pSync = pXML->AddElement( "RemoteId" );
	pSync->SetValue( xRemoteId );
	
	if ( xCallerId && _tcslen(xCallerId) )
	{
		pSync = pXML->AddElement( "CallerId" );
		pSync->SetValue( xCallerId );
	}
	
	BOOL bResult = Network.DialupWith( pXML );
	pXML->Delete();
	
	if ( ! bResult ) return S_FALSE;
	//	XRenderPreview(PreviewPayload(PayloadMask_Video|PayloadMask_Audio), Network.m_xUserId);
	XRenderPreview(PreviewPayload(PayloadMask_Audio), Network.m_xUserId);
	
	return S_OK;
}

HRESULT WINAPI XLeaveTalk()
{
	CSingleLock pLock( &m_pSection, TRUE );

	if ( m_pRenderer ) delete m_pRenderer;
	m_pRenderer	= NULL;
	
	Network.LeaveTalk();
	return S_OK;
}

HRESULT WINAPI XRecordFile()
{
	if ( Network.OnRecordFile() ) return S_OK;
	return S_FALSE;
}

void WINAPI XStartTalk()
{
	Network.StartTalking();
}

//////////////////////////////////////////////////////////////////////////
// Debug operations

TCHAR szMessageBuffer[16384];

void WINAPI XDebugLog(int nType, LPCTSTR pszFormat, ...)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	va_list pArgs;
	
	va_start( pArgs, pszFormat );
	_vsntprintf( szMessageBuffer, 16380, pszFormat, pArgs );
	
	OutputDebugString( szMessageBuffer );
	va_end( pArgs );
}

DWORD WINAPI XGetLastError()
{
	return Network.m_nLastError;
}

//////////////////////////////////////////////////////////////////////
// String conversion

CString ToHex(BYTE* pBuffer, DWORD nLength)
{
	LPCTSTR pszHex = _T("0123456789ABCDEF");
	CString strDump;
	
	LPTSTR pszDump = strDump.GetBuffer( nLength * 3 );
	
	for ( DWORD i = 0 ; i < nLength ; i++ )
	{
		int nChar = pBuffer[i];
		if ( i ) *pszDump++ = ' ';
		*pszDump++ = pszHex[ nChar >> 4 ];
		*pszDump++ = pszHex[ nChar & 0x0F ];
	}
	
	*pszDump = 0;
	strDump.ReleaseBuffer();
	
	return strDump;
}

CString ToASCII(BYTE* pBuffer, DWORD nLength)
{
	CString strDump;
	
	LPTSTR pszDump = strDump.GetBuffer( nLength + 1 );
	
	for ( DWORD i = 0 ; i < nLength ; i++ )
	{
		int nChar = pBuffer[i];
		*pszDump++ = ( nChar >= 32 ? nChar : '.' );
	}
	
	*pszDump = 0;
	strDump.ReleaseBuffer();
	
	return strDump;
}

/////////////////////////////////////////////////////////////////////////////
// Case independent string search

LPCTSTR _tcsistr(LPCTSTR pszString, LPCTSTR pszPattern)
{
	LPCTSTR pptr, sptr, start;
	DWORD slen, plen;

	for (	start	= pszString,
			pptr	= pszPattern,
			slen	= _tcslen( pszString ),
			plen	= _tcslen( pszPattern ) ;
			slen >= plen ; start++, slen-- )
	{
		while ( toupper( *start ) != toupper( *pszPattern ) )
		{
			start++;
			slen--;

			if ( slen < plen ) return NULL;
		}

		sptr = start;
		pptr = pszPattern;

		while ( toupper( *sptr ) == toupper( *pptr ) )
		{
			sptr++;
			pptr++;

			if ( ! *pptr) return start;
		}
	}

	return NULL;
}

LPCTSTR _tcsnistr(LPCTSTR pszString, LPCTSTR pszPattern, DWORD plen)
{
	LPCTSTR pptr, sptr, start;
	DWORD slen, plen2;

	for (	start	= pszString,
			pptr	= pszPattern,
			slen	= _tcslen( pszString ) ;
			slen >= plen ; start++, slen-- )
	{
		while ( toupper( *start ) != toupper( *pszPattern ) )
		{
			start++;
			slen--;

			if ( slen < plen ) return NULL;
		}

		sptr = start;
		pptr = pszPattern;
		plen2 = plen;

		while ( toupper( *sptr ) == toupper( *pptr ) )
		{
			sptr++;
			pptr++;

			if ( ! --plen2 ) return start;
		}
	}

	return NULL;
}
