#include "XGD/Qt/Util.h"

#include <QCoreApplication>
#include <QtCoreVersion>

#include <spdlog/spdlog.h>

void xgd::qt::printVersion() {
    spdlog::info("qt.version={}, app.version=", QTCORE_VERSION_STR,
                 QCoreApplication::applicationVersion().toStdString()
    );
}