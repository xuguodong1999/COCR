#include "XGD/Init/OpenBabel.h"
#include "XGD/Init/Qt.h"
#include "XGD/Init/Spdlog.h"
#include "Common/MainTabWidget.h"
#include "App.h"
#include <QSurface>

int main(int argc, char *argv[]) {
    if (!xgd::init::setupSpdlog()) { return EXIT_FAILURE; }
    if (!xgd::init::setupQt()) { return EXIT_FAILURE; };
    if (!xgd::init::setupOpenbabel()) { return EXIT_FAILURE; };
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    Application::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
#if defined(Q_OS_LINUX)
    qputenv("QT3D_RENDERER", "rhi");
    qputenv("QSG_RHI_BACKEND", "vulkan");
#elif defined(Q_OS_ANDROID)
    qputenv("QT3D_RENDERER", "opengl");
#endif
    Application app(argc, argv);
    MainTabWidget w;
#ifndef Q_OS_WASM
    w.resize(540, 360);
    w.show();
//    w.showMaximized();
#else
    w.show();
#endif
    return Application::exec();
}
