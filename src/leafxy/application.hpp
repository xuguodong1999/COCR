#pragma once
#include <QApplication>
#include <QSettings>

#define leafxyApp (static_cast<Application *>(QCoreApplication::instance()))

class Application : public QApplication {
public:
    Application(int &argc, char **argv, int flag = ApplicationFlags);

    ~Application() override;

    QSettings &getSettings();
};