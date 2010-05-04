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
	EBase_Success		= 0,	// �ɹ�
	EBase_Unknown		= 8000,	// δ֪����
	EBase_NoUser		= 8001,	// ���޴���
	EBase_NoPay			= 8002,	// Ƿ��ͣ��
};

//
// �����붨��
//

#define	IBase_SyncOrderRelation	"SOR"	// ��ϵ����
#define	IBase_SyncHostFile		"SHF"	// ����ͬ��
#define	IBase_SyncDataView		"SDV"	// ��������
#define	IBase_FaultResp			"FAL"	// ��������

//
// ͬ���붨��
//

#define	ISyncOrderRelation		"SyncOrderRelation"		// ���ƹ�ϵ
#define	ISyncMirrorView			"SyncMirrorView"		// �����ϵ
#define	ISyncDataView			"SyncDataView"			// ��������
#define	ISyncHostFile			"SyncHostFile"			// ������ͼ

//
// �����붨��
//

#define	IFaultResp				"FaultResp"				// ����ظ�


#endif // !defined(AFX_PACKETBASE_H__704EE319_64A8_4398_A593_F422E8B4D9E0__INCLUDED_)
