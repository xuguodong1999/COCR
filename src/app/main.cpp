#include "ui/main_tab_widget.h"
#include "application.h"

int main(int argc, char *argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    Application::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    Application app(argc, argv);
    MainTabWidget w;
#ifndef Q_OS_WASM
    w.showMaximized();
#else
    w.show();
#endif
    return Application::exec();
}
