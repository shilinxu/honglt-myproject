//
// KernelManager.h
//

#if !defined(KERNEL_MANAGER_H__INCLUDED_)
#define KERNEL_MANAGER_H__INCLUDED_

//
// Configuration
//

#ifdef KERNEL_MANAGER_DLL
	#define KERNEL_MANAGER_CLASS	Platform_Proper_Export
#else
	#define KERNEL_MANAGER_CLASS	Platform_Proper_Import
#endif
// 
// #ifdef _DEBUG
// 	#define KERNEL_MANAGER_DLL_NAME	TEXT("KernelManagerD.dll")
// #else
// 	#define KERNEL_MANAGER_DLL_NAME	TEXT("KernelManager.dll")
// #endif

#define MY_VERSION(major,minor)		(100 * major + minor)

#ifndef VER_Kernel
	#define VER_Kernel				MY_VERSION(1,0)	// Default ver(1.0)
#endif

//
// Definitions
//
// 
// class CPacket;
// class CTransfer;
// 
// typedef CArrayTemplate<CTransfer *> CTransferPtrArray;
// 
// typedef	void (CALLBACK* LPONCONNECTED)(CTransfer* pTransfer);
// typedef	void (CALLBACK* LPONDISCONNECTED)(CTransfer* pTransfer, int nErrCode);
// typedef	void (CALLBACK* LPONREADCOMPLETED)(CTransfer* pTransfer, CPacket* pPacket);
// typedef	void (CALLBACK* LPONREADCOMPLETED2)(CTransfer* pTransfer, BYTE* pBuffer, DWORD nLength);
// 
// #undef StartService
// 
// #ifndef EVENTSERVICE_SUPPORT
// #define	EVENTSERVICE_SUPPORT
// #endif
// 
// #ifndef INET32SERVICE_SUPPORT
// #define	INET32SERVICE_SUPPORT
// #endif

/////////////////////////////////////////////////////////////////////////////
// 
// struct tagProxyInfo
// {
// 	BYTE							cbProxyType;
// 	WORD							wProxyPort;
// 	CString							strProxyName;
// 	CString							strProxyPass;
// 	CString							strProxyServer;
// 	CString							strHttpDomain;
// };
// 
// #define PROXY_NONE					0
// #define PROXY_HTTP					1
// // #define PROXY_SOCKS4				2
// #define PROXY_SOCKS5				2

/////////////////////////////////////////////////////////////////////////////
// 
// enum TCPSocketType
// {
// 	TCPSocketType_Stream			= 0,
// 	TCPSocketType_Http				= 1,
// 
// 	TCPProxyLayer_Socks5			= (PROXY_SOCKS5 << 12) | TCPSocketType_Stream,
// 	TCPProxyLayer_Http				= (PROXY_HTTP << 12) | TCPSocketType_Stream,
// };

/////////////////////////////////////////////////////////////////////////////

//
// Common
//

#include "Sys\Overlapped.h"
// #include "Sys\Packet.h"

//
// Interface
//
// 
// interface ITCPSocketSink
// {
// 	virtual void	OnConnected(CTransfer* pTransfer) = NULL;
// 	virtual void	OnDisconnected(CTransfer* pTransfer, int nErrCode) = NULL;
// 	virtual void	OnReadCompleted(CTransfer* pTransfer, CPacket* pPacket) = NULL;
// 	virtual CPacket*ReadBuffer(CBuffer* pBuffer, CTransfer* pTransfer) = NULL;
// 	virtual BOOL	Attach(CTransfer* pTransfer, DWORD nIdentifier) = NULL;
// 	virtual BOOL	Detach(CTransfer* pTransfer, BOOL /*bPeek*/) = NULL;
// };

/////////////////////////////////////////////////////////////////////////////
// 
// interface IServiceEngine : public IUnknownEx
// {
// 	virtual BOOL	StartService() = NULL;
// 	virtual BOOL	StopService() = NULL;
// };
// 
// interface IServiceEngineSink : public IUnknownEx
// {
// };
// 
// #define VER_IServiceEngine INTERFACE_VERSION(1,1)
// static const GUID IID_IServiceEngine={0x8075d9c3,0x6d56,0x47c8,0x8c,0x2d,0xeb,0xe1,0xde,0xef,0xa8,0x28};
// 
// #define VER_IServiceEngineSink INTERFACE_VERSION(1,1)
// static const GUID IID_IServiceEngineSink={0xf9c04357,0x94e2,0x4f73,0x82,0xff,0x46,0x4d,0x67,0x22,0xb1,0x22};

/////////////////////////////////////////////////////////////////////////////

