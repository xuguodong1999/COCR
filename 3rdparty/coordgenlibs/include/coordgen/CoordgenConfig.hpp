#pragma once

#include <QtCore/qglobal.h>

#ifdef LEAFXY_STATIC
#define EXPORT_COORDGEN
#else
#ifdef BUILD_LIBS
#define EXPORT_COORDGEN Q_DECL_EXPORT
#else
#define EXPORT_COORDGEN Q_DECL_IMPORT
#endif
#endif



