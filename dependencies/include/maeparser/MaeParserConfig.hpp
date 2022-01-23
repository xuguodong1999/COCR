#pragma once

#include <QtCore/qglobal.h>

#if defined(MAEPARSER_LIBRARY)
#  define MAEPARSER_EXPORT Q_DECL_EXPORT
#else
#  define MAEPARSER_EXPORT Q_DECL_IMPORT
#endif