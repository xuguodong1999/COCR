#pragma once
#include <QtCore/qglobal.h>

#if defined(OPENBABEL_LIBRARY)
# define OB_EXPORT Q_DECL_EXPORT
#else
# define OB_EXPORT Q_DECL_IMPORT
#endif

#ifdef _MSC_VER
#define THREAD_LOCAL __declspec(thread)
#define strncasecmp _strnicmp
#else
#define THREAD_LOCAL thread_local
#endif

/* The version of Open Babel */
#define BABEL_VERSION "3.1.1"