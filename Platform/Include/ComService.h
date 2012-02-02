//
// ComService.h
//

#ifndef COM_SERVICE_H__INCLUDED_
#define COM_SERVICE_H__INCLUDED_

//
// Compiler
//

#include "Sys\Config.h"
#include "Sys\Typedefs.h"

//
// Configuration
//

#if defined(WIN32)

#ifdef COM_SERVICE_DLL
#define COM_SERVICE_CLASS       Platform_Proper_Export
#else
#define COM_SERVICE_CLASS       Platform_Proper_Import
#endif // COM_SERVICE_Export

#ifdef COM_SERVICE_CLASS
#define COM_SERVICE_API			COM_SERVICE_CLASS
#endif

#else

#define COM_SERVICE_CLASS
#endif // WIN32

#if !defined (WIN32) && !defined (_WIN32_WCE)
#include "CString.h"
#endif

#include "XML.h"
#include "Buffer.h"
#include "ServiceThread.h"

//
// String
//

BEGIN_VERSIONED_NAMESPACE_DECL(Convert)

extern COM_SERVICE_API CString	ToHex(BYTE* pBuffer, DWORD nLength);
COM_SERVICE_API CString	ToASCII(BYTE* pBuffer, DWORD nLength); 

END_VERSIONED_NAMESPACE_DECL()
using namespace Convert;

//
// UTF8
//

BEGIN_VERSIONED_NAMESPACE_DECL(UTF8)

COM_SERVICE_API CStringA UTF8Encode(__in const CStringW& strInput);
COM_SERVICE_API CStringA UTF8Encode(__in_bcount(nInput) LPCWSTR psInput, __in int nInput);

END_VERSIONED_NAMESPACE_DECL()
using namespace UTF8;

#endif // COM_SERVICE_H__INCLUDED_
