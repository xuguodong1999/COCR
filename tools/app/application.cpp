#include "application.hpp"
#include <QSettings>
#include <QDebug>
#include <QTranslator>
#include <QFontDatabase>
#include <QFile>

Application::Application(int &argc, char **argv, int flag) : QApplication(argc, argv, flag) {
    setOrganizationName("leafxy");
    setOrganizationDomain("xuguodong1999.github.io");
    setApplicationName("leafxy");
    static QTranslator translator;
    QString qmFile = ":/" + applicationName() + "_" + QLocale::system().name() + ".qm";
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
    QFile fontFile(":/fonts/simfang.subset.ttf");
    if (fontFile.open(QIODevice::ReadOnly)) {
        int nFontId = QFontDatabase::addApplicationFontFromData(fontFile.readAll());
        fontFile.close();
        if (nFontId != -1) {
            QStringList lFontFamily = QFontDatabase::applicationFontFamilies(nFontId);
            if (!lFontFamily.empty()) {
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
