//
// VersionUpgrader.h
//

#if !defined(AFX_VERSIONUPGRADER_H__64AABADD_8192_406E_AC85_07C581E2DDCC__INCLUDED_)
#define AFX_VERSIONUPGRADER_H__64AABADD_8192_406E_AC85_07C581E2DDCC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Transfer.h"

class CVersionUpgrader : public CTransfer  
{
// Construction
public:
	CVersionUpgrader(SOCKET hSocket, SOCKADDR_IN* pHost);
	virtual ~CVersionUpgrader();

// Attributes
public:
	
// Operations
public:
	virtual void	Disconnect();
protected:
	BOOL			OnDownloadFile(LPCTSTR pszFile);
	int				ReadBuffer(CBuffer* pBuffer);
protected:
	virtual BOOL	OnRead();
	virtual BOOL	OnWrite();
	virtual BOOL	SendPacket(const void * pData, DWORD nLength);
};

#endif // !defined(AFX_VERSIONUPGRADER_H__64AABADD_8192_406E_AC85_07C581E2DDCC__INCLUDED_)
