#ifndef _IO_UTIL_H_
#define _IO_UTIL_H_

#include "cocr.h"
#include <QString>
#include <QFile>
#include <QDir>

namespace cocr {
    ErrorCode releaseToDir(const char *from, const char *to);
}
#endif //_IO_UTIL_H_