interface ITransferable
{
	virtual BOOL	OnRun() = NULL;
	virtual	void	Release() = NULL;
	virtual void	Destroy(BOOL bPeek) = NULL;
	virtual BOOL	OnReadCompleted(COverlapped* pOverlapped, DWORD nLength) = NULL;
	virtual BOOL	OnWriteCompleted(COverlapped* pOverlapped, DWORD nLength) = NULL;
};
// 
// interface ITransferManager// : IServiceEngine
// {
// 	virtual BOOL	Attach(ITransferable* pTransfer, HANDLE hHandle) = NULL;
// 	virtual BOOL	Detach(ITransferable* pTransfer) = NULL;
// 	virtual	DWORD	GetIdentifier() const = NULL;
// 	virtual BOOL	StartService() = NULL;
// 	virtual BOOL	StopService() = NULL;
// };
// 
// #define VER_ITransferManager INTERFACE_VERSION(1,1)
// static const GUID IID_ITransferManager={0xaa294135,0xbc2b,0x4471,0xbd,0xa,0x4f,0x7e,0xe2,0x77,0x91,0x2d};

/////////////////////////////////////////////////////////////////////////////
// 
// interface IQueueService : IServiceEngine
// {
// 	virtual	BOOL	PutQueue(DWORD nIdentifier, void* pBuffer, DWORD nLength) = NULL;
// };
// 
// // #if !defined(VER_Kernel) || VER_Kernel < MY_VERSION(2,0)
// interface IQueueServiceSink
// // #else
// // interface IQueueServiceSink : IUnknownEx
// // #endif
// {
// 	virtual	BOOL	OnQueueService(DWORD nIdentifier, void* pBuffer, DWORD nLength) = NULL;
// };
// 
// #define VER_IQueueService INTERFACE_VERSION(1,1)
// static const GUID IID_IQueueService={0x21e9f9c8, 0x86ea, 0x47f5, 0xa2, 0xac, 0x60, 0x58, 0x49, 0xcb, 0x83, 0x4b};
// 
// #define VER_IQueueServiceSink INTERFACE_VERSION(1,1)
// static const GUID IID_IQueueServiceSink={0xae8c0f24, 0x2ad, 0x4268, 0x95, 0xec, 0xe5, 0xa2, 0x2a, 0xd5, 0xd2, 0xf6};

/////////////////////////////////////////////////////////////////////////////

interface IApplication : public IUnknown
{
	virtual BOOL	PrintMessage(int nType, LPCTSTR pszMsg) = NULL;
};

DEFINE_GUID(IID_IApplication, 0xda0f60ff, 0xbbed, 0x45dd, 0x9f, 0xe8, 0xc7, 0x86, 0xc5, 0xd8, 0x71, 0xd9);

/////////////////////////////////////////////////////////////////////////////
// 
// interface IApplySettings : public IUnknownEx
// {
// 	virtual	void*	GetSettings(LPCTSTR pszSection) = NULL;
// };
// 
// #define VER_IApplySettings INTERFACE_VERSION(1,1)
// static const GUID IID_IApplySettings={0xe20aa8a5,0xda70,0x491e,0xb9,0x74,0x58,0xb8,0x6,0x75,0x18,0xac};

/////////////////////////////////////////////////////////////////////////////
// 
// interface IAttemperEngine : public IServiceEngine
// {
// 	virtual	BOOL	SetApplicationSink(IUnknownEx* pUnknown) = NULL;
// 	virtual	void*	GetApplicationSink(REFGUID Guid, DWORD dwQueryVer) = NULL;
// };
// 
// #define VER_IAttemperEngine INTERFACE_VERSION(1,1)
// static const GUID IID_IAttemperEngine={0x9dfd8ede,0x3e77,0x46d9,0x93,0xc6,0x55,0x56,0xc,0x71,0x7f,0x7d};

/////////////////////////////////////////////////////////////////////////////
// 
// interface IDataService : public IUnknownEx
// {
// 	virtual BOOL	DoPacket(CTransfer* pTransfer, CPacket* pPacket) = NULL;
// };
// 
// #define VER_IDataService INTERFACE_VERSION(1,1)
// static const GUID IID_IDataService={0x9218de7e,0x18eb,0x4d01,0xa7,0x96,0x4e,0xc2,0xec,0xa2,0xbd,0xe1};

//
// Helper
//

// DECLARE_MODULE_HELPER(TCPManagerEngine,KERNEL_MANAGER_DLL_NAME,"CreateTCPManagerEngine")

//
// Utility
//
// 
// #ifdef EVENTSERVICE_SUPPORT
// #include "EventService.h"
// #endif // EVENTSERVICE_SUPPORT
// 
// #ifdef INET32SERVICE_SUPPORT
// #include "Transfer.h"
// #include "QueueService.h"
// 
// #include "TCPClient.h"
// #include "TCPServer.h"
// #include "HttpRequest.h"
// #include "TransferManager.h"
// #endif // INET32SERVICE_SUPPORT

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(KERNEL_MANAGER_H__INCLUDED_)
