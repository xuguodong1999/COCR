#pragma once
#include <QtCore/qglobal.h>

#if defined(OPENBABEL_LIBRARY)
# define OB_EXPORT Q_DECL_EXPORT
#else
# define OB_EXPORT Q_DECL_IMPORT
#endif

/* The version of Open Babel */
#define BABEL_VERSION "3.1.1"
