/* src/config.h.in. Generated from configure.in by autoheader. */

/* Where the data files are located */
#define BABEL_DATADIR "/usr/local/share/openbabel"

/* The version of Open Babel */
#define BABEL_VERSION "3.1.1"

/* Version check macro
 Can be used like #if (OB_VERSION >= OB_VERSION_CHECK(2, 2, 99)) */
#define OB_VERSION_CHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))

/* OB_VERSION is (major << 16) + (minor << 8) + patch */
#define OB_VERSION OB_VERSION_CHECK(3, 1, 1)

/* The file extension used for shared modules */
#define MODULE_EXTENSION ".so"


#if defined(OPENBABEL_LIBRARY)
# define OB_EXPORT Q_DECL_EXPORT
#else
# define OB_EXPORT Q_DECL_IMPORT
#endif

#include <QtCore/qglobal.h>

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
/* have <conio.h> */
/* #undef HAVE_CONIO_H */

/* have <sys/time.h> */
#define HAVE_SYS_TIME_H 1

/* have <time.h> */
#define HAVE_TIME_H 1

/* have symbol clock_t */
#define HAVE_CLOCK_T 1

/* have symbol rint */
/* #undef HAVE_RINT */

/* have symbol snprintf */
#define HAVE_SNPRINTF 1

/* have symbol sranddev */
/* #undef HAVE_SRANDDEV */

/* have symbol strcasecmp */
#define HAVE_STRCASECMP 1

/* have symbol strncasecmp */
#define HAVE_STRNCASECMP 1

/* have struct clock_t */
#define HAVE_CLOCK_T 1

#if defined(WIN32)
#ifndef HAVE_SNPRINTF
#define snprintf _snprintf
#define HAVE_SNPRINTF 1
#endif

#ifndef HAVE_STRCASECMP
#define strcasecmp _stricmp
#define HAVE_STRCASECMP 1
#endif

#ifndef HAVE_STRNCASECMP
#define strncasecmp _strnicmp
#define HAVE_STRNCASECMP 1
#endif
#endif // WIN32

/* #undef SCANDIR_NEEDS_CONST */
#ifdef SCANDIR_NEEDS_CONST
#define SCANDIR_CONST const
#else
#define SCANDIR_CONST
#endif

#define OB_MODULE_PATH "/usr/local/lib/openbabel/3.1.1"

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
