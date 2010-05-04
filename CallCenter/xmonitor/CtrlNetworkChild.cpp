//
// CtrlNetworkChild.cpp
//

#include "stdafx.h"
#include "Xmonitor.h"
#include "CoolInterface.h"
#include "CtrlNetworkChild.h"
#include "PacketCore.h"
#include "XML.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CNetworkChild, CWnd)
BEGIN_MESSAGE_MAP(CNetworkChild, CWnd)
	//{{AFX_MSG_MAP(CNetworkChild)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetworkChild construction

CNetworkChild::CNetworkChild(CXMLElement* pXML) : m_pXML( pXML ? pXML->Clone() : NULL )
{
	m_strChType	= _T("0|1|2|3|4|5|6|7|8|9|10|11|12|20");
	
	if ( ! m_pXML ) m_pXML = new CXMLElement( NULL, _T("SyncTrunkView") );
	ASSERT( m_pXML && m_pXML->IsNamed( _T("SyncTrunkView") ) );
}

CNetworkChild::~CNetworkChild()
{
	if ( m_pXML ) delete m_pXML;
	
	m_pList.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// CNetworkChild attributes

POSITION CNetworkChild::GetIterator() const
{
	if ( ! m_pXML ) return NULL;
	
	return m_pXML->GetElementIterator();
}

CXMLElement* CNetworkChild::GetNext(POSITION& posNext, int nChType) const
{
	if ( posNext == NULL ) return NULL;
	
	CXMLElement* pTrunk = m_pXML->GetNextElement( posNext );
	if ( pTrunk->IsNamed( _T("Trunk") ) == TRUE )
	{
		CString strValue = pTrunk->GetValue();
		
		if ( nChType == -1 ) return pTrunk;
		if ( _ttoi(strValue) == nChType ) return pTrunk;
	}
	
	return GetNext( posNext, nChType );
}

BOOL CNetworkChild::IsEmbededChType(int nChType) const
{
	CString strValue = m_strChType;
	
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
			
		if ( _ttoi(str) == nChType ) return TRUE;
	}
	
	return FALSE;
}

BOOL CNetworkChild::IsEmbededChType(LPCTSTR pszChType) const
{
	CString strValue = pszChType;
	
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		if ( ! IsEmbededChType( _ttoi(str) ) ) return FALSE;
	}
	
	return TRUE;
}

int CNetworkChild::CalcRowCount(LPRECT lpRect) const
{
	int nRowCount = 0;
	int nRowsOf = (lpRect->right - lpRect->left) / 32;
	
	for ( POSITION posNext = GetIterator() ; nRowsOf && posNext ; )
	{
		CXMLElement* pXML = m_pXML->GetNextElement( posNext );
		if ( ! pXML->IsNamed( _T("Trunk") ) ) continue;
		
		CString strValue = pXML->GetValue();
		if ( ! IsEmbededChType( _ttoi(strValue) ) ) continue;
		
		strValue = pXML->GetAttributeValue( "ChCount" );
		nRowCount += _ttoi(strValue) / nRowsOf + 1;
	}
	
	return nRowCount;
}

void CNetworkChild::ResetMap()
{
	if ( m_pXML ) delete m_pXML;
	m_pList.RemoveAll();
	
	m_strChType	= _T("0|1|2|3|4|5|6|7|8|9|10|11|12|20");
	m_pXML = new CXMLElement( NULL, _T("SyncTrunkView") );
	
	UpdateScroll();
	InvalidateRect( NULL );
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkChild message handlers

BOOL CNetworkChild::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	dwStyle |= WS_CHILD|WS_VSCROLL;
	return CWnd::Create( NULL, NULL, dwStyle, rect, pParentWnd, nID, NULL );
}

void CNetworkChild::UpdateScroll()
{
	CRect rc;
	GetClientRect( &rc );
	
	SCROLLINFO pInfo;
	pInfo.cbSize	= sizeof(pInfo);
	pInfo.fMask		= SIF_RANGE|SIF_PAGE|SIF_DISABLENOSCROLL;
	pInfo.nMin		= 0;
	pInfo.nPage		= rc.Height() / 16;
	pInfo.nMax		= CalcRowCount( &rc ) - 1;
	
//	TRACE("UpdateScroll -> nMax:= %i nPage:=%i\n", pInfo.nMax, pInfo.nPage);
	SetScrollInfo( SB_VERT, &pInfo, TRUE );
}

