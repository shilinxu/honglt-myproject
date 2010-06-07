//
// Overlapped.h
//

#ifndef OVERLAPPED_H__INCLUDED_
#define OVERLAPPED_H__INCLUDED_

#pragma once

//////////////////////////////////////////////////////////////////////////

enum EOperationType
{
	OperationType_Read,
	OperationType_Write,
};

template<EOperationType TYPE> class CWSAOverlapped;
typedef class CWSAOverlapped<OperationType_Read> COverlappedRead;
typedef class CWSAOverlapped<OperationType_Write> COverlappedWrite;

class COverlapped : public OVERLAPPED
{
// Construction
public:
	COverlapped(EOperationType OperationType);

// Attributes
public:
	const EOperationType	OperationType;
	
// Operations
public:
	BOOL			IsCompleted() const;
};

template<EOperationType TYPE>
class CWSAOverlapped : public COverlapped
{
// Construction
public:
	CWSAOverlapped();
	
// Attributes
public:
	WSABUF			m_pBuffer;
};

//////////////////////////////////////////////////////////////////////////
// COverlapped

AFX_INLINE COverlapped::COverlapped(EOperationType nType) : OperationType(nType)
{
	ZeroMemory( this, sizeof(OVERLAPPED) );
}

AFX_INLINE BOOL COverlapped::IsCompleted() const
{
	return HasOverlappedIoCompleted( this );
}

//////////////////////////////////////////////////////////////////////////
// CWSAOverlapped

template<EOperationType TYPE>
CWSAOverlapped<TYPE>::CWSAOverlapped() : COverlapped(TYPE)
{
	ZeroMemory( &m_pBuffer, sizeof(WSABUF) );
}

//////////////////////////////////////////////////////////////////////////

#endif // OVERLAPPED_H__INCLUDED_