//
// IPacket.inl
//

//////////////////////////////////////////////////////////////////////
// IPacket command access

BOOL IPacket::IsCommandId(LPCTSTR xCommandId) const
{
	return _tcscmp( CommandId, xCommandId ) == 0;
}

void IPacket::Release()
{
	if ( this == NULL ) return;
	
	delete this;
}

//////////////////////////////////////////////////////////////////////
// IBindPacket fields access

int IBindPacket::GetCallType(LPTSTR xCallType) const
{
	int nLength = _tcslen( CallType ); nLength = min( nLength, 4 );
	CopyMemory( xCallType, CallType, nLength ); return nLength;
}

int IBindPacket::GetLocalId(LPTSTR xLocalId) const
{
	int nLength = _tcslen( LocalId ); nLength = min( nLength, 21 );
	CopyMemory( xLocalId, LocalId, nLength ); return nLength;
}

int IBindPacket::GetRemoteId(LPTSTR xRemoteId) const
{
	int nLength = _tcslen( RemoteId ); nLength = min( nLength, 21 );
	CopyMemory( xRemoteId, RemoteId, nLength ); return nLength;
}

int IBindPacket::GetFeeNumber(LPTSTR xFeeNumber) const
{
	int nLength = _tcslen( FeeNumber ); nLength = min( nLength, 21 );
	CopyMemory( xFeeNumber, FeeNumber, nLength ); return nLength;
}

int IBindPacket::GetFeeTable(LPTSTR xFeeTable) const
{
	int nLength = _tcslen( FeeTable ); nLength = min( nLength, 4 );
	CopyMemory( xFeeTable, FeeTable, nLength ); return nLength;
}

void IBindPacket::SetCallType(LPCTSTR xCallType)
{
	int nLength = _tcslen( xCallType ); nLength = min( nLength, 4 );
	CopyMemory( CallType, xCallType, nLength );
	ZeroMemory( CallType + nLength, 4 - nLength );
}

void IBindPacket::SetLocalId(LPCTSTR xLocalId)
{
	int nLength = _tcslen( xLocalId ); nLength = min( nLength, 21 );
	CopyMemory( LocalId, xLocalId, nLength );
	ZeroMemory( LocalId + nLength, 21 - nLength );
}

void IBindPacket::SetRemoteId(LPCTSTR xRemoteId)
{
	int nLength = _tcslen( xRemoteId ); nLength = min( nLength, 21 );
	CopyMemory( RemoteId, xRemoteId, nLength );
	ZeroMemory( RemoteId + nLength, 21 - nLength );
}

void IBindPacket::SetFeeNumber(LPCTSTR xFeeNumber)
{
	int nLength = _tcslen( xFeeNumber ); nLength = min( nLength, 21 );
	CopyMemory( FeeNumber, xFeeNumber, nLength );
	ZeroMemory( FeeNumber + nLength, 21 - nLength );
}

void IBindPacket::SetFeeTable(LPCTSTR xFeeTable)
{
	int nLength = _tcslen( xFeeTable ); nLength = min( nLength, 4 );
	CopyMemory( FeeTable, xFeeTable, nLength );
	ZeroMemory( FeeTable + nLength, 4 - nLength );
}

//////////////////////////////////////////////////////////////////////
// IRedPacket fields access

int IRedPacket::GetMinutes() const
{
	return Minutes;
}

void IRedPacket::SetMinutes(int nMinutes)
{
	Minutes	= nMinutes;
}

//////////////////////////////////////////////////////////////////////
// IHandPacket fields access

int IHandPacket::GetClientId(LPTSTR xClientId) const
{
	int nLength = _tcslen( ClientId ); nLength = min( nLength, 8 );
	CopyMemory( xClientId, ClientId, nLength ); return nLength;
}

void IHandPacket::SetClientId(LPCTSTR xClientId)
{
	int nLength = _tcslen( xClientId ); nLength = min( nLength, 8 );
	CopyMemory( ClientId, xClientId, nLength );
	ZeroMemory( ClientId + nLength, 8 - nLength );
}

//////////////////////////////////////////////////////////////////////
// IRingPacket fields access

int IRingPacket::GetCallerId(LPTSTR xCallerId) const
{
	int nLength = _tcslen( CallerId ); nLength = min( nLength, 21 );
	CopyMemory( xCallerId, CallerId, nLength ); return nLength;
}

