#include "qml_sketchitem.hpp"
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

int main(int argc, char **argv) {
    qputenv("QML_DISABLE_DISK_CACHE", "1");
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    addFontData();
    addTranslator();

    QQmlApplicationEngine engine;
    qmlRegisterType<SketchItem>(SketchItem::uri, 1, 0,
                                SketchItem::qmlName);

    QFile qmlFile(":/main.qml");
    qmlFile.open(QIODevice::ReadOnly);

    engine.loadData(qmlFile.readAll());
    qmlFile.close();
    return app.exec();
}
