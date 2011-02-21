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
#include "Config-unix.h"
#endif // WIN32

//
// Configuration
//

#include <stdio.h>
#include <stdlib.h>

#if 0
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
#endif

#endif // !defined (CONFIG_H__INCLUDED_)