int IRingPacket::GetPhonNumber(LPTSTR xPhonNumber) const
{
	int nLength = _tcslen( PhonNumber ); nLength = min( nLength, 21 );
	CopyMemory( xPhonNumber, PhonNumber, nLength ); return nLength;
}

void IRingPacket::SetCallerId(LPCTSTR xCallerId)
{
	int nLength = _tcslen( xCallerId ); nLength = min( nLength, 21 );
	CopyMemory( CallerId, xCallerId, nLength );
	ZeroMemory( CallerId + nLength, 21 - nLength );
}

void IRingPacket::SetPhonNumber(LPCTSTR xPhonNumber)
{
	int nLength = _tcslen( xPhonNumber ); nLength = min( nLength, 21 );
	CopyMemory( PhonNumber, xPhonNumber, nLength );
	ZeroMemory( PhonNumber + nLength, 21 - nLength );
}

//////////////////////////////////////////////////////////////////////
// IDialPacket fields access

int IDialPacket::GetLocalId(LPTSTR xLocalId) const
{
	int nLength = _tcslen( LocalId ); nLength = min( nLength, 21 );
	if ( xLocalId ) CopyMemory( xLocalId, LocalId, nLength );
	
	return nLength;
}

int IDialPacket::GetRemoteId(LPTSTR xRemoteId) const
{
	int nLength = _tcslen( RemoteId ); nLength = min( nLength, 21 );
	CopyMemory( xRemoteId, RemoteId, nLength ); return nLength;
}

int IDialPacket::GetCallerId(LPTSTR xCallerId) const
{
	int nLength = _tcslen( CallerId ); nLength = min( nLength, 21 );
	if ( nLength == 0 ) return GetLocalId( xCallerId );
	CopyMemory( xCallerId, CallerId, nLength ); return nLength;
}

int IDialPacket::GetFeeNumber(LPTSTR xFeeNumber) const
{
	int nLength = _tcslen( FeeNumber ); nLength = min( nLength, 21 );
	CopyMemory( xFeeNumber, FeeNumber, nLength ); return nLength;
}

void IDialPacket::SetLocalId(LPCTSTR xLocalId)
{
	int nLength = _tcslen( xLocalId ); nLength = min( nLength, 21 );
	CopyMemory( LocalId, xLocalId, nLength );
	ZeroMemory( LocalId + nLength, 21 - nLength );
}

void IDialPacket::SetRemoteId(LPCTSTR xRemoteId)
{
	int nLength = _tcslen( xRemoteId ); nLength = min( nLength, 21 );
	CopyMemory( RemoteId, xRemoteId, nLength );
	ZeroMemory( RemoteId + nLength, 21 - nLength );
}

void IDialPacket::SetCallerId(LPCTSTR xCallerId)
{
	int nLength = _tcslen( xCallerId ); nLength = min( nLength, 21 );
	CopyMemory( CallerId, xCallerId, nLength );
	ZeroMemory( CallerId + nLength, 21 - nLength );
}

void IDialPacket::SetFeeNumber(LPCTSTR xFeeNumber)
{
	int nLength = _tcslen( xFeeNumber ); nLength = min( nLength, 21 );
	CopyMemory( FeeNumber, xFeeNumber, nLength );
	ZeroMemory( FeeNumber + nLength, 21 - nLength );
}

void IDialPacket::SetTrunkset(WORD nTrunkId, WORD nLinkId)
{
	Trunkset = MAKELONG( nTrunkId, nLinkId );
}

//////////////////////////////////////////////////////////////////////
// IRoutPacket fields access

int IRoutPacket::GetRemoteId(LPTSTR xRemoteId) const
{
	int nLength = _tcslen( RemoteId ); nLength = min( nLength, 21 );
	CopyMemory( xRemoteId, RemoteId, nLength ); return nLength;
}

void IRoutPacket::SetRemoteId(LPCTSTR xRemoteId)
{
	int nLength = _tcslen( xRemoteId ); nLength = min( nLength, 21 );
	CopyMemory( RemoteId, xRemoteId, nLength );
	ZeroMemory( RemoteId + nLength, 21 - nLength );
}

//////////////////////////////////////////////////////////////////////
// IWalkPacket fields access

void IWalkPacket::SetWalkId(LPTSTR xWalkId)
{
	int nLength = _tcslen( xWalkId ); nLength = min( nLength, 21 );
	CopyMemory( WalkId, xWalkId, nLength );
	ZeroMemory( WalkId + nLength, 21 - nLength );
}
