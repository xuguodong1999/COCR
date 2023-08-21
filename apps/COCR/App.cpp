#include "App.h"
#include <QSettings>
#include <QDebug>
#include <QTranslator>
#include <QtGui/QFontDatabase>
#include <QFile>

Application::Application(int &argc, char **argv, int flag) : QApplication(argc, argv, flag) {
    setOrganizationName("COCR");
    setOrganizationDomain("xuguodong1999.github.io");
    setApplicationName("COCR");
    static QTranslator translator;
    QString qmFile = ":/i18n/" + applicationName() + "_" + QLocale::system().name() + ".qm";
    qDebug() << __FUNCTION__ << "qmFile=" << qmFile;
    if (translator.load(qmFile)) {
        if (!installTranslator(&translator)) {
            qDebug() << __FUNCTION__ << "fail to install translations";
        } else {
            qDebug() << __FUNCTION__ << "install translations succeed";
        }
    } else {
        qDebug() << __FUNCTION__ << "no translations";
    }
    QFile fontFile(":/fonts/cocr.simfang.ttf");
    if (fontFile.open(QIODevice::ReadOnly)) {
        qDebug() << "open font file success";
        int nFontId = QFontDatabase::addApplicationFontFromData(fontFile.readAll());
        fontFile.close();
        if (nFontId != -1) {
            QStringList lFontFamily = QFontDatabase::applicationFontFamilies(nFontId);
            if (!lFontFamily.empty()) {
                qDebug() << "set font success";
                QFont font(lFontFamily.at(0));
                setFont(font);
            }
        }
    }
}

QSettings &Application::getSettings() {
    static QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                              organizationName(), applicationName());
    return settings;
}

Application::~Application() {

}
