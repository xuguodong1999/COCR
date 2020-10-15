#include <QFont>
#include <QFontDatabase>

#include <QDebug>

#include <QTranslator>

#include <QApplication>
#include <QQmlApplicationEngine>

#include <QDir>
#include <QWidget>

#include <string>
#include <filesystem>
void releaseOBabelData(){
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
    const std::string obFsPrefix(std::filesystem::current_path().string() + "/data/");
    putenv(const_cast<char*>(("BABEL_DATADIR="+obFsPrefix).c_str()));
    QDir dataDir(obFsPrefix.c_str());
    if (!dataDir.exists()) {
        dataDir.mkpath(obFsPrefix.c_str());
    }
    const QString obRccPrefix(":/obabel/");
    for (const auto &entry:QDir(obRccPrefix).entryList()) {
        QFile fileFs(obFsPrefix.c_str() + entry);
        if (fileFs.exists())continue;
        QFile fileRcc(obRccPrefix + entry);
        fileRcc.open(QIODevice::ReadOnly);
        fileFs.open(QIODevice::WriteOnly);
        fileFs.write(fileRcc.readAll());
        fileFs.close();
        fileRcc.close();
    }
#endif//!Q_OS_ANDROID && !Q_OS_IOS
}
void addFontData(){
    QFontDatabase::addApplicationFont(":/part.simhei.ttf");
    QGuiApplication::setFont(QFont("simhei"));
}
void addTranslator(){
    static QTranslator translator;
    if (translator.load(":/demo_zh_CN.qm")) {
        if (!QApplication::installTranslator(&translator)) {
            qDebug() << "fail to load " << translator.filePath();
        }
    }
}
int main(int argc, char **argv) {
    qputenv("QML_DISABLE_DISK_CACHE", "1");
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    releaseOBabelData();
    qInfo()<<QDir(":/").entryList();
    addFontData();
    addTranslator();
    QQmlApplicationEngine engine;
    const QUrl url("qrc:/main.qml");
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &a, [url](QObject *obj, const QUrl &objUrl) {
                if (!obj && url == objUrl) {
                    QCoreApplication::exit(-1);
                }
            }, Qt::QueuedConnection);
    engine.load(url);
    return a.exec();
}
