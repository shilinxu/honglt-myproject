//
// EDClock.cpp
//

#include "stdafx.h"
#include "MakeReport.h"
#include "EDClock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CEDClock, CStatic)

BEGIN_MESSAGE_MAP(CEDClock, CStatic)
	//{{AFX_MSG_MAP(CEDClock)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEDClock construction

CEDClock::CEDClock()
{
	m_dwStyle	= XDC_NORMAL;
	m_tNavigated= time( NULL );
}

CEDClock::~CEDClock()
{
}

/////////////////////////////////////////////////////////////////////////////
// CEDClock create

BOOL CEDClock::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	dwStyle |= WS_CHILD|WS_VISIBLE;
	return CStatic::Create( NULL, dwStyle, rect, pParentWnd, nID );
}

/////////////////////////////////////////////////////////////////////////////
// CEDClock operations

CEDClock::EClockStyle CEDClock::GetStyle() const
{
	return m_dwStyle;
}

CEDClock::EClockStyle CEDClock::SetStyle(EClockStyle dwStyle)
{
	m_tNavigated = time( NULL );
	m_dwStyle = dwStyle; Invalidate();
	
	return m_dwStyle;
}

CEDClock::EClockStyle CEDClock::SetStyle(EClockStyle dwStyle, DWORD nFlowCount)
{
	if ( dwStyle == XDC_FLOWBACK )
	{
		m_tNavigated = time( NULL ) + nFlowCount;
		m_dwStyle = dwStyle; Invalidate();
		
		return m_dwStyle;
	}
	
	return SetStyle( dwStyle );
}

int CEDClock::GetFlowCount() const
{
	if ( GetStyle() == XDC_FLOWBACK )
	{
		return m_tNavigated - time( NULL );
	}
	
	return time( NULL ) - m_tNavigated;
}

BOOL CEDClock::IsFlowOver() const
{
	if ( GetStyle() != XDC_FLOWBACK ) return FALSE;
	
	return GetFlowCount() <= 0;
}

/////////////////////////////////////////////////////////////////////////////
// CEDClock message handlers

int CEDClock::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;

	CBitmap bmClock;
	bmClock.LoadBitmap( IDB_EDCLOCK );
	m_pClock.Create( 13, 23, ILC_COLOR24, 12, 0 );
	m_pClock.Add( &bmClock, RGB( 128, 0, 0 ) );
	
	SetTimer( 1, 1000, NULL );
	PostMessage( WM_TIMER, 1 );
	
	return 0;
}

void CEDClock::OnDestroy() 
{
	CStatic::OnDestroy();
	
	KillTimer( 1 );	
}

BOOL CEDClock::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CEDClock::OnPaint() 
{
	CPaintDC dc( this );
	
	if ( GetStyle() == XDC_NORMAL )
	{
		CTime tNow = CTime::GetCurrentTime();
		DrawNowTime( tNow );
	}
	else
	if ( GetStyle() == XDC_FLOWNEXT )
	{
		DrawFlowCount( GetFlowCount() );
	}
	else
	if ( GetStyle() == XDC_FLOWBACK )
	{
		DrawFlowCount( max( 0, GetFlowCount() ) );
	}
}

void CEDClock::OnTimer(UINT nIDEvent) 
{
	if ( GetStyle() == XDC_NORMAL )
	{
		CTime tNow = CTime::GetCurrentTime();
		DrawNowTime( tNow );
	}
	else
	if ( GetStyle() == XDC_FLOWNEXT )
	{
		DrawFlowCount( GetFlowCount() );
	}
	else
	if ( GetStyle() == XDC_FLOWBACK )
	{
		if ( GetFlowCount() >= 0 )
		DrawFlowCount( GetFlowCount() );
		else
		GetParent()->PostMessage( WM_TIMEROUT );
	}
}

void CEDClock::DrawNowTime(CTime& tNow)
{
	BYTE nByte;
	
	if ( nByte = tNow.GetHour() / 10 )
	{
		DrawClockBit( nByte, 0 );
	}
	else
	{
		DrawClockBit( 11, 0 );
	}
	DrawClockBit( tNow.GetHour() % 10, 1 );
	DrawClockBit( 10, 2 );
	
	DrawClockBit( tNow.GetMinute() / 10, 3 );
	DrawClockBit( tNow.GetMinute() % 10, 4 );
	DrawClockBit( 10, 5 );
	
	DrawClockBit( tNow.GetSecond() / 10, 6 );
	DrawClockBit( tNow.GetSecond() % 10, 7 );
}

void CEDClock::DrawFlowCount(DWORD nFlowCount)
{
	int nHour = ( nFlowCount / 3600 ) % 24;
	DrawClockBit( (nHour / 10) ? nHour / 10 : 11, 0 );
	DrawClockBit( nHour % 10, 1 );
	DrawClockBit( 10, 2 );
	
	int nMinute = ( nFlowCount / 60 ) % 60;
	DrawClockBit( nMinute / 10, 3 );
	DrawClockBit( nMinute % 10, 4 );
	DrawClockBit( 10, 5 );
	
	int nSecond = nFlowCount % 60;
	DrawClockBit( nSecond / 10, 6 );
	DrawClockBit( nSecond % 10, 7 );
}

void CEDClock::DrawClockBit(BYTE nDigit, int nPos)
{
	CRect rc;
	GetClientRect( &rc );
	
	CClientDC dc( this );
	CBitmap bmBuf, *pbmOld;
	
	CDC dcBuf;
	dcBuf.CreateCompatibleDC( &dc );
	bmBuf.CreateCompatibleBitmap( &dc, rc.Width(), rc.Height() );
	pbmOld = (CBitmap*)dcBuf.SelectObject( &bmBuf );

	dcBuf.BitBlt(rc.left,rc.top,rc.right,rc.bottom,&dc,0,0,SRCCOPY);
//dcBuf.FillSolidRect(0,0,rc.Width(), rc.Height(),RGB(255,255,255));
	
	ImageList_DrawEx( m_pClock, nDigit, dcBuf.GetSafeHdc(), 13*nPos,
		0, 13, 23, CLR_DEFAULT, CLR_DEFAULT, ILD_NORMAL );
	dc.BitBlt( 0, 0, rc.Width(), rc.Height(), &dcBuf, 0, 0, SRCCOPY );
	
	dcBuf.SelectObject( pbmOld );
	dcBuf.DeleteDC();
	bmBuf.DeleteObject();
}
