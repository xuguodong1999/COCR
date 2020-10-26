#include <QFont>
#include <QFontDatabase>

#include <QDebug>

#include <QTranslator>

#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QDir>

#include <string>
#include <filesystem>

int main(int argc, char **argv) {
    qputenv("QML_DISABLE_DISK_CACHE", "1");
    QGuiApplication a(argc, argv);
    qDebug()<<QDir(":/cocr_app").entryList();
    qDebug()<<QDir(":/cocr_app/obabel").entryList();
    qDebug()<<QDir(":/cocr_app/big").entryList();
    qDebug()<<QDir(":/cocr_app/img").entryList();
// #if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
//     const std::string obFsPrefix(std::filesystem::current_path().string() + "/data/");
//     _putenv_s("BABEL_DATADIR", obFsPrefix.c_str());
//     QDir dataDir(obFsPrefix.c_str());
//     if (!dataDir.exists()) {
//         dataDir.mkpath(obFsPrefix.c_str());
//     }
//     const QString obRccPrefix(":/obabel/");
//     for (const auto &entry:QDir(obRccPrefix).entryList()) {
//         QFile fileFs(obFsPrefix.c_str() + entry);
//         if (fileFs.exists())continue;
//         QFile fileRcc(obRccPrefix + entry);
//         fileRcc.open(QIODevice::ReadOnly);
//         fileFs.open(QIODevice::WriteOnly);
//         fileFs.write(fileRcc.readAll());
//         fileFs.close();
//         fileRcc.close();
//     }
// #endif//!Q_OS_ANDROID && !Q_OS_IOS
//     QFontDatabase::addApplicationFont(":/part.simhei.ttf");
//     QGuiApplication::setFont(QFont("simhei"));
//     QTranslator translator;
//     if (translator.load(":/demo_zh_CN.qm")) {
//         if (!QApplication::installTranslator(&translator)) {
//             qDebug() << "fail to load " << translator.filePath();
//         }
//     }
    QQmlApplicationEngine engine;
    const QUrl url("qrc:/cocr_app/qml/main.qml");
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &a, [url](QObject *obj, const QUrl &objUrl) {
                if (!obj && url == objUrl){
                    qDebug()<<"error load qml";
                    QCoreApplication::exit(-1);
                }
            }, Qt::QueuedConnection);
    engine.load(url);
    return a.exec();
}
