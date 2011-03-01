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

#include "Sysdefs.h"

//
// Utility
//

#endif // !defined (CONFIG_H__INCLUDED_)