int CNetworkChild::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	SCROLLINFO pScroll;
	ZeroMemory( &pScroll, sizeof(pScroll) );
	pScroll.cbSize	= sizeof(pScroll);
	pScroll.fMask	= SIF_RANGE|SIF_PAGE|SIF_DISABLENOSCROLL;
	pScroll.nMin	= 0;
	pScroll.nMax	= 0; 
	pScroll.nPage	= 1;
	SetScrollInfo( SB_VERT, &pScroll, TRUE );
	
	return 0;
}

void CNetworkChild::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	SCROLLINFO pInfo;
	
	pInfo.cbSize	= sizeof(pInfo);
	pInfo.fMask		= SIF_ALL & ~SIF_TRACKPOS;
	
	GetScrollInfo( SB_VERT, &pInfo );
	int nDelta = pInfo.nPos;
	
	switch ( nSBCode )
	{
	case SB_TOP:
		pInfo.nPos = 0;
		break;
	case SB_BOTTOM:
		pInfo.nPos = pInfo.nMax - pInfo.nPage;
		break;
	case SB_LINEUP:
		pInfo.nPos --;
		break;
	case SB_LINEDOWN:
		pInfo.nPos ++;
		break;
	case SB_PAGEUP:
		pInfo.nPos -= pInfo.nPage;
		break;
	case SB_PAGEDOWN:
		pInfo.nPos += pInfo.nPage;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		pInfo.nPos = nPos;
		break;
	}
	
	pInfo.nPos = max( 0, min( pInfo.nPos, pInfo.nMax - (int)pInfo.nPage + 1 ) );
	if ( pInfo.nPos == nDelta ) return;
	
	SetScrollInfo( SB_VERT, &pInfo, TRUE );
	ScrollWindow( 0, (nDelta - pInfo.nPos)*16 );
}

void CNetworkChild::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize( nType, cx, cy );
	
	UpdateScroll();
}

BOOL CNetworkChild::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CNetworkChild::OnPaint() 
{
	CRect rc;
	GetClientRect( &rc );
	
	CPaintDC dc( this );
	dc.FillSolidRect( &rc, RGB( 250, 250, 255 ) );
	
	int nScroll = GetScrollPos( SB_VERT );
//	TRACE("--------------GetScrollPos:= %i\n", nScroll);
	dc.SetWindowOrg( 0, nScroll * 16 );
	
	for ( POSITION posNext = GetIterator() ; posNext ; )
	{
		CXMLElement* pXML = m_pXML->GetNextElement( posNext );
		if ( ! pXML->IsNamed( _T("Trunk") ) ) continue;
		
		CString strValue = pXML->GetValue();
		if ( IsEmbededChType( _ttoi(strValue) ) ) DoPaint( &dc, rc, pXML );
	}
}

void CNetworkChild::DoPaint(CDC* pDC, CRect& rcClient, CXMLElement* pXML)
{
	CString strValue;
	strValue = pXML->GetAttributeValue( "ChFirst" );
	int nChFirst = _ttoi( strValue );
	
	strValue = pXML->GetAttributeValue( "ChCount" );
	int nChCount = _ttoi( strValue );
	
	for ( int nChan = nChFirst; nChan < nChCount + nChFirst; nChan++ )
	{
		int nCol = (nChan - nChFirst) % ( rcClient.Width() / 32 );
		int nRow = (nChan - nChFirst) / ( rcClient.Width() / 32 );
		
		DrawChImage( pDC, rcClient.left + nCol * 32, rcClient.top + nRow * 16, nChan, TRUE );
	}
	
	rcClient.top += nChCount / ( rcClient.Width() / 32 ) *  16;
	rcClient.top += 16;
}

