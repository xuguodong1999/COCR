#pragma once
#include <QtCore/qglobal.h>

#if defined(OPENBABEL_LIBRARY)
# define OB_EXPORT Q_DECL_EXPORT
#else
# define OB_EXPORT Q_DECL_IMPORT
#endif

/* Where the data files are located */
#define BABEL_DATADIR "/usr/local/share/openbabel"

/* The version of Open Babel */
#define BABEL_VERSION "3.1.1"

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
