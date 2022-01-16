#include "appwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    setAppAttribute();
    QApplication app(argc, argv);
    // if (QLocale().language() == QLocale::Chinese) {
    // 	setAppTranslator("zh_CN");
    // }
    (new AppWindow)->showMaximized();
#ifdef USE_QT_ANDROID
    getAndroidPermission();
#endif // USE_QT_ANDROID
    qDebug() << "Enter Event Loop...";
    return app.exec();
}
