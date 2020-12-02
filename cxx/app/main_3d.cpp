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

#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QAspectEngine>

#include <Qt3DInput/QInputAspect>

#include <Qt3DExtras/QFirstPersonCameraController>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DRender/QRenderAspect>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QTorusMesh>
#include <Qt3DExtras/Qt3DWindow>

int main(int argc, char **argv) {
    qputenv("QML_DISABLE_DISK_CACHE", "1");
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    addFontData();
    addTranslator();

//    qmlRegisterType<SketchItem>(SketchItem::uri, 1, 0,
//                                SketchItem::qmlName);
//    QQmlApplicationEngine engine(QUrl("qrc:/main.qml"));

    auto *scene = new Qt3DCore::QEntity;

    auto *sphereEntity = new Qt3DCore::QEntity(scene);
    auto *sphereMesh = new Qt3DExtras::QSphereMesh;
    sphereMesh->setRadius(3);
    sphereMesh->setGenerateTangents(true);
    sphereMesh->setSlices(128);

    auto *cylinderMesh=new Qt3DExtras::QCylinderMesh;
    cylinderMesh->setLength(20);
    cylinderMesh->setRadius(5);

    auto *material = new Qt3DExtras::QPhongMaterial(scene);
//    material->setSpecular(Qt::darkRed);

//    auto *material = new Qt3DExtras::QDiffuseSpecularMaterial(scene);

    material->setAmbient(Qt::lightGray);
    material->setDiffuse(Qt::darkGray);
    material->setShininess(100);

    auto sphereTransform = new Qt3DCore::QTransform;
    sphereTransform->setScale(1);
//    sphereTransform->setTranslation(QVector3D(-3,-3,0));

    sphereEntity->addComponent(material);
    sphereEntity->addComponent(cylinderMesh);
    sphereEntity->addComponent(sphereTransform);

    Qt3DExtras::Qt3DWindow view;

    // Camera
    auto *camera = view.camera();
//    camera->lens()->setPerspectiveProjection(80.0f, 1.0f/1.0f, 0.01f, 1024000.0f);
    camera->setPosition(QVector3D(0, 0, 0));
    camera->setUpVector(QVector3D(0,0,1));
    auto locPos=QVector3D(50,50,0);
    camera->setViewCenter(QVector3D(100,100,0));
    sphereTransform->setTranslation(locPos);

    // For camera controls
    auto camController = new Qt3DExtras::QOrbitCameraController(scene);
//    auto camController = new Qt3DExtras::QFirstPersonCameraController(scene);
    camController->setLinearSpeed( 50.0f );
    camController->setLookSpeed( 180.0f );
    camController->setCamera(camera);

    view.setRootEntity(scene);
    view.showMaximized();

    return app.exec();
}
