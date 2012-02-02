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

#define NAME_VERSIONED_NAMESPACE_DECL

//
// Utility
//

#ifdef NAME_VERSIONED_NAMESPACE_DECL
#define BEGIN_VERSIONED_NAMESPACE_DECL(class_name) namespace class_name {
#define END_VERSIONED_NAMESPACE_DECL() }

#else
#define BEGIN_VERSIONED_NAMESPACE_DECL(class_name)
#define END_VERSIONED_NAMESPACE_DECL
#endif // NAME_VERSIONED_NAMESPACE_DECL

#endif // !defined (CONFIG_H__INCLUDED_)
