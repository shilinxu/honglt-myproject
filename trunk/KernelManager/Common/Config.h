//
// Config.h
//

#if !defined (CONFIG_H__INCLUDED_)
#define CONFIG_H__INCLUDED_

//
// Compiler
//

#if defined (WIN32)
#include "Config-win32.h"
#else
#error Compiler is not supported
#endif // WIN32

//
// Utility
//

#define AKM_VERSIONED_NAMESPACE_NAME AKM

#ifdef AKM_VERSIONED_NAMESPACE_NAME
#define AKM_BEGIN_VERSIONED_NAMESPACE_DECL namespace AKM_VERSIONED_NAMESPACE_NAME {
#define AKM_END_VERSIONED_NAMESPACE_DECL } \
using namespace AKM_VERSIONED_NAMESPACE_NAME;

#else
#define AKM_VERSIONED_NAMESPACE_NAME

#define AKM_BEGIN_VERSIONED_NAMESPACE_DECL
#define AKM_END_VERSIONED_NAMESPACE_DECL
#endif // AKM_VERSIONED_NAMESPACE_NAME

#endif // !defined (CONFIG_H__INCLUDED_)