void CNetworkChild::DrawChImage(CDC* pDC, int nX, int nY, int nChan, BOOL bGrid)
{
	CDC dcBuf;
	CBitmap bmBuf, *pbmOld;
	
	dcBuf.CreateCompatibleDC( pDC );
	bmBuf.CreateCompatibleBitmap( pDC, 32, 16 );
	
	pbmOld = (CBitmap*)dcBuf.SelectObject( &bmBuf );
	
	dcBuf.FillSolidRect( 0, 0, 31, 15, GetChColour( GetChState( nChan ) ) );
	if ( bGrid == TRUE )
	{
		dcBuf.Draw3dRect( 0, 16, 32, 0, RGB( 70, 70, 70 ), RGB( 70, 70, 70 ) );
		dcBuf.Draw3dRect( 32, 0, 0, 16, RGB( 70, 70, 70 ), RGB( 70, 70, 70 ) );
	}
	
	CFont* pOldFont = (CFont*)dcBuf.SelectObject( &CoolInterface.m_fntNormal );
	dcBuf.SetBkColor( GetChColour( GetChState( nChan ) ) );
	dcBuf.SetBkMode( TRANSPARENT );
	dcBuf.SetTextColor( CoolInterface.m_crCmdText );
	
	CString strText;
	strText.Format( "%i", nChan );
	
	CRect rc(1, 1, 31, 15);
	CSize sz = dcBuf.GetTextExtent( strText );
	CPoint pt = rc.CenterPoint();
	pt.x -= sz.cx / 2; pt.y -= sz.cy / 2 + 1;
	
	dcBuf.ExtTextOut( pt.x, pt.y, ETO_CLIPPED|ETO_OPAQUE, rc, strText, NULL );
	dcBuf.SelectObject( pOldFont );
	
	pDC->BitBlt( nX, nY, 32, 16, &dcBuf, 0, 0, SRCCOPY );
	
	dcBuf.SelectObject( pbmOld );
	bmBuf.DeleteObject();
	dcBuf.DeleteDC();
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkChild operations

int CNetworkChild::MapClassToXML(CXMLElement* pXML, int nChState)
{
	if ( m_pXML ) delete m_pXML;
	
	m_pXML = pXML->Clone() ;
	m_pList.RemoveAll();
	
	int nTrunkCCount = 0;
	
	for ( POSITION posNext = GetIterator() ; posNext ; nTrunkCCount++ )
	{
		CXMLElement* pTrunk = GetNext( posNext, -1 );
		if ( pTrunk == NULL ) break;
		
		CString strValue;
		strValue = pTrunk->GetAttributeValue( "ChFirst" );
		int nChFirst = _ttoi( strValue );
		
		strValue = pTrunk->GetAttributeValue( "ChCount" );
		int nChCount = _ttoi( strValue );
		
		for ( int nChan = nChFirst; nChan < nChCount + nChFirst; nChan++ )
		{
			m_pList.SetAt( nChan, nChState );
		}
	}
	
	UpdateScroll();
	InvalidateRect( NULL );
	
	return nTrunkCCount;
}

int CNetworkChild::MapClassToXML(int nChType, int nChFirst, int nChLength, int nChState)
{
	CString strValue;
	
	CXMLElement* pXML = m_pXML->AddElement( _T("Trunk") );
	
	strValue.Format( "%i", nChType );
	//pXML->AddAttribute( _T("ChType"), strValue );
	pXML->SetValue( strValue );
	
	strValue.Format( "%i", nChFirst );
	pXML->AddAttribute( _T("ChFirst"), strValue );
	
	strValue.Format( "%i", nChLength );
	pXML->AddAttribute( _T("ChCount"), strValue );
	
	for ( int nChan = nChFirst; nChan < nChLength + nChFirst; nChan++ )
	{
		m_pList.SetAt( nChan, nChState );
	}
	
	UpdateScroll();
	InvalidateRect( NULL );
	
	return m_pXML->GetElementCount();
}

CXMLElement* CNetworkChild::FindXMLFirstBy(LPCTSTR pszChType) const
{
	CString strValue = pszChType;
	
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		POSITION posNext = GetIterator();
			
		CXMLElement* pXML = GetNext( posNext, _ttoi(str) );
		if ( pXML ) return pXML;
	}
	
	return NULL;
}

