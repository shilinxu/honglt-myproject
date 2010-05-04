//
// PacketBase.cpp
//

#include "stdafx.h"
#include "PacketBase.h"
#include "Buffer.h"
#include "md5.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CPacketBase construction

CPacketBase::CPacketBase(LPCTSTR sCommandId) : CPacket(sCommandId)
{
	m_pXML	= NULL;
}

CPacketBase::~CPacketBase()
{
	if ( m_pXML ) delete m_pXML;
}

//////////////////////////////////////////////////////////////////////
// CPacketBase new packet from buffer

CPacket* CPacketBase::ReadBuffer(LPBYTE pBuffer, DWORD nLength)
{
	CHAR sCommandId[10];
	CPacket::ReadField( pBuffer, sCommandId, 10 );
	
	CPacket* pPacket = NULL;
	
	if ( ! _tcscmp(sCommandId, IBase_SyncOrderRelation) )
	{
		pPacket->Release(); pPacket = new CSyncOrderRelation;
	}
	if ( ! _tcscmp(sCommandId, IBase_SyncDataView) )
	{
		pPacket->Release(); pPacket = new CSyncDataView;
	}
	if ( ! _tcscmp(sCommandId, IBase_SyncHostFile) )
	{
		pPacket->Release(); pPacket = new CSyncHostFile;
	}
	if ( ! _tcscmp(sCommandId, IBase_FaultResp) )
	{
		pPacket->Release(); pPacket = new CFaultResp;
	}
	
	if ( ! pPacket ) return NULL;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;
	
	pPacket->Release();
	return NULL;
}

CPacket* CPacketBase::Clone()
{
	CPacketBase* pPacket = new CPacketBase( CommandId );
	
	if ( m_pXML ) pPacket->m_pXML = m_pXML->Clone();
	return pPacket;
}

//////////////////////////////////////////////////////////////////////
// CPacketBase operations

int CPacketBase::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	DWORD nRead = CPacket::LoadPacket( pBuffer, nLength );
	if ( nRead == -1 ) return -1;
	
	CHAR szTemp[16];
	BYTE* ptr = pBuffer + nRead;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, szTemp, 16 );
	nRead += nField; ptr += nField;
	
	ASSERT( m_pXML == NULL );
	DWORD nXMLLength = _ttoi( szTemp );
	
	if ( nLength - nRead < 2 + nXMLLength )
	{
		return BufferSize = 0;
	}
	
	CXMLElement* pXML = CXMLElement::FromBytes( pBuffer + nRead + 2, nXMLLength, FALSE );
	m_pXML = ( pXML ) ? LoadBody( pXML ) : NULL;
	
	if ( ! m_pXML && pXML ) delete pXML;

	BufferSize = nRead + 2 + nXMLLength;
	return BufferSize;
}

int CPacketBase::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( CommandId ) );
	DWORD nLength = pBuffer->m_nLength;
	
	ASSERT( m_pXML != NULL );
	EncodeBody( m_pXML );
	
	CString strXML = m_pXML->ToString( FALSE, TRUE );
	
	CString strValue;
	strValue.Format( "%s %i\r\n", CommandId, strXML.GetLength() );
	
	pBuffer->Print( (LPCTSTR)strValue );
	
	pBuffer->Print( (LPCTSTR)strXML );
	pBuffer->Print( "\r\n" );
	
	return pBuffer->m_nLength - nLength;
}

CXMLElement* CPacketBase::LoadBody(CXMLElement* pXML)
{
	ASSERT( pXML != NULL );
	
	return pXML;
}

CXMLElement* CPacketBase::EncodeBody(CXMLElement* pXML)
{
	ASSERT( pXML != NULL );
	
	return pXML;
}

//////////////////////////////////////////////////////////////////////
// CSyncOrderRelation construction

CSyncOrderRelation::CSyncOrderRelation(CXMLElement* pXML) : CPacketBase(IBase_SyncOrderRelation)
{
	m_pXML = pXML ? pXML->Clone() : NULL;
	ASSERT( ! m_pXML || m_pXML->IsNamed("SyncOrderRelation") );
}

CSyncOrderRelation::~CSyncOrderRelation()
{
}

//////////////////////////////////////////////////////////////////////
// CSyncOrderRelation operations

CXMLElement* CSyncOrderRelation::LoadBody(CXMLElement* pXML)
{
	if ( ! pXML->IsNamed(ISyncOrderRelation) ) return NULL;
	
	return pXML;
}

CXMLElement* CSyncOrderRelation::EncodeBody(CXMLElement* pXML)
{
	return CPacketBase::EncodeBody(pXML);
}

//////////////////////////////////////////////////////////////////////
// CSyncHostFile construction

CSyncHostFile::CSyncHostFile(CXMLElement* pXML) : CPacketBase(IBase_SyncHostFile)
{
	m_pXML = pXML ? pXML->Clone() : NULL;
	ASSERT( ! m_pXML || m_pXML->IsNamed(ISyncHostFile) );
}

CSyncHostFile::~CSyncHostFile()
{
}

//////////////////////////////////////////////////////////////////////
// CSyncDataView construction

CSyncDataView::CSyncDataView(CXMLElement* pXML) : CPacketBase(IBase_SyncDataView)
{
	m_pXML = pXML ? pXML->Clone() : NULL;
	ASSERT( ! m_pXML || m_pXML->IsNamed(ISyncDataView) );
}

CSyncDataView::~CSyncDataView()
{
}

//////////////////////////////////////////////////////////////////////
// CFaultResp

CFaultResp::CFaultResp(DWORD faultCode) : CPacketBase(IBase_FaultResp)
{
	if ( ! faultCode ) return;
	
	ASSERT( m_pXML == NULL );
	m_pXML = new CXMLElement( NULL, "FaultResp" );
	
	CString strValue;
	strValue.Format( "%i", faultCode );
	
	m_pXML->AddAttribute( "faultcode", strValue );
}

CFaultResp::CFaultResp(LPCTSTR faultString) : CPacketBase(IBase_FaultResp)
{
	ASSERT( m_pXML == NULL );
	m_pXML = new CXMLElement( NULL, "FaultResp" );
	
	m_pXML->AddAttribute( "faultstring", faultString );
}

CFaultResp::~CFaultResp()
{
}

//////////////////////////////////////////////////////////////////////
// String functions

int CalcHashBytes(LPTSTR xHashing, LPCTSTR xString)
{
	MD5 ctx; BYTE pHash[16];
	ctx.update( (LPBYTE)xString, _tcslen(xString) );
	ctx.finalize(); ctx.raw_digest( pHash );
	
	ctx.tostring( (LPBYTE)xHashing );
	return 32;
}
