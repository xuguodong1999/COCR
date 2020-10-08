#include <QFont>
#include <QFontDatabase>
#include <QDebug>
#include <QTranslator>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QWidget>
#include <QDir>
int main(int argc, char **argv) {
#ifdef WIN32
    _putenv("BABEL_DATADIR=C:\\StaticLibs\\openbabel-3.1.1\\data");
#elif defined(unix)
    putenv("BABEL_DATADIR=/home/xgd/install/openbabel/share/openbabel/3.1.0");
#endif
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    QFontDatabase::addApplicationFont(QStringLiteral(":/fonts/cocr.simhei.ttf"));
    QGuiApplication::setFont(QFont("simhei"));
    QDir dir(":/");
    qInfo()<<dir.entryList();
    static QString fuck = QWidget::tr("test");
    QTranslator translator;
    if (translator.load(QStringLiteral(":/demo_zh_CN.qm"))) {
        qInfo() << "load translation file: "
                << QApplication::installTranslator(&translator);
    }
//    (new QWidget)->showNormal();
    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &a, [url](QObject *obj, const QUrl &objUrl) {
                if (!obj && url == objUrl)
                    QCoreApplication::exit(-1);
            }, Qt::QueuedConnection);
    engine.load(url);
    return a.exec();
}
