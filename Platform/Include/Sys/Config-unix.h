//
// Config-unix.h
//

#if !defined (CONFIG_UNIX_H__INCLUDED_)
#define CONFIG_UNIX_H__INCLUDED_

//
// Configuration
//

#include <stdlib.h>

//
// Definitions
//

//
// Debug assert
//

#ifndef ASSERT
    #include <assert.h>
    #define ASSERT(f) assert((f))
#endif

#ifndef VERIFY
    #define VERIFY(f)
#endif

#endif // !defined (CONFIG_UNIX_H__INCLUDED_)

