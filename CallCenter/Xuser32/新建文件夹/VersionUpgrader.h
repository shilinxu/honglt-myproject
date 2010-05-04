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
	CVersionUpgrader();
	virtual ~CVersionUpgrader();

// Attributes
public:
	HANDLE			m_hFile;
	CEvent			m_pWakeup;
protected:
	CString			m_strMD5hash;
	CString			m_strFile;
	DWORD			m_nLength;
	DWORD			m_nCookie;
protected:
	void	(CVersionUpgrader::*m_fnProgress)(HANDLE);
	
// Operations
public:
	virtual BOOL	ConnectTo(LPCTSTR pAddress, int nPort);
	BOOL			DownloadFile(LPCTSTR xFile);
protected:
	virtual BOOL	OnRead();
	virtual BOOL	OnConnected();
	virtual void	OnDropped(BOOL bError);
protected:
	int			ReadBuffer(CBuffer* pBuffer);
	BOOL		OnFileHeader(LPCTSTR lpszPacket);
	int			OnFileContent(CBuffer* pBuffer);
	BOOL		OnFileCompletion(LPCTSTR xFile);
protected:
	void		OnUpdateList(HANDLE hFile);
protected:
	int			ReadField(BYTE* pBuffer, LPTSTR sField, int nField, CHAR cPart = ' ');
	BOOL		HashWithFile(HANDLE hFile);
};

#endif // !defined(AFX_VERSIONUPGRADER_H__64AABADD_8192_406E_AC85_07C581E2DDCC__INCLUDED_)