CXMLElement* CNetworkChild::FindXMLBy(int nChan, int nAtom, int* pnRow) const
{
	int nRow = 0;
	if ( ! pnRow ) pnRow = &nRow;
	
	for ( POSITION posNext = GetIterator() ; posNext ; )
	{
		CXMLElement* pXML = m_pXML->GetNextElement( posNext );
		if ( ! pXML->IsNamed( _T("Trunk") ) ) continue;
		
		CString strValue = pXML->GetValue();
		if ( ! IsEmbededChType( _ttoi(strValue) ) ) continue;
		
		strValue = pXML->GetAttributeValue( "ChFirst" );
		int nChFirst = _ttoi( strValue );
		strValue = pXML->GetAttributeValue( "ChCount" );
		int nChCount = _ttoi( strValue );
		
		if ( nChan >= nChFirst && nChan - nChFirst < nChCount )
		{
			* pnRow += (nChan - nChFirst) / nAtom; return pXML;
		}
		else
		{
			* pnRow += nChCount / nAtom + 1;
		}
	}
	
	return NULL;
}

LPCTSTR CNetworkChild::SetFilter(LPCTSTR pszChType)
{
	m_strChType	= pszChType;
	
	UpdateScroll();
	InvalidateRect( NULL );
	
	return pszChType;
}
#if 0
void CNetworkChild::UpdateChState(CSyncLineState* pPacket)
{
	int nChan = pPacket->m_nChannel;
	m_pList.SetAt( nChan, pPacket->m_nChState );
	
	CRect rc;
	GetClientRect( &rc );
	
	int nRow = 0;
	CXMLElement* pXML = FindXMLBy( nChan, rc.Width() / 32, &nRow );
	if ( pXML == NULL ) return;
	
	CString strValue = pXML->GetAttributeValue( "ChFirst" );
	int nLeft = (nChan -_ttoi(strValue)) % (rc.Width() / 32) * 32;
	int nTop = nRow * 16;
	
	CClientDC dc( this );
	int nScroll = GetScrollPos( SB_VERT );
	dc.SetWindowOrg( 0, nScroll * 16 );
	
	DrawChImage( &dc, nLeft, nTop, nChan, TRUE );
}
#endif
void CNetworkChild::UpdateChState(int nChan, int nChState)
{
	m_pList.SetAt( nChan, nChState );
	
	CRect rc;
	GetClientRect( &rc );
	
	int nRow = 0;
	CXMLElement* pXML = FindXMLBy( nChan, rc.Width() / 32, &nRow );
	if ( pXML == NULL ) return;
	
	CString strValue = pXML->GetAttributeValue( "ChFirst" );
	int nLeft = (nChan -_ttoi(strValue)) % (rc.Width() / 32) * 32;
	int nTop = nRow * 16;
	
	CClientDC dc( this );
	int nScroll = GetScrollPos( SB_VERT );
	dc.SetWindowOrg( 0, nScroll * 16 );
	
	DrawChImage( &dc, nLeft, nTop, nChan, TRUE );
}
/////////////////////////////////////////////////////////////////////////////
// CNetworkChild Helpers

COLORREF CNetworkChild::GetChColour(DWORD nChState) const
{
	switch ( nChState )
	{
	case	S_CALL_STANDBY:
		return GetSysColor( COLOR_BTNFACE );//RGB( 192, 192, 192 );
		
	case	S_CALL_PICKUPED:
		return RGB( 255, 247, 88 );
		
	case	S_CALL_RINGING:
		return RGB( 249, 59, 55 );
		
	case	S_CALL_TALKING:
		return RGB( 0, 168, 0 );
		
	case	S_CALL_PENDING:
	case	S_CALL_RemoteBlock:
	case	S_CALL_LocalBlock:
		return RGB( 250, 250, 255 );
		
	case	S_CALL_WAIT_REMOTE_PICKUP:
		return RGB( 0, 125, 251 );
		
	case	S_CALL_UNAVAILABLE:
		return RGB( 222, 81, 42 );
		
	case	S_TUP_WaitPcmReset:
	case	S_ISUP_WaitReset:
		return RGB( 255, 0, 128 );
		
	default:
		return RGB( 192, rand(), 192 );
	}
}
