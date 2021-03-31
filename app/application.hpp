#ifndef _XGD_APPLICATION_HPP_
#define _XGD_APPLICATION_HPP_

#include <QApplication>
#include <QSettings>

class Application : public QApplication {
public:
    Application(int &argc, char **argv, int flag = ApplicationFlags);

    ~Application() override;

    QSettings &getSettings();
};

#endif//_XGD_APPLICATION_HPP_
