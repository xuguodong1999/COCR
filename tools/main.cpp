#include "ui/main_tab_widget.h"
#include "application.hpp"

int main(int argc, char *argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    Application app(argc, argv);
    MainTabWidget w;
    w.show();
    return app.exec();
}
