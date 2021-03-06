#pragma once
#include <QtCore/qglobal.h>
#ifndef STATIC_COORDGEN

#ifdef WIN32
#ifdef IN_COORDGEN
#define EXPORT_COORDGEN Q_DECL_EXPORT
#else
#define EXPORT_COORDGEN Q_DECL_EXPORT
#endif // IN_COORDGEN

#else

#define EXPORT_COORDGEN __attribute__((visibility("default")))

#endif // WIN32

#else

#define EXPORT_COORDGEN

#endif // STATIC_COORDGEN
