#include "application.hpp"
#include "log_manager.hpp"
#include <QSettings>
#include <QDebug>
#include <QTranslator>

Application::Application(int &argc, char **argv, int flag) : QApplication(argc, argv, flag) {
    setOrganizationName("leafxy");
    setOrganizationDomain("xuguodong1999.github.io");
    setApplicationName("leafxy app");
//    getSettings().setValue("test_key1","你好");
#ifndef Q_OS_ANDROID
    auto logger = LogManager::GetInstance();
    logger->init(applicationDirPath());
#endif
    static QTranslator translator;
    if (translator.load(qAppName() + "_" + QLocale::system().name())) {
        if (!installTranslator(&translator)) {
            qDebug() << __FUNCTION__ << "fail to install translations";
        } else {
            qDebug() << __FUNCTION__ << "install translations succeed";
        }
    } else {
        qDebug() << __FUNCTION__ << "no translations";
    }
}

QSettings &Application::getSettings() {
    static QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                              organizationName(), applicationName());
    return settings;
}

Application::~Application() {

}
