#pragma once

#include <openbabel_export.h>

# define OB_EXPORT OPENBABEL_EXPORT

#ifdef _MSC_VER
#define THREAD_LOCAL __declspec(thread)
#define strncasecmp _strnicmp
#else
#define THREAD_LOCAL thread_local
#endif

/* The version of Open Babel */
#define BABEL_VERSION "3.1.1"