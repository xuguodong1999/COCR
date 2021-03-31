#include "application.hpp"
#include "log_manager.hpp"
#include <QSettings>
#include <QDebug>

Application::Application(int &argc, char **argv, int flag) : QApplication(argc, argv, flag) {
    qApp->setOrganizationName("leafxy");
    qApp->setOrganizationDomain("xuguodong1999.github.io");
    qApp->setApplicationName("leafxy app");
//    getSettings().setValue("test_key1","你好");
    auto logger = LogManager::GetInstance();
    logger->init(qApp->applicationDirPath());
//    qDebug() << getSettings().value("test_key1", "默认值");
}

QSettings &Application::getSettings() {
    static QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                              qApp->organizationName(), qApp->applicationName());
    return settings;
}

Application::~Application() {

}
