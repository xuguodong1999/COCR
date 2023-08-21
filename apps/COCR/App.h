#pragma once

#include <QtWidgets/QApplication>
#include <QSettings>

#define cocrSettings (dynamic_cast<Application *>(QCoreApplication::instance())->getSettings())

class Application : public QApplication {
public:
    Application(int &argc, char **argv, int flag = ApplicationFlags);

    ~Application() override;

    QSettings &getSettings();
};