#include "qml_sketchitem.hpp"
#include <QFontDatabase>
#include <QTranslator>
#include <QApplication>
#include <QQmlApplicationEngine>

const char *fontUrl = ":/simfang.subset.ttf";
const char *transUrl = ":/trans_zh_CN.qm";

inline void addFontData() {
    QFontDatabase database;
    int id = QFontDatabase::addApplicationFont(fontUrl);
    try {
        qApp->setFont(QFont(database.applicationFontFamilies(id).at(0)));
    } catch (std::exception e) {
        qWarning() << e.what();
    }
}

inline void addTranslator() {
    static QTranslator translator;
    if (translator.load(transUrl)) {
        if (!qApp->installTranslator(&translator))
            qWarning() << "fail to install " << translator.filePath();
    } else {
        qWarning() << "fail to load " << translator.filePath();
    }
}

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QAspectEngine>

#include <Qt3DInput/QInputAspect>

#include <Qt3DRender/QRenderAspect>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QTorusMesh>


int main(int argc, char **argv) {
    qputenv("QML_DISABLE_DISK_CACHE", "1");
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    addFontData();
    addTranslator();

    qmlRegisterType<SketchItem>(SketchItem::uri, 1, 0,
                                SketchItem::qmlName);
    QQmlApplicationEngine engine(QUrl("qrc:/main.qml"));

    return app.exec();
}
