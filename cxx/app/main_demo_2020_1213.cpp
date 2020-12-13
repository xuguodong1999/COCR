#include "demowidget_2020_1213.h"

#include <QApplication>
#include <exception>
#include <iostream>

int main(int argc, char *argv[]) {
        qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
        QApplication a(argc, argv);
        DemoWidget_2020_1213 w;
        w.show();
        return a.exec();
}
