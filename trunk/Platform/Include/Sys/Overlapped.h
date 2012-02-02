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

class COverlapped : public OVERLAPPED
{
// Construction
public:
	COverlapped(EOperationType nType);

// Attributes
public:
	const EOperationType	OperationType;
	
// Operations
public:
	virtual BOOL			IsCompleted() const;
};

//////////////////////////////////////////////////////////////////////////
// COverlapped

inline COverlapped::COverlapped(EOperationType nType) : OperationType(nType)
{
	ZeroMemory( static_cast<LPOVERLAPPED>(this), sizeof(OVERLAPPED) );
}

inline BOOL COverlapped::IsCompleted() const
{
	return HasOverlappedIoCompleted( this );
}

#ifdef _WINSOCKAPI_

//////////////////////////////////////////////////////////////////////////
// CWSAOverlapped

template<EOperationType TYPE>
class CWSAOverlapped : public COverlapped
{
// Construction
public:
	CWSAOverlapped();
	
// Attributes
public:
	WSABUF		m_pBuffer;
	SOCKADDR_IN	m_pAddress;
	INT			m_nAddressLen;
};

//////////////////////////////////////////////////////////////////////////
// CWSAOverlapped

template<EOperationType TYPE>
inline CWSAOverlapped<TYPE>::CWSAOverlapped() : COverlapped(TYPE)
{
	ZeroMemory( &m_pBuffer, sizeof(WSABUF) );
	ZeroMemory( &m_pAddress, sizeof(SOCKADDR_IN) );
	m_nAddressLen	= sizeof(SOCKADDR_IN);
}

template<EOperationType TYPE> class CWSAOverlapped;
typedef class CWSAOverlapped<OperationType_Read> CWSAOverlappedRead;
typedef class CWSAOverlapped<OperationType_Write> CWSAOverlappedWrite;

#endif // _WINSOCKAPI_

//////////////////////////////////////////////////////////////////////////

#endif // OVERLAPPED_H__INCLUDED_
