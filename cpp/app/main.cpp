#include "sketchitem.hpp"
#include <QFontDatabase>
#include <QTranslator>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

inline void addFontData() {
    QFontDatabase database;
    int id = QFontDatabase::addApplicationFont(":/simfang.subset.ttf");
    try {
        qApp->setFont(QFont(database.applicationFontFamilies(id).at(0)));
    } catch (std::exception e) {
        qWarning() << e.what();
    }
}

inline void addTranslator() {
    static QTranslator translator;
    if (translator.load(":/demo_zh_CN.qm")) {
        if (!qApp->installTranslator(&translator))
            qWarning() << "fail to load " << translator.filePath();
    }
}

#include <QDir>

int main(int argc, char **argv) {
    qputenv("QML_DISABLE_DISK_CACHE", "1");
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
//    qFatal("致命");
//    qCritical() << "2";
//    qWarning() << "1";

    addFontData();
    addTranslator();
    qDebug() << QDir(":/qt-project.org").entryList();
    QQmlApplicationEngine engine;
    const char *si = "SketchItem";
    qmlRegisterType<SketchItem>(si, 1, 0, si);
    QFile qmlFile(":/main.qml");
    qmlFile.open(QIODevice::ReadOnly);
//    const QUrl url(QStringLiteral());
//    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app,
//                     [url](QObject *obj, const QUrl &objUrl) {
//                         if (!obj && url == objUrl)
//                             QCoreApplication::exit(-1);
//                     }, Qt::QueuedConnection);
    engine.loadData(qmlFile.readAll());
    qmlFile.close();
    return app.exec();
}
