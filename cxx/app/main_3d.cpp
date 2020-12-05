#include "qml_sketchitem.hpp"
#include "isomer.hpp"
#include "mol3d.hpp"
#include "mol3dwindow.hpp"

#include <QFontDatabase>
#include <QTranslator>
#include <QQmlApplicationEngine>

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtGui/QScreen>

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
#include <Qt3DExtras/Qt3DWindow>

int main(int argc, char **argv) {
    qputenv("QML_DISABLE_DISK_CACHE", "1");
    // FIXME: qt3d high dpi error. disable HighDpiScaling before solving this bug
    // see Qt3DRender::QRenderSurfaceSelector
    // see https://codereview.qt-project.org/c/qt/qt3d/+/160557
//    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);
    addFontData();
    addTranslator();

//    qmlRegisterType<SketchItem>(SketchItem::uri, 1, 0,
//                                SketchItem::qmlName);
//    QQmlApplicationEngine engine(QUrl("qrc:/main.qml"));

    // Root entity
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();
    Mol3D *sceneBuilder = new Mol3D(rootEntity);

    auto &isomer = IsomerCounter::GetInstance();
    auto alkanes = isomer.getIsomers({
                                             3, 4, 5, 6, 7, 8, 10, 11, 12, 13, 14, 15
                                     });
    size_t index = alkanes.size() / 2;
    auto view = new Mol3DWindow(rootEntity);

    QObject::connect(view, &Mol3DWindow::spaceOrEnterPressed, [&]() {
        auto mol = std::make_shared<JMol>();
        mol->set(alkanes[index]);
        mol->randomize();
        sceneBuilder->resetMol(mol);
        index = (index + 1) & alkanes.size();
    });

    QWidget *container = QWidget::createWindowContainer(view);

    // Show window
    container->showMaximized();

    return app.exec();
}
