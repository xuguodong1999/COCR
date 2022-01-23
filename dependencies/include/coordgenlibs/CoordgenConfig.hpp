#pragma once

#include <QtCore/qglobal.h>

#if defined(COORDGENLIBS_LIBRARY)
#  define COORDGENLIBS_EXPORT Q_DECL_EXPORT
#else
#  define COORDGENLIBS_EXPORT Q_DECL_IMPORT
#endif
