#pragma once
#include <QApplication>
#include <QSettings>

#define leafxySettings (static_cast<Application *>(QCoreApplication::instance())->getSettings())

class Application : public QApplication {
public:
    Application(int &argc, char **argv, int flag = ApplicationFlags);

    ~Application() override;

    QSettings &getSettings();
};