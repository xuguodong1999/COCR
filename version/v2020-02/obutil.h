#ifndef OBUTIL_H
#define OBUTIL_H

#include "cocr.h"

#include <QApplication>
#include <QString>
#include <QByteArray>
#include <string>

class COCR_EXPORTS OBUtil {
public:
    OBUtil();

    std::string getpdb(const char *smiles);

    // private:
    bool releasePlugin();

    const QByteArray obPluginPath = ":/obabel";
    const QByteArray obPluginFilePath = getAppDirPath().toUtf8() + "/obabel";
};

#endif  // OBUTIL_H
