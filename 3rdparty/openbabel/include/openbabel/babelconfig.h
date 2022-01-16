#include <QtCore/qglobal.h>

/* The version of Open Babel */
#define BABEL_VERSION "3.1.1"

#ifdef LEAFXY_STATIC
#define OB_EXPORT
#define OB_IMPORT
#define OB_HIDDEN
#else
#define OB_EXPORT Q_DECL_EXPORT
#define OB_IMPORT Q_DECL_IMPORT
#define OB_HIDDEN Q_DECL_HIDDEN
#endif

/* Used to export symbols for DLL / shared library builds */
#if defined(BUILD_LIBS) // e.g. in src/main.cpp
#ifndef OB_EXTERN
#define OB_EXTERN OB_EXPORT extern
#endif

#ifndef OBAPI
#define OBAPI OB_EXPORT
#endif

#ifndef OBCOMMON
#define OBCOMMON OB_EXPORT
#endif

#ifndef OBCONV
#define OBCONV OB_EXPORT
#endif

#ifndef OBERROR
#define OBERROR OB_EXPORT
#endif

#ifndef OBFPRT
#define OBFPRT OB_EXPORT
#endif

#ifndef OBFPTR
#define OBFPTR OB_EXPORT
#endif

#ifndef OBMCDL
#define OBMCDL OB_EXPORT
#endif

#ifndef OBDEPICT
#define OBDEPICT OB_EXPORT
#endif

#else // defined(MAKE_OBDLL)

#ifndef OB_EXTERN
#define OB_EXTERN OB_IMPORT extern
#endif

#ifndef OBAPI
#define OBAPI OB_IMPORT
#endif

#ifndef OBCOMMON
#define OBCOMMON OB_IMPORT
#endif

#ifndef OBCONV
#define OBCONV OB_IMPORT
#endif

#ifndef OBERROR
#define OBERROR OB_IMPORT
#endif

#ifndef OBFPRT
#define OBFPRT OB_IMPORT
#endif

#ifndef OBFPTR
#define OBFPTR OB_IMPORT
#endif

#ifndef OBMCDL
#define OBMCDL OB_IMPORT
#endif

#ifndef OBDEPICT
#define OBDEPICT OB_IMPORT
#endif

#endif


#ifdef _MSC_VER
// Suppress warning on deprecated functions
#pragma warning(disable : 4996)
// Suppress warning that compiler is ignoring C++ exception specification
#pragma warning( disable : 4290 )
// Suppress warning on signed/unsigned comparison with < or > (harmless, but maybe should be fixed)
#pragma warning( disable : 4018 )
// Suppress warning on forcing int etc. value to bool 'true' or 'false' (performance warning)
#pragma warning( disable : 4800 )
//
#pragma warning( disable : 4251 )


#include <algorithm> // std::min and std::max were moved here in C++11
#include <crtdbg.h>

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#else
#define DEBUG_NEW new
#endif
#endif // _MSC_VER

/* have <sys/time.h> */
#define HAVE_SYS_TIME_H 0


/* have symbol clock_t */
#define HAVE_CLOCK_T 1


/* have symbol strncasecmp */
#define HAVE_STRNCASECMP 1

/* have struct clock_t */
#define HAVE_CLOCK_T 1

#ifndef TIME_WITH_SYS_TIME
#ifdef HAVE_SYS_TIME
#ifdef HAVE_TIME
#define TIME_WITH_SYS_TIME 1
#else
#define TIME_WITH_SYS_TIME 0
#endif
#else
#define TIME_WITH_SYS_TIME 0
#endif
#endif

#ifdef _MSC_VER
#define strtoll _strtoi64
#define strtoull _strtoui64
#define snprintf _snprintf
#define popen _popen
#define pclose _pclose
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif 
