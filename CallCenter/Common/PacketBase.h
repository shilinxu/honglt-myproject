//
// PacketBase.h
//

#if !defined(AFX_PACKETBASE_H__704EE319_64A8_4398_A593_F422E8B4D9E0__INCLUDED_)
#define AFX_PACKETBASE_H__704EE319_64A8_4398_A593_F422E8B4D9E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Packet.h"
#include "XML.h"

class CXMLElement;
class CBuffer;

// CPacketBase

class CPacketBase : public CPacket
{
public:
	CPacketBase(LPCTSTR sCommandId);
	virtual ~CPacketBase();

public:
	CXMLElement*	m_pXML;
	
public:
	inline CString	GetMsgType() const;
	inline CString	GetMethod() const;
public:
	virtual CPacket* Clone();
	static CPacket*	ReadBuffer(BYTE* pBuffer, DWORD nLength);
protected:
	virtual int		LoadPacket(BYTE* pBuffer, DWORD nLength);
	virtual int		EncodeBody(CBuffer *pBuffer);
protected:
	virtual CXMLElement*	LoadBody(CXMLElement* xmlBody);
	virtual CXMLElement*	EncodeBody(CXMLElement* xmlBody);
};

// CSyncOrderRelation

class CSyncOrderRelation : public CPacketBase
{
public:
	CSyncOrderRelation(CXMLElement* pXML = NULL);
	virtual ~CSyncOrderRelation();

protected:
	virtual CXMLElement*	LoadBody(CXMLElement* xmlBody);
	virtual CXMLElement*	EncodeBody(CXMLElement* pXML);
};

// CSyncHostFile

class CSyncHostFile : public CPacketBase
{
public:
	CSyncHostFile(CXMLElement* pXML = NULL);
	virtual ~CSyncHostFile();
};

// CSyncDataView

class CSyncDataView : public CPacketBase
{
public:
	CSyncDataView(CXMLElement* pXML = NULL);
	virtual ~CSyncDataView();
};

// CFaultResp

class CFaultResp : public CPacketBase
{
public:
	CFaultResp(DWORD faultCode = 0);
	CFaultResp(LPCTSTR faultString);
	virtual ~CFaultResp();
};

//////////////////////////////////////////////////////////////////////
// CPacketBase fields access

CString CPacketBase::GetMsgType() const
{
	if ( ! m_pXML ) return _T("");
	return m_pXML->GetName();
}

CString CPacketBase::GetMethod() const
{
	ASSERT ( m_pXML != NULL );
	
	return m_pXML->GetAttributeValue( "method" );
}

extern int	CalcHashBytes(LPTSTR xHashing, LPCTSTR xString);

//
// Error types
//

enum{
	EBase_Success		= 0,	// 成功
	EBase_Unknown		= 8000,	// 未知错误
	EBase_NoUser		= 8001,	// 查无此人
	EBase_NoPay			= 8002,	// 欠费停机
};

//
// 命令码定义
//

#define	IBase_SyncOrderRelation	"SOR"	// 关系定制
#define	IBase_SyncHostFile		"SHF"	// 主机同步
#define	IBase_SyncDataView		"SDV"	// 数据请求
#define	IBase_FaultResp			"FAL"	// 错误描述

//
// 同步码定义
//

#define	ISyncOrderRelation		"SyncOrderRelation"		// 定制关系
#define	ISyncMirrorView			"SyncMirrorView"		// 镜像关系
#define	ISyncDataView			"SyncDataView"			// 数据请求
#define	ISyncHostFile			"SyncHostFile"			// 主机视图

//
// 错误码定义
//

#define	IFaultResp				"FaultResp"				// 错误回复


#endif // !defined(AFX_PACKETBASE_H__704EE319_64A8_4398_A593_F422E8B4D9E0__INCLUDED_)
